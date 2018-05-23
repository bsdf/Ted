#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<Xm/PushB.h>

/************************************************************************/
/*									*/
/*  Make a in a form containing a row of buttons.			*/
/*									*/
/************************************************************************/

void appMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan,
				int			showAsDefault )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	labelString;
    Widget	button;

    labelString= XmStringCreateLocalized( (char *)text );

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ colspan ); ac++;

    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		0 ); ac++;

    if  ( showAsDefault )
	{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 1 ); ac++;
	XtSetArg( al[ac], XmNshowAsDefault, showAsDefault != 0 ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
	}

    button= XmCreatePushButton( row, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    if  ( showAsDefault )
	{
	XtVaSetValues( row,
			XmNdefaultButton,	button,
			NULL );
	}

    *pButton= button; return;
    }

void appMakeButtonInColumn(	APP_WIDGET *		pButton,
				APP_WIDGET		column,
				const char *		buttonText,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			showAsDefault )
    {
    Widget		button;

    XmString		labelString;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( (char *)buttonText );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    if  ( showAsDefault )
	{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 1 ); ac++;
	XtSetArg( al[ac], XmNshowAsDefault, showAsDefault != 0 ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
	}

    button= XmCreatePushButton( column, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    if  ( showAsDefault )
	{
	XtVaSetValues( column,
			XmNdefaultButton,	button,
			NULL );
	}

    appMotifTurnOfSashTraversal( column );

    *pButton= button; return;
    }


void appGuiChangeButtonText(	APP_WIDGET	button,
				const char *	label )
    {
    appGuiChangeLabelText( button, label );
    }

#   endif
