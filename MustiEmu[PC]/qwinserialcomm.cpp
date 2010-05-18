/****************************************************************************
**
** Copyright (C) 2009 Du Hui.
**
****************************************************************************/


#include "qwinserialcomm.h"

bool  WinSerialComm::SetPortSettings(const PORTSETTINGS* portSettings)
{
    if ( !IsOpen() )
    {
        return false;
    }

    if ( portSettings == NULL )
    {
        BuildDefaultSettings();
    }
    else
    {
        if ( portSettings->SettingsValidMask & BUFFERSIZE_VALID_MASK )
        {
            _InBufferSize = portSettings->BufferSize.InputBufferSize;
            _OutBufferSeze = portSettings->BufferSize.OutputBufferSize;
        }
        if ( portSettings->SettingsValidMask & TIMEOUTS_VALID_MASK )
        {
            _CO.ReadIntervalTimeout = portSettings->Timeout.ReadIntervalTimeout;
            _CO.ReadTotalTimeoutMultiplier = portSettings->Timeout.ReadTotalTimeoutMultiplier;
            _CO.ReadTotalTimeoutConstant = portSettings->Timeout.ReadTotalTimeoutConstant;
            _CO.WriteTotalTimeoutMultiplier = portSettings->Timeout.WriteTotalTimeoutMultiplier;
            _CO.WriteTotalTimeoutConstant = portSettings->Timeout.WriteTotalTimeoutConstant;
        }          
        if ( portSettings->SettingsValidMask &PROPERTY_VALID_MASK )
        {
            BuildDcbFrom(portSettings->Properties);
        }
    }

    return ::SetCommState(_hCommHandle, &_DCB) 
        && ::SetCommTimeouts(_hCommHandle, &_CO)
        && ::SetupComm(_hCommHandle, _InBufferSize, _OutBufferSeze);
}

bool  WinSerialComm::GetPortSettings(PORTSETTINGS &portSettings)
{
    if ( !IsOpen() )
    {
        return false;
    }

    portSettings.SettingsValidMask = BUFFERSIZE_VALID_MASK;
    portSettings.BufferSize.InputBufferSize = _InBufferSize;
    portSettings.BufferSize.OutputBufferSize =_OutBufferSeze;

    bool timeoutValid = ::GetCommTimeouts(_hCommHandle, &_CO);
    if ( timeoutValid )
    {
        portSettings.SettingsValidMask |= TIMEOUTS_VALID_MASK;
        portSettings.Timeout.ReadIntervalTimeout = _CO.ReadIntervalTimeout;
        portSettings.Timeout.ReadTotalTimeoutMultiplier = _CO.ReadTotalTimeoutMultiplier;
        portSettings.Timeout.ReadTotalTimeoutConstant = _CO.ReadTotalTimeoutConstant;
        portSettings.Timeout.WriteTotalTimeoutMultiplier = _CO.WriteTotalTimeoutMultiplier;
        portSettings.Timeout.WriteTotalTimeoutConstant = _CO.WriteTotalTimeoutConstant;  
    }  

    bool stateValid =  ::GetCommState(_hCommHandle, &_DCB);
    if ( stateValid )
    {
        portSettings.SettingsValidMask |= PROPERTY_VALID_MASK;
        portSettings.Properties.BaudRate = _DCB.BaudRate;
        portSettings.Properties.DataBits = _DCB.ByteSize;

        if ( _DCB.fOutxCtsFlow == TRUE)
        {
            portSettings.Properties.FlowControl = FLOW_TYPE_HARDWARE;
        }
        else
        {
            portSettings.Properties.FlowControl = ( _DCB.fInX == TRUE ) ? FLOW_TYPE_XONXOFF : FLOW_TYPE_OFF;
        }


        portSettings.Properties.Parity = _DCB.Parity;
        portSettings.Properties.StopBits = _DCB.StopBits;
    }  

    return true;
}


bool WinSerialComm::open ( const wchar_t * comport,
           const PORTSETTINGS* portSettings,
           QIODevice::OpenMode mode )
{
    wcscpy( _szComDeviceName, comport );

    if( !OpenCommPort() )
        return false;

    if (!SetPortSettings(portSettings))
    {
        return false;
    }

    if(!::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ))
        return false; 

    _SettingAvailable = true;
    return true;
}

bool WinSerialComm::reopen ()
{
    if (!_SettingAvailable)
    {
        return false;
    }

    if( !OpenCommPort() )
        return false;   

    if ( !::SetCommState(_hCommHandle, &_DCB) 
        || !::SetCommTimeouts(_hCommHandle, &_CO)
        || !::SetupComm(_hCommHandle, _InBufferSize, _OutBufferSeze) )
    {
        return false;
    }

    if(!::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ))
        return false; 

    return true;
}

// Sync Read Comm port
DWORD WinSerialComm::ReadSync(LPVOID Buffer, DWORD dwBufferLength)
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
DWORD WinSerialComm::WriteSync(const void* Buffer, DWORD dwBufferLength)
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


// Close Comm port, and related thread
void WinSerialComm::Close()
{
    if(IsOpen())  
    {
        PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR); 

        ::CloseHandle(_hCommHandle);

        _hCommHandle = INVALID_HANDLE_VALUE;
    }
}

// Not support in nonoverlapped IO
bool WinSerialComm::WaitForBytesWritten ( int msecs )
{
    return true;
}

// Not support in nonoverlapped IO
bool WinSerialComm::WaitForReadyRead ( int msecs )
 {
     return true;
 }

// Init 
void WinSerialComm::Init()
{
    memset(_szComDeviceName, 0, sizeof(_szComDeviceName) );
    BuildDefaultDcb();
    _hCommHandle = INVALID_HANDLE_VALUE;
} 

// Destroy resource
void WinSerialComm::Destroy()
{
}

// Open Comm port
bool WinSerialComm::OpenCommPort()
{
    if(IsOpen())
        Close();

    _hCommHandle = ::CreateFile(
        _szComDeviceName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 
        NULL
        );

    return IsOpen();
}


void WinSerialComm::BuildDefaultDcb()
{
    memset( &_DCB, 0, sizeof(_DCB) );
    _DCB.DCBlength = sizeof(_DCB);	
    _DCB.BaudRate = CBR_115200;
    _DCB.fBinary = TRUE;
    _DCB.Parity = NOPARITY;
    _DCB.StopBits = ONESTOPBIT;
    _DCB.ByteSize = 8;
    _DCB.fOutxCtsFlow=FALSE;
    _DCB.fRtsControl=RTS_CONTROL_DISABLE;
    _DCB.fInX=FALSE;
    _DCB.fOutX=FALSE;
}

void WinSerialComm::BuildDcbFrom(const PORTPROPERTY & property)
{
    // Baud rate
    switch (property.BaudRate) 
    {
        // Default and windows not support baud rate set to 115200
    case BAUD_RATE_TYPE_50:
    case BAUD_RATE_TYPE_75:
    case BAUD_RATE_TYPE_134:
    case BAUD_RATE_TYPE_150:
    case BAUD_RATE_TYPE_200:
    case BAUD_RATE_TYPE_1800:
    case BAUD_RATE_TYPE_76800:
    default:
    case BAUD_RATE_TYPE_115200:
        _DCB.BaudRate=CBR_115200;
        break;

    case BAUD_RATE_TYPE_110:
        _DCB.BaudRate=CBR_110;
        break;

    case BAUD_RATE_TYPE_300:
        _DCB.BaudRate=CBR_300;
        break;

    case BAUD_RATE_TYPE_600:
        _DCB.BaudRate=CBR_600;
        break;

    case BAUD_RATE_TYPE_1200:
        _DCB.BaudRate=CBR_1200;
        break;

    case BAUD_RATE_TYPE_2400:
        _DCB.BaudRate=CBR_2400;
        break;

    case BAUD_RATE_TYPE_4800:
        _DCB.BaudRate=CBR_4800;
        break;

    case BAUD_RATE_TYPE_9600:
        _DCB.BaudRate=CBR_9600;
        break;

    case BAUD_RATE_TYPE_14400:
        _DCB.BaudRate=CBR_14400;
        break;

    case BAUD_RATE_TYPE_19200:
        _DCB.BaudRate=CBR_19200;
        break;

    case BAUD_RATE_TYPE_38400:
        _DCB.BaudRate=CBR_38400;
        break;

    case BAUD_RATE_TYPE_56000:
        _DCB.BaudRate=CBR_56000;
        break;

    case BAUD_RATE_TYPE_57600:
        _DCB.BaudRate=CBR_57600;
        break;

    case BAUD_RATE_TYPE_128000:
        _DCB.BaudRate=CBR_128000;
        break;

    case BAUD_RATE_TYPE_256000:
        _DCB.BaudRate=CBR_256000;
        break;
    }

    // data bits and stop bits
    if ((property.StopBits==STOP_BITS_TYPE_2 && property.DataBits==DATA_BITS_TYPE_5) 
        || (property.StopBits==STOP_BITS_TYPE_1_5 && property.DataBits!=DATA_BITS_TYPE_5)
        || property.DataBits!=DATA_BITS_TYPE_4 )
    {
        _DCB.ByteSize = 8;
        _DCB.StopBits = 1;
    }
    else
    {
        _DCB.ByteSize = property.DataBits;
        _DCB.StopBits = property.StopBits;
    }

    switch(property.FlowControl) 
    {
        /*no flow control*/
    case FLOW_TYPE_OFF:
        _DCB.fOutxCtsFlow=FALSE;
        _DCB.fRtsControl=RTS_CONTROL_DISABLE;
        _DCB.fInX=FALSE;
        _DCB.fOutX=FALSE;
        break;

        /*software (XON/XOFF) flow control*/
    case FLOW_TYPE_XONXOFF:
        _DCB.fOutxCtsFlow=FALSE;
        _DCB.fRtsControl=RTS_CONTROL_DISABLE;
        _DCB.fInX=TRUE;
        _DCB.fOutX=TRUE;
        break;

    case FLOW_TYPE_HARDWARE:
        _DCB.fOutxCtsFlow=TRUE;
        _DCB.fRtsControl=RTS_CONTROL_HANDSHAKE;
        _DCB.fInX=FALSE;
        _DCB.fOutX=FALSE;
        break;
    }

    _DCB.Parity=(unsigned char)property.Parity;
    switch ( property.Parity ) 
    {
        //even parity, fall through
    case PARITY_TYPE_EVEN:        
        //odd parity, fall through
    case PARITY_TYPE_ODD:   
        //space parity, fall through
    case PARITY_TYPE_SPACE:
        //mark parity - WINDOWS ONLY, fall through
    case PARITY_TYPE_MARK:
        _DCB.fParity=TRUE;
        break;

        //no parity, fall through
    case PARITY_TYPE_NONE:
    default:
        _DCB.fParity=FALSE;
        break;
    }
}
