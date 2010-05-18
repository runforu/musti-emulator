/*
========================================================================
 Name        : TraceManagerAppUi.h
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

#ifndef TRACEMANAGERAPPUI_H
#define TRACEMANAGERAPPUI_H

#include <aknviewappui.h>

class CTraceManagerMainView;
class CTraceManagerSettingView;
class CTMBtWriter;
/**
 * @class	CTraceManagerAppUi TraceManagerAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        view management and the default menu, control pane, and status pane.
 */
class CTraceManagerAppUi : public CAknViewAppUi
	{
public: 
	// constructor and destructor
	CTraceManagerAppUi();
	virtual ~CTraceManagerAppUi();
	void ConstructL();

public:
	// from CCoeAppUi
	TKeyResponse HandleKeyEventL(
				const TKeyEvent& aKeyEvent,
				TEventCode aType );

	// from CEikAppUi
	void HandleCommandL( TInt aCommand );
	void HandleResourceChangeL( TInt aType );

	// from CAknAppUi
	void HandleViewDeactivation( 
			const TVwsViewId& aViewIdToBeDeactivated, 
			const TVwsViewId& aNewlyActivatedViewId );

private:
	void InitializeContainersL();
public: 
	
private: 
	CTraceManagerMainView* iTraceManagerMainView;
	CTraceManagerSettingView* iTraceManagerSettingView;
	
protected: 
	
	};

#endif // TRACEMANAGERAPPUI_H			
