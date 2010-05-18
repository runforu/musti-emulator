/*
========================================================================
 Name        : TMFileWriter.h
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

#ifndef TMFILEWRITER_H
#define TMFILEWRITER_H

// INCLUDES
#include <e32base.h>  
#include <f32file.h>     

#include "TmWriterIf.h"
// CLASS DECLARATION

/**
 *  CTMFileWriter
 * 
 */
class CTMFileWriter : public CBase, public MTmWriter
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CTMFileWriter();

    /**
     * Two-phased constructor.
     */
    static CTMFileWriter* NewL( MTmWriterObserver& aObserver );

    /**
     * Two-phased constructor.
     */
    static CTMFileWriter* NewLC( MTmWriterObserver& aObserver );
    
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
    CTMFileWriter( MTmWriterObserver& aObserver );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
private:
    RFs iFs;
    RFile iFile;
    TBool iIsConnected;
    MTmWriterObserver& iObserver;
    };

#endif // TMFILEWRITER_H_
