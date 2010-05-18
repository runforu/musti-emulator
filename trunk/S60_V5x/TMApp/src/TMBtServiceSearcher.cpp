/*
========================================================================
 Name        : TmBtServiceSearcher.cpp
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

// INCLUDE FILES
#include <StringLoader.h>

#include "TMBtServiceSearcher.h"
#include "TMLog.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::CTmBtServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CTmBtServiceSearcher::CTmBtServiceSearcher()
    : iIsDeviceSelectorConnected( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::~CTmBtServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CTmBtServiceSearcher::~CTmBtServiceSearcher()
    {
    if ( iIsDeviceSelectorConnected )
        {
        iDeviceSelector.CancelNotifier( KDeviceSelectionNotifierUid );
        iDeviceSelector.Close();
        }
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent; 
    iAgent = NULL;
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::SelectDeviceByDiscoveryL()
// Select a device.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::SelectDeviceByDiscoveryL( 
    TRequestStatus& aObserverRequestStatus )
    {
    TM_LOG_FN_GUARD( "CTmBtServiceSearcher::SelectDeviceByDiscoveryL" );
    if ( ! iIsDeviceSelectorConnected )
        {
        User::LeaveIfError( iDeviceSelector.Connect() );
        iIsDeviceSelectorConnected = ETrue;
        }


    iSelectionFilter().SetUUID( ServiceClass() );

    iDeviceSelector.StartNotifierAndGetResponse( 
        aObserverRequestStatus, 
        KDeviceSelectionNotifierUid, 
        iSelectionFilter, 
        iResponse );
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::FindServiceL()
// Find a service on the specified device.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::FindServiceL( TRequestStatus& aObserverRequestStatus )
    {
    TM_LOG_FN_GUARD( "CTmBtServiceSearcher::FindServiceL" );
    if ( !iResponse().IsValidBDAddr() )
        {
        User::Leave( KErrNotFound );
        }
    iHasFoundService = EFalse;

    // delete any existing agent and search pattern
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;

    iAgent = CSdpAgent::NewL( *this, BTDevAddr() );

    iSdpSearchPattern = CSdpSearchPattern::NewL();
   
    iSdpSearchPattern->AddL( ServiceClass() );
    // return code is the position in the list that the UUID is inserted at 
    // and is intentionally ignored
  
    iAgent->SetRecordFilterL( *iSdpSearchPattern );

    iStatusObserver = &aObserverRequestStatus;

    iAgent->NextRecordRequestL();
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::NextRecordRequestComplete()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::NextRecordRequestComplete( 
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount )
    {
    TRAPD( error, 
        NextRecordRequestCompleteL( aError, aHandle, aTotalRecordsCount ) );

    if ( error != KErrNone ) 
        {
        User::Leave( error );
        }
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::NextRecordRequestCompleteL()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::NextRecordRequestCompleteL( 
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount )
    {
    if ( aError == KErrEof )
        {
        Finished();
        return;
        }

    if ( aError != KErrNone )
        {   
        TM_LOG_2( "CTmBtServiceSearcher::NextRecordRequestCompleteL aError = %d.", aError );
        Finished( aError );
        return;
        }

    if ( aTotalRecordsCount == 0 )
        {
        TM_LOG_1( "CTmBtServiceSearcher::NextRecordRequestCompleteL not found." );
        Finished( KErrNotFound );
        return;
        }

    //  Request its attributes
    iAgent->AttributeRequestL( aHandle, KSdpAttrIdProtocolDescriptorList );
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::AttributeRequestResult()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::AttributeRequestResult( 
    TSdpServRecordHandle aHandle, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue )
    {
    TRAPD( error, AttributeRequestResultL( aHandle, aAttrID, aAttrValue ) );
    if ( error != KErrNone )
        {
        User::Leave(error);
        }
    // Delete obsolete local atribute pointer. 
    delete aAttrValue;
    aAttrValue = NULL;
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::AttributeRequestResultL()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::AttributeRequestResultL( 
    TSdpServRecordHandle /*aHandle*/, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue )
    {
    __ASSERT_ALWAYS( aAttrID == KSdpAttrIdProtocolDescriptorList, 
                     User::Leave( KErrNotFound ) );
    TTmSdpAttributeParser parser( ProtocolList(), *this );

    // Validate the attribute value, and extract the RFCOMM channel
    aAttrValue->AcceptVisitorL( parser );

    if ( parser.HasFinished() )
        {
        // Found a suitable record so change state
        iHasFoundService = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CTmBtServiceSearcher::AttributeRequestComplete()
// Process the attribute request completion.
// -----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::AttributeRequestComplete( TSdpServRecordHandle aHandle,
    TInt aError )
    {
    TRAPD( error, AttributeRequestCompleteL( aHandle, aError ) );
    if ( error != KErrNone )
        {
        User::Leave(error);
        }
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::AttributeRequestCompleteL()
// Process the attribute request completion.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::AttributeRequestCompleteL( 
    TSdpServRecordHandle /*aHandle*/,
    TInt aError )
    {
    if ( aError != KErrNone )
        {
        TM_LOG_2( "CTmBtServiceSearcher::AttributeRequestCompleteL aError = %d", aError );
        }
    else if ( !HasFinishedSearching() )
        {
        // have not found a suitable record so request another
        iAgent->NextRecordRequestL();
        }
    else
        {
        Finished();
        }
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::Finished()
// The search has finished and notify the observer 
// that the process is complete.
// ----------------------------------------------------------------------------
//
void CTmBtServiceSearcher::Finished( TInt aError /* default = KErrNone */ )
    {
    TM_LOG_FN_GUARD( "CTmBtServiceSearcher::Finished" );
    if ( aError == KErrNone && !HasFoundService() )
        {
        aError = KErrNotFound;
        }
    User::RequestComplete( iStatusObserver, aError );
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::HasFinishedSearching()
// Is the instance still wanting to search.
// ----------------------------------------------------------------------------
//
TBool CTmBtServiceSearcher::HasFinishedSearching() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::BTDevAddr()
// Returns the bluetooth device address.
// ----------------------------------------------------------------------------
//
const TBTDevAddr& CTmBtServiceSearcher::BTDevAddr()
    {
    return iResponse().BDAddr();
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::ResponseParams()
// Returns information about the device selected by the user.
// ----------------------------------------------------------------------------
//
const TBTDeviceResponseParams& CTmBtServiceSearcher::ResponseParams()
    {
    return iResponse();
    }

// ----------------------------------------------------------------------------
// CTmBtServiceSearcher::HasFoundService()
// True if a service has been found.
// ----------------------------------------------------------------------------
//
TBool CTmBtServiceSearcher::HasFoundService() const
    {
    return iHasFoundService;
    }

// End of File
