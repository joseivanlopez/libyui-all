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

  File:	      YQTree.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQTree_h
#define YQTree_h

#include <QFrame>
#include <QTreeWidget>

#include "YTree.h"


class YQWidgetCaption;
class YQTreeItem;


class YQTree : public QFrame, public YTree
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQTree( YWidget * parent, const string & label );

    /**
     * Destructor.
     **/
    virtual ~YQTree();

    /**
     * Change the label text.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void setLabel( const string & label );

    /**
     * Rebuild the displayed tree from the internally stored YTreeItems.
     *
     * The application should call this (once) after all items have been added
     * with addItem(). YTree::addItems() calls this automatically.
     *
     * Implemented from YTree.
     **/
    virtual void rebuildTree();

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Deselect all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Set enabled/disbled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();


protected slots:

    void slotContextMenu ( const QPoint & pos );


    /**
     * Propagate a tree item selection.
     *
     * This will trigger a 'SelectionChanged' event if 'notify' is set.
     **/
    void slotSelectionChanged();

    /**
     * Propagate a double click or pressing the space key on a tree item.
     *
     * This will trigger an 'Activated' event if 'notify' is set.
     **/
    void slotActivated( QTreeWidgetItem * item);

    /**
     * Propagate an "item expanded" event to the underlying YTreeItem.
     **/
    void slotItemExpanded( QTreeWidgetItem * item );
    
    /**
     * Propagate an "item collapsed" event to the underlying YTreeItem.
     **/
    void slotItemCollapsed( QTreeWidgetItem * item );


protected:

    /**
     * Select an item via the corresponding YQTreItem.
     **/
    void selectItem( YQTreeItem * item );

    /**
     * Open the branch of 'item' recursively to its toplevel item.
     **/
    void openBranch( YQTreeItem * item );

    /**
     * Build a tree of items that will be displayed (YQTreeItems) from the
     * original items between iterators 'begin' and 'end' as child items of
     * 'parentItem' (or as toplevel items if 'parentItem' is 0).
     **/
    void buildDisplayTree( YQTreeItem *		parentItem,
			   YItemIterator	begin,
			   YItemIterator 	end );

    //
    // Data members
    //

    YQWidgetCaption *	_caption;
    QTreeWidget * 	_qt_treeWidget;
    int			_nextSerialNo;
};


class YQTreeItem: public QTreeWidgetItem
{
public:

    /**
     * Constructor for a top level item.
     **/
    YQTreeItem( YQTree	*	tree,
		QTreeWidget *	parent,
		YTreeItem *	origItem,
		int		serial	);

    /**
     * Constructor for a non-top level item.
     **/
    YQTreeItem( YQTree	*	tree,
		YQTreeItem *	parent,
		YTreeItem *	origItem,
		int		serial	);

    /**
     * Returns the original YTreeItem of which this item is a clone.
     **/
    YTreeItem * origItem() { return _origItem; }

    /**
     * Open this item.
     *
     * Reimplemented from QTreeWidgetItem.
     **/
    virtual void setOpen( bool open );


private:

    /**
     * Init function. All constructors end up here.
     **/
    void init( YQTree *		tree,
	       YTreeItem *	yTreeItem,
	       int		serial );


protected:

    /**
     * Sort key of this item.
     *
     * Reimplemented from QTreeWidgetItem.
     **/
    QString key( int column, bool ascending ) const;


    //
    // Data members
    //

    YQTree *	_tree;
    YTreeItem *	_origItem;
    int 	_serialNo;

};


#endif // ifndef YQTree_h
