/*
========================================================================
 Name        : TraceManagerSettingView.h
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


#ifndef TRACEMANAGERSETTINGVIEW_H
#define TRACEMANAGERSETTINGVIEW_H

#include <aknview.h>
#include "TraceManagerSettings.h"




class CTmSettingList;

/**
 * Avkon view class for TraceManagerSettingView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CTraceManagerSettingView TraceManagerSettingView.h
 */						
			
class CTraceManagerSettingView : public CAknView
	{
public:
	// constructors and destructor
	CTraceManagerSettingView();
	static CTraceManagerSettingView* NewL();
	static CTraceManagerSettingView* NewLC();        
	void ConstructL();
	virtual ~CTraceManagerSettingView();
						
	// from base class CAknView
	TUid Id() const;
	void HandleCommandL( TInt aCommand );
		
protected:
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
	void HandleStatusPaneSizeChange();	
	TBool HandleChangeSelectedSettingItemL( TInt aCommand );
	
private:
	void SetupStatusPaneL();
	void CleanupStatusPane();
	
	CTmSettingList* iTmSettingList;
	TTraceManagerSettings iSettings;	
	};

#endif // TRACEMANAGERSETTINGVIEW_H
