/*
========================================================================
 Name        : TraceManagerMain.h
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

#ifndef TRACEMANAGERMAIN_H
#define TRACEMANAGERMAIN_H

#include <coecntrl.h>		
#include <e32cmn.h>
#include "TraceManagerMainView.h"

class CTraceManagerMain : public CCoeControl
	{
public:
	CTraceManagerMain( CTraceManagerMainView *aView );
	static CTraceManagerMain* NewL( CTraceManagerMainView *aView );
	static CTraceManagerMain* NewLC( CTraceManagerMainView *aView );
	void ConstructL( const TRect& aRect );
	virtual ~CTraceManagerMain();
	void CTraceManagerMain::SizeChanged();
	void CTraceManagerMain::HandleResourceChange( TInt aType );
	void AppendInfo( const TDesC &aReport );
	void Clear();
	
private:
	void Draw( const TRect& aRect ) const;
	
private: 
	HBufC *iInformation;	
	CTraceManagerMainView *iView;
	};
				
#endif // TRACEMANAGERMAIN_H
