/*
========================================================================
 Name        : CTraceClient.cpp
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
//#include <BAUTILS.H>

#include "TraceClient.h"

_LIT8( KNewLine, "\n" );
const TInt KMaxTraceLen = 256;

CTraceClient::CTraceClient(MTraceObserver & aOb) :
    CActive( EPriorityStandard ),
    iObserver(aOb)
    {
    }

CTraceClient* CTraceClient::NewLC(MTraceObserver & aOb)
    {
    CTraceClient* self = new ( ELeave ) CTraceClient(aOb);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CTraceClient* CTraceClient::NewL(MTraceObserver & aOb)
    {
    CTraceClient* self = CTraceClient::NewLC(aOb);
    CleanupStack::Pop(); // self;
    return self;
    }

void CTraceClient::ConstructL()
    {
    User::LoadLogicalDevice( _L( "TRACEREDIRECTION" ) );
    iTraceRedirect.Open();
    CActiveScheduler::Add( this ); // Add to scheduler
    }

CTraceClient::~CTraceClient()
    {
    Cancel(); // Cancel any request, if outstanding
    iTraceRedirect.Close();
    }

void CTraceClient::DoCancel()
    {
    iTraceRedirect.CancelRead();
    }

void CTraceClient::StartL()
    {
    Cancel();
    iStatus = KRequestPending;
    iTraceBuf.Zero();
    iTraceRedirect.Read( iStatus, iTraceBuf, KMaxTraceLen );
    SetActive();
    iNeedToStop = EFalse;   
    }

void CTraceClient::StopL()
    {
    Cancel();
    iNeedToStop = ETrue;
    }

void CTraceClient::RunL()
    {
    if( iTraceBuf.Length() && iStatus.Int() == KErrNone )
        {
        if( iTraceBuf[iTraceBuf.Length()-1] != '\n' )
            iTraceBuf.Append(KNewLine);
        
        iObserver.NewTraceReady(iTraceBuf);
        }
    if( !iNeedToStop )
        {   
        iStatus = KRequestPending;
        iTraceBuf.Zero();
        iTraceRedirect.Read( iStatus, iTraceBuf, KMaxTraceLen );
        SetActive(); 
        }
    }

TInt CTraceClient::RunError( TInt aError )
    {
    return aError;
    }
