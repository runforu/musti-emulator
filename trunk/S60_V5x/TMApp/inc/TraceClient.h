/*
========================================================================
 Name        : TraceClient.h
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


#ifndef CTRACECLIENT_H
#define CTRACECLIENT_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <f32file.h>     // For RTimer, link against: euser.lib

#include "d_traceredirect.h"

class MTraceObserver
    {
public:
    virtual void NewTraceReady(const TDesC8& aTrace) = 0;
    };

class CTraceClient : public CActive
    {
public:
    // Cancel and destroy
    ~CTraceClient();

    // Two-phased constructor.
    static CTraceClient* NewL(MTraceObserver & aOb);

    // Two-phased constructor.
    static CTraceClient* NewLC(MTraceObserver & aOb);

public:
    // New functions
    // Function for making the initial request
    void StartL();
    void StopL();
private:
    // C++ constructor
    CTraceClient(MTraceObserver & aOb);

    // Second-phase constructor
    void ConstructL();

private:
    void RunL();

    void DoCancel();

    TInt RunError( TInt aError );

private:
    RTraceRedirect iTraceRedirect; // Provides async timing service
    TBuf8<256> iTraceBuf;
    TBool iNeedToStop;
    MTraceObserver &iObserver;
	RFs iFs;
	RFile iFile;
    };

#endif // CTRACECLIENT_H
