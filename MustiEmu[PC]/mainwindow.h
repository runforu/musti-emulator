/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qserialcomm.h"
#include "qcommlistenthread.h"

class MustiEmuChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
//    void open();
    void save();
    void saveAs();
    void trace();
    void copy();
    void pause();
    void clean();
    void settings();
    void about();
    void updateMenus();
    void updateWindowMenu();
    void updateTraceStatus();
    MustiEmuChild *createMdiChild();
    void setActiveSubWindow(QWidget *window);
    void listenThreadStarted();
    void listenThreadStopped();
    void getLine(QString str);
    void outputWindowClose();
private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MustiEmuChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *traceMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QToolBar *toolBar;

    QAction *saveAsAct;
    QAction *exitAct;


    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;

    QAction *newAct;
    QAction *saveAct;

    QAction *copyAct;
    QAction *cleanAct;

    QAction *settingAct;
    QAction *pauseAct;

    MustiEmuChild *outputWindow;

    bool isPaused;
    bool isComAvailable;

    QString commPortName;
    PORTPROPERTY comProperty;

    QCommListenThread * listenThread;
};

#endif
