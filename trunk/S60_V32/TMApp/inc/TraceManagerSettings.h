/*
========================================================================
 Name        : TraceManagerSettings.h
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


#ifndef TRACEMANAGERSETTINGSETTINGS_H
#define TRACEMANAGERSETTINGSETTINGS_H
			
#include <e32std.h>


/**
 * @class	TTraceManagerSettings TraceManagerSettings.h
 */
class TTraceManagerSettings
	{
public:
    enum TConnType
        {
        EUSBConn = 0, 
        EBlueTooth = 1,
        ELogToFile = 2,
        };		

	TTraceManagerSettings();

	TInt& ConnType();
	void SetConnType(const TInt& aValue);
	
protected:
	TInt iConnType;	
	};
#endif // TRACEMANAGERSETTINGSETTINGS_H
