/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#include <QtGui>


#include "mainwindow.h"
#include "MustiEmuchild.h"
#include "comportdlg.h"
#include "comconfigdialog.h"

MainWindow::MainWindow()
{
    setWindowIcon( QIcon(":/images/app_icon.png") );

    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
            this, SLOT(setActiveSubWindow(QWidget *)));

    listenThread = new QCommListenThread(this);
    connect(listenThread, SIGNAL(started()),
        this, SLOT(listenThreadStarted()));

    connect(listenThread, SIGNAL(terminated()),
        this, SLOT(listenThreadStopped()));

    connect(listenThread, SIGNAL(lineAvailable(QString)),
        this, SLOT(getLine(QString)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("Musti Emulator"));
    setUnifiedTitleAndToolBarOnMac(true);
    outputWindow = NULL;
    isComAvailable = false;
    isPaused = false;
    pauseAct->setEnabled(false);
    newFile();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (activeMdiChild()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MustiEmuChild *child = createMdiChild();
    child->newFile();
    child->show();
    if( outputWindow )
    {
        outputWindow->flush();
        disconnect(outputWindow, SIGNAL(toBeClosed()),
            this, SLOT(getLine(QString)));
    }

    outputWindow = child;
    connect(outputWindow, SIGNAL(toBeClosed()),
        this, SLOT(outputWindowClose()));

    isPaused = false;
    pauseAct->setEnabled(listenThread->isRunning());
    pauseAct->setIcon(QIcon(":/images/pause.png"));
}

void MainWindow::outputWindowClose()
{
    outputWindow = NULL;
    isPaused = false;
    pauseAct->setEnabled(false);
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::settings()
{
     comPortDlg availablePorts;

     if( availablePorts.exec() == QDialog::Accepted )
     {
         QString comPorts = availablePorts.selectedCommPorts();
         comConfigDialog configPorts(comPorts);
         if( configPorts.exec() == QDialog::Accepted )
         {
             commPortName = comPorts;
             comProperty = configPorts.property();
             isComAvailable = true;
             trace();
         }
         else
         {
             QMessageBox::about(this, tr("Comm port not configed:"),
                            tr("Comm Ports not configed.\n"
                               "No availabe com ports to perform tracing.") );
             isComAvailable = false;
         }
     }
     else
     {
         QMessageBox::about(this, tr("No availabe com ports:"),
                            tr("No availabe com ports to perform tracing.") );
         isComAvailable = false;
     }
}

void MainWindow::clean()
{
    if (activeMdiChild() && !activeMdiChild()->document()->isEmpty() )
        activeMdiChild()->clear();
}


void MainWindow::trace()
{
    PORTSETTINGS settings;
    settings.Properties = comProperty;
    listenThread->startCommListen(commPortName, &settings);
}

void MainWindow::pause()
{
    isPaused = !isPaused; 
    if (isPaused)
    {
        pauseAct->setIcon(QIcon(":/images/continue.png"));
        if( outputWindow )
            outputWindow->pauseTrace();
    }
    else
    {
        pauseAct->setIcon(QIcon(":/images/pause.png"));
        if( outputWindow )
            outputWindow->continueTrace();
    }       
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Musti Emulator"),
            tr("The <b>Musti Emulator</b> is to emulate Musti for Symbian OS.\n"
               "All right is reserved by Du Hui."));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());

    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MustiEmuChild *child = qobject_cast<MustiEmuChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

void MainWindow::updateTraceStatus()
{
    //TODO: updateTraceStatus
}

MustiEmuChild *MainWindow::createMdiChild()
{
    MustiEmuChild *child = new MustiEmuChild;
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    cleanAct = new QAction(QIcon(":/images/clean.png"), tr("&Clean"), this);
    //copyAct->setShortcuts(QKeySequence::Copy);
    cleanAct->setStatusTip(tr("Clean the output window content."));
    connect(cleanAct, SIGNAL(triggered()), this, SLOT(clean()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setShortcut(tr("Ctrl+F4"));
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    settingAct = new QAction(QIcon(":/images/settings.png"), tr("&Settings"), this);
    //settingAct->setShortcuts(QKeySequence::Copy);
    settingAct->setStatusTip(tr("Config connection"));
    connect(settingAct, SIGNAL(triggered()), this, SLOT(settings()));

    pauseAct = new QAction(QIcon(":/images/pause.png"), tr("&Pause"), this);
    //pauseAct->setShortcuts(QKeySequence::Copy);
    pauseAct->setStatusTip(tr("Pause or resume trace"));
    connect(pauseAct, SIGNAL(triggered()), this, SLOT(pause()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(copyAct);
    editMenu->addAction(cleanAct);

    traceMenu = menuBar()->addMenu(tr("&Trace"));
    traceMenu->addAction(settingAct);
    traceMenu->addSeparator();
    traceMenu->addAction(pauseAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    toolBar = addToolBar(tr("File"));
    toolBar->addAction(newAct);
    toolBar->addAction(saveAct);
    toolBar->addSeparator();

    toolBar->addAction(copyAct);
    toolBar->addAction(cleanAct);
    toolBar->addSeparator();

    toolBar->addAction(settingAct);
    toolBar->addAction(pauseAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
//    QSettings settings("Trolltech", "Musti Emulator");
//    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
//    QSize size = settings.value("size", QSize(400, 400)).toSize();
//    move(pos);
//    resize(size);
}

void MainWindow::writeSettings()
{
//    QSettings settings("Trolltech", "Musti Emulator");
//    settings.setValue("pos", pos());
//    settings.setValue("size", size());
}

MustiEmuChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MustiEmuChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MustiEmuChild *mdiChild = qobject_cast<MustiEmuChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::listenThreadStarted()
{
    pauseAct->setEnabled( outputWindow );
    pauseAct->setIcon(QIcon(":/images/pause.png"));
    isPaused = false;
}

void MainWindow::listenThreadStopped()
{
    pauseAct->setEnabled(false); 
    pauseAct->setIcon(QIcon(":/images/pause.png"));
}

void MainWindow::getLine(QString line)
{
    if(outputWindow)
        outputWindow->addNewLine(line);
}

