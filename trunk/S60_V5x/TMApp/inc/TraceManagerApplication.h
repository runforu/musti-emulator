/*
========================================================================
 Name        : TraceManagerApplication.h
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

#ifndef TRACEMANAGERAPPLICATION_H
#define TRACEMANAGERAPPLICATION_H

#include <aknapp.h>

const TUid KUidTraceManagerApplication = { 0xE8ABABAB };

/**
 *
 * @class	CTraceManagerApplication TraceManagerApplication.h
 * @brief	A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CTraceManagerApplication : public CAknApplication
	{
private:
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	
	};
			
#endif // TRACEMANAGERAPPLICATION_H		
