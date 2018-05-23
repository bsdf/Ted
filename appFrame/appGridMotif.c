#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/Form.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/SashP.h>

/************************************************************************/
/*									*/
/*  Tame the sashes in an XmPanedWindow.				*/
/*									*/
/************************************************************************/

void appMotifTurnOfSashTraversal(	Widget		column )
    {
    WidgetList		children;
    Cardinal		childCount;
    int			i;

    XtVaGetValues( column,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    for ( i= 0; i < childCount; children++, i++ )
	{
	if  ( XmIsSash( children[0] ) )
	    {
	    XtVaSetValues(	children[0],
				    XmNtraversalOn,	False,
				    XmNsensitive,	False,
				    NULL );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row inside a column.						*/
/*									*/
/************************************************************************/

APP_WIDGET appMakeRowInColumn(	APP_WIDGET	column,
				int		columnCount,
				int		heightResizable )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		row;

    ac= 0;
    XtSetArg( al[ac],	XmNfractionBase,	columnCount ); ac++;
    XtSetArg( al[ac],	XmNskipAdjust,		heightResizable == 0 ); ac++;

    XtSetArg( al[ac],	XmNallowResize,		True ); ac++;

    row= XmCreateForm( column, WIDGET_NAME, al, ac );

    XtManageChild( row );

    appMotifTurnOfSashTraversal( column );

    return row;
    }

/************************************************************************/
/*									*/
/*  Make a column inside a row.						*/
/*									*/
/************************************************************************/

void appMakeColumnInRow(	APP_WIDGET *	pColumn,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		column;

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ colspan ); ac++;

    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;

    column= XmCreatePanedWindow( row, WIDGET_NAME, al, ac );

    XtManageChild( column );

    *pColumn= column; return;
    }

void appMakeHBoxInRow(		APP_WIDGET *	pHbox,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		hbox;

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ colspan ); ac++;

    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;

    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;

    hbox= XmCreateForm( row, WIDGET_NAME, al, ac );

    XtManageChild( hbox );

    *pHbox= hbox; return;
    }

#   endif
