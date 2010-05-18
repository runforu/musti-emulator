/*
========================================================================
 Name        : TMBtWriter.cpp
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

#include <f32file.h>
#include <s32file.h>
#include <bt_sock.h>
#include "TMBtWriter.h"
#include "TMServiceSearcher.h"
#include "TmLog.h"

_LIT( KBtWriterDes, "Logging over BT.\n");
_LIT( KNewBtWriterCreated, "BT writer created for logging.\n");
_LIT( KBtWriterDeviceFound, "Finding BT host.\n");
_LIT( KBtWriterServiceFound, "Finding service of BT host.\n");
_LIT( KBtWriterConnectingError, "Error found \n  when connecting to BT host.\n");
_LIT( KBtWriterConnected, "BT connection created, \n  ready for writing!\n");
_LIT( KBtWriterConnectedError, "Error found \n  when BT connected !\n");
_LIT( KBtWriterDisconnectingError, "Error found \n  when disconnecting to BT host.\n");
_LIT( KBtWriterDisconnected, "Logging over BT is terminated!\n");
_LIT( KBtWriterError, "Logging over BT has errors.\n");



CTMBtWriter::CTMBtWriter( MTmWriterObserver& aObserver ) 
        : CActive( CActive::EPriorityHigh ), iObserver( aObserver )
    {
    SetState( ENoAvailableDevice );
    CActiveScheduler::Add( this );
    }

CTMBtWriter::~CTMBtWriter()
    {
    if(IsActive())
        {
        Cancel();
        }
    iSocket.Close();
    iSocketServer.Close();
    delete iServiceSearcher;
    iServiceSearcher = NULL;
    }

CTMBtWriter* CTMBtWriter::NewLC( MTmWriterObserver& aObserver )
    {
    CTMBtWriter* self = new ( ELeave ) CTMBtWriter( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CTMBtWriter* CTMBtWriter::NewL( MTmWriterObserver& aObserver )
    {
    CTMBtWriter* self = CTMBtWriter::NewLC( aObserver );
    CleanupStack::Pop(); // self;
    return self;
    }

void CTMBtWriter::ConstructL()
    {
    iServiceSearcher = CTmServiceSearcher::NewL();
    User::LeaveIfError( iSocketServer.Connect() );
	iObserver.StateReport( KNewBtWriterCreated );
    }

void CTMBtWriter::Write( const TDesC8& aData )
    { 
    // Sync write to avoid capture losing.
    if (State() != EConnected)
        {
        return;
        }

    TRequestStatus status = KRequestPending;
    iSocket.Write(aData, status );
    User::WaitForRequest( status ); 
    iObserver.HandleWriteCompleteL( status.Int() );
    }

void CTMBtWriter::DoCancel()
    {
    }

void CTMBtWriter::HandleError()
    {
    switch( State() )
        {
        case EFindingDevice:  
        case EFindingService:   
        case ECreatingConnection:   
            iObserver.StateReport( KBtWriterConnectingError );   
            iObserver.HandleDisconnectedL();
            SetState( ENoAvailableDevice );
            break;

        case EConnected:   
            iObserver.StateReport( KBtWriterConnectedError );
            DisconnectFromServerL();
            break;

        case EDisconnecting:             
            iObserver.HandleDisconnectedL();
            iObserver.StateReport( KBtWriterDisconnectingError );
            iSocket.Close();
            SetState( ENoAvailableDevice );
            break;

        default:
            break;
        }
    }

void CTMBtWriter::HandleComplete()
    {
    switch( State() )
        {
        case EFindingDevice: 
            iObserver.StateReport( KBtWriterDeviceFound );
            SetState( EFindingService );
            iStatus = KRequestPending; 
            iServiceSearcher->FindServiceL( iStatus );
            SetActive();
            break;

        case EFindingService:
            iObserver.StateReport( KBtWriterServiceFound );
            SetState( ECreatingConnection );
            ConnectToServerL();
            break;

        case ECreatingConnection:
            iObserver.StateReport( KBtWriterConnected );
            iObserver.HandleConnectedL();
            SetState( EConnected );
            break;

        case EDisconnecting:
            iObserver.HandleDisconnectedL();
            iObserver.StateReport( KBtWriterDisconnected );
            iSocket.Close();
            SetState( ENoAvailableDevice );
            break;
            
        default:
            break;
        };
    }
	
TUid CTMBtWriter::Type()
    {
    return TUid::Uid( 0x01 );
    }

const TDesC & CTMBtWriter::Descriptor()
    {
    return KBtWriterDes;
    }	
	
void CTMBtWriter::RunL()
    {
    if( iStatus != KErrNone )
        {
        HandleError();
        }
    else
        {
        HandleComplete();
        }
    }

TInt CTMBtWriter::RunError( TInt aError )
    {
	iObserver.StateReport( KBtWriterError );
    return aError;
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::SetState()
// 
// ----------------------------------------------------------------------------
//
void CTMBtWriter::SetState( TBtWriterState aState )
    {
    iState = aState;
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::State()
// 
// ----------------------------------------------------------------------------
//    
TInt CTMBtWriter::State()
    {
    return iState;
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CTMBtWriter::ConnectL()
    {
    if( State() == ENoAvailableDevice && !IsActive() )
        {
        SetState( EFindingDevice );
        iServiceSearcher->SelectDeviceByDiscoveryL( iStatus );
        SetActive();
        }
    else
        {
        User::Leave( KErrInUse );
        }
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CTMBtWriter::DisconnectL()
    {
    if( ( State() == EConnected ) )
        {
        DisconnectFromServerL();
        SetState( EDisconnecting );
        }
    else
        {
        User::Leave( KErrDisconnected );
        }
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
TBool CTMBtWriter::IsConnected()
    {
    return State() == EConnected;
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::DisconnectFromServerL()
// Disconnects from the service
// ----------------------------------------------------------------------------
//
void CTMBtWriter::DisconnectFromServerL()
    {
    // Terminate all operations
    iSocket.CancelAll();
    Cancel();
    iStatus = KRequestPending;
    iSocket.Shutdown( RSocket::ENormal, iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CTMBtWriter::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CTMBtWriter::ConnectToServerL()
    {
    User::LeaveIfError( iSocket.Open( iSocketServer, KRFCOMMDesC ) );

    TBTSockAddr address;
    address.SetBTAddr( iServiceSearcher->BTDevAddr() );
    address.SetPort( iServiceSearcher->Port() );
    iStatus = KRequestPending;
    iSocket.Connect( address, iStatus );

#ifdef __WINS__
    // Fix to allow emulator client to connect to server
    User::After( 1 );
#endif

    SetActive();
    }

