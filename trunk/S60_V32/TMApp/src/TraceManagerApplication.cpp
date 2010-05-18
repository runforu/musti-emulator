/*
========================================================================
 Name        : TraceManagerApplication.cpp
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

#include "TraceManagerApplication.h"
#include "TraceManagerDocument.h"
#ifdef EKA2
#include <eikstart.h>
#endif

#include "tmlog.h"

/**
 * @brief Returns the application's UID (override from CApaApplication::AppDllUid())
 * @return UID for this application (KUidTraceManagerApplication)
 */
TUid CTraceManagerApplication::AppDllUid() const
	{ 
	return KUidTraceManagerApplication;
	}

/**
 * @brief Creates the application's document (override from CApaApplication::CreateDocumentL())
 * @return Pointer to the created document object (CTraceManagerDocument)
 */
CApaDocument* CTraceManagerApplication::CreateDocumentL()
	{  
	return CTraceManagerDocument::NewL( *this );
	}

#ifdef EKA2

/**
 *	@brief Called by the application framework to construct the application object
 *  @return The application (CTraceManagerApplication)
 */	
LOCAL_C CApaApplication* NewApplication()
	{
	return new CTraceManagerApplication;
	}

/**
* @brief This standard export is the entry point for all Series 60 applications
* @return error code
 */	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
	
#else 	// Series 60 2.x main DLL program code

/**
* @brief This standard export constructs the application object.
* @return The application (CTraceManagerApplication)
*/
EXPORT_C CApaApplication* NewApplication()
	{
	return new CTraceManagerApplication;
	}

/**
* @brief This standard export is the entry point for all Series 60 applications
* @return error code
*/
GLDEF_C TInt E32Dll(TDllReason /*reason*/)
	{
	return KErrNone;
	}

#endif // EKA2
