/*
========================================================================
 Name        : d_traceredirect.h
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

#ifndef __D_TRACEREDIRECT_H__
#define __D_TRACEREDIRECT_H__

#include <e32cmn.h>
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

class TCapsTestV01
    {
public:
    TVersion iVersion;
    };

class RTraceRedirect : public RBusLogicalChannel
    {
public:
    enum TControl
        {};
    enum TRequest
        {
        ERequestReadTrace = 0x01,
        ERequestReadCancel = 0x02,
        };
public:
#ifndef __KERNEL_MODE__
    inline TInt Open();
    inline TInt Read( TRequestStatus& aStatus, TDes8 &aDes, TInt aLength );
    inline void CancelRead();
#endif
    };

#ifndef __KERNEL_MODE__

inline TInt RTraceRedirect::Open()
    {
    return DoCreate( _L("TRACEREDIRECTION"), TVersion(0, 1, 1), KNullUnit, NULL, NULL );
    }

inline TInt RTraceRedirect::Read( TRequestStatus& aStatus, TDes8 &aDes, TInt aMaxLength )
    {
    
    DoRequest( ERequestReadTrace, aStatus, (TAny *)&aDes, (TAny *)aMaxLength );
    return KErrNone;
    }

inline void RTraceRedirect::CancelRead()
    {
    return DoCancel( ERequestReadCancel );
    }
#endif

#endif

