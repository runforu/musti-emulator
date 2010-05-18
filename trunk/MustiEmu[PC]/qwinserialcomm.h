/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <qt_windows.h>
#include "qserialcomm.h"
#include <QIODevice>

#define  COM_DEVICE_NAME_LENGTH 32

class WinSerialComm
{
public:
    // Constructor
    WinSerialComm()
    {
        Init(); 
        _SettingAvailable = false;
    }

    // Destructor
    virtual ~WinSerialComm()
    {
        Close(); 
        Destroy();
    }


    bool  SetPortSettings(const PORTSETTINGS* portSettings);

    bool  GetPortSettings(PORTSETTINGS &portSettings);

    // Test if Com port is open
    inline bool IsOpen()
    {
        return _hCommHandle != INVALID_HANDLE_VALUE;
    }

    // Return Comm port name
    const wchar_t *GetPortName()
    {
        return _szComDeviceName;
    }

    // Discards all characters from the input buffer of a specified communications resource
    void ClearInputBuffer()
    {
        if(IsOpen())
            ::PurgeComm(_hCommHandle, PURGE_RXABORT | PURGE_RXCLEAR );
    }

    // Discards all characters from the output buffer of a specified communications resource
    void ClearOutputBuffer()
    {
        if(IsOpen())
            ::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR );
    }

    // Get the bytes count in input buffer
    DWORD BytesAvailable()
    {
        COMSTAT Stat;
        DWORD dwError;

        return ::ClearCommError(_hCommHandle, &dwError, &Stat) == TRUE  ? Stat.cbInQue : (DWORD)-1L;
    }

    // Get the bytes count in out buffer, This value will be zero for a nonoverlapped write
    DWORD BytesToWrite()
    {
        COMSTAT Stat;
        DWORD dwError;

        return ::ClearCommError(_hCommHandle, &dwError, &Stat) == TRUE  ? Stat.cbOutQue : (DWORD)-1L;
    }

   bool open ( const wchar_t * comport,
               const PORTSETTINGS* portSettings = NULL,
               QIODevice::OpenMode mode = QIODevice::ReadWrite );

   bool reopen ();

    // Sync Read Comm port
    DWORD ReadSync(LPVOID Buffer, DWORD dwBufferLength);
   

    // Sync Write Comm port
    DWORD WriteSync(const void* Buffer, DWORD dwBufferLength);
   

    // Close Comm port, and related thread
    virtual void Close();

    // Not support in nonoverlapped IO
    virtual bool WaitForBytesWritten ( int msecs );

    // Not support in nonoverlapped IO
    virtual bool WaitForReadyRead ( int msecs );

    // set Data-Terminal-Ready
    bool SetDTR(bool OnOrOff)
    {
        return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETDTR : CLRDTR) != 0 : false;
    }

    //Set Request-To-Send
    bool SetRTS(bool OnOrOff)
    {
        return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETRTS : CLRRTS) != 0 : false;
    }

    // Suspends character transmission and places the transmission line in a break state 
    // until the ClearCommBreak function is called (or EscapeCommFunction is called with the CLRBREAK extended function code).
    bool SetBreak(bool OnOrOff)
    {
        return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETBREAK : CLRBREAK) != 0 : false;
    }


private:  //the function protected 

    WinSerialComm(const WinSerialComm&);
    WinSerialComm &operator = (const WinSerialComm&);

    // Init 
    void Init();
    // Destroy resource
    void Destroy();
    // Open Comm port
    bool OpenCommPort();

    void BuildDefaultSettings()
    {
        BuildDefaultDcb();
        _CO.ReadIntervalTimeout = 0;
        _CO.ReadTotalTimeoutMultiplier = 10;
        _CO.ReadTotalTimeoutConstant = 100;
        _CO.WriteTotalTimeoutMultiplier = 10;
        _CO.WriteTotalTimeoutConstant = 100;
        _InBufferSize = 4096;
        _OutBufferSeze = 4096;
    }

    void BuildDefaultDcb();

    void BuildDcbFrom(const PORTPROPERTY & property);

private: 
    // Comm port
    volatile HANDLE _hCommHandle;  // Comm port handler
    wchar_t _szComDeviceName[COM_DEVICE_NAME_LENGTH];  // Store "\\\\.\\COM1"

    // Comm Settings 
    DCB _DCB;  // Baud rate, stop bit ...
    COMMTIMEOUTS _CO; // Timeout struct
    DWORD _InBufferSize;
    DWORD _OutBufferSeze;
    bool _SettingAvailable;
};

#endif // SERIALCOMM_H
