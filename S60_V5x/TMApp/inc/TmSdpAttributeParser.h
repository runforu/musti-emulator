/*
========================================================================
 Name        : TmSdpAttributeParser.h
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

#ifndef TMSDPATTRIBUTEPARSER_H
#define TMSDPATTRIBUTEPARSER_H

// INCLUDES
#include <btsdp.h>

// FORWARD DECLARATIONS
class MTmSdpAttributeNotifier;

// CLASS DECLARATIONS
/**
* TTmSdpAttributeParser
* An instance of TTmSdpAttributeParser is used to check an SDP 
* attribute value, and read selected parts
*/
class TTmSdpAttributeParser : public MSdpAttributeValueVisitor 
    {
    public:   // Enumerations

        /**
        * TNodeCommand
        * The command to be carried out at a node
        * ECheckType. Check the type of the value
        * ECheckValue. Check the type and the value
        * ECheckEnd. Check that a list ends at this point
        * ESkip. Do not check this value - can not match a list end
        * EReadValue. Pass the value onto the observer
        * EFinished. Marks the end of the node list
        */
        enum TNodeCommand
            {
            ECheckType,     
            ECheckValue,  
            ECheckEnd,      
            ESkip,        
            EReadValue,    
            EFinished
            };

        /**
        * TTmSdpAttributeNode
        * An instance of TTmSdpAttributeNode is used to determine how
        * to parse an SDP attribute value data element
        */
        class TTmSdpAttributeNode
        {
        public:
        
            /** 
            * TTmSdpAttributeNode
            * Construct a TTmSdpAttributeNode 
            */
          TTmSdpAttributeNode();
          
          /** 
          * ~TTmSdpAttributeNode
            * destructor 
            */
          ~TTmSdpAttributeNode();
          
          /** 
            * SetCommand
            * set value of iCommand member variable, 
            * iCommand is the command for the node
            * @param aCommand the value of the iCommand
            */  
          void SetCommand( TNodeCommand aCommand );
          
          /** 
            * SetType
            * set value of iType member variable
            * iType is the expected type
            * @param aType the value of the iType
            */  
          void SetType( TSdpElementType aType );
          
          /** 
            * SetValue
            * set value of iValue member variable, 
            * iValue the expected value for ECheckValue,the value of aKey
            * passed to the observer for EReadValue
            * @param aValue the value of the iValue
            */  
          void SetValue( TInt aValue );
          
          /** 
            * Command
            * get value of iCommand member variable
            * @return the value of the iCommand
            */    
          TNodeCommand Command() const;
          
          /** 
            * Type
            * get value of iType member variable
            * @return the value of the iType
            */    
          TSdpElementType Type() const;
          
          /** 
            * Value
            * get value of iValue member variable
            * @return the value of the iValue
            */  
          TInt Value() const;   
            
       private:
            
            /** 
            * iCommand the command for the node 
            */
            TNodeCommand iCommand;

            /** 
            * iType the expected type 
            */
            TSdpElementType iType;

            /** 
            * iValue the expected value for ECheckValue,the value of aKey
            * passed to the observer for EReadValue 
            */
            TInt iValue;    
        };


        /**
        * TTmSdpAttributeParser
        * Construct a TTmSdpAttributeParser
        * @param aNodeList the list of expected nodes 
        * @param aObserver an observer to read specified node values
        */
        TTmSdpAttributeParser( 
            RArray<TTmSdpAttributeParser::TTmSdpAttributeNode>& aNodeList,
            MTmSdpAttributeNotifier& aObserver );

		/**
        * ~TTmSdpAttributeParser
        * Destructor
        */
        virtual ~TTmSdpAttributeParser();

        /**
        * HasFinished
        * Check if parsing processed the whole list
        * @return true is the index refers to the EFinished node
        */
        TBool HasFinished() const;

    public: // from MSdpAttributeValueVisitor

        /**
        * VisitAttributeValueL
        * Process a data element
        * @param aValue the data element 
        * @param aType the type of the data element
        */
        void VisitAttributeValueL( CSdpAttrValue& aValue, 
            TSdpElementType aType );

        /**
        * StartListL
        * Process the start of a data element list
        * @param aList the data element list 
        */
        void StartListL( CSdpAttrValueList& aList );

        /**
        * EndListL
        * Process the end of a data element list
        */
        void EndListL();

    private:    // Functions from base classes

        /**
        * CheckTypeL
        * Check the type of the current node is the same as 
        * the specified type.
        * @param aElementType the type of the current data element
        */
        void CheckTypeL( TSdpElementType aElementType ) const;

        /**
        * CheckValueL
        * Check the value of the current node is the same as the specified value
        * @param aValue the value of the current data element.
        */
        void CheckValueL( CSdpAttrValue& aValue ) const;

        /**
        * ReadValueL
        * Pass the data element value to the observer
        * @param aValue the value of the current data element.
        */
        void ReadValueL( CSdpAttrValue& aValue ) const;

        /**
        * AdvanceL
        * Advance to the next node. Leaves with KErrEof 
        * if at the finished node.
        */
        void AdvanceL();

    private:    // data

        /** 
        * iObserver the observer to read values 
        */
        MTmSdpAttributeNotifier& iObserver;

        /** 
        * iNodeList a list defining the expected 
        * structure of the value 
        */
        RArray<TTmSdpAttributeParser::TTmSdpAttributeNode>& iNodeList;
    
        /** 
        * iCurrentNodeIndex the index of the current node in iNodeList 
        */
        TInt iCurrentNodeIndex;
        
        /** 
        * iCurrentNode the current node 
        */
        TTmSdpAttributeNode iCurrentNode;
    };



#endif // TMSDPATTRIBUTEPARSER_H

// End of File
