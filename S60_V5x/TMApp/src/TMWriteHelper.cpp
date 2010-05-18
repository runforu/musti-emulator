/*
 ========================================================================
 Name        : TMConnFactory.cpp
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

#include <centralrepository.h>

#include "TMWriteHelper.h"
#include "TMFileWriter.h"
#include "TMBtWriter.h"
#include "TMUsbwriter.h"
#include "TmCenRepKeys.h"
#include "TraceManagerSettings.h"
#include "TMLog.h"

_LIT( KConnectionError, "Connection fails.");

CTMWriteHelper* CTMWriteHelper::NewL( MTMWriterStateObserver & aObserver )
    {
    CTMWriteHelper* self = CTMWriteHelper::NewLC( aObserver );
    CleanupStack::Pop(); // self;
    return self;
    }

CTMWriteHelper* CTMWriteHelper::NewLC( MTMWriterStateObserver & aObserver )
    {
    CTMWriteHelper* self = new ( ELeave ) CTMWriteHelper( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

TBool CTMWriteHelper::IsConnected()
    {
    return iActiveWriter && iActiveWriter->IsConnected();
    }

TBool CTMWriteHelper::IsCreatingConnection()
    {
    return iCreatingConnection;
    }

CTMWriteHelper::~CTMWriteHelper()
    {
    delete iActiveWriter;
    iActiveWriter = NULL;
    }

void CTMWriteHelper::ConnectL()
    {
    if( IsConnected() )
        {
        return;
        }

    CRepository* repository = CRepository::NewLC( KCRUidTmSettings );
    iCreatingConnection = ETrue;
    TInt setting = 0;
    TInt rt = repository->Get( KTmConnectionType, setting );
    CleanupStack::PopAndDestroy(); //repository

    if( rt != KErrNone || setting == TTraceManagerSettings::EUSBConn )
        {
        iActiveWriter = CUsbWriter::NewL( *this );
        }
    else if( setting == TTraceManagerSettings::EBlueTooth )
        {
        iActiveWriter = CTMBtWriter::NewL( *this );
        }
    else if( setting == TTraceManagerSettings::ELogToFile )
        {
        iActiveWriter = CTMFileWriter::NewL( *this );
        }

    if( iActiveWriter )
        {
        TRAPD( err,iActiveWriter->ConnectL() );
        if( err != KErrNone )
            {
            delete iActiveWriter;
            iActiveWriter = NULL;
            iCreatingConnection = EFalse;  
            iObserver.NewWriterReport( KConnectionError );
            }
        }
    else
        {
        iObserver.NewWriterReport( KConnectionError );
        }
    }

void CTMWriteHelper::DisconnectL()
    {
    delete iActiveWriter;
    iActiveWriter = NULL;
    }

void CTMWriteHelper::WriteTrace( const TDesC8 &aTrace )
    {
    if( iActiveWriter )
        {
        iActiveWriter->Write( aTrace );
        }
    }

void CTMWriteHelper::HandleConnectedL()
    {
    iCreatingConnection = EFalse;
    }

void CTMWriteHelper::HandleDisconnectedL()
    {
    iCreatingConnection = EFalse;
    }

void CTMWriteHelper::HandleWriteCompleteL( TInt  )
    {
    //TODO: Request next writing if queue is not empty.
    // CompleteSelf();
    }

void CTMWriteHelper::StateReport( const TDesC &aReport )
    {
    iObserver.NewWriterReport( aReport );
    }

void CTMWriteHelper::CompleteSelf()
    {
    //TODO: Request next writing.
    }

CTMWriteHelper::CTMWriteHelper( MTMWriterStateObserver & aObserver ) 
        :CActive( EPriorityHigh ), iObserver( aObserver )
    {
    }

void CTMWriteHelper::ConstructL()
    {        
    }

void CTMWriteHelper::RunL()
    {
    //TODO: Perform next writing if queue is not empty.
    }

void CTMWriteHelper::DoCancel()
    {
    }

