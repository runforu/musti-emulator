/*
========================================================================
 Name        : TmServiceSearcher.cpp
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
#include <bt_sock.h>

#include "TmServiceSearcher.h"
#include "TmProtocolConstants.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CTmServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CTmServiceSearcher* CTmServiceSearcher::NewL()
    {
    CTmServiceSearcher* self = CTmServiceSearcher::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CTmServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CTmServiceSearcher* CTmServiceSearcher::NewLC()
    {
    CTmServiceSearcher* self = new ( ELeave ) CTmServiceSearcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::CTmServiceSearcher()
// Construcor.
// ----------------------------------------------------------------------------
//
CTmServiceSearcher::CTmServiceSearcher()
        : CTmBtServiceSearcher(),
          iServiceClass( KSerialPortService ),
          iPort( -1 )
    {
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::~CTmServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CTmServiceSearcher::~CTmServiceSearcher()
    {
    iProtocolArray.Close();
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CTmServiceSearcher::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CTmServiceSearcher::ServiceClass() const
    {
    return iServiceClass;
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::ProtocolList()
// The list of Protocols required by the service.
// ----------------------------------------------------------------------------
//
 RArray <TTmSdpAttributeParser::TTmSdpAttributeNode>& CTmServiceSearcher
::ProtocolList()
    {
    TTmSdpAttributeParser::TTmSdpAttributeNode attrib;
    attrib.SetCommand( TTmSdpAttributeParser::ECheckType );
    attrib.SetType( ETypeDES );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TTmSdpAttributeParser::ECheckType );
    attrib.SetType( ETypeDES );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TTmSdpAttributeParser::ECheckValue );
    attrib.SetType( ETypeUUID );
    attrib.SetValue( KL2CAP );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TTmSdpAttributeParser::ECheckEnd );
    iProtocolArray.Append( attrib );
  
    attrib.SetCommand( TTmSdpAttributeParser::ECheckType );
    attrib.SetType( ETypeDES );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TTmSdpAttributeParser::ECheckValue );
    attrib.SetType( ETypeUUID );
    attrib.SetValue( KRFCOMM );
    iProtocolArray.Append( attrib );
  
    attrib.SetCommand( TTmSdpAttributeParser::EReadValue );
    attrib.SetType( ETypeUint );
    attrib.SetValue( KRfcommChannel );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TTmSdpAttributeParser::ECheckEnd );
    iProtocolArray.Append( attrib );
  
    attrib.SetCommand( TTmSdpAttributeParser::ECheckEnd );
    iProtocolArray.Append( attrib );

    attrib.SetCommand( TTmSdpAttributeParser::EFinished );
    iProtocolArray.Append( attrib );
  
    return iProtocolArray;
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CTmServiceSearcher::FoundElementL( TInt aKey, CSdpAttrValue& aValue )
    {
    iPort = aValue.Uint();
    }

// ----------------------------------------------------------------------------
// CTmServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CTmServiceSearcher::Port()
    {
    return iPort;
    }

// End of File
