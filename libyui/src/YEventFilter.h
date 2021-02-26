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

  File:		YEventFilter.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YEventFilter_h
#define YEventFilter_h


#include "ImplPtr.h"


class YEvent;
class YDialog;

class YEventFilterPrivate;


/**
 * Abstract base class to filter events.
 *
 * This class can be used to examine events just before they are delivered to
 * the application. This is most useful for higher-level widgets or for
 * libraries that need to react to certain events and either consume them, have
 * them delivered unchanged to the application, or exchange an event with
 * another one.
 *
 * A YEventFilter belongs to one specific dialog. Each dialog can have any
 * number of event filters. Each of those event filters is called (its
 * YEventFilter::filter() method) for each event inside
 * YDialog::waitForEvent(). The order in which event filters are called is
 * undefined.
 *
 * YEventFilter objects should be created with 'new' (on the heap). Since an
 * YEventFilter registers itself with its dialog, the dialog will delete it in
 * its destructor if it still exists after all child widgets are deleted.
 *
 * Thus, it is safe to store a pointer to an YEventFilter until the
 * corresponding dialog is deleted. After that, the pointer becomes invalid.
 *
 * See YHelpButtonHandler in YDialog.cc for an example.
 **/
class YEventFilter
{
protected:
    /**
     * Constructor.
     *
     * This registers the event filter with the specified dialog. The dialog
     * assumes ownership of this object and will delete it in its destructor
     * (unless this object is destroyed before that time).
     *
     * If 'dialog' is 0, YDialog::currentDialog() is used (which can throw a
     * YUINoDialogException if there is no dialog).
     **/
    YEventFilter( YDialog * dialog = 0 );

public:
    /**
     * Destructor.
     *
     * This will unregister this object with its dialog.
     **/
    virtual ~YEventFilter();

    /**
     * The heart of the matter: The event filter function.
     * Derived classes are required to implement this.
     *
     * This method can inspect the event it receives.  Hint: event->widget()
     * is typically the most interesting information.
     *
     * This method can react on individual events and
     *
     *   - consume the event (i.e., return 0)
     *   - pass the event through unchanged (simply return the event)
     *   - create a new event (typically based on data in the received event).
     *
     * If 0 or a new event (another value than 'event') is returned, the old
     * event is deleted. If a value different from 'event' or 0 is returned,
     * that value is assumed to be a pointer to a newly created event. The
     * dialog will assume ownership of that event and delete it when
     * appropriate.
     *
     * Note: Never delete 'event' in this method! Return 0 or a new event
     * instead; the caller will take care of deleting the old event.
     **/
    virtual YEvent * filter( YEvent * event ) = 0;

    /**
     * Return the dialog this event filter belongs to.
     **/
    YDialog * dialog() const;

private:

    ImplPtr<YEventFilterPrivate> priv;
};


#endif // YEventFilter_h
