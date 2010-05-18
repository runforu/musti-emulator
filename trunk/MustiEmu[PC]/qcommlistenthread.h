#ifndef QCOMMLISTENTHREAD_H
#define QCOMMLISTENTHREAD_H

#include <QThread>
#include "qserialcomm.h"
#include <QMutex>
#include <QWaitCondition>

class QCommListenThread: public QThread
{
    Q_OBJECT
public:
    explicit QCommListenThread( QObject * parent = 0 );
    virtual ~QCommListenThread();

    // Begin thread execution.
    bool startCommListen( const QString& comport,
                          const PORTSETTINGS* portSettings = NULL,
                          QIODevice::OpenMode mode = QIODevice::ReadWrite); 

    // Do safe termination. Stop thread execution.
    void terminate ();

    // Not terminate thread execution, just make thread sleep
    void stopExecute();
    // Wake up thread
    void startExecute();


signals:
    void lineAvailable(QString line);

private:
    void run();

    QSerialComm * commPort;
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    bool stop;
    QString commPortName;
    PORTSETTINGS portSettings;
};

#endif // QCOMMLISTENTHREAD_H
