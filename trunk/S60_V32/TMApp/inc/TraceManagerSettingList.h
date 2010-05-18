/*
========================================================================
 Name        : TraceManagerSettingList.h
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


#ifndef TRACEMANAGERSETTING_H
#define TRACEMANAGERSETTING_H

#include <aknsettingitemlist.h>

class MEikCommandObserver;
class TTraceManagerSettings;

/**
 * @class	CTmSettingList TraceManagerSettingList.h
 */
class CTmSettingList : public CAknSettingItemList
    {
public:
    // constructors and destructor

    CTmSettingList( TTraceManagerSettings& settings,
            MEikCommandObserver* aCommandObserver );
    
    virtual ~CTmSettingList();
    
    static CTmSettingList* NewL( TTraceManagerSettings& aSettings,
            MEikCommandObserver* aCommandObserver  );
    
public:

    // from CCoeControl
    void HandleResourceChange( TInt aType );

    // overrides of CAknSettingItemList
    CAknSettingItem* CreateSettingItemL( TInt id );
    
    void EditItemL( TInt aIndex, TBool aCalledFromMenu );
    
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    // utility function for menu
    void ChangeSelectedItemL();

private:
    // override of CAknSettingItemList
    void SizeChanged();
    void ConstructL();
    
private:
    // current settings values
    TTraceManagerSettings& iSettings;
    MEikCommandObserver* iCommandObserver;
    };
#endif // TRACEMANAGERSETTING_H
