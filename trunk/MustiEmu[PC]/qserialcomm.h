/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#ifndef QSERIALCOMM_H
#define QSERIALCOMM_H

#include <QIODevice>

class WinSerialComm;

enum BAUD_RATE_TYPE {
    BAUD_RATE_TYPE_50 = 50,                //POSIX ONLY
    BAUD_RATE_TYPE_75 = 75,                //POSIX ONLY
    BAUD_RATE_TYPE_110 = 110,
    BAUD_RATE_TYPE_134 = 134,               //POSIX ONLY
    BAUD_RATE_TYPE_150 = 150,               //POSIX ONLY
    BAUD_RATE_TYPE_200 = 200,               //POSIX ONLY
    BAUD_RATE_TYPE_300 = 300,
    BAUD_RATE_TYPE_600 = 600,
    BAUD_RATE_TYPE_1200 = 1200,
    BAUD_RATE_TYPE_1800 = 1800,              //POSIX ONLY
    BAUD_RATE_TYPE_2400 = 2400,
    BAUD_RATE_TYPE_4800 = 4800,
    BAUD_RATE_TYPE_9600 = 9600,
    BAUD_RATE_TYPE_14400 = 14400,             //WINDOWS ONLY
    BAUD_RATE_TYPE_19200 = 19200,
    BAUD_RATE_TYPE_38400 = 38400,
    BAUD_RATE_TYPE_56000 = 56000,             //WINDOWS ONLY
    BAUD_RATE_TYPE_57600 = 56700,
    BAUD_RATE_TYPE_76800 = 76800,             //POSIX ONLY
    BAUD_RATE_TYPE_115200 = 115200,
    BAUD_RATE_TYPE_128000 = 128000,            //WINDOWS ONLY
    BAUD_RATE_TYPE_256000 = 256000             //WINDOWS ONLY
};

enum DATA_BITS_TYPE {
    DATA_BITS_TYPE_4 = 0,  // Do not use it in 8250
    DATA_BITS_TYPE_5 = 1,
    DATA_BITS_TYPE_6 = 2,
    DATA_BITS_TYPE_7 = 3,
    DATA_BITS_TYPE_8 = 4
};

enum PARITY_TYPE {
    PARITY_TYPE_NONE = 0,
    PARITY_TYPE_ODD = 1,
    PARITY_TYPE_EVEN = 2,
    PARITY_TYPE_MARK = 3,               //WINDOWS ONLY
    PARITY_TYPE_SPACE = 4
};

enum STOP_BITS_TYPE {
    STOP_BITS_TYPE_1 = 0,
    STOP_BITS_TYPE_1_5 = 1,               //WINDOWS ONLY
    STOP_BITS_TYPE_2 = 2
};

enum FLOW_TYPE {
    FLOW_TYPE_OFF = 0 ,
    FLOW_TYPE_HARDWARE = 1,
    FLOW_TYPE_XONXOFF = 2
};

struct  PORTPROPERTY
{
    PORTPROPERTY():
            BaudRate(115200),
            FlowControl(FLOW_TYPE_OFF),
            DataBits(DATA_BITS_TYPE_8),
            Parity(PARITY_TYPE_NONE),
            StopBits(STOP_BITS_TYPE_1)
    {
    }
    int BaudRate;
    FLOW_TYPE FlowControl;
    unsigned char DataBits;
    unsigned char Parity;
    unsigned char StopBits;

};

struct PORTBUFFERSIZE 
{
    PORTBUFFERSIZE():InputBufferSize(4096),OutputBufferSize(4096)
    {}
    qint32 InputBufferSize;
    qint32 OutputBufferSize;
};

struct PORTTIMEOUTS 
{  
    PORTTIMEOUTS():
            ReadIntervalTimeout(0),
            ReadTotalTimeoutMultiplier(100),
            ReadTotalTimeoutConstant(1000),
            WriteTotalTimeoutMultiplier(100),
            WriteTotalTimeoutConstant(1000)
    {}
    int ReadIntervalTimeout;
    int ReadTotalTimeoutMultiplier;
    int ReadTotalTimeoutConstant;
    int WriteTotalTimeoutMultiplier;
    int WriteTotalTimeoutConstant;
};

#define  PROPERTY_VALID_MASK  0X01
#define  BUFFERSIZE_VALID_MASK  0X02
#define  TIMEOUTS_VALID_MASK  0X04

/*structure to contain port settings*/
struct PORTSETTINGS
{
    PORTSETTINGS():SettingsValidMask( PROPERTY_VALID_MASK|BUFFERSIZE_VALID_MASK|TIMEOUTS_VALID_MASK )
    {}
    PORTPROPERTY Properties;
    PORTBUFFERSIZE BufferSize;
    PORTTIMEOUTS Timeout;
    int SettingsValidMask;
};

class QSerialComm : public QIODevice
{
    Q_OBJECT

public:
    explicit QSerialComm( QObject * parent = NULL );
    ~QSerialComm ();

    // From QIODevice
    virtual bool atEnd () const
    {
        return false;
    }

    // From QIODevice
    virtual qint64 bytesAvailable () const;

    // From QIODevice
    virtual qint64 bytesToWrite () const;

    // From QIODevice
    virtual bool canReadLine () const
    {
        return true;
    }

     // From QIODevice
    virtual void close ();

     // From QIODevice
    virtual bool isSequential () const
    {
        return true;
    }

    // From QIODevice
    virtual qint64 size () const
    {
        return bytesAvailable();
    }

    // From QIODevice
    virtual bool waitForBytesWritten ( int msecs );

    // From QIODevice
    virtual bool waitForReadyRead ( int msecs );


protected:
    
    // From QIODevice
    virtual qint64 readData ( char * data, qint64 maxSize );
    
    // From QIODevice
    virtual qint64 readLineData ( char * data, qint64 maxSize );
    
    // From QIODevice
    virtual qint64 writeData ( const char * data, qint64 maxSize );


// ----------------------------Add new methods------------------------------------------
public:
    bool open ( const QString& comport, const PORTSETTINGS* portSettings = NULL, OpenMode mode = QIODevice::ReadWrite );
    bool reopen ();

    bool  setPortSettings(const PORTSETTINGS* portSettings);
    bool  getPortSettings(PORTSETTINGS &portSettings);

    int  unreadBytes();

    const QString  getPortName();
    bool setDTR(bool OnOrOff);
    bool setRTS(bool OnOrOff);
    bool setBreak(bool OnOrOff);

    void clearInputBuffer();

    // Discards all characters from the output buffer of a specified communications resource
    void clearOutputBuffer();


private: // Comm not support method. Override them
    // From QIODevice
    virtual bool open ( OpenMode mode )
    {    
        return false;
    }
    // From QIODevice
    virtual bool seek ( qint64 pos )
    {
        return false;
    }
    // From QIODevice
    virtual bool reset ()
    {
        return false;
    }
    // From QIODevice
    virtual qint64 pos () const
    {
        return -1;
    }

private:  // Hide QIODevice method
    bool isOpen () const;


// ----------------------------data member------------------------------------------

private:
    WinSerialComm * serialCommImp;
};

#endif // QSERIALCOMM_H
