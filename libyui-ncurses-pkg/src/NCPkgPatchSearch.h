/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


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

   File:       NCPkgPatchSearch.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgPatchSearch_h
#define NCPkgPatchSearch_h

#include <iosfwd>
#include <vector>
#include <string>

#include <yui/ncurses/NCCheckBox.h>
#include <yui/ncurses/NCComboBox.h>
#include <yui/ncurses/NCLabel.h>
#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCPushButton.h>
#include <yui/ncurses/NCRichText.h>


class NCPackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPatchSearch
//
//	DESCRIPTION :
//
class NCPkgPatchSearch : public NCPopup
{
    NCPkgPatchSearch & operator=( const NCPkgPatchSearch & );
    NCPkgPatchSearch            ( const NCPkgPatchSearch & );

private:

    NCComboBox * searchExpr;

    YItem * searchName;
    YItem * searchSum;

    NCPushButton * cancelButton;
    NCPushButton * okButton;

    NCPackageSelector * packager;		// connection to the package selector

protected:

    std::string getSearchExpression() const;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgPatchSearch( const wpos at, NCPackageSelector *pkger );
    virtual ~NCPkgPatchSearch();

    virtual int preferredWidth();
    virtual int preferredHeight();

    void createLayout( const std::string & headline );

    NCursesEvent & showSearchPopup();

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPatchSearch_h
