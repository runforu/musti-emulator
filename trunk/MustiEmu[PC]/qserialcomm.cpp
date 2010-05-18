/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/

#include "qserialcomm.h"
#include "qwinserialcomm.h"
#include <QDebug>
QSerialComm::QSerialComm( QObject * parent )
{
    serialCommImp = new WinSerialComm;
}

QSerialComm::~QSerialComm()
{
    delete serialCommImp;
}

// From QIODevice
qint64 QSerialComm::bytesAvailable () const
{
    if (isOpen()) {
        int count = serialCommImp->BytesAvailable();
        if ( -1L == count)
        {
            return -1;
        }
        return count + QIODevice::bytesAvailable();
    }
    return 0;
}

// From QIODevice
qint64 QSerialComm::bytesToWrite () const
{
    if (isOpen())
    {
        return serialCommImp->BytesToWrite();
    }
    return 0;
}

// From QIODevice
void QSerialComm::close ()
{
    serialCommImp->Close(); 
}


// From QIODevice
bool QSerialComm::waitForBytesWritten ( int msecs )
{
    return serialCommImp->WaitForBytesWritten(msecs);
}

// From QIODevice
bool QSerialComm::waitForReadyRead ( int msecs )
{
    return serialCommImp->WaitForReadyRead(msecs);
}

// From QIODevice
qint64 QSerialComm::readData ( char * data, qint64 maxSize )
{
    return serialCommImp->ReadSync(data,maxSize);
}

// From QIODevice
qint64 QSerialComm::readLineData ( char * data, qint64 maxSize )
{
    char* pData = data;
    //read a byte at a time for MIN(bytesAvail, maxSize - 1) iterations, or until a newline
    while (pData<(data+maxSize-1))
    {
        int n = readData(pData, 1);
        if (n == 0)
            return -1;
        if (*pData++ == '\n')
        {
            break;
        }
    }

    //return size of data read
    return (pData-data);
}

// From QIODevice
qint64 QSerialComm::writeData ( const char * data, qint64 maxSize )
{
    return serialCommImp->WriteSync(data,maxSize);
}

// ----------------------------Add new methods------------------------------------------

bool QSerialComm::open ( const QString& comport,
                        const PORTSETTINGS* portSettings,
                        OpenMode mode )
{
    if ( mode == QIODevice::Text || mode == QIODevice::Unbuffered )
    {
        mode = QIODevice::ReadWrite;
    }

    return serialCommImp->open( (const wchar_t*)comport.utf16(), portSettings, mode );
}

bool QSerialComm::reopen ()
{
    return serialCommImp->reopen();
}

bool  QSerialComm::setPortSettings(const PORTSETTINGS* portSettings)
{
    return serialCommImp->SetPortSettings(portSettings);
}

bool  QSerialComm::getPortSettings(PORTSETTINGS &portSettings)
{
    return serialCommImp->GetPortSettings(portSettings);
}

const QString  QSerialComm::getPortName()
{
    return QString::fromUtf16( (const ushort*)serialCommImp->GetPortName());
}

bool QSerialComm::setDTR(bool OnOrOff)
{
    return serialCommImp->SetDTR(OnOrOff);
}

bool QSerialComm::setRTS(bool OnOrOff)
{
    return serialCommImp->SetRTS(OnOrOff);
}

bool QSerialComm::setBreak(bool OnOrOff)
{
    return serialCommImp->SetBreak(OnOrOff);
}

void QSerialComm::clearInputBuffer()
{
    return serialCommImp->ClearInputBuffer();
}

// Discards all characters from the output buffer of a specified communications resource
void QSerialComm::clearOutputBuffer()
{
    return serialCommImp->ClearOutputBuffer();
}

bool QSerialComm::isOpen () const
{
    return serialCommImp->IsOpen();
}




