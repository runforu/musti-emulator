#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#undef UNICODE

#include <assert.h>
#include <stdio.h>
#include <qt_windows.h>
#include <qthread>
#include <QMutex>
#include <QWaitCondition>


//EV_BREAK: A break was detected on input.

//EV_CTS: The CTS (clear-to-send) signal changed state.

//EV_DSR: The DSR (data-set-ready) signal changed state.

//EV_ERR: A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.

//EV_RING: A ring indicator was detected.

//EV_RLSD: The RLSD (receive-line-signal-detect) signal changed state.

//EV_RXCHAR: A character was received and placed in the input buffer.

//EV_RXFLAG: The event character was received and placed in the input buffer. 
//           The event character is specified in the device's DCB structure, which is applied to a serial port by using the SetCommState function.

//EV_TXEMPTY: The last character in the output buffer was sent.

#define DEFAULT_COM_MASK_EVENT  EV_RXCHAR | EV_ERR | EV_CTS | EV_DSR | EV_BREAK | EV_TXEMPTY | EV_RING | EV_RLSD


class WinSerialComm : public QThread
{

public:
    // Constructor
    WinSerialComm(bool fAutoBeginThread = true, DWORD dwIOMode = FILE_FLAG_OVERLAPPED)
        : _dwIOMode(dwIOMode)
        ,_fAutoBeginThread(fAutoBeginThread)
    {
        Init(); 
    }

    // Destructor
    virtual ~WinSerialComm()
    {
        Close(); 
        Destroy();
    }

    // Test if Com port is open
    inline bool IsOpen()
    {
        return _hCommHandle != INVALID_HANDLE_VALUE;
    }

    // Test if Comm port is open
    operator bool ()
    {
        return _hCommHandle != INVALID_HANDLE_VALUE;
    }
    
    // Return Comm handler
    HANDLE GetHandle()
    {
        return _hCommHandle;
    }

    // Return Comm handler
    operator HANDLE()
    {
        return _hCommHandle;
    }

    // Return Comm port ID
    const int GetPortID()
    {
        return _dwPort;
    }

    // Return Comm port name
    const char *GetPortName()
    {
        return _szCommStr;
    }

    // Get Comm port state
    DCB *GetState()
    {
        return IsOpen() && ::GetCommState(_hCommHandle, &_DCB) == TRUE ? &_DCB : NULL;
    }

    // Set Comm port state
    bool SetState(DCB *pdcb = NULL)
    {
        if ( pdcb )
        {
        }
        return IsOpen() ? ::SetCommState(_hCommHandle, &_DCB ) : false;
    }

    // Set Comm timeout
    LPCOMMTIMEOUTS GetTimeouts(void)
    {
        return IsOpen() && ::GetCommTimeouts(_hCommHandle, &_CO) == TRUE  ? &_CO : NULL;
    }

    // Set Comm timeout
    bool SetTimeouts(LPCOMMTIMEOUTS lpCO)
    {
        return IsOpen() ? ::SetCommTimeouts(_hCommHandle, lpCO) == TRUE : false;
    }

    // Set Comm timeout
    bool SetBufferSize(DWORD dwInputSize, DWORD dwOutputSize)
    {
        return IsOpen() ? ::SetupComm(_hCommHandle, dwInputSize, dwOutputSize) == TRUE : false; 
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

    // Set the minimum received bytes to notify
    inline void SetNotifyNum(DWORD dwNum)
    {
        _dwNotifyNum = dwNum;
    }

    // Set listening event, take effect before open
    void SetMaskEvent(DWORD dwEvent = DEFAULT_COM_MASK_EVENT)
    {
        _dwMaskEvent = dwEvent;
    }

    // Get read buffer size
    int GetInputSize()
    {
        COMSTAT Stat;
        DWORD dwError;

        return ::ClearCommError(_hCommHandle, &dwError, &Stat) == TRUE  ? Stat.cbInQue : (DWORD)-1L;
    }

    // Open Comm port using baud rate, default: 8, n, 1
    bool Open(DWORD dwPort, DCB * pDcb = NULL )
    {
        if(dwPort < 1 || dwPort > 1024)
            return false;

        BindCommPort(dwPort);

        if(!OpenCommPort())
            return false;

        if(!SetupPort())
            return false;

        return SetState( pDcb );
    }

    // Read maximum dwBufferLength bytes to Buffer, return the length of reading string in fact
    DWORD Read(LPVOID Buffer, DWORD dwBufferLength, DWORD dwWaitTime = 10)
    {
        if(!IsOpen())
            return 0;

        COMSTAT  Stat;
        DWORD dwError;

        if(::ClearCommError(_hCommHandle, &dwError, &Stat) && dwError > 0)
        {
            ::PurgeComm(_hCommHandle, PURGE_RXABORT | PURGE_RXCLEAR);
            return 0;
        }

        // No data in Comm buffer
        if(!Stat.cbInQue) 
            return 0;

        unsigned long uReadLength = 0;

        dwBufferLength = dwBufferLength > Stat.cbInQue ? Stat.cbInQue : dwBufferLength;

        if(!::ReadFile(_hCommHandle, Buffer, dwBufferLength, &uReadLength, &_ReadOverlapped)) 
        {
            if(::GetLastError() == ERROR_IO_PENDING) 
            {
                // End async IO
                WaitForSingleObject(_ReadOverlapped.hEvent, dwWaitTime); 
                if(!::GetOverlappedResult(_hCommHandle, &_ReadOverlapped, &uReadLength, false))
                {
                    if(::GetLastError() != ERROR_IO_INCOMPLETE)
                        uReadLength = 0;
                }
            }
            else
                uReadLength = 0;
        }

        return uReadLength;
    }

    // Read dwBufferLength-1 bytes to szBuffer, return ANSI string pointer
    char * ReadString(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime = 20)
    {
        unsigned long uReadLength = Read(szBuffer, dwBufferLength - 1, dwWaitTime);
        szBuffer[uReadLength] = '\0';
        return szBuffer;
    }

    // Write Buffer to Comm port, any type data "abc" or "\0x1\0x2"
    DWORD Write(LPVOID Buffer, DWORD dwBufferLength)
    {
        if(!IsOpen())
            return 0;

        DWORD dwError;

        if(::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0) 
            ::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 

        unsigned long uWriteLength = 0;

        if(!::WriteFile(_hCommHandle, Buffer, dwBufferLength, &uWriteLength, &_WriteOverlapped))
            if(::GetLastError() != ERROR_IO_PENDING)
                uWriteLength = 0;

        return uWriteLength;
    }

    // Write C string to Comm port
    DWORD Write(const char *szBuffer)
    {
        assert(szBuffer);
        return Write((void *)szBuffer, strlen(szBuffer));
    }

    // Sync Read Comm port
    DWORD ReadSync(LPVOID Buffer, DWORD dwBufferLength)
    {
        if(!IsOpen())
            return 0;

        DWORD dwError;
        if(::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0)
        {
            ::PurgeComm(_hCommHandle, PURGE_RXABORT | PURGE_RXCLEAR);
            return 0;
        }

        DWORD uReadLength = 0;
        ::ReadFile(_hCommHandle, Buffer, dwBufferLength, &uReadLength, NULL);
        return uReadLength;
    }

    // Sync Write Comm port
    DWORD WriteSync(LPVOID Buffer, DWORD dwBufferLength)
    {
        if(!IsOpen())
            return 0;

        DWORD dwError;
        if(::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0) 
            ::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 

        unsigned long uWriteLength = 0;
        ::WriteFile(_hCommHandle, Buffer, dwBufferLength, &uWriteLength, NULL);
        return uWriteLength;
    }

    // Printf like write Comm port method
    DWORD Write(char *szBuffer, DWORD dwBufferLength, char * szFormat, ...)
    {
        if(!IsOpen())
            return 0;

        va_list va;
        va_start(va, szFormat);
        _vsnprintf(szBuffer, dwBufferLength, szFormat, va);
        va_end(va);

        return Write(szBuffer);
    }

    // Printf like write Comm port method, no length check
    DWORD Write(char *szBuffer, char * szFormat, ...)
    {
        if(!IsOpen())
            return 0;

        va_list va;
        va_start(va, szFormat);
        vsprintf(szBuffer, szFormat, va);
        va_end(va);

        return Write(szBuffer);
    }

    // Close Comm port, and related thread
    virtual void Close()
    {
        if(IsOpen())  
        {
            PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 

            ::CloseHandle(_hCommHandle);

            _hCommHandle = INVALID_HANDLE_VALUE;
        }
    }
   
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


signals:
    void OnReceive();
    void OnDSR();
    void OnCTS();
    void OnBreak();
    void OnTXEmpty();
    void OnError();
    void OnRing();
    void OnRLSD();


    void BuildDefaultDcb();
    // Init 
    virtual void Init();
    // Destroy resource
    virtual void Destroy();
    // Bind Comm port
    void BindCommPort(DWORD dwPort);
    // Open Comm port
    virtual bool OpenCommPort();
    // Setup Comm port
    virtual bool SetupPort();

    // Listen Comm event.
    void run ();

private:
    volatile DWORD _dwPort;   // Comm port
    volatile HANDLE _hCommHandle;  // Comm port handler
    char _szCommStr[20];  // Store "\\\\.\\COM1"

    DCB _DCB;  // Baud rate, stop bit ...
    COMMTIMEOUTS _CO; // Timeout struct

    DWORD _dwIOMode;  // IO mode: 0 sync; FILE_FLAG_OVERLAPPED async
    OVERLAPPED _ReadOverlapped, _WriteOverlapped;  // Overlapped IO

    volatile HANDLE _hThreadHandle; // Listen thread handler
    volatile DWORD _dwNotifyNum;  // Maximum bytes to send message.
    volatile DWORD _dwMaskEvent;  // Listened event mask.
    bool _fAutoBeginThread;  // Auto start listen thread

    OVERLAPPED _WaitOverlapped;  //WaitCommEvent use

    QMutex _Mutex;
    QWaitCondition _Condition;
    bool _Abort;
};

#endif // SERIALCOMM_H
