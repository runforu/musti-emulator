/*
========================================================================
 Name        : ServiceSearcher.h
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


#ifndef TMSERVICESEARCHER_H
#define TMSERVICESEARCHER_H

// INCLUDES
#include <e32base.h>
#include "TmBtServiceSearcher.h"


// CLASS DECLARATIONS
/**
* CServiceSearcher
* Searches for a chat service.
*/
class CTmServiceSearcher : public CTmBtServiceSearcher
    {
    public:     // Constructors and destructor

        /*
        * NewL()
        * Create a CServiceSearcher object
        * @param aLog the log to send output to
        * @return a pointer to the created instance of 
        * CServiceSearcher
        */
        static CTmServiceSearcher* NewL();

        /**
        * NewLC()
        * Create a CServiceSearcher object
        * @param aLog the log to send output to
        * @return a pointer to the created instance of 
        * CServiceSearcher
        */
        static CTmServiceSearcher* NewLC();

        /**
        * ~CMessageServiceSearcher()
        * Destroy the object and release all memory objects.
        */
        virtual ~CTmServiceSearcher();
       

    public: // New function

        /**
        * Port()
        * Retrieve the port on which the service is installed
        * @return the port number
        */
        TInt Port();

    protected:  // New functions

        /**
        * ServiceClass()
        * @return the service class uid.
        */
        const TUUID& ServiceClass() const;

        /**
        * FoundElementL()
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue );

        /**
        * ProtocolList()
        * @return the attribute list.
        */
        RArray<TTmSdpAttributeParser::TTmSdpAttributeNode>& ProtocolList();
      
    private:    // Constructors

        /**
        * CServiceSearcher()
        * Constructs this object
        * @param aLog the log to send output to
        */
        CTmServiceSearcher();

        /**
        * ConstructL()
        * Performs second phase construction of this object
        */
        void ConstructL();

    private:    // data

        /** 
        * iServiceClass 
        * the service class UUID to search for 
        */
        TUUID iServiceClass;

        /** 
        * iPort
        * the port number that the remote service is installed 
        */
        TInt iPort;
        
        /**
        * iProtocolArray
        * the attribute list 
        */
        RArray<TTmSdpAttributeParser::TTmSdpAttributeNode> iProtocolArray;
    };

#endif // TMSERVICESEARCHER_H

// End of File
