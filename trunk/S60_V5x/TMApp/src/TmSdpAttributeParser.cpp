/*
========================================================================
 Name        : TmSdpAttributeParser.cpp
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

#include "TmSdpAttributeParser.h"
#include "TmSdpAttributeNotifier.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeParser()
// Construct a TTmSdpAttributeParser.
// ----------------------------------------------------------------------------
//
TTmSdpAttributeParser::TTmSdpAttributeParser(
    RArray<TTmSdpAttributeParser::TTmSdpAttributeNode>& aNodeList,
    MTmSdpAttributeNotifier& aObserver ) : 
    iObserver( aObserver ),
    iNodeList( aNodeList ),
    iCurrentNodeIndex( 0 )
    {
    iCurrentNode = iNodeList[iCurrentNodeIndex];
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::~TTmSdpAttributeParser()
// Destructor.
// ----------------------------------------------------------------------------
//
TTmSdpAttributeParser::~TTmSdpAttributeParser()
	{
	// No implementation needed	
	}
	
// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::HasFinished()
// Check if parsing processed the whole list.
// ----------------------------------------------------------------------------
//
TBool TTmSdpAttributeParser::HasFinished() const
    {
    return ( iCurrentNode.Command() == EFinished );
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::VisitAttributeValueL()
// Process a data element.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::VisitAttributeValueL( CSdpAttrValue& aValue, 
    TSdpElementType aType )
    {
    switch ( iCurrentNode.Command() )
        {
        case ECheckType:
            CheckTypeL( aType );
            break;

        case ECheckValue:
            CheckTypeL( aType );
            CheckValueL( aValue );
            break;

        case ECheckEnd:
            User::Leave( KErrTooBig ); //list element contains too many items
            break;

        case ESkip:
            break;  // no checking required

        case EReadValue:
            CheckTypeL( aType );
            ReadValueL( aValue );
            break;

        case EFinished:
            User::Leave( KErrEof ); // element is after 
            return;                     // value should have ended

        default:
            break;
        }

    AdvanceL();
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::StartListL()
// Process the start of a data element list.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::StartListL( CSdpAttrValueList& /*aList*/ )
    {
    // no checks done here
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::EndListL()
// Process the end of a data element list.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::EndListL()
    {
    // check we are at the end of a list
    if ( iCurrentNode.Command() != ECheckEnd )
        {
        User::Leave( KErrGeneral );
        }

    AdvanceL();
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::CheckTypeL()
// Check the type of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::CheckTypeL( TSdpElementType aElementType ) const
    {
    if ( iCurrentNode.Type() != aElementType )
        {
        User::Leave( KErrGeneral );
        }
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::CheckValueL()
// Check the value of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::CheckValueL( CSdpAttrValue& aValue ) const
    {
    switch ( aValue.Type() )
        {
        case ETypeNil:            
            break;

        case ETypeUint:
            if ( aValue.Uint() != ( TUint ) iCurrentNode.Value() )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeInt:
            if ( aValue.Int() != iCurrentNode.Value() )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeBoolean:
            if ( aValue.Bool() != iCurrentNode.Value() )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeUUID:
            if ( aValue.UUID() != TUUID( iCurrentNode.Value() ) )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeDES:
        case ETypeDEA:
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::ReadValueL()
// Pass the data element value to the observer.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::ReadValueL( CSdpAttrValue& aValue ) const
    {
    iObserver.FoundElementL( iCurrentNode.Value(), aValue );
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::AdvanceL()
// Advance to the next node.
// ----------------------------------------------------------------------------
//
void TTmSdpAttributeParser::AdvanceL()
    {
    // check not at end
    if ( iCurrentNode.Command() == EFinished )
        {
        User::Leave( KErrEof );
        }

    // move to the next item
    iCurrentNode = iNodeList[++iCurrentNodeIndex];
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::TTmSdpAttributeNode()
// constructor.
// ----------------------------------------------------------------------------
//
TTmSdpAttributeParser::TTmSdpAttributeNode::TTmSdpAttributeNode()
  {
  // no implementation needed
  }
            
// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::~TTmSdpAttributeNode()
// destructor.
// ----------------------------------------------------------------------------
//
TTmSdpAttributeParser::TTmSdpAttributeNode::~TTmSdpAttributeNode()
  {
    // no implementation needed
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::SetCommand( TNodeCommand aCommand )
// set iCommand member variable.
// ----------------------------------------------------------------------------
//            
void TTmSdpAttributeParser::TTmSdpAttributeNode::SetCommand( TNodeCommand aCommand )
    {
    iCommand = aCommand;
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::SetType( TTmSdpElementType aType )
// set iType member variable.
// ----------------------------------------------------------------------------
//          
void TTmSdpAttributeParser::TTmSdpAttributeNode::SetType( TSdpElementType aType )
    {
    iType = aType;
    }

// ----------------------------------------------------------------------------
// void TTmSdpAttributeParser::TTmSdpAttributeNode::SetValue( TInt aValue )
// set iValue member variable.
// ----------------------------------------------------------------------------
//          
void TTmSdpAttributeParser::TTmSdpAttributeNode::SetValue( TInt aValue )
    {
    iValue = aValue;
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::Command()
// get iCommand member variable value.
// ----------------------------------------------------------------------------
//            
TTmSdpAttributeParser::TNodeCommand TTmSdpAttributeParser::TTmSdpAttributeNode::Command() const
    {
    return iCommand;
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::Type()
// get iType member variable value.
// ----------------------------------------------------------------------------
//            
TSdpElementType TTmSdpAttributeParser::TTmSdpAttributeNode::Type() const
    {
    return iType;
    }

// ----------------------------------------------------------------------------
// TTmSdpAttributeParser::TTmSdpAttributeNode::Value()
// get iValue member variable value.
// ----------------------------------------------------------------------------
//            
TInt TTmSdpAttributeParser::TTmSdpAttributeNode::Value() const
    {
    return iValue;
    }   
            

// End of File
