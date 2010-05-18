/*
 ========================================================================
 Name        : d_traceredirect.cpp
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
#include <kernel.h>
#include <kern_priv.h>
#include "d_traceredirect.h"
_LIT(KLddName,"TRACEREDIRECTION");

const TInt KMajorVersionNumber = 0;
const TInt KMinorVersionNumber = 1;
const TInt KBuildVersionNumber = 1;

const TInt KMaxTraceLength = 255;
const TInt KTraceBufferSize = 256;
const TInt KTraceBufferSizeMask = 0xff;
const TInt KWaitTime = 0;

// "NTimer::OneShot(TInt aTime, TDfc& aDfc);" is supported since S60 V5.0
// So we just use NTimer::OneShot(TInt aTime);
#define DFC_CALLBACK 

class DTraceRedirect;

class TTraceEvent
    {
public:
    TInt iLength;
    TUint8 iText[KMaxTraceLength];
    };

class DTraceEventHandler : public DKernelEventHandler
    {
public:
    DTraceEventHandler() :
        DKernelEventHandler( HandleEvent, this )
        {
        }

    TInt Create( DLogicalDevice* aDevice );

    ~DTraceEventHandler();

    TInt GetNextTrace( DThread* aClientThread, TDes8* aClientDes,
            TInt aMaxLength );

private:
    static TUint HandleEvent( TKernelEvent aEvent, TAny* a1, TAny* a2,
            TAny* aThis );

    void HandleUserTrace( TText* aStr, TInt aLen );

public:
    DTraceRedirect *iChannel;
    TTraceEvent *iTraceBuffer;
    TInt iFront;
    TInt iBack;
    DMutex* iLock; // serialise calls to handler
    DLogicalDevice* iDevice; // open reference to LDD for avoiding lifetime issues
    };

TInt DTraceEventHandler::Create( DLogicalDevice* aDevice )
    {
    TInt r;
    r = aDevice->Open();
    if( r != KErrNone )
        return r;
    iDevice = aDevice;
    iFront = 0;
    iBack = 0;
    iTraceBuffer = new TTraceEvent[KTraceBufferSize];
    if( iTraceBuffer == NULL )
        return KErrNoMemory;
    _LIT(KDataMutexName, "EventHandlerMutex");
    r = Kern::MutexCreate( iLock, KDataMutexName, KMutexOrdDebug );
    if( r != KErrNone )
        return r;
    return Add();
    }

DTraceEventHandler::~DTraceEventHandler()
    {
    if( iLock )
        iLock->Close( NULL);
    delete[] iTraceBuffer;
    if( iDevice )
        iDevice->Close( NULL);
    }

TUint DTraceEventHandler::HandleEvent( TKernelEvent aEvent, TAny* a1,
        TAny* a2, TAny* aThis )
    {
    if( aEvent == EEventUserTrace )
        {
        ( (DTraceEventHandler*) aThis )->HandleUserTrace( (TText*) a1,
                (TInt) a2 );
        return ETraceHandled;
        }
    return ERunNext;
    }

// called in CS
void DTraceEventHandler::HandleUserTrace( TText* aStr, TInt aLen )
    {
    Kern::MutexWait( *iLock );
    TInt frontplus1 = ( iFront + 1 ) & KTraceBufferSizeMask;
    if( frontplus1 != iBack ) // check overflow
        {
        TTraceEvent *e = &iTraceBuffer[iFront];
        TInt len = aLen;
        if( len > KMaxTraceLength )
            {
            len = KMaxTraceLength;
            }
        XTRAPD(r, XT_DEFAULT, kumemget(e->iText, aStr, len));
        if( r == KErrNone )
            e->iLength = len;
        else
            e->iLength = -1; // an error will be reported in GetNextTrace()
        iFront = frontplus1;
        }
    Kern::MutexSignal( *iLock );
    }

TInt DTraceEventHandler::GetNextTrace( DThread*aClientThead,
        TDes8 *aClientDes, TInt aMaxLength )
    {
    if( iBack == iFront )
        { // buffer empty
        return KErrNotReady;
        }

    TTraceEvent *e = &iTraceBuffer[iBack];

    if( e->iLength == -1 )
        {
        return KErrCorrupt;
        }
    else
        {
        TInt len = aMaxLength;
        if( e->iLength < aMaxLength )
            len = e->iLength;
        TPtr8 ptr( e->iText, len, aMaxLength );

        Kern::ThreadDesWrite( aClientThead, (TAny*) aClientDes, ptr, 0 );
        iBack = ( iBack + 1 ) & KTraceBufferSizeMask;
        }

    return KErrNone;
    }

class DTraceRedirect : public DLogicalChannelBase
    {
public:
    virtual ~DTraceRedirect();
    DTraceRedirect();

#ifdef DFC_CALLBACK    
    static void RxTimerDfc( TAny* aPtr );
#else
    static void RxTimerCallBack(TAny* aPtr);
#endif

protected:
    virtual TInt DoCreate( TInt aUnit, const TDesC8* anInfo,
            const TVersion& aVer );

    virtual TInt Request( TInt aFunction, TAny* a1, TAny* a2 );

private:

    TInt DoRequest( TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2 );
    TInt DoCancel( TInt aReqMask );
    TInt DoControl( TInt aFunction, TAny* a1, TAny* a2 );
    TInt DoRead();
public:
    DTraceEventHandler *iEventHandler;
    TRequestStatus* iClientRequestStatus;
    DThread* iClientThread;
    NTimer iRxTimer;
#ifdef DFC_CALLBACK   
    TDfc iRxCompleteDfc;
#endif
    TDes8*iClientDes;
    TInt iClientDesMaxLength;
    };

TInt DTraceRedirect::DoCreate( TInt /*aUnit*/, const TDesC8* /*aInfo*/,
        const TVersion& aVer )
/**
 Create channel
 */
    {
    if( !Kern::QueryVersionSupported( TVersion( KMajorVersionNumber,
            KMinorVersionNumber, KBuildVersionNumber ), aVer ) )
        return KErrNotSupported;

#ifdef DFC_CALLBACK     
    iRxCompleteDfc.SetDfcQ( Kern::TimerDfcQ() );
#endif

    iEventHandler = new DTraceEventHandler;
    if( iEventHandler == NULL )
        return KErrNoMemory;
    return iEventHandler->Create( iDevice );
    }

DTraceRedirect::DTraceRedirect() :
#ifdef DFC_CALLBACK
            iRxCompleteDfc( DTraceRedirect::RxTimerDfc, this, 2 ), 
            iRxTimer( NULL, this )
#else
            iRxTimer(DTraceRedirect::RxTimerCallBack, this)
#endif    
    {
    iClientThread = &Kern::CurrentThread();
    ( (DObject*) iClientThread )->Open();
    }

DTraceRedirect::~DTraceRedirect()
    {
    iRxTimer.Cancel();
    iEventHandler->Close();
    Kern::SafeClose( (DObject*&) iClientThread, NULL);
    }

#ifndef DFC_CALLBACK

void DTraceRedirect::RxTimerCallBack(TAny* aPtr)
    {
    DTraceRedirect *pChannel = (DTraceRedirect*)aPtr;
    pChannel->DoRead();
    }

#else

void DTraceRedirect::RxTimerDfc( TAny* aPtr )
    {
    DTraceRedirect *pChannel = (DTraceRedirect*) aPtr;
    pChannel->DoRead();
    }

#endif

TInt DTraceRedirect::Request( TInt aFunction, TAny* a1, TAny* a2 )
    {
    TInt r = KErrNone;
    if( aFunction == KMaxTInt )
        {
        // DoCancel
        r = DoCancel( TInt( a1 ) );
        }
    if( aFunction < 0 )
        {
        // DoRequest
        TAny* arg[2];
        kumemget32( arg, a2, 2 * sizeof(TAny*) );
        r = DoRequest( ~aFunction, (TRequestStatus*) ( a1 ), arg[0], arg[1] );
        }
    else
        {
        // DoControl
        r = DoControl( aFunction, a1, a2 );
        }

    return r;
    }

TInt DTraceRedirect::DoCancel( TInt aReqMask )
    {
    if( !iClientRequestStatus )
        {
        return KErrNone;
        }

#ifdef DFC_CALLBACK
    iRxCompleteDfc.Cancel();
#endif   

    iRxTimer.Cancel();
    Kern::RequestComplete( iClientThread, iClientRequestStatus, KErrCancel );
    iClientRequestStatus = 0;
    return KErrNone;
    }

TInt DTraceRedirect::DoRequest( TInt aReqNo, TRequestStatus* aStatus,
        TAny* a1, TAny* a2 )
    {
    TInt r = KErrNone;
    if( iClientRequestStatus )
        {
        Kern::ThreadKill( iClientThread, EExitPanic, ERequestAlreadyPending,
                KLitKernExec );
        return ( KErrNotSupported );
        }
    switch( aReqNo )
        {
        case RTraceRedirect::ERequestReadTrace:
            iClientRequestStatus = aStatus;
            iClientDes = (TDes8*) a1;
            iClientDesMaxLength = (TInt) a2;
            if( iClientDes == NULL || a2 == 0 )
                {
                Kern::RequestComplete( iClientThread, aStatus, KErrArgument );
                iClientDes = 0;
                iClientDesMaxLength = 0;
                }
            else
                {
                r = DoRead();
                }
            break;

        default:
            Kern::RequestComplete( iClientThread, aStatus, KErrCancel );
            r = KErrCancel;
        }
    return r;
    }

TInt DTraceRedirect::DoControl( TInt aFunction, TAny* a1, TAny* a2 )
    {
    switch( aFunction )
        {
        }
    return KErrNone;
    }

TInt DTraceRedirect::DoRead()
    {
    TInt r = iEventHandler->GetNextTrace( iClientThread, iClientDes,
            iClientDesMaxLength );
    if( r != KErrNotReady )
        {
        Kern::RequestComplete( iClientThread, iClientRequestStatus, r );
        iClientRequestStatus = 0;
        iClientDes = 0;
        }
    else
        {
        // wait it     
#ifdef DFC_CALLBACK
        iRxTimer.OneShot( NKern::TimerTicks( KWaitTime ), iRxCompleteDfc );
#else
        iRxTimer.OneShot( NKern::TimerTicks( KWaitTime ), ETrue );
#endif
        }
    return r;
    }

class DTraceRedirectFactory : public DLogicalDevice
    {
public:
    DTraceRedirectFactory();

    virtual TInt Install();

    virtual void GetCaps( TDes8& aDes ) const;

    virtual TInt Create( DLogicalChannelBase*& aChannel );
    };

//
// DTraceRedirectFactory
//

DTraceRedirectFactory::DTraceRedirectFactory()
    {
    iVersion = TVersion( KMajorVersionNumber, KMinorVersionNumber,
            KBuildVersionNumber );
    }

TInt DTraceRedirectFactory::Create( DLogicalChannelBase*& aChannel )
/**
 Create a new DSchedhookTest on this logical device
 */
    {
    aChannel = new DTraceRedirect;
    return aChannel ? KErrNone : KErrNoMemory;
    }

TInt DTraceRedirectFactory::Install()
/**
 Install the LDD - overriding pure virtual
 */
    {
    return SetName( &KLddName );
    }

void DTraceRedirectFactory::GetCaps( TDes8& aDes ) const
/**
 Get capabilities - overriding pure virtual
 */
    {
    TCapsTestV01 b;
    b.iVersion = TVersion( KMajorVersionNumber, KMinorVersionNumber,
            KBuildVersionNumber );
    aDes.FillZ( aDes.MaxLength() );
    aDes.Copy( (TUint8*) &b, Min( aDes.MaxLength(), sizeof( b ) ) );
    }

DECLARE_STANDARD_LDD()
    {
    return new DTraceRedirectFactory;
    }

