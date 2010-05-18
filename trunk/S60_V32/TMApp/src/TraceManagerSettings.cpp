/*
========================================================================
 Name        : TraceManagerSettings.cpp
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

#include <e32base.h>
#include <stringloader.h>
#include <barsread.h>
#include <TraceManager.rsg>
#include "TraceManagerSettings.h"

/**
 * C/C++ constructor for settings data, cannot throw
 */
TTraceManagerSettings::TTraceManagerSettings()
	{	
	SetConnType( 0 );	
	}
	
TInt& TTraceManagerSettings::ConnType()
	{
	return iConnType;
	}

void TTraceManagerSettings::SetConnType(const TInt& aValue)
	{
	iConnType = aValue;
	}


