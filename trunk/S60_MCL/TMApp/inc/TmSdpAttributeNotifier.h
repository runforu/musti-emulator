/*
========================================================================
 Name        : TmSdpAttributeNotifier.h
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

#ifndef TMSDPATTRIBUTENOTIFIER_H
#define TMSDPATTRIBUTENOTIFIER_H

// FORWARD DECLARATIONS
class CSdpAttrValue;

// CLASS DECLARATIONS
/**
* MTmSdpAttributeNotifier
* An instance of MTmSdpAttributeNotifier is used to read selected SDP Attribute
* elements from an attribute value.
*/
class MTmSdpAttributeNotifier
    {
    public: // New functions

        /**
        * FoundElementL
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue ) = 0;
    };

#endif // TMSDPATTRIBUTENOTIFIER_H

// End of File
