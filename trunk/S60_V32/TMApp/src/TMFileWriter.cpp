/*
========================================================================
 Name        : TMUsbwriter.cpp
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

#include <BAUTILS.H>

#include "TMFileWriter.h"
#include "TMLog.h"

_LIT( KLogFilePathFormat, "C:\\Data\\Trace\\trace_%03d.log" );

_LIT( KFileWriterDes, "Logging to File.\n");
_LIT( KNewFileWriterCreated, "Trace to file mode.\n");
_LIT( KFileWriterConnected, "Trace outputs to file:\n [%S].\n");

CTMFileWriter::CTMFileWriter( MTmWriterObserver& aObserver )
        : iObserver( aObserver )
    {
    }

CTMFileWriter::~CTMFileWriter()
    {  
    if( iIsConnected )
        {
        DisconnectL();
        }
    }

CTMFileWriter* CTMFileWriter::NewLC( MTmWriterObserver& aObserver )
    {
    CTMFileWriter* self = new ( ELeave ) CTMFileWriter( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CTMFileWriter* CTMFileWriter::NewL( MTmWriterObserver& aObserver )
    {
    CTMFileWriter* self = CTMFileWriter::NewLC( aObserver );
    CleanupStack::Pop(); // self;
    return self;
    }


void CTMFileWriter::ConstructL()
    {
    iFs.Connect();
    iObserver.StateReport( KNewFileWriterCreated );
    }

void CTMFileWriter::ConnectL()
    {
    TFileName fn;  
    
    for( TInt i = 0; i < 1000; i++ )
        {
        fn.Format( KLogFilePathFormat, i );
        if( !BaflUtils::FileExists( iFs, fn ) )
            {
            break;
            }
        }
    
    BaflUtils::EnsurePathExistsL( iFs, fn );
    iFile.Create( iFs, fn, EFileWrite );
    iIsConnected = ETrue;
    
    iObserver.HandleConnectedL();
    TBuf<256> buf;
    buf.Format( KFileWriterConnected, &fn );
    iObserver.StateReport( buf );
    }

void CTMFileWriter::DisconnectL()
    {
    iFile.Close();
    iFs.Close();
    iIsConnected = EFalse;
    }

void CTMFileWriter::Write( const TDesC8 &aDes )
    {
    // Sync write to avoid capture losing.
    iFile.Write( aDes );
    iObserver.HandleWriteCompleteL( KErrNone );
    }

TUid CTMFileWriter::Type()
    {
    return TUid::Uid( 0x02 );
    }

const TDesC & CTMFileWriter::Descriptor()
    {
    return KFileWriterDes;
    }

TBool CTMFileWriter::IsConnected()
    {
    return iIsConnected;
    }











