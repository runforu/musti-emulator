/*
========================================================================
 Name        : TraceManagerDocument.h
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


#ifndef TRACEMANAGERDOCUMENT_H
#define TRACEMANAGERDOCUMENT_H

#include <akndoc.h>
		
class CEikAppUi;

/**
* @class	CTraceManagerDocument TraceManagerDocument.h
* @brief	A CAknDocument-derived class is required by the S60 application 
*           framework. It is responsible for creating the AppUi object. 
*/
class CTraceManagerDocument : public CAknDocument
	{
public: 
	// constructor
	static CTraceManagerDocument* NewL( CEikApplication& aApp );

private: 
	// constructors
	CTraceManagerDocument( CEikApplication& aApp );
	void ConstructL();
	
public: 
	// from base class CEikDocument
	CEikAppUi* CreateAppUiL();
	};
#endif // TRACEMANAGERDOCUMENT_H
