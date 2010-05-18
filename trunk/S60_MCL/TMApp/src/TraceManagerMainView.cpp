/*
 ========================================================================
 Name        : TraceManagerMainView.cpp
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
#include <barsread.h>
#include <stringloader.h>
#include <gdi.h>
#include <eikedwin.h>
#include <eikenv.h>
#include <akncontext.h>
#include <akntitle.h>
#include <eikbtgpc.h>
#include <TraceManager.rsg>

#include "TraceManager.hrh"
#include "TraceManagerMainView.h"
#include "TraceManagerMain.hrh"
#include "TraceManagerSetting.hrh"
#include "TraceManagerMain.h"
#include "TmWriterIf.h"
#include "TmCenRepKeys.h"
#include "TraceManagerSettings.h"
#include "TMBtWriter.h"
#include "TMUsbWriter.h"
#include "TMFileWriter.h"
#include "TMLog.h"

static const TInt KMaxTraceBufSize = 512;

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CTraceManagerMainView::CTraceManagerMainView()
    {
    }

/** 
 * The view's destructor removes the container from the control
 * stack and destroys it.
 */
CTraceManagerMainView::~CTraceManagerMainView()
    {
    delete iTraceManagerMain;
    delete iTrace;
    delete iTraceBuf;
    delete iWriteHelper;  
    }

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CTraceManagerMainView
 */
CTraceManagerMainView* CTraceManagerMainView::NewL()
    {
    CTraceManagerMainView* self = CTraceManagerMainView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CTraceManagerMainView
 */
CTraceManagerMainView* CTraceManagerMainView::NewLC()
    {
    CTraceManagerMainView* self = new ( ELeave ) CTraceManagerMainView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */
void CTraceManagerMainView::ConstructL()
    {
    BaseConstructL( R_TRACEMANAGER_MAIN_TRACEMANAGER_MAIN_VIEW );
    iTraceBuf = HBufC8::NewL( KMaxTraceBufSize );
    iTrace = CTraceClient::NewL( *this );
    iTrace->StartL();
    iWriteHelper = CTMWriteHelper::NewL( *this );
    }

/**
 * @return The UID for this view
 */
TUid CTraceManagerMainView::Id() const
    {
    return TUid::Uid( ETraceManagerMainViewId );
    }

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CTraceManagerMainView::HandleCommandL( TInt aCommand )
    {
    TBool commandHandled = EFalse;
    
    if( iWriteHelper->IsCreatingConnection() )
        {
        return;
        }
    
    switch( aCommand )
        {
        case ETraceManagerMainViewDisconnectCommand:
            commandHandled = HandleDisconnectSelectedL( aCommand );
            break;
        case ETraceManagerMainViewConnectCommand:
            commandHandled = HandleConnectSelectedL( aCommand );
            break;
        case ETraceManagerMainViewSettingsCommand:
            commandHandled = HandleSettingsSelectedL( aCommand );
            break;
        case EAknSoftkeyHide:
            {
            TApaTask task( iEikonEnv->WsSession() );
            task.SetWgId( CEikonEnv::Static()->RootWin().Identifier() );
            task.SendToBackground();
            }
            break;
        default:
            break;
        }

    if( !commandHandled )
        {
        if( aCommand == EAknCmdExit )
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            }
        }
    }

/**
 *	Handles user actions during activation of the view, 
 *	such as initializing the content.
 */
void CTraceManagerMainView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    SetupStatusPaneL();

    if( iTraceManagerMain == NULL )
        {
        iTraceManagerMain = CreateContainerL();
        iTraceManagerMain->SetMopParent( this );
        AppUi()->AddToStackL( *this, iTraceManagerMain );
        }
    }

/**
 */
void CTraceManagerMainView::DoDeactivate()
    {
    CleanupStatusPane();

    if( iTraceManagerMain != NULL )
        {
        AppUi()->RemoveFromViewStack( *this, iTraceManagerMain );
        delete iTraceManagerMain;
        iTraceManagerMain = NULL;
        }
    }

/** 
 * Handle status pane size change for this view (override)
 */
void CTraceManagerMainView::HandleStatusPaneSizeChange()
    {
    CAknView::HandleStatusPaneSizeChange();

    // this may fail, but we're not able to propagate exceptions here
    TVwsViewId view;
    AppUi()->GetActiveViewId( view );
    if( view.iViewUid == Id() )
        {
        TRAPD( result, SetupStatusPaneL() );
        }
    }

void CTraceManagerMainView::SetupStatusPaneL()
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
                R_TRACEMANAGER_MAIN_TITLE_RESOURCE );
        title->SetFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader internal state
        }
    }

void CTraceManagerMainView::CleanupStatusPane()
    {
    }

/**
 *	Creates the top-level container for the view.  You may modify this method's
 *	contents and the CTraceManagerMain::NewL() signature as needed to initialize the
 *	container, but the signature for this method is fixed.
 *	@return new initialized instance of CTraceManagerMain
 */
CTraceManagerMain* CTraceManagerMainView::CreateContainerL()
    {
    return CTraceManagerMain::NewL( this );
    }

void CTraceManagerMainView::NewWriterReport( const TDesC &aReport )
    {
    iTraceManagerMain->AppendInfo( aReport );
    }
/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CTraceManagerMainView::HandleDisconnectSelectedL( TInt /*aCommand*/ )
    {
    iWriteHelper->DisconnectL();
    iTraceManagerMain->Clear();
    return ETrue;
    }

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CTraceManagerMainView::HandleConnectSelectedL( TInt /*aCommand*/ )
    {
    iWriteHelper->ConnectL();
    return ETrue;
    }

/** 
 * Handle the selected event.
 * @param aCommand the command id invoked
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CTraceManagerMainView::HandleSettingsSelectedL( TInt /*aCommand*/ )
    {
    // TODO: implement selected event handler
    AppUi()->ActivateLocalViewL( TUid::Uid( ETraceManagerSettingViewId ) );
    return ETrue;
    }

void CTraceManagerMainView::DynInitMenuPaneL( TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    if( R_TRACEMANAGER_MAIN_MENU_PANE1_MENU_PANE == aResourceId )
        {

        aMenuPane->SetItemDimmed( ETraceManagerMainViewDisconnectCommand,
                !iWriteHelper->IsConnected() );
        aMenuPane->SetItemDimmed( ETraceManagerMainViewConnectCommand,
                iWriteHelper->IsConnected() );

        }
    }
/** 
 * Handle the middleSoftKeyPressed event.
 * @return ETrue if the command was handled, EFalse if not
 */
TBool CTraceManagerMainView::HandleControlPaneMiddleSoftKeyPressedL(
        TInt /*aCommand*/ )
    {
    // TODO: implement middleSoftKeyPressed event handler
    return EFalse;
    }

void CTraceManagerMainView::NewTraceReady( const TDesC8& aTrace )
    {
    _LIT( KTimeFormat, "[%F%Y-%M-%D %H:%T:%S.%*C3] " );
    TTime t;
    t.HomeTime();
    TBuf<32> timeBuf;
    t.FormatL( timeBuf, KTimeFormat );
    TPtr8 tmp = iTraceBuf->Des();
    tmp.Copy( timeBuf );
    tmp.Append( aTrace );
    iWriteHelper->WriteTrace( tmp );
    }
