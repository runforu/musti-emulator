/*
========================================================================
 Name        : TMUsbwriter.cpp
 Author      : DH
 Copyright   : All right is reserved!
 Version     : 
 E-Mail      : dh.come@gmail.com 
 Description : 
 
 Copyright (c) 2009-2015 DH.
 This material, including documentation and any related 
 computer programs, is protected by copyright controlled BY Du Hui(DH)
========================================================================
*/

#include "TMUsbwriter.h"

#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include "TMLog.h"

_LIT(CSYMOD, "ECACM");
    

_LIT( KUsbWriterDes, "Logging over USB.\n");
_LIT( KNewUsbWriterCreated, "New USB created for logging.\n");
_LIT( KUsbWriterConnected, "USB connection is created, \n  logging over USB is ready!\n");

CUsbWriter::CUsbWriter( MTmWriterObserver& aObserver )
		: iObserver( aObserver )
    {
    }

CUsbWriter::~CUsbWriter()
    {    
    if( iIsConnected )
        {
        DisconnectL();
        }
    iServer.UnloadCommModule( CSYMOD );
    iServer.Close();
    }

CUsbWriter* CUsbWriter::NewLC( MTmWriterObserver& aObserver )
    {
    TM_LOG_FN_GUARD( "CUsbWriter::NewLC" );
    CUsbWriter* self = new ( ELeave ) CUsbWriter( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CUsbWriter* CUsbWriter::NewL( MTmWriterObserver& aObserver )
    {
    TM_LOG_FN_GUARD( "CUsbWriter::NewL" );
    CUsbWriter* self = CUsbWriter::NewLC( aObserver );
    CleanupStack::Pop(); // self;
    return self;
    }


void CUsbWriter::ConstructL()
    {
    TM_LOG_FN_GUARD( "CUsbWriter::ConstructL" );
    // RComm is a client to the RCommServ Comms iServer
    // Start this service before any connections are made.
    TInt ret = StartC32();
    if( ret != KErrNone && ret != KErrAlreadyExists )
        {
        User::Leave( ret );
        }
    // Connect to CommServer
    User::LeaveIfError( iServer.Connect() );
    // Load CSY Module
    User::LeaveIfError( iServer.LoadCommModule( CSYMOD ) );
	iObserver.StateReport( KNewUsbWriterCreated );
    }

void CUsbWriter::ConnectL()
    {
    TM_LOG_FN_GUARD( "CUsbWriter::ConnectL" );
    _LIT(KACMPort, "ACM::1");

    TM_LOG_1( "Open Com port." );
    //User::LeaveIfError( iCommPort.Open( iServer, KACMPort, ECommShared ) );
    TInt x = iCommPort.Open( iServer, KACMPort, ECommShared  ); 
    TM_LOG_2( "Open Com port ACM::1 %d.", x );
    
    // Verify capabilities of the port and configure it
    TCommCaps portCaps;
    iCommPort.Caps( portCaps );
    if( ( ( portCaps().iRate & KCapsBps115200 ) == 0 )
            | ( ( portCaps().iDataBits & KCapsData8 ) == 0 )
            | ( ( portCaps().iStopBits & KCapsStop1 ) == 0 )
            | ( ( portCaps().iParity & KCapsParityNone ) == 0 ) )
        {
        TM_LOG_1( "Com port not support capability." );
        User::Leave( KErrNotSupported );
        }
        
    TCommConfig portCfg;
    TM_LOG_1( "Get com port config." );   
    iCommPort.Config( portCfg );
    portCfg().iRate = EBps115200;
    portCfg().iParity = EParityNone;
    portCfg().iDataBits = EData8;
    portCfg().iStopBits = EStop1;
    portCfg().iHandshake = 0;
    
    TM_LOG_1( "Config Com port." );
    User::LeaveIfError( iCommPort.SetConfig( portCfg ) );    
    iCommPort.SetSignals (KSignalDTR, 0);
    iCommPort.SetSignals (KSignalRTS, 0);
    iIsConnected = ETrue;
    iObserver.HandleConnectedL();
    iObserver.StateReport( KUsbWriterConnected );
    }

void CUsbWriter::DisconnectL()
    {
    iCommPort.Close();
    iIsConnected = EFalse;
    }

void CUsbWriter::Write( const TDesC8 &aDes )
    {
    // Sync write to avoid capture losing.
    TRequestStatus status = KRequestPending;
    iCommPort.Write( status, aDes);
    User::WaitForRequest( status );
    iObserver.HandleWriteCompleteL( status.Int() );
    }
	
TUid CUsbWriter::Type()
    {
    return TUid::Uid( 0x00 );
    }

const TDesC & CUsbWriter::Descriptor()
    {
    return KUsbWriterDes;
    }
	
TBool CUsbWriter::IsConnected()
    {
    return iIsConnected;
    }











