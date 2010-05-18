#include "qcommlistenthread.h"
#include <qdebug>

QCommListenThread::QCommListenThread( QObject * parent )
        :QThread(parent)
{
    abort = false;
    stop = false;
    commPort = new QSerialComm( this );
    setTerminationEnabled(false);
}

QCommListenThread::~QCommListenThread()
{
    mutex.lock();
    abort = true;
    // wake up thread, because thread may be wait on stop.
    condition.wakeOne();
    mutex.unlock();

    wait();
    delete commPort;
}

void QCommListenThread::terminate ()
{
    mutex.lock();
    abort = true;
    // wake up thread, because thread may be wait on stop.
    condition.wakeOne();
    mutex.unlock();
    wait();
    emit terminated();
}

bool QCommListenThread::startCommListen( const QString& comport,
                          const PORTSETTINGS* portSettings,
                          QIODevice::OpenMode mode )
{
    mutex.lock();
    abort = false;
    if ( isRunning() )
    {
        commPort->close();
    }
    this->commPortName = comport;
    if(portSettings)
        this->portSettings = *portSettings;

    commPort->open( comport, portSettings, mode );
    mutex.unlock();
    if ( !isRunning() )
    {
        start(LowPriority);
    }    
    return true;
}

void QCommListenThread::stopExecute()
{
    mutex.lock();
    if (!stop)
    { 
        stop = true;
        // do not need wake thread, because thread only wait on stop
    }
    mutex.unlock();
    // if stopped, do nothing
}

void QCommListenThread::startExecute()
{
    mutex.lock();   
    if (stop)
    {  
        stop = false;
        // wake up thread only when thread is waiting on stop
        condition.wakeOne();
    }
    mutex.unlock();
}

void QCommListenThread::run()
{
    while(true)
    {
        char buf[256+1] = {0};
        mutex.lock();
        int readBytes = commPort->readLine(buf, 256+1);
        mutex.unlock();
        qDebug()<<readBytes;

        if( readBytes != -1 && readBytes != 0 && !(readBytes == 1 && buf[0] == '\n') )
        {
            buf[readBytes-1] = 0;
            QString str(buf);
            emit lineAvailable( str );
        }

        mutex.lock();
        if (stop)
            condition.wait(&mutex);
        mutex.unlock();      
        
        if (abort)
        {
            return;
        }
    }
}
