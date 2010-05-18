/*
========================================================================
 Name        : TmWriterIf.h
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

#ifndef TMWRITERIF_H_
#define TMWRITERIF_H_

#include <e32def.h>

class MTmWriterObserver
    {
public:
    virtual void HandleConnectedL() = 0;
    virtual void HandleDisconnectedL() = 0;
    virtual void HandleWriteCompleteL( TInt aError ) = 0;   
    virtual void StateReport( const TDesC &aReport ) = 0;
    };

class MTmWriter
    {   
public:
    virtual void ConnectL() = 0;
    virtual void DisconnectL() = 0;    
    virtual TBool IsConnected() = 0;
    virtual void Write( const TDesC8 &aDes ) = 0;
    virtual TUid Type() = 0;
    virtual const TDesC & Descriptor() =0;
    virtual ~MTmWriter()
        {
        }
    };

#endif /* TMWRITERIF_H_ */
