/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgFilterContainer.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgFilterContainer_h
#define NCPkgFilterContainer_h

#include <iosfwd>

#include <string>
#include <set>

#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCPackageSelector.h"


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterContainer
//
//	DESCRIPTION :
//
class NCPkgFilterContainer : public NCPkgTable {

    NCPkgFilterContainer & operator=( const NCPkgFilterContainer & );
    NCPkgFilterContainer            ( const NCPkgFilterContainer & );

private:

    NCPackageSelector * packager;		// connection to the PackageSelector,

protected:

    std::string getCurrentLine();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:

    enum SelType {
	S_Pattern,
	S_Language,
	S_Unknown
    };
    
    NCPkgFilterContainer( YWidget *parent, YTableHeader *header, NCPackageSelector * pkg, SelType type );
    virtual ~NCPkgFilterContainer();
    
    void createLayout( YWidget *parent, const string & label );

    /**
     * Fills the list with the available selections (and the status info)
     * @return bool
     */
    bool fillContainerList (  SelType type );

    bool showContainerContent(  const set<string> & wanted );
    string showDescription( ZyppObj objPtr );	
   
    /**
     * Shows the popup with the add ons (package categories).
     * @return NCursesEvent
     */
    void showContainerPackages( );

private:
    
    SelType type;
};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterContainer_h