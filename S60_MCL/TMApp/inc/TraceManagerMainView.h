/*
========================================================================
 Name        : TraceManagerMainView.h
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

#ifndef TRACEMANAGERMAINVIEW_H
#define TRACEMANAGERMAINVIEW_H

#include <e32std.h>
#include <aknview.h>
#include "TraceClient.h"
#include "TmWriterIf.h"
#include "TmWriteHelper.h"

class CTraceManagerMain;

/**
 * Avkon view class for TraceManagerMainView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CTraceManagerMainView TraceManagerMainView.h
 */						
			
class CTraceManagerMainView : public CAknView, MTraceObserver, MTMWriterStateObserver
	{
public:
	// constructors and destructor
	CTraceManagerMainView();
	static CTraceManagerMainView* NewL();
	static CTraceManagerMainView* NewLC();        
	void ConstructL();
	virtual ~CTraceManagerMainView();
						
	// from base class CAknView
	TUid Id() const;
	void HandleCommandL( TInt aCommand );
	
	CTraceManagerMain* CreateContainerL();
	
    void NewWriterReport( const TDesC &aReport );
	
protected:
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
	void HandleStatusPaneSizeChange();
	
	TBool HandleDisconnectSelectedL( TInt aCommand );
	TBool HandleConnectSelectedL( TInt aCommand );
	TBool HandleSettingsSelectedL( TInt aCommand );
	TBool HandleControlPaneMiddleSoftKeyPressedL( TInt aCommand );
	
	void  NewTraceReady(const TDesC8& aTrace);
	
private:
	void SetupStatusPaneL();
	void CleanupStatusPane();
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);	
	
private:	
	CTraceManagerMain* iTraceManagerMain;
	CTMWriteHelper *iWriteHelper;
	TBool iIsConnected;
	TInt iConnType;
	CTraceClient * iTrace;
	HBufC8 * iTraceBuf;
	};

#endif // TRACEMANAGERMAINVIEW_H
