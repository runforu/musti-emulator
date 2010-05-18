/*
========================================================================
 Name        : TMUsbwriter.h
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

#ifndef TMUSBWRITER_H
#define TMUSBWRITER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <c32comm.h>
#include <e32base.h>

#include "TmWriterIf.h"
// CLASS DECLARATION

/**
 *  CUsbWriter
 * 
 */
class CUsbWriter : public CBase, public MTmWriter
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUsbWriter();

    /**
     * Two-phased constructor.
     */
    static CUsbWriter* NewL( MTmWriterObserver& aObserver );

    /**
     * Two-phased constructor.
     */
    static CUsbWriter* NewLC( MTmWriterObserver& aObserver );
    
    void ConnectL();
    
    void DisconnectL();
    
    TBool IsConnected();
    
    void Write( const TDesC8 &aDes );
    
    TUid Type();
    
    const TDesC & Descriptor();
    
private:
    /**
     * Constructor for performing 1st stage construction
     */
    CUsbWriter( MTmWriterObserver& aObserver );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
private:

    RCommServ iServer;
    RComm iCommPort;
    TBool iIsConnected;
    MTmWriterObserver& iObserver;
    };

#endif // TMUSBWRITER_H
