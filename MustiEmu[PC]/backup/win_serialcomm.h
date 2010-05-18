#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <assert.h>
#include <stdio.h>
#include <qt_windows.h>
#include <QThread>


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



class SerialComm : public QThread
{
    Q_OBJECT
public:
    // Constructor
    SerialComm(bool fAutoBeginThread = true, DWORD dwIOMode = FILE_FLAG_OVERLAPPED)
        : _dwIOMode(dwIOMode)
        ,_fAutoBeginThread(fAutoBeginThread)
    {
        Init(); 
    }

    // Destructor
    virtual ~SerialComm()
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
        return IsOpen() ? ::SetCommState(_hCommHandle, pdcb == NULL ? &_DCB : pdcb) == TRUE : false;
    }

    // Set Comm port state
    bool SetState(char *szSetStr) 
    {
        if(IsOpen())
        {
            if(::GetCommState(_hCommHandle, &_DCB) != TRUE)
                return false;
            if(::BuildCommDCBA(szSetStr, &_DCB) != TRUE)
                return false;
            return ::SetCommState(_hCommHandle, &_DCB) == TRUE;
        }
        return false;
    }

    // Set Comm port state
    bool SetState(DWORD dwBaudRate, DWORD dwByteSize = 8, DWORD dwParity = NOPARITY, DWORD dwStopBits = ONESTOPBIT)
    {
        if(IsOpen())
        {
            if(::GetCommState(_hCommHandle, &_DCB) != TRUE)
                return false;
            _DCB.BaudRate = dwBaudRate;
            _DCB.ByteSize = (unsigned char)dwByteSize;
            _DCB.Parity   = (unsigned char)dwParity;
            _DCB.StopBits = (unsigned char)dwStopBits;
            return ::SetCommState(_hCommHandle, &_DCB) == TRUE;
        }
        return false;
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

    // TODO: remove it
    inline void SetWnd(HWND hWnd)
    {
		assert(::IsWindow(hWnd));
        _hNotifyWnd = hWnd;
    }

    // Set the minimum received bytes to notify
    inline void SetNotifyNum(DWORD dwNum)
    {
        _dwNotifyNum = dwNum;
    }

    // Test if thread is running
    inline bool IsThreadRunning()
    {
        return _hThreadHandle != NULL;
    }

    // Get thread handler
    inline HANDLE GetThread()
    {
        return _hThreadHandle;
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

    // Open Comm port, default: 9600, 8, n, 1
    bool Open(DWORD dwPort)
    {
        return Open(dwPort, 9600);
    }

    // Open Comm port using baud rate, default: 8, n, 1
    bool Open(DWORD dwPort, DWORD dwBaudRate)
    {
        if(dwPort < 1 || dwPort > 1024)
            return false;

        BindCommPort(dwPort);

        if(!OpenCommPort())
            return false;

        if(!SetupPort())
            return false;

        return SetState(dwBaudRate);
    }

    // Open Comm port using string such as "9600, 8, n, 1"
    bool Open(DWORD dwPort, char *szSetStr)
    {
        if(dwPort < 1 || dwPort > 1024)
            return false;

        BindCommPort(dwPort);

        if(!OpenCommPort())
            return false;

        if(!SetupPort())
            return false;

        return SetState(szSetStr);
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

            EndThread();
            ::CloseHandle(_hCommHandle);

            _hCommHandle = INVALID_HANDLE_VALUE;
        }
    }

    // Resource lock
    void Lock()
    {
        ::EnterCriticalSection(&_Mutex);
    }

    // Resource unlock
    void Unlock()
    {
        ::LeaveCriticalSection(&_Mutex);
    }

    // inner lock
    struct InnerLock
    {
        SerialComm* ptr;

        InnerLock(SerialComm* p) : ptr(p)
        {
            ptr->Lock();
        }
        ~InnerLock()
        {
            ptr->Unlock();
        }
    };
    
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

    // Create listen thread.
    bool BeginThread() 
    {
        if(!IsThreadRunning()) 
        {
            _fRunFlag = true;
            _hThreadHandle = NULL;

#ifdef _MT
            unsigned int id;
            _hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, CommThreadProc, this, 0, &id);
#else
            DWORD id;
            _hThreadHandle = ::CreateThread(NULL, 0, CommThreadProc, this, 0, &id); 
#endif

            return (_hThreadHandle != NULL); 
        }
        return false;
    }

    // Suspend listen thread.
    inline bool SuspendThread()
    {
        return IsThreadRunning() ? ::SuspendThread(_hThreadHandle) != 0xFFFFFFFF : false;
    }

    // Resume listen thread.
    inline bool ResumeThread()
    {
        return IsThreadRunning() ? ::ResumeThread(_hThreadHandle) != 0xFFFFFFFF : false;
    }

    // Terminate listen thread
    bool EndThread(DWORD dwWaitTime = 100)
    {
        if(IsThreadRunning()) 
        {
            _fRunFlag = false;
            ::SetCommMask(_hCommHandle, 0);
            ::SetEvent(_WaitOverlapped.hEvent);
            if(::WaitForSingleObject(_hThreadHandle, dwWaitTime) != WAIT_OBJECT_0)
                if(!::TerminateThread(_hThreadHandle, 0))
                    return false;

            ::CloseHandle(_hThreadHandle);
            ::ResetEvent(_WaitOverlapped.hEvent);

            _hThreadHandle = NULL;

            return true;
        }
        return false;
    }

protected:

    volatile DWORD _dwPort;   // Comm port
    volatile HANDLE _hCommHandle;  // Comm port handler
    char _szCommStr[20];  // Store "\\\\.\\COM1"

    DCB _DCB;  // Baud rate, stop bit ...
    COMMTIMEOUTS _CO; // Timeout struct

    DWORD _dwIOMode;  // IO mode: 0 sync; FILE_FLAG_OVERLAPPED async
    OVERLAPPED _ReadOverlapped, _WriteOverlapped;  // Overlapped IO

    volatile HANDLE _hThreadHandle; // Listen thread handler
    volatile HWND _hNotifyWnd;  // TODO: remove it
    volatile DWORD _dwNotifyNum;  // Maximum bytes to send message.
    volatile DWORD _dwMaskEvent;  // Listened event mask.
    volatile bool _fRunFlag;  // Thead is running?
    bool _fAutoBeginThread;  // Auto start listen thread
    OVERLAPPED _WaitOverlapped;  //WaitCommEvent use

    // Init 
    virtual void Init() 
    {
        memset(_szCommStr, 0, 20);
        memset(&_DCB, 0, sizeof(_DCB));
        _DCB.DCBlength = sizeof(_DCB);
        _hCommHandle = INVALID_HANDLE_VALUE;

        memset(&_ReadOverlapped, 0, sizeof(_ReadOverlapped));
        memset(&_WriteOverlapped, 0, sizeof(_WriteOverlapped));

        _ReadOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
        assert(_ReadOverlapped.hEvent != NULL); 

        _WriteOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
        assert(_WriteOverlapped.hEvent != NULL);

        _hNotifyWnd = NULL;
        _dwNotifyNum = 0;
        _dwMaskEvent = DEFAULT_COM_MASK_EVENT;
        _hThreadHandle = NULL;

        memset(&_WaitOverlapped, 0, sizeof(_WaitOverlapped));
        _WaitOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
        assert(_WaitOverlapped.hEvent != NULL); 

        ::InitializeCriticalSection(&_Mutex);
    } 

    // Destroy resource
    virtual void Destroy()
    {
        if(_ReadOverlapped.hEvent != NULL)
            CloseHandle(_ReadOverlapped.hEvent);

        if(_WriteOverlapped.hEvent != NULL)
            CloseHandle(_WriteOverlapped.hEvent);

        if(_WaitOverlapped.hEvent != NULL)
            CloseHandle(_WaitOverlapped.hEvent);

        ::DeleteCriticalSection(&_Mutex);
    }

    // Bind Comm port
    void BindCommPort(DWORD dwPort)
    {
        assert(dwPort >= 1 && dwPort <= 1024);

        char p[5];

        _dwPort = dwPort;
        strcpy(_szCommStr, "\\\\.\\COM"); 
        ltoa(_dwPort, p, 10);
        strcat(_szCommStr, p);
    }

    // Open Comm port
    virtual bool OpenCommPort()
    {
        if(IsOpen())
            Close();

        _hCommHandle = ::CreateFile(
            _szCommStr,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | _dwIOMode, 
            NULL
            );

        if(_fAutoBeginThread)
        {
            if(IsOpen() && BeginThread())
                return true;
            else
            {
                Close();  // Creating thread fails
                return false;
            }
        }
        return IsOpen();
    }

    // Setup Comm port
    virtual bool SetupPort()
    {
        if(!IsOpen())
            return false;


        if(!::SetupComm(_hCommHandle, 4096, 4096))
            return false; 


        if(!::GetCommTimeouts(_hCommHandle, &_CO))
            return false;
        _CO.ReadIntervalTimeout = 0;
        _CO.ReadTotalTimeoutMultiplier = 1;
        _CO.ReadTotalTimeoutConstant = 1000;
        _CO.WriteTotalTimeoutMultiplier = 1;
        _CO.WriteTotalTimeoutConstant = 1000;
        if(!::SetCommTimeouts(_hCommHandle, &_CO))
            return false; 


        if(!::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ))
            return false; 

        return true;
    } 

    //---------------------------------------threads callback-----------------------------------------------------

    // TODO: replace HWND based message notification by QT signals and slots
    // Thread receive data, then call it to send message to hwnd.
    virtual void OnReceive()//EV_RXCHAR
    {
        if(::IsWindow(_hNotifyWnd))
            ::PostMessage(_hNotifyWnd, ON_COM_RECEIVE, WPARAM(_dwPort), LPARAM(0));
    }

    virtual void OnDSR()
    {
        if(::IsWindow(_hNotifyWnd))
        {
            DWORD Status;
            if(GetCommModemStatus(_hCommHandle, &Status))
                ::PostMessage(_hNotifyWnd, ON_COM_DSR, WPARAM(_dwPort), LPARAM( (Status & MS_DSR_ON) ? 1 : 0));
        }
    }

    virtual void OnCTS()
    {
        if(::IsWindow(_hNotifyWnd))
        {
            DWORD Status;
            if(GetCommModemStatus(_hCommHandle, &Status))
                ::PostMessage(_hNotifyWnd, ON_COM_CTS, WPARAM(_dwPort), LPARAM( (Status & MS_CTS_ON) ? 1 : 0));
        }
    }

    virtual void OnBreak()
    {
        if(::IsWindow(_hNotifyWnd))
        {
            ::PostMessage(_hNotifyWnd, ON_COM_BREAK , WPARAM(_dwPort), LPARAM(0));
        }
    }

    virtual void OnTXEmpty()
    {
        if(::IsWindow(_hNotifyWnd))
            ::PostMessage(_hNotifyWnd, ON_COM_TXEMPTY, WPARAM(_dwPort), LPARAM(0));
    }

    virtual void OnError()
    {
        DWORD dwError;
        ::ClearCommError(_hCommHandle, &dwError, NULL);
        if(::IsWindow(_hNotifyWnd))
            ::PostMessage(_hNotifyWnd, ON_COM_ERROR, WPARAM(_dwPort), LPARAM(dwError));
    }

    virtual void OnRing()
    {
        if(::IsWindow(_hNotifyWnd))
            ::PostMessage(_hNotifyWnd, ON_COM_RING, WPARAM(_dwPort), LPARAM(0));
    }

    virtual void OnRLSD()
    {
        if(::IsWindow(_hNotifyWnd))
            ::PostMessage(_hNotifyWnd, ON_COM_RLSD, WPARAM(_dwPort), LPARAM(0));
    }

    virtual DWORD ThreadFunc()
    {
        if(!::SetCommMask(_hCommHandle, _dwMaskEvent))
        {
            char szBuffer[256];
            _snprintf(szBuffer, 255, "%s(%d) : COM%d Call WINAPI SetCommMask(%x, %x) Fail, thread work invalid! GetLastError() = %d;",
                __FILE__, __LINE__, _dwPort, _hCommHandle, _dwMaskEvent, GetLastError());
            MessageBox(NULL, szBuffer, "Class SerialComm", MB_OK);
            return 1;
        }

        COMSTAT Stat;
        DWORD dwError;

        for(DWORD dwLength, dwMask = 0; _fRunFlag && IsOpen(); dwMask = 0)
        {
            if(!::WaitCommEvent(_hCommHandle, &dwMask, &_WaitOverlapped))
            {
                if(::GetLastError() == ERROR_IO_PENDING)// asynchronous
                    ::GetOverlappedResult(_hCommHandle, &_WaitOverlapped, &dwLength, TRUE);
                else
                    continue;
            }

            if(dwMask == 0)
                continue;

            switch( dwMask & 0x1ff )
            {
            case EV_RXCHAR :
                ::ClearCommError(_hCommHandle, &dwError, &Stat);
                if(Stat.cbInQue >= _dwNotifyNum)
                    OnReceive();
                break;

            case EV_TXEMPTY :
                OnTXEmpty();
                break;

            case EV_CTS :
                OnCTS();
                break;

            case EV_DSR :
                OnDSR();
                break;

            case EV_RING :
                OnRing();
                break;

            case EV_RLSD :
                OnRLSD();
                break;

            case EV_BREAK:
                OnBreak();
                break;

            case EV_ERR :
                OnError();
                break;

            }
        }
        return 0;
    }

private:  //the function protected 

    SerialComm(const SerialComm&);
    SerialComm &operator = (const SerialComm&);
    CRITICAL_SECTION _Mutex;

    //base function for thread
#ifdef _MT
    static UINT APIENTRY CommThreadProc(LPVOID lpPara)
#else
    static DWORD WINAPI CommThreadProc(LPVOID lpPara)
#endif
    {
        return ( (SerialComm *)lpPara )->ThreadFunc();
    }
};

#endif // SERIALCOMM_H
