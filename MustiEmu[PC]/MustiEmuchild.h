/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>

class MustiEmuChild : public QTextEdit
{
    Q_OBJECT

public:
    MustiEmuChild();
    ~MustiEmuChild();
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    void pauseTrace();
    void continueTrace();
    void flush();
    void addNewLine( QString &line );
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
signals:
    void toBeClosed();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void clear();

private slots:    
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;
    bool isPaused;
    QVector<QString> vecString;
    int shownSize;    
    QFont *font;
};

#endif
