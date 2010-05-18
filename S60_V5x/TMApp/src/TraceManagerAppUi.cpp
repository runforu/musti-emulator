/*
========================================================================
 Name        : TraceManagerAppUi.cpp
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

#include <eikmenub.h>
#include <akncontext.h>
#include <akntitle.h>
#include <TraceManager.rsg>
#include <e32cmn.h>

#include "TraceManagerAppUi.h"
#include "TraceManager.hrh"
#include "TraceManagerSetting.hrh"
#include "TraceManagerMainView.h"
#include "TraceManagerSettingView.h"
#include "TMBtWriter.h"

/**
 * Construct the CTraceManagerAppUi instance
 */ 
CTraceManagerAppUi::CTraceManagerAppUi()
	{	
	}

/** 
 * The appui's destructor removes the container from the control
 * stack and destroys it.
 */
CTraceManagerAppUi::~CTraceManagerAppUi()
	{
	}

void CTraceManagerAppUi::InitializeContainersL()
	{
	iTraceManagerMainView = CTraceManagerMainView::NewL();
	AddViewL( iTraceManagerMainView );
	SetDefaultViewL( *iTraceManagerMainView );
	iTraceManagerSettingView = CTraceManagerSettingView::NewL();
	AddViewL( iTraceManagerSettingView );
	}

/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CTraceManagerAppUi::HandleCommandL( TInt aCommand )
	{
    switch (aCommand)
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case EAknCmdExit:
            Exit();
            break;
        }
	
	}

/** 
 * Override of the HandleResourceChangeL virtual function
 */
void CTraceManagerAppUi::HandleResourceChangeL( TInt aType )
	{
	CAknViewAppUi::HandleResourceChangeL( aType );
	}
				
/** 
 * Override of the HandleKeyEventL virtual function
 * @return EKeyWasConsumed if event was handled, EKeyWasNotConsumed if not
 * @param aKeyEvent 
 * @param aType 
 */
TKeyResponse CTraceManagerAppUi::HandleKeyEventL(
		const TKeyEvent& aKeyEvent,
		TEventCode aType )
	{
	// The inherited HandleKeyEventL is private and cannot be called
	
	return EKeyWasNotConsumed;
	}

/** 
 * Override of the HandleViewDeactivation virtual function
 *
 * @param aViewIdToBeDeactivated 
 * @param aNewlyActivatedViewId 
 */
void CTraceManagerAppUi::HandleViewDeactivation( 
		const TVwsViewId& aViewIdToBeDeactivated, 
		const TVwsViewId& aNewlyActivatedViewId )
	{
	CAknViewAppUi::HandleViewDeactivation( 
			aViewIdToBeDeactivated, 
			aNewlyActivatedViewId );	
	}

void CTraceManagerAppUi::ConstructL()
	{	
    BaseConstructL( EAknEnableSkin 
            | EAknEnableMSK         
            | EAknTouchCompatible );

	InitializeContainersL();
	}

