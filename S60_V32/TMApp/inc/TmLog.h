/*
 ========================================================================
 Name        : TmLog.h
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
#ifndef TMLOG_H
#define TMLOG_H
// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <BAUTILS.H>
#include <f32file.h>
#include <e32svr.h>
#ifdef _DEBUG
_LIT( KLogFilePath, "C:\\Data\\TMLOG.log" );
_LIT8( KNewLine, "\n" );
#define KLogBufferSize 256

#ifndef __EXE__
#define SET_TLS(a)      Dll::Tls(a)     
#define GET_TLS         Dll::Tls()
#define FREE_TLS        DLL::FreeTls()
#else
#define SET_TLS(a)      UserSvr::DllSetTls( 0xE8ABABAB, a)     
#define GET_TLS         UserSvr::DllTls(0xE8ABABAB)
#define FREE_TLS        UserSvr::DllFreeTls(0xE8ABABAB)
#endif

NONSHARABLE_CLASS(TMLogger)
    {
public:
    inline static void LogEnd()
        {
        TMLogger* logger = (TMLogger*) GET_TLS;
        if( logger )
            {        
            logger->iFile.Close();
            logger->iFs.Close();
            delete GET_TLS;  
            FREE_TLS;
            }
        }

    inline static void Log1( const TDesC16& aFmt )
        {
        TMLogger* logger = (TMLogger*) GET_TLS;

        if( !logger )
            {
            logger = new TMLogger;
            logger->iFs.Connect();
            BaflUtils::EnsurePathExistsL( logger->iFs, KLogFilePath );
            if( logger->iFile.Create( logger->iFs, KLogFilePath, EFileWrite ) )
                logger->iFile.Open( logger->iFs, KLogFilePath, EFileWrite );
            SET_TLS( logger );
            }

        TInt pos( 0 );
        logger->iFile.Seek( ESeekEnd, pos );

        HBufC8* buf8 = HBufC8::NewLC( aFmt.Length() );
        buf8->Des().Copy( aFmt );
        logger->iFile.Write( *buf8 );
        logger->iFile.Write( KNewLine );
        CleanupStack::PopAndDestroy( buf8 );
        }

    inline static void Log( TRefByValue<const TDesC16> aFmt, ... )
        {
        TMLogger* logger = (TMLogger*) GET_TLS;

        if( !logger )
            {
            logger = new TMLogger;
            logger->iFs.Connect();
            BaflUtils::EnsurePathExistsL( logger->iFs, KLogFilePath );
            if( logger->iFile.Create( logger->iFs, KLogFilePath, EFileWrite ) )
                logger->iFile.Open( logger->iFs, KLogFilePath, EFileWrite );
            SET_TLS( logger );
            }
        TInt pos( 0 );
        logger->iFile.Seek( ESeekEnd, pos );

        VA_LIST list;
        VA_START(list,aFmt);

        TBuf16<KLogBufferSize> tmpBuf;
        tmpBuf.AppendFormatList( aFmt, list );
        HBufC8* buf8 = HBufC8::NewLC( tmpBuf.Length() );
        buf8->Des().Copy( tmpBuf );

        logger->iFile.Write( *buf8 );
        logger->iFile.Write( KNewLine );
        CleanupStack::PopAndDestroy( buf8 );
        VA_END(list);
        }

    inline static void Log1( const TDesC8& aFmt )
        {
        TMLogger* logger = (TMLogger*) GET_TLS;

        if( !logger )
            {
            logger = new TMLogger;
            logger->iFs.Connect();
            BaflUtils::EnsurePathExistsL( logger->iFs, KLogFilePath );
            if( logger->iFile.Create( logger->iFs, KLogFilePath, EFileWrite ) )
                logger->iFile.Open( logger->iFs, KLogFilePath, EFileWrite );
            SET_TLS( logger );
            }

        TInt pos( 0 );
        logger->iFile.Seek( ESeekEnd, pos );
        logger->iFile.Write( aFmt );
        logger->iFile.Write( KNewLine );
        }

    inline static void Log( TRefByValue<const TDesC8> aFmt, ... )
        {
        TMLogger* logger = (TMLogger*) GET_TLS;

        if( !logger )
            {
            logger = new TMLogger;
            logger->iFs.Connect();
            BaflUtils::EnsurePathExistsL( logger->iFs, KLogFilePath );
            if( logger->iFile.Create( logger->iFs, KLogFilePath, EFileWrite ) )
                logger->iFile.Open( logger->iFs, KLogFilePath, EFileWrite );
            SET_TLS( logger );
            }

        TInt pos( 0 );
        logger->iFile.Seek( ESeekEnd, pos );

        VA_LIST list;
        VA_START(list,aFmt);
        TBuf8<KLogBufferSize> tmpBuf;
        tmpBuf.AppendFormatList( aFmt, list );
        logger->iFile.Write( tmpBuf );
        logger->iFile.Write( KNewLine );
        VA_END(list);
        }

    RFs iFs;
    RFile iFile;
    };

NONSHARABLE_CLASS(CFunctionGuard) : public CBase
    {
public:
    CFunctionGuard( const TDesC16& aFunc )
        {
        iFunc = HBufC::New( aFunc.Length() );

        if( iFunc )
            {
            iFunc->Des().Copy( aFunc );
            }

        TMLogger::Log( _L("-->[%S]"), iFunc );
        }

    ~CFunctionGuard()
        {
        TMLogger::Log( _L("<--[%S]"), iFunc );
        delete iFunc;
        }
public:
    HBufC* iFunc;
    };

#define _T8( a )                            TPtrC8( (const TText8*)(a) )
#define TM_LOG_FN_GUARD( a1 )   CFunctionGuard NoNamed( _L(a1) )
#define TM_LOG_1( a1 )          TMLogger::Log( _L(a1) )
#define TM_LOG_2( a1, a2 )      TMLogger::Log( _L(a1), a2 )
#define TM_LOG_3( a1, a2, a3 )  TMLogger::Log( _L(a1), a2, a3 )

#define TM_LOG8_1( a1 )          TMLogger::Log( _T8(a1) )
#define TM_LOG8_2( a1, a2 )      TMLogger::Log( _T8(a1), a2 )
#define TM_LOG8_3( a1, a2, a3 )  TMLogger::Log( _T8(a1), a2, a3 )
#define TM_LOG_END              TMLogger::LogEnd()

#else 

#define TM_LOG_FN_GUARD( a1 )
#define TM_LOG_1( a1 )
#define TM_LOG_2( a1, a2 )
#define TM_LOG_3( a1, a2, a3 )
#endif

#endif // TMLOG_H
