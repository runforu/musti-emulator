/*
========================================================================
 Name        : TraceManagerSettingList.cpp
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

#include <avkon.hrh>
#include <avkon.rsg>
#include <eikmenup.h>
#include <aknappui.h>
#include <eikcmobs.h>
#include <barsread.h>
#include <stringloader.h>
#include <aknpopupfieldtext.h>
#include <eikenv.h>
#include <eikappui.h>
#include <aknviewappui.h>
#include <TraceManager.rsg>

#include "TraceManagerSettingList.h"
#include "TraceManagerSettings.h"
#include "TraceManager.hrh"
#include "TraceManagerMain.hrh"
#include "TraceManagerSetting.hrh"
#include "TraceManagerSettingView.h"


/**
 * Construct the CTmSettingList instance
 * @param aCommandObserver command observer
 */
CTmSettingList::CTmSettingList( TTraceManagerSettings& aSettings,
        MEikCommandObserver* aCommandObserver ) :
    iSettings( aSettings ), iCommandObserver( aCommandObserver )
    {
    }

void CTmSettingList::ConstructL()
    {
    }

CTmSettingList* CTmSettingList::NewL( TTraceManagerSettings& aSettings,
        MEikCommandObserver* aCommandObserver )
    {
    CTmSettingList* self = new ( ELeave ) CTmSettingList( aSettings,
            aCommandObserver );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop();
    return self;
    }

/** 
 * Destroy any instance variables
 */
CTmSettingList::~CTmSettingList()
    {
    }

/**
 * Handle system notification that the container's size has changed.
 */
void CTmSettingList::SizeChanged()
    {
    if( ListBox() )
        {
        ListBox()->SetRect( Rect() );
        }
    }

/**
 * Create one setting item at a time, identified by id.
 * CAknSettingItemList calls this method and takes ownership
 * of the returned value.  The CAknSettingItem object owns
 * a reference to the underlying data, which EditItemL() uses
 * to edit and store the value.
 */
CAknSettingItem* CTmSettingList::CreateSettingItemL( TInt aId )
    {
    switch( aId )
        {
        case ETraceManagerSettingViewConnType:
            {
            CAknEnumeratedTextPopupSettingItem* item =
                    new ( ELeave ) CAknEnumeratedTextPopupSettingItem( aId,
                            iSettings.ConnType() );
            return item;
            }
        }

    return NULL;
    }

void CTmSettingList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
	TInt idx = SettingItemArray()->ItemIndexFromVisibleIndex( aIndex );
	CAknSettingItem* item = ( *SettingItemArray() )[idx];
	CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
	item->StoreL();         
    }

void CTmSettingList::ChangeSelectedItemL()
    {
    EditItemL( ListBox()->CurrentItemIndex(), ETrue );
    }


void CTmSettingList::HandleResourceChange( TInt aType )
    {
    CAknSettingItemList::HandleResourceChange( aType );
    SetRect( iAvkonViewAppUi->View( TUid::Uid( ETraceManagerSettingViewId ) )->ClientRect() );
    }

/** 
 * Handle key event (override)
 * @param aKeyEvent key event
 * @param aType event code
 * @return EKeyWasConsumed if the event was handled, else EKeyWasNotConsumed
*/
TKeyResponse CTmSettingList::OfferKeyEventL( const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    return CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
    } 

