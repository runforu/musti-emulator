/*
========================================================================
 Name        : TMConnFactory.h
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

#ifndef TMCONNFACTORY_H
#define TMCONNFACTORY_H

#include <e32std.h>
#include "TmWriterIf.h"

#define KMAX_BUFFER_SIZE 0x40;
#define KMAX_BUFFER_INDEX_MASK 0x3F;

class MTMWriterStateObserver
    {
public:
    virtual void NewWriterReport( const TDesC &aReport ) = 0;
    };

class CTMWriteHelper : public CActive, public MTmWriterObserver
    {
public:
    /**
     * Two-phased constructor.
     */
    static CTMWriteHelper* NewL( MTMWriterStateObserver & aObserver );

    /**
     * Two-phased constructor.
     */
    static CTMWriteHelper* NewLC( MTMWriterStateObserver & aObserver );
    
    TBool IsConnected();
    TBool IsCreatingConnection();

    void ConnectL();
    
    void DisconnectL();
    
    void WriteTrace( const TDesC8 &aTrace );
    
    /**
     * Constructor for performing 1st stage construction
     */
    ~CTMWriteHelper();    
    
private:
    
    void HandleConnectedL();
    
    void HandleDisconnectedL();
    
    void HandleWriteCompleteL( TInt aError );
    
    void StateReport( const TDesC &aReport );   
    
    void CompleteSelf();

    CTMWriteHelper( MTMWriterStateObserver & aObserver );

    void ConstructL();
    
    void RunL();
    
    void DoCancel();
private:
    MTmWriter *iActiveWriter;
    MTMWriterStateObserver & iObserver;
    TBool iCreatingConnection; 
//    HBufC * iBuffer[KMAX_BUFFER_SIZE];
//    TInt iHead;
//    TInt iTail;
    };

#endif /* TMCONNFACTORY_H */
