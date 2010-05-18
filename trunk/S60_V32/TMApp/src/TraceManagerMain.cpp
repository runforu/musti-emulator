/*
 ========================================================================
 Name        : TraceManagerMain.cpp
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

#include <barsread.h>
#include <eikenv.h>
#include <aknviewappui.h>
#include <eikappui.h>
#include <TraceManager.rsg>

#include "TraceManagerMain.h"
#include "TraceManagerMainView.h"
#include "TraceManager.hrh"
#include "TraceManagerMain.hrh"
#include "TraceManagerSetting.hrh"

#define KMAX_BUFFER_LENGTH 1024
/**
 * First phase of Symbian two-phase construction. Should not 
 * contain any code that could leave.
 */
CTraceManagerMain::CTraceManagerMain( CTraceManagerMainView *aView ) 
        :iView( aView )
    {
    }

CTraceManagerMain::~CTraceManagerMain()
    {
    delete iInformation;
    }

CTraceManagerMain* CTraceManagerMain::NewL( CTraceManagerMainView *aView )
    {
    CTraceManagerMain* self = CTraceManagerMain::NewLC( aView );
    CleanupStack::Pop( self );
    return self;
    }

CTraceManagerMain* CTraceManagerMain::NewLC( CTraceManagerMainView *aView )
    {
    CTraceManagerMain* self = new ( ELeave ) CTraceManagerMain( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aView->ClientRect() );
    return self;
    }

void CTraceManagerMain::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetBlank();
    SetRect( aRect );
    ActivateL();
    iInformation = HBufC::NewL( KMAX_BUFFER_LENGTH );
    }

void CTraceManagerMain::SizeChanged()
    {
    CCoeControl::SizeChanged(); 
    }

void CTraceManagerMain::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    SetRect( iAvkonViewAppUi->View( TUid::Uid( ETraceManagerMainViewId ) )->ClientRect() );  
    }

void CTraceManagerMain::AppendInfo( const TDesC &aReport )
    {
    iInformation->Des().Append( aReport );
    DrawDeferred();
    }

void CTraceManagerMain::Clear()
    {
    iInformation->Des().Zero();
    DrawDeferred();
    }

void CTraceManagerMain::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );

    const CFont *font = iEikonEnv->AnnotationFont();
    gc.UseFont( font );
    
    TInt vert = 32; 
    TPtrC ptr( *iInformation );
    TInt pos = 0;
    TInt lineStart = 0;
    TInt length = 0;
    _LIT( KNewLine, "\n" );
    
    while (KErrNotFound != (pos = ptr.Find(KNewLine)))
    {
        ptr.Set( &(*iInformation)[lineStart], pos);
        lineStart = lineStart + pos + 1;  
        gc.DrawText( ptr, aRect, vert );        
        vert += font->HeightInPixels()+4;
        length = iInformation->Length() - lineStart;
        if (length > 0)
        {
            ptr.Set( &(*iInformation)[lineStart], length);
        }
    }
           
    gc.DiscardFont();
    }

