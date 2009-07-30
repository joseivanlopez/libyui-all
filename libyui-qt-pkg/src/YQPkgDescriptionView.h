/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgDescriptionView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgDescriptionView_h
#define YQPkgDescriptionView_h

#include <QUrl>
#include "YQPkgGenericDetailsView.h"

using std::list;
using std::string;


/**
 * @short Display the description of a ZyppObj derived object along with its
 * name and summary.
 **/
class YQPkgDescriptionView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgDescriptionView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgDescriptionView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

    /**
     * Get the document pointed to by a hyperlink.
     *
     * Reimplemented from QTextBrowser to avoid having an empty text each time
     * the user clicks on a hyperlink.
     **/
    virtual void setSource( const QUrl & name );

protected:

    /**
     * Format a multi-line text into paragraphs
     **/
    QString simpleHtmlParagraphs( QString text );
    
    /**
     * Show information for a hyperlinked object,
     * e.g., a "pkg:somepkg" link to another package.
     **/
    void showLink( const QUrl & url );

    /**
     * Return html text that contains a list of application icons.
     **/
    QString applicationIconList( const list<string> & fileList ) const;

    /**
     * Find absolute file name (incl. path) for a icon.
     **/
    QString findDesktopIcon ( const QString& iconName ) const;

    /**
     * Extract name, icon and exec attributes from a desktop file.
     **/
    QMap<QString, QString> readDesktopFile( const QString & fileName ) const;

    /**
     * Search for all desktop files in a file list.
     **/
    QStringList findDesktopFiles( const list<string> & fileList ) const;

    /**
     * Initialize the language code (lang).
     **/
    void initLang();

private:
    QString langWithCountry;
    QString lang;

};


#endif // ifndef YQPkgDescriptionView_h