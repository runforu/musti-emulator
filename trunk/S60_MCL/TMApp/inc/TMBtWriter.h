/*
========================================================================
 Name        : TMBtWriter.h
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

#ifndef CTMBTWRITER_H
#define CTMBTWRITER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <c32comm.h>
#include <e32base.h>
#include <ES_SOCK.H>

#include "TmWriterIf.h"

// CLASS DECLARATION
class CTmServiceSearcher;
/**
 *  CTMBtWriter
 * 
 */
class CTMBtWriter : public CActive, public MTmWriter
    {
public:
    enum TBtWriterState
        {
        ENoAvailableDevice,
        EFindingDevice,
        EFindingService,
        ECreatingConnection,
        EConnected, //Ready to write
        EDisconnecting
        };
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CTMBtWriter();

    /**
     * Two-phased constructor.
     */
    static CTMBtWriter* NewL( MTmWriterObserver& aObserver );

    /**
     * Two-phased constructor.
     */
    static CTMBtWriter* NewLC( MTmWriterObserver& aObserver );
    
    void Write( const TDesC8& aData );
    
    /**
    * SetState()
    * Set state
    * @param aState the state of the engine
    */
    void SetState( TBtWriterState aState );
    /**
    * State()
    * Get State
    * @return state
    */
    TInt State();
    /**
    * ConnectL()
    * Connect to an available service on a remote machine
    */
    void ConnectL();

    /** 
    * DisconnectL()
    * Disconnect from remote machine
    */
    void DisconnectL();
    
    TBool IsConnected();
    
    TUid Type();
    
    const TDesC & Descriptor();
    
private:
    // From CActive
    // Handle completion
    void RunL();

    void HandleError();
    
    void HandleComplete();
    
    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError( TInt aError );

    /**
     * Constructor for performing 1st stage construction
     */
    CTMBtWriter( MTmWriterObserver& aObserver );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    /**
    * ConnectToServerL
    * Connects to the service
    */    
    void ConnectToServerL();

    /**
    * DisonnectFromServerL()
    * Disconnects from the service
    */    
    void DisconnectFromServerL();
    
private:
   
    RSocketServ iSocketServer;
    
    /** 
     * iServiceSearcher searches for service this 
     * client can connect to.
     */
    CTmServiceSearcher* iServiceSearcher;
    
    /** 
    * iSocket 
    * the socket to bind a address and a port, listen connection from client. 
    */
    RSocket iSocket;
    
    TBtWriterState iState;
    CActiveSchedulerWait* iActiveSchedulerWait;
	MTmWriterObserver& iObserver;
    };

#endif // CTMBTWRITER_H
