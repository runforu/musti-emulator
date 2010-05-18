/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/

#include <QtGui>
#include <QDebug>

#include "MustiEmuchild.h"

MustiEmuChild::MustiEmuChild()
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    font = new QFont("Verdana", 10 );
    setFont(*font);
    //setReadOnly(true);
}

MustiEmuChild::~MustiEmuChild()
{
    delete font;
}

void MustiEmuChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    shownSize = 0;
    isPaused = false;
    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool MustiEmuChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    return true;
}

bool MustiEmuChild::save()
{
    flush();
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MustiEmuChild::saveAs()
{
    flush();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MustiEmuChild::pauseTrace()
{
    isPaused = true;
    shownSize = vecString.size();
}

void MustiEmuChild::continueTrace()
{
    isPaused = false;
    flush();
}

void MustiEmuChild::flush()
{
    for(int i = shownSize; i < vecString.size(); i++)
        append(vecString.at(i));
    shownSize = vecString.size();
}

void MustiEmuChild::addNewLine(QString &line)
{
    vecString.append(line);
    if(!isPaused)
    {
        append(line);
        shownSize = vecString.size();
    }
}

bool MustiEmuChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString MustiEmuChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MustiEmuChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        emit toBeClosed();
        event->accept();
    } else {
        event->ignore();
    }
}

void MustiEmuChild::clear()
{
    vecString.clear();
    QTextEdit::clear();
}

void MustiEmuChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool MustiEmuChild::maybeSave()
{
    if (document()->isModified()) {
	QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
		     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MustiEmuChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MustiEmuChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
