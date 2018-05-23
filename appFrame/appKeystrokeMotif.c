#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>
#   include	<uniUtf8.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<X11/keysym.h>

static void appGuiDebugKey(	const char *		how,
				XKeyPressedEvent *	keyEvent,
				APP_KEY_VALUE		keysym,
				char *			b )
    {
    const char *	keyString= XKeysymToString( keysym );
    int			state= keyEvent->state;

    if  ( ! keyString )
	{ keyString= "<Unknown>";	}

    appDebug( "KEY: %s (%s%s%s%s%s%s%s%s<Key>%s): ",
		APP_X11EventNames[keyEvent->type],
		(state&KEY_SHIFT_MASK)?"Shift":"",
		(state&KEY_CONTROL_MASK)?"Ctrl":"",
		(state&LockMask)?"Lock":"",
		(state&Mod1Mask)?"Mod1":"",
		(state&Mod2Mask)?"Mod2":"",
		(state&Mod3Mask)?"Mod3":"",
		(state&Mod4Mask)?"Mod4":"",
		(state&Mod5Mask)?"Mod5":"",
		keyString );

    if  ( b && b[0] )
	{ appDebug( "XLookup%s -> \"%s\" (0x%02x)\n", how, b, b[0] );	}
    else{ appDebug( "XLookup%s -|\n", how );				}
    }

static void appGuiHandleKeysym(		DocumentWidget *	dw,
					char *			scratch,
					APP_KEY_VALUE		keysym,
					int			state )
    {
    if  ( keysym != 0			&&
	  ( keysym & 0xff00 ) != 0xff00 )
	{
	int	unicode= appKeysymX11ToUnicode( keysym );

	if  ( unicode >= 0 )
	    {
	    int	step= uniPutUtf8( scratch, unicode );
	    if  ( step < 1 )
		{ LLDEB(unicode,step);	}
	    else{
		scratch[step]= '\0';
		(*dw->dwGotString)( dw->dwOwner, (char *)scratch, step );
		return;
		}
	    }
	}

    (*dw->dwGotKey)( dw->dwOwner, keysym, state );

    return;
    }

static APP_EVENT_HANDLER_H( appKeyPressed, w, voiddw, event )
    {
    DocumentWidget *		dw= (DocumentWidget *)voiddw;

    APP_KEY_VALUE		keysym;
    XKeyPressedEvent *		keyEvent= &(event->xkey);

    char			scratch[12+1];

    static int			debugKeys= 0;

    if  ( debugKeys == 0 )
	{
	if  ( getenv( "TED_DEBUG_KEYS" ) )
	    { debugKeys=  1;	}
	else{ debugKeys= -1;	}
	}

    if  ( event->type != KeyPress )
	{ LLDEB(event->type,KeyPress); return;	}

    keysym= 0;
    scratch[0]= '\0';

    if  ( dw->dwInputContext )
	{
	Status	status;

	(void) /* gotString= */ XmbLookupString( dw->dwInputContext, keyEvent,
				(char *)scratch, sizeof(scratch)- 1,
				&keysym, &status );
	switch( status )
	    {
	    case XBufferOverflow:
		LSDEB(status,"XBufferOverflow"); return;
	    case XLookupNone:
		LSDEB(status,"XLookupNone"); return;
	    case XLookupChars:
		LSDEB(status,"XLookupChars"); return;
	    default:
		LSDEB(status,"default"); return;

	    case XLookupKeySym:
	    case XLookupBoth:
		scratch[0]= '\0';
		appGuiHandleKeysym( dw, scratch, keysym, keyEvent->state );
		if  ( debugKeys > 0 )
		    {
		    appGuiDebugKey( status==XLookupBoth?"Both":"KeySym",
						keyEvent, keysym, scratch );
		    }
		return;
	    }
	}
    else{
	(void) /* gotString= */ XLookupString( keyEvent,
				(char *)scratch, sizeof(scratch)- 1,
				&keysym, (XComposeStatus *)0 );

	scratch[0]= '\0';
	appGuiHandleKeysym( dw, scratch, keysym, keyEvent->state );
	if  ( debugKeys > 0 )
	    { appGuiDebugKey( ":NoIm", keyEvent, keysym, scratch );	}

	return;
	}

    }

/************************************************************************/
/*									*/
/*  Set an old-fashined X11 Input style for accented characters etc.	*/
/*									*/
/************************************************************************/

static XIMStyle	appXimPreferences[]=
    {
    XIMPreeditCallbacks,
    XIMPreeditPosition,
    XIMPreeditArea,
    XIMPreeditNothing,
    XIMStatusCallbacks,
    XIMStatusArea,
    XIMStatusNothing,
    };

void appDocumentSetInputContext(	APP_INPUT_METHOD	im,
					DocumentWidget *	dw )
    {
    XtAddEventHandler( dw->dwWidget, KeyPressMask, False,
						appKeyPressed, (void *)dw );

    if  ( im )
	{
	XIMStyles *	availableStyles= (XIMStyles *)0;
	XIMStyle	supportedStyles;
	XIMStyle	likedStyle;
	int		style;

	XGetIMValues( im,
		XNQueryInputStyle, &availableStyles,
		NULL );

	supportedStyles=	XIMPreeditArea		|
				XIMPreeditNothing	|
				XIMPreeditNone		|
				XIMStatusArea		|
				XIMStatusNothing	|
				XIMStatusNone		;

	likedStyle= 0;
	for ( style= 0; style < availableStyles->count_styles; style++ )
	    {
	    XIMStyle	foundStyle= availableStyles->supported_styles[style];
	    unsigned	j;

	    if  ( ( supportedStyles & foundStyle ) != foundStyle )
		{ continue;	}

	    if  ( ! likedStyle )
		{ likedStyle= foundStyle; continue;	}

	    for ( j= 0; j < sizeof(appXimPreferences)/sizeof(XIMStyle); j++ )
		{
		if  (   ( foundStyle & appXimPreferences[j] )	&&
		      ! ( likedStyle & appXimPreferences[j] )	)
		    { likedStyle= foundStyle; break;	}
		}
	    }

	if  ( likedStyle )
	    {
	    dw->dwInputContext= XCreateIC( im,
		XNClientWindow,		XtWindow( dw->dwWidget ),
		XNInputStyle,		likedStyle,
		/*
		XNPeeditAttributes,	attributeList,
		XNStatusAttributes,	attributeList,
		*/
		NULL );
	    }
	else{ dw->dwInputContext= (XIC)0;	}

	if  ( availableStyles )
	    { XFree( availableStyles );	}
	}
    else{ dw->dwInputContext= (XIC)0;	}

    return;
    }

#   endif
