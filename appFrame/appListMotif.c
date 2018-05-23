#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/List.h>

/************************************************************************/
/*									*/
/*  List specific functionality.					*/
/*									*/
/************************************************************************/

void appGuiMakeListInColumn(	Widget *		pList,
				Widget			column,
				int			visibleItems,
				APP_LIST_CALLBACK_T	listCallback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    Widget		list;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		False ); ac++;
    XtSetArg( al[ac], XmNallowResize,		False ); ac++;
    if  ( visibleItems > 0 )
	{
	XtSetArg( al[ac], XmNvisibleItemCount,	visibleItems ); ac++;
	}

    XtSetArg( al[ac], XmNwidth,			25 ); ac++;

    if  ( visibleItems >= 0 )
	{
	XtSetArg( al[ac], XmNscrollBarDisplayPolicy,	XmAS_NEEDED ); ac++;
	XtSetArg( al[ac], XmNlistSizePolicy,		XmCONSTANT ); ac++;

	list= XmCreateScrolledList( column, WIDGET_NAME, al, ac );
	}
    else{
	list= XmCreateList( column, WIDGET_NAME, al, ac );
	}

    XtManageChild( list );

    if  ( listCallback )
	{
	XtAddCallback( list, XmNbrowseSelectionCallback,
						listCallback, through );
	}

    if  ( actionCallback )
	{
	XtAddCallback( list, XmNdefaultActionCallback,
						actionCallback, through );
	}

    appMotifTurnOfSashTraversal( column );

    *pList= list; return;
    }

void appGuiEmptyListWidget(	Widget			list )
    {
    XmListDeselectAllItems( list );
    XmListDeleteAllItems( list );

    return;
    }

void appGuiDeletePositionFromListWidget(	APP_WIDGET	list,
						int		pos )
    {
    XmListDeletePos( list, pos+ 1 );
    }

void appGuiReplaceValueInListWidget(	APP_WIDGET	list,
					int		pos,
					const char *	value )
    {
    int *	selected= (int *)0;
    int		selectedCount= 0;
    int		selectNew= 0;

    XmString	labelString;

    if  ( XmListGetSelectedPos( list, &selected, &selectedCount ) )
	{
	int		i;

	for ( i= 0; i < selectedCount; i++ )
	    {
	    if  ( selected[i] == pos+ 1 )
		{ selectNew= 1; break;	}
	    }
	}

    labelString= XmStringCreateLocalized( (char *)value );

    if  ( selectNew )
	{
	XmListAddItem( list, labelString, pos+ 1 );	
	XmListDeselectPos( list, pos+ 1 );
	}
    else{ XmListAddItemUnselected( list, labelString, pos+ 1 );	}

    XmStringFree( labelString );

    XmListDeletePos( list, pos+ 2 );
    }

void appGuiAddValueToListWidget(	Widget		list,
					int		pos,
					const char *	value )
    {
    XmString	labelString;

    labelString= XmStringCreateLocalized( (char *)value );

    if  ( pos < 0 )
	{ XmListAddItemUnselected( list, labelString, 0 );	}
    else{ XmListAddItemUnselected( list, labelString, pos+ 1 );	}

    XmStringFree( labelString );

    return;
    }

static void appGuiScrollListToPosMotif(		APP_WIDGET	list,
						int		pp1 )
    {
    int		firstVisible;
    int		nVisible;

    /* If the selected item is not visible, scroll the list */
    XtVaGetValues( list,
		XmNtopItemPosition,	&firstVisible,
		XmNvisibleItemCount,	&nVisible,
		NULL);

    if  ( pp1 < firstVisible )
	{ XmListSetPos( list, pp1 );			}
    if  ( pp1 >= firstVisible+ nVisible )
	{ XmListSetPos( list, pp1- nVisible+ 1 );	}

    return;
    }

void appGuiSelectPositionInListWidget(		APP_WIDGET	list,
						int		position )
    {
    int		pp1= position+ 1;	/*  Silly Motif counts from 1	*/

    XmListSelectPos( list, pp1, False );

    /* If the selected item is not visible, scroll the list */
    appGuiScrollListToPosMotif( list, pp1 );

    return;
    }

void appGuiSelectValueInListWidget(		APP_WIDGET	list,
						const char *	value )
    {
    XmString	labelString;
    int *	selected= (int *)0;
    int		selectedCount= 0;

    labelString= XmStringCreateLocalized( (char *)value );

    XmListSelectItem( list, labelString, False );

    XmStringFree( labelString );

    if  ( XmListGetSelectedPos( list, &selected, &selectedCount ) )
	{
	appGuiScrollListToPosMotif( list, selected[0] );
	}

    return;
    }

void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list )
    {
    XmListDeselectAllItems( list );

    return;
    }

int appGuiGetPositionFromListCallback(		APP_WIDGET	list,
						void *		voidlcs )
    {
    XmListCallbackStruct *	lcs= (XmListCallbackStruct *)voidlcs;

    return lcs->item_position- 1;
    }

char * appGuiGetStringFromListCallback(		APP_WIDGET	list,
						void *		voidlcs )
    {
    XmListCallbackStruct *	lcs= (XmListCallbackStruct *)voidlcs;
    char *			text= (char *)0;

    XmStringGetLtoR( lcs->item, XmFONTLIST_DEFAULT_TAG, &text );

    return text;
    }

void appFreeStringFromListCallback(	char *		s )
    { XtFree( s );	}

#   endif
