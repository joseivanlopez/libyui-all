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

   File:       NCFileSelection.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCFileSelection.h"
#include "NCTable.h"
#include "NCi18n.h"

#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <string.h> // strerror()
#include <sys/types.h>

/*
  Textdomain "ncurses"
*/



NCFileInfo::NCFileInfo( string 	fileName,
			struct stat64 *	statInfo,
			bool 	link )
{
    _name   = fileName;
    _mode   = statInfo->st_mode;
    _device = statInfo->st_dev;
    _links  = statInfo->st_nlink;
    _size   = statInfo->st_size;
    _mtime  = statInfo->st_mtime;

    if ( link )
    {
	char tmpName[PATH_MAX+1];
	// get actual file name
	int len = readlink( fileName.c_str(), tmpName, PATH_MAX );
	if ( len >= 0 )
	{
	    tmpName[len] = '\0';
	    _realName = tmpName;
	}
	_tag = " @";	// set tag
    }
    else if ( S_ISREG(_mode)
	      && (_mode & S_IXUSR) )
	_tag = " *";	// user executable files
    else
	_tag = "  ";

    // get user and group name
    struct passwd * pwdInfo  = getpwuid( statInfo->st_uid );
    if ( pwdInfo )
	_user = pwdInfo->pw_name;

    struct group * groupInfo = getgrgid( statInfo->st_gid );
    if ( groupInfo )
	_group = groupInfo->gr_name;
    
    if ( _mode & S_IRUSR )
	_perm += "r";
    else
	_perm += "-";
    if ( _mode & S_IWUSR )
	_perm += "w";
    else
	_perm += "-";
    if ( _mode & S_IXUSR )
	_perm += "x";
    else
	_perm += "-";

    if ( _mode & S_IRGRP )
	_perm += "r";
    else
	_perm += "-";
    if ( _mode & S_IWGRP )
	_perm += "w";
    else
	_perm += "-";
    if ( _mode & S_IXGRP )
	_perm += "x";
    else
	_perm += "-";

    if ( _mode & S_IROTH )
	_perm += "r";
    else
	_perm += "-";
    if ( _mode & S_IWOTH )
	_perm += "w";
    else
	_perm += "-";
    if ( _mode & S_IXOTH )
	_perm += "x";
    else
	_perm += "-";
}



NCFileInfo::NCFileInfo( )
{
    _name   = "";
    _realName = "";
    _tag    = "";
    _perm   = "";
    _user   = "";
    _group  = "";
    _mode   = (mode_t)0;
    _device = (dev_t)0;
    _links  = (nlink_t)0;
    _size   = (off64_t)0;
    _mtime  = (time_t)0;
}



NCFileSelectionTag::NCFileSelectionTag( const NCFileInfo & info )
   : NCTableCol( NCstring( "  " ), SEPARATOR )
   , fileInfo( info )
{

}



void NCFileSelectionTag::DrawAt( NCursesWindow & w, const wrect at,
				 NCTableStyle & tableStyle,
				 NCTableLine::STATE linestate,
				 unsigned colidx ) const
{
    NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );

    w.addch( at.Pos.L, at.Pos.C, fileInfo._tag.c_str()[0] );
    w.addch( at.Pos.L, at.Pos.C +1, fileInfo._tag.c_str()[1] );
}



NCFileSelection::NCFileSelection( YWidget * parent,
				  YTableHeader * tableHeader,
				  NCFileSelectionType type,
				  const string & iniDir )
    : NCTable( parent, tableHeader )
      , startDir( iniDir )
      , currentDir( iniDir )
      , tableType( type )
{
    SetSepChar( ' ' );
    setTextdomain( "ncurses" );

    struct stat64 statInfo;
    if ( !iniDir.empty() )
    {
	stat64( iniDir.c_str(), &statInfo );
    }
    
    if ( iniDir.empty()
	 || !S_ISDIR(statInfo.st_mode) )
    {
	char wDir[PATH_MAX+1];		// <limits.h>
    
	// start with working directory
	if ( getcwd( wDir, PATH_MAX ) )
	{
	    startDir = wDir;
	    currentDir = wDir; 
	}
	else
	{
	    startDir = "/";
	    currentDir = "/";
	}
    }
    
    yuiDebug() << endl;
}




NCFileSelection::~NCFileSelection()
{
    yuiDebug() << endl;
}



// getCurrentLine()
// returns the currently selected line 
string  NCFileSelection::getCurrentLine( )
{
    int index = getCurrentItem();

    if ( index != -1 )
    {
	NCFileInfo info = getFileInfo( index );
	return info._name;
    }
    else
    {
	return "";
    }
}



void NCFileSelection::setCurrentDir()
{
    string selected = getCurrentLine();
    yuiMilestone() << "Current line: " << selected << endl;
    if ( selected != ".." )
    {
	if ( startDir != "/" )
	{
	    currentDir = startDir + "/" + selected;
	}
	else
	{
	    currentDir = startDir + selected;
	}
    }
    else
    {
	size_t pos;
	if ( ( pos = currentDir.find_last_of("/") ) != 0 )
	{
	    currentDir = currentDir.substr( 0, pos );
	}
	else
	{
	    currentDir = "/";
	}
    }
}



void NCFileSelection::addLine( const vector<string> & elements,
			       const NCFileInfo & info )
{
    vector<NCTableCol*> Items( elements.size()+1, 0 );
    
    // fill first column (containing the file information)
    Items[0] = new NCFileSelectionTag( info );

    for ( unsigned i = 1; i < elements.size()+1; ++i ) {
	// use NCstring to enforce recoding from 'utf8'
	Items[i] = new NCTableCol( NCstring( elements[i-1] ), NCTableCol::PLAIN );
    }
    myPad()->Append( Items );
}



void NCFileSelection::deleteAllItems()
{
    return NCTable::deleteAllItems();  
}

// createListEntry
bool NCFileTable::createListEntry ( const NCFileInfo & fileInfo )
{
    vector<string> data;

    switch ( tableType )
    {
	case T_Overview: {
	    data.reserve(2);
	    data.push_back( fileInfo._name );
	    break;
	}
	case T_Detailed: {
	    data.reserve(6);
	    data.push_back( fileInfo._name );
	    char size_buf[50];
	    sprintf( size_buf, "%lld", fileInfo._size);
	    data.push_back( size_buf );
	    data.push_back( fileInfo._perm );
	    data.push_back( fileInfo._user );
	    data.push_back( fileInfo._group );
	    break;
	}
	default: {
	    data.reserve(2);
	    data.push_back( " " );
	    data.push_back( " " );
	    break;
	}
    }
	
    addLine( data, fileInfo );

    return true;
}

// createListEntry
bool NCDirectoryTable::createListEntry ( const NCFileInfo & fileInfo )
{
    vector<string> data;

    switch ( tableType )
    {
	case T_Overview: {
	    data.reserve(2);
	    data.push_back( fileInfo._name );
	    break;
	}
	case T_Detailed: {
	    data.reserve(4);
	    data.push_back( fileInfo._name );
	    data.push_back( fileInfo._perm );
	    data.push_back( fileInfo._user );
	    data.push_back( fileInfo._group );
	    break;
	}
	default: {
	    data.reserve(2);
	    data.push_back( " " );
	    data.push_back( " " );
	    break;
	}
    }
	
    addLine( data, fileInfo );

    return true;
}



NCFileInfo NCFileSelection::getFileInfo( int index )
{
    // get the tag 
    NCFileSelectionTag *cc = getTag( index );
    if ( !cc )
	return NCFileInfo( );

    return cc->getFileInfo();
}



NCFileSelectionTag * NCFileSelection::getTag( const int & index )
{
    // get the table line 
    NCTableLine * cl = myPad()->ModifyLine( index );
    if ( !cl )
	return 0;

    // get first column (the column containing the status info)
    NCFileSelectionTag * cc = static_cast<NCFileSelectionTag *>( cl->GetCol( 0 ) );

    return cc;
}





NCFileTable::NCFileTable( YWidget * parent,
			  YTableHeader * tableHeader,
			  NCFileSelectionType type,
			  const string & filter,
			  const string & iniDir )
    : NCFileSelection( parent, tableHeader, type, iniDir )
{
    //fillHeader();

    string filterStr = filter;
    const string delims( " \t" );
    string::size_type begin, end;

    begin = filterStr.find_first_not_of( delims );
    
    while ( begin != string::npos )
    {
	end = filterStr.find_first_of( delims, begin );
	if ( end == string::npos )
	{
	    end = filterStr.length();
	}
	pattern.push_back( filterStr.substr( begin, end-begin ) );
	begin = filterStr.find_first_not_of( delims, end );
    }
}

// fillHeader
// Fillup the column headers of the file table 
void NCFileTable::fillHeader( )
{
    vector<string> header;

    switch ( tableType )
    {
	case T_Overview: {
	    header.reserve(2);
	    header.push_back( "L" + string("  ") );
	    // column header name of the file 
	    header.push_back( "L" + string(_( "File name" )) );
	    break;
	}
	case T_Detailed: {
	    header.reserve(6);
	    header.push_back( "L" + string("  ") );
    	    // column header name of the file 
	    header.push_back( "L" + string(_( "File name" )) );
	    // column header size of the file  
	    header.push_back( "L" + string(_( "Size" )) );
	    // column header file permissions 
	    header.push_back( "L" + string(_( "Permissions" )) );
	    // column header user
	    header.push_back( "L" + string(_( "User" )) );
	    // column header group
	    header.push_back( "L" + string(_( "Group" )) );
	    break;
	}
	default: {
	    header.reserve(2);
	    header.push_back( "L" + string("   ") );
	    header.push_back( "L" + string(_( "File name" )) );
	    break;
	}
    }
    setHeader( header );
}



bool NCFileTable::filterMatch( const string & fileEntry )
{
    if ( pattern.empty() )
	return true;
	
    bool match = false;
    list<string>::iterator it = pattern.begin();
    while ( it != pattern.end() )
    {
	if ( fnmatch ( (*it).c_str(), fileEntry.c_str(), FNM_PATHNAME ) == 0 )
	    match = true;
	++it;
    }
    return match;
}



NCursesEvent NCFileSelection::handleKeyEvents( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none; 

    if ( sendKeyEvents() &&
	 (key == KEY_LEFT || key == KEY_RIGHT) )
    {
	ret = NCursesEvent::key;
	switch ( key ) {
	    case KEY_LEFT:
		ret.keySymbol = "CursorLeft";
		break;
	    case KEY_RIGHT:
		ret.keySymbol = "CursorRight";
		break;
	}
    }
    return ret;
}




NCursesEvent NCFileTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = handleKeyEvents( key );

    // return key event
    if ( ret == NCursesEvent::key )
	return ret;
  
    // call handleInput of NCPad
    handleInput( key );

    currentFile = getCurrentLine();

    switch ( key )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_HOME:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_END: {
	    ret = NCursesEvent::SelectionChanged;
	    ret.result = currentFile;
	    break;
	}
	default:
	    ret = NCursesEvent::none;
    }

    yuiDebug() << "CURRENT_FILE: " << currentFile << endl;
    return ret;
}



bool NCFileTable::fillList ( )
{
    struct stat64 	statInfo;
    struct stat64	linkInfo;
    struct dirent *	entry;
    list<string>	tmpList;
    list<string>::iterator   it;

    fillHeader();	// create the column headers
	
    DIR * diskDir = opendir( currentDir.c_str() );
    
    if ( ( diskDir = opendir( currentDir.c_str() ) ) )
    {
	deleteAllItems();
	while ( ( entry = readdir( diskDir ) ) )
	{
	    string entryName = entry->d_name;
	    if ( entryName != "."
		 && filterMatch( entryName ) )
	    {
		tmpList.push_back( entryName );
	    }
	}
	// sort the list and fill the table widget with file entries
	tmpList.sort( );
	it = tmpList.begin();
	while ( it != tmpList.end() )
	{
	    string fullName = currentDir + "/" + (*it);

	    if ( lstat64( fullName.c_str(), &statInfo ) == 0 )
	    {
		if ( S_ISREG( statInfo.st_mode ) )
		{
		    if ( ( (*it) == ".." && currentDir != "/" )
			 || (*it) != ".." )
		    {
			createListEntry( NCFileInfo( (*it), &statInfo ) );
		    }
		}
		else if ( S_ISLNK( statInfo.st_mode ) )	
		{
		    if ( stat64( fullName.c_str(), &linkInfo ) == 0 )
		    {
			if ( S_ISREG( linkInfo.st_mode ) )
			{
			    createListEntry( NCFileInfo( (*it), &linkInfo, true ) );
			}
		    }
		}
	    }	
	    ++it;
	}
	drawList();		// draw the list

	if ( getNumLines() > 0 )
	{
	    setCurrentItem( 0 );	// set focus to the first list entry
	    currentFile = getCurrentLine();
	}
	else
	{
	    currentFile = "";
	}
    }
    else
    {
	yuiError() << "ERROR opening directory: " << currentDir << " errno: "
		   << strerror( errno ) << endl;
	return false;
    }
    closedir( diskDir );
    
    return true;
}




NCDirectoryTable::NCDirectoryTable( YWidget * parent,
				    YTableHeader * tableHeader,
				    NCFileSelectionType type,
				    const string & iniDir )
    : NCFileSelection( parent, tableHeader, type, iniDir )
{
    //fillHeader();    
}

// fillHeader
// Fillup the column headers of the table 
void NCDirectoryTable::fillHeader( )
{
    vector<string> header;

    switch ( tableType )
    {
	case T_Overview: {
	    header.reserve(2);
	    header.push_back( "L" + string("  ") );
	    // column header name of diretcory
	    header.push_back( "L" + string(_( "Directory Name" )) );
	    break;
	}
	case T_Detailed: {
	    header.reserve(5);
	    header.push_back( "L" + string("  ") );
    	    // column header name of diretcory
	    header.push_back( "L" + string(_( "Directory Name" )) );
	    header.push_back( "L" + string(_( "Permissions" )) );
	    header.push_back( "L" + string(_( "User" )) );
	    header.push_back( "L" + string(_( "Group" )) );
	    break;
	}
	default: {
	    header.reserve(2);
	    header.push_back( "L" + string("   ") );
	    header.push_back( "L" + string(_( "Directory Name" )) );
	    break;
	}
    }
    setHeader( header );
}



bool NCDirectoryTable::fillList ( )
{
    struct stat64 	statInfo;
    struct stat64	linkInfo;
    struct dirent *	entry;
    list<string>	tmpList;
    list<string>::iterator   it;

    fillHeader();	// create the column headers
	
    DIR * diskDir = opendir( currentDir.c_str() );
    
    if ( ( diskDir = opendir( currentDir.c_str() ) ) )
    {
	deleteAllItems();
	while ( ( entry = readdir( diskDir ) ) )
	{
	    string entryName = entry->d_name;
	    if ( entryName != "." )
	    {
		tmpList.push_back( entryName );
	    }
	}
	
	// sort the list and fill the table widget with directory entries
	tmpList.sort( );
	it = tmpList.begin();
	while ( it != tmpList.end() )
	{
	    string fullName = currentDir + "/" + (*it);
	    if ( lstat64( fullName.c_str(), &statInfo ) == 0 )
	    {
		if ( S_ISDIR( statInfo.st_mode ) )
		{
		    if ( ( (*it) == ".." && currentDir != "/" )
			 || (*it) != ".." )
		    {
			createListEntry( NCFileInfo( (*it), &statInfo ) );
		    }
		}
		else if ( S_ISLNK( statInfo.st_mode ) )	
		{
		    if ( stat64( fullName.c_str(), &linkInfo ) == 0 )
		    {
			if ( S_ISDIR( linkInfo.st_mode ) )
			{
			    createListEntry( NCFileInfo( (*it), &linkInfo, true ) );
			}
		    }
		}
	    }	
	    ++it;
	}
	drawList();		// draw the list
	startDir = currentDir;	// set start directory

	if ( getNumLines() > 0 )
	    setCurrentItem( 0 );	// set focus to the first list entry
    }
    else
    {
	yuiError() << "ERROR opening directory: " << currentDir << " errno: "
	      << strerror( errno ) << endl;
	return false;
    }
    closedir( diskDir );
    
    return true;
}



NCursesEvent NCDirectoryTable::wHandleInput( wint_t key )
{
    NCursesEvent ret = handleKeyEvents( key );

    // return key event
    if ( ret == NCursesEvent::key )
	return ret;
    
    unsigned int old_pos = getCurrentItem();

    // call handleInput of NCPad
    handleInput( key );

    switch ( key )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_HOME: 	{
	    if ( old_pos != 0 )
	    {
		setCurrentDir();
		ret = NCursesEvent::SelectionChanged;
		ret.result = currentDir;
	    }
	    break;
	}
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_END: {
	    	setCurrentDir();
		ret = NCursesEvent::SelectionChanged;
		ret.result = currentDir;
		break;
	}
	case KEY_RETURN:
	case KEY_SPACE: {
	    setCurrentDir();
	    ret = NCursesEvent::Activated;
	    ret.result = currentDir;
	    break;
	}
	default:
	    ret = NCursesEvent::none;
    }

    yuiDebug() << "CURRENT: " << currentDir << " START DIR: " << startDir << endl;
    return ret;
}



