/*
========================================================================
 Name        : TMBtServiceSearcher.h
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


#ifndef TMBTSERVICESEARCHER_H
#define TMBTSERVICESEARCHER_H

// INCLUDES
#include <e32base.h>
#include <bttypes.h>
#include <btextnotifiers.h>

#include "TmSdpAttributeParser.h"
#include "TmSdpAttributeNotifier.h"
#include "TmProtocolConstants.h"

// CONSTANTS
_LIT( KErrNRRCErr, "NRRC ERR" );

// CLASS DECLARATIONS

/**
* CTmBtServiceSearcher
* Searches for a service on a remote machine
*/

class CTmBtServiceSearcher : public CBase,
    public MSdpAgentNotifier,
    public MTmSdpAttributeNotifier
    {
    public: // Destructor

        /**
        * ~CTmBtServiceSearcher()
        * Destroy the object and release all memory objects
        */
        virtual ~CTmBtServiceSearcher();

    public: // New functions

        /**
        * SelectDeviceByDiscoveryL()
        * Select a device
        * @param aObserverRequestStatus the observer that is to
        * be notified when the device selection is complete.
        */
        void SelectDeviceByDiscoveryL( 
            TRequestStatus& aObserverRequestStatus );

        /**
        * FindServiceL()
        * Find a service on the specified device
        * @param aObserverRequestStatus the observer that is to be notified
        * when the service search is complete
        */    
        void FindServiceL( TRequestStatus& aObserverRequestStatus );

        /** 
        * BTDevAddr()
        * @return the bluetooth device address
        */
        const TBTDevAddr& BTDevAddr();

        /**
        * ResponseParams()
        * @return Information about the device selected by the user
        */
        const TBTDeviceResponseParams& ResponseParams();

    protected:  // New functions

        /**
        * CTmBtServiceSearcher()
        * Constructs this object
        * @param aLog the log to send output to
        */
        CTmBtServiceSearcher();

        /**
        * Finished()
        * The search has finished. Notify the observer 
        * that the process is complete.
        */
        virtual void Finished( TInt aError = KErrNone );

        /** 
        * HasFinishedSearching()
        * Is the instance still wanting to search.
        * @return EFalse if the instance wants searching to continue.
        */
        virtual TBool HasFinishedSearching() const;

        /**
        * HasFoundService()
        * @return ETrue if a service has been found
        */
        TBool HasFoundService() const;

    protected: // abstract methods

        /**
        * ProtocolList()
        * The list of Protocols required by the service.
        */
        virtual RArray <TTmSdpAttributeParser::TTmSdpAttributeNode>& 
            ProtocolList() = 0;
        
        /**
        * ServiceClass()
        * The service class to search for
        * @return the service class UUID
        */
        virtual const TUUID& ServiceClass() const = 0;

        /**
        * FoundElementL()
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue ) = 0;

    public: // from MSdpAgentNotifier

        /**
        * NextRecordRequestComplete()
        * Process the result of the next record request
        * @param aError the error code
        * @param aHandle the handle of the service record
        * @param aTotalRecordsCount the total number of matching 
        * service records
        */
        void NextRecordRequestComplete( TInt aError, 
            TSdpServRecordHandle aHandle, 
            TInt aTotalRecordsCount );

        /** 
        * AttributeRequestResult()
        * Process the next attribute requested
        * @param aHandle the handle of the service record
        * @param aAttrID the id of the attribute
        * @param aAttrValue the value of the attribute
        */
        void AttributeRequestResult( TSdpServRecordHandle aHandle, 
            TSdpAttributeID aAttrID, 
            CSdpAttrValue* aAttrValue );

        /** 
        * AttributeRequestComplete()
        * Process the arrtibute request completion
        * @param aHandle the handle of the service record
        * @param aError the error code
        */
        void AttributeRequestComplete( TSdpServRecordHandle aHandle, 
            TInt aError );

    private:    // Functions from base classes

        /**
        * NextRecordRequestCompleteL()
        * Process the result of the next record request
        * @param aError the error code
        * @param aHandle the handle of the service record
        * @param aTotalRecordsCount the total number of 
        * matching service records
        */
        void NextRecordRequestCompleteL( TInt aError, 
            TSdpServRecordHandle aHandle, 
            TInt aTotalRecordsCount );

        /**
        * AttributeRequestResultL()
        * Process the next attribute requested
        * @param aHandle the handle of the service record
        * @param aAttrID the id of the attribute
        * @param aAttrValue the value of the attribute
        */
        void AttributeRequestResultL( TSdpServRecordHandle aHandle, 
            TSdpAttributeID aAttrID, 
            CSdpAttrValue* aAttrValue );

        /**
        * AttributeRequestCompleteL()
        * Process the arrtibute request completion
        * @param aHandle the handle of the service record
        * @param aError the error code
        */
        void AttributeRequestCompleteL( TSdpServRecordHandle, 
            TInt aError );

    private:    // data

        /** 
        * iStatusObserver pointer to the request status observer 
        * Not owned by CTmBtServiceSearcher
        */
        TRequestStatus* iStatusObserver;

        /** 
        * iIsDeviceSelectorConnected is the device 
        * selector connected? 
        */
        TBool iIsDeviceSelectorConnected;

        /** 
        * iDeviceSelector 
        * a handle to the bluetooth device selector notifier 
        */
        RNotifier iDeviceSelector;

        /** 
        * iResponse 
        * the response of the device selection.
        */
        TBTDeviceResponseParamsPckg iResponse;

        /**
        * iSelectionFilter
        * Request a device selection 
        */
        TBTDeviceSelectionParamsPckg iSelectionFilter;
        /** 
        * iAgent a connetction to the SDP client 
        * Owned by CTmBtServiceSearcher
        */
        CSdpAgent* iAgent;

        /** 
        * iSdpSearchPattern a search pattern 
        * Owned by CTmBtServiceSearcher
        */
        CSdpSearchPattern* iSdpSearchPattern;

        /** 
        * iHasFoundService has the service been found ? 
        */
        TBool iHasFoundService;
    };

#endif // TMBTSERVICESEARCHER_H

// End of File
