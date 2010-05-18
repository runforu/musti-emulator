/*
========================================================================
 Name        : TraceManagerDocument.cpp
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

#include "TraceManagerDocument.h"
#include "TraceManagerAppUi.h"

/**
 * @brief Constructs the document class for the application.
 * @param anApplication the application instance
 */
CTraceManagerDocument::CTraceManagerDocument( CEikApplication& anApplication )
	: CAknDocument( anApplication )
	{
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CTraceManagerDocument::ConstructL()
	{
	}
	
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CTraceManagerDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CTraceManagerDocument
 */
CTraceManagerDocument* CTraceManagerDocument::NewL( CEikApplication& aApp )
	{
	CTraceManagerDocument* self = new ( ELeave ) CTraceManagerDocument( aApp );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * @brief Creates the application UI object for this document.
 * @return the new instance
 */	
CEikAppUi* CTraceManagerDocument::CreateAppUiL()
	{
	return new ( ELeave ) CTraceManagerAppUi;
	}
				
