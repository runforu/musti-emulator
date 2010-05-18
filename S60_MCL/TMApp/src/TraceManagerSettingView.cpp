/*
========================================================================
 Name        : TraceManagerSettingView.cpp
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

#include <aknviewappui.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <akncontext.h>
#include <akntitle.h>
#include <stringloader.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <TraceManager.rsg>
#include <centralrepository.h>

#include "TraceManager.hrh"
#include "TraceManagerSettingView.h"
#include "TraceManagerMain.hrh"
#include "TraceManagerSetting.hrh"
#include "TraceManagerSettingList.h"
#include "TmCenRepKeys.h"
#include "TMLog.h"

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CTraceManagerSettingView::CTraceManagerSettingView()
    {

    }

/** 
 * The view's destructor removes the container from the control
 * stack and destroys it.
 */
CTraceManagerSettingView::~CTraceManagerSettingView()
    {

    }

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CTraceManagerSettingView
 */
CTraceManagerSettingView* CTraceManagerSettingView::NewL()
    {
    CTraceManagerSettingView* self = CTraceManagerSettingView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CTraceManagerSettingView
 */
CTraceManagerSettingView* CTraceManagerSettingView::NewLC()
    {
    CTraceManagerSettingView* self =
            new ( ELeave ) CTraceManagerSettingView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */
void CTraceManagerSettingView::ConstructL()
    {
    BaseConstructL( R_TRACEMANAGER_SETTING_TRACEMANAGER_SETTING_VIEW );
    CRepository* repository = CRepository::NewLC( KCRUidTmSettings );
    repository->Get( KTmConnectionType, iSettings.ConnType() );// may leave
    CleanupStack::PopAndDestroy(); //repository
    }

/**
 * @return The UID for this view
 */
TUid CTraceManagerSettingView::Id() const
    {
    return TUid::Uid( ETraceManagerSettingViewId );
    }

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CTraceManagerSettingView::HandleCommandL( TInt aCommand )
    {
    TBool commandHandled = EFalse;
    switch( aCommand )
        { // code to dispatch to the AknView's menu and CBA commands is generated here
        case ETraceManagerSettingViewChangeCommand:
            commandHandled = HandleChangeSelectedSettingItemL( aCommand );
            break;
        default:
            break;
        }

    if( !commandHandled )
        {
        if( aCommand == EAknSoftkeyDone )
            {
            AppUi()->ActivateLocalViewL( TUid::Uid( ETraceManagerMainViewId ) );
            }
        }
    }

/**
 *	Handles user actions during activation of the view, 
 *	such as initializing the content.
 */
void CTraceManagerSettingView::DoActivateL(
        const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/)
    {    
    SetupStatusPaneL();

    if( iTmSettingList == NULL )
        {
        iTmSettingList = CTmSettingList::NewL( iSettings, this );
        iTmSettingList->SetMopParent( this );
        iTmSettingList->ConstructFromResourceL(
                R_TRACEMANAGER_SETTING_TRACEMANAGER_SETTING );
        iTmSettingList->ActivateL();
        iTmSettingList->LoadSettingsL();
        AppUi()->AddToStackL( *this, iTmSettingList );
        }
    }

/**
 */
void CTraceManagerSettingView::DoDeactivate()
    {
    CleanupStatusPane();

    if( iTmSettingList != NULL )
        {
        AppUi()->RemoveFromStack( iTmSettingList );
        delete iTmSettingList;
        iTmSettingList = NULL;
        }
    
    CRepository* repository = CRepository::NewLC( KCRUidTmSettings );
    repository->Set( KTmConnectionType, iSettings.ConnType() );// may leave
    CleanupStack::PopAndDestroy(); //repository
    }

/** 
 * Handle status pane size change for this view (override)
 */
void CTraceManagerSettingView::HandleStatusPaneSizeChange()
    {
    CAknView::HandleStatusPaneSizeChange();

    // this may fail, but we're not able to propagate exceptions here
    TVwsViewId view;
    AppUi()->GetActiveViewId( view );
    if( view.iViewUid == Id() )
        {
        TInt result;
            TRAP( result, SetupStatusPaneL() );
        }
    }

void CTraceManagerSettingView::SetupStatusPaneL()
    {
    // reset the context pane
    TUid contextPaneUid = TUid::Uid( EEikStatusPaneUidContext );
    CEikStatusPaneBase::TPaneCapabilities subPaneContext =
            StatusPane()->PaneCapabilities( contextPaneUid );
    if( subPaneContext.IsPresent() && subPaneContext.IsAppOwned() )
        {
        CAknContextPane* context =
                static_cast<CAknContextPane*> ( StatusPane()->ControlL(
                        contextPaneUid ) );
        context->SetPictureToDefaultL();
        }

    // setup the title pane
    TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
    CEikStatusPaneBase::TPaneCapabilities subPaneTitle =
            StatusPane()->PaneCapabilities( titlePaneUid );
    if( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
        {
        CAknTitlePane* title =
                static_cast<CAknTitlePane*> ( StatusPane()->ControlL(
                        titlePaneUid ) );
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC( reader,
                R_TRACEMANAGER_SETTING_TITLE_RESOURCE );
        title->SetFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader internal state
        }
    }

void CTraceManagerSettingView::CleanupStatusPane()
    {
    }

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CTraceManagerSettingView::HandleChangeSelectedSettingItemL(
        TInt aCommand )
    {
    iTmSettingList->ChangeSelectedItemL();
    return ETrue;
    }

