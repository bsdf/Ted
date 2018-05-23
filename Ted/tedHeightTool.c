/************************************************************************/
/*									*/
/*  Widget to select a height on the Format Tool.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appUnit.h>

#   include	"tedHeightTool.h"
#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Enable a height chooser.						*/
/*									*/
/************************************************************************/

void tedFormatEnableHeightChooser(	HeightChooser *	hc,
					int		enabled )
    {
    hc->hcEnabled= enabled;

    guiEnableText( hc->hcText, enabled && hc->hcHow != HC_FREE );
    guiEnableWidget( hc->hcRow, enabled );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh a height chooser.						*/
/*									*/
/************************************************************************/

void tedFormatRefreshHeightChooser(	HeightChooser *	hc,
					int		height )
    {
    if  ( height == 0 )
	{
	guiEnableText( hc->hcText, 0 );

	hc->hcHow= HC_FREE;
	appStringToTextWidget( hc->hcText, "" );
	}
    else{
	guiEnableText( hc->hcText, hc->hcEnabled );

	if  ( height > 0 )
	    {
	    hc->hcHow= HC_AT_LEAST;
	    appLengthToTextWidget( hc->hcText,  height, UNITtyPOINTS );
	    }
	else{
	    hc->hcHow= HC_EXACTLY;
	    appLengthToTextWidget( hc->hcText, -height, UNITtyPOINTS );
	    }
	}

    appSetOptionmenu( &(hc->hcOptionmenu), hc->hcHow );
    }

int tedFormatToolGetHeight(	int *		pHeight,
				HeightChooser *	hc )
    {
    int			sign;

    int			height;
    int			changed;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    switch( hc->hcHow )
	{
	case HC_FREE:
	    sign= height= 0;
	    break;
	case HC_AT_LEAST:
	    sign=  1;
	    break;
	case HC_EXACTLY:
	    sign= -1;
	    break;
	default:
	    LDEB(hc->hcHow); return -1;
	}

    height= *pHeight;
    height *= sign;

    if  ( sign != 0 )
	{
	if  ( appGetLengthFromTextWidget( hc->hcText,
				&height, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1; }

	height *= sign;
	}

    *pHeight= height;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Callback for a height chooser.					*/
/*									*/
/************************************************************************/

void tedFormatHeightChosen(	int			how,
				HeightChooser *		hc,
				int			defaultValue )
    {
    int			val;

    switch( how )
	{
	case HC_FREE:
	    guiEnableText( hc->hcText, 0 );
	    appStringToTextWidget( hc->hcText, "" );
	    hc->hcHow= how;
	    return;

	case HC_AT_LEAST:
	    guiEnableText( hc->hcText, 1 );
	    val= defaultValue;
	    if  ( val < 0 )
		{ val= -val;	}
	    appLengthToTextWidget( hc->hcText, val, UNITtyPOINTS );
	    hc->hcHow= how;
	    return;

	case HC_EXACTLY:
	    guiEnableText( hc->hcText, 1 );
	    val= defaultValue;
	    if  ( val < 0 )
		{ val= -val;	}
	    appLengthToTextWidget( hc->hcText, val, UNITtyPOINTS );
	    hc->hcHow= how;
	    return;

	default:
	    LDEB(how); return;
	}
    }

int tedHeightToolValidateDimension(	int *			pValue,
					int *			pChanged,
					const HeightChooser *	hc,
					int			orig )
    {
    int				value;
    int				changed= 0;

    if  ( tedFormatValidateDimension( &value, &changed, hc->hcText, orig ) )
	{ return -1;	}

    *pValue= value;
    *pChanged= changed;
    return 0;
    }

void tedFormatMakeHeightRow(	void *				through,
				APP_WIDGET			parent,
				HeightChooser *			hc,
				APP_TXACTIVATE_CALLBACK_T	textCallback,
				OptionmenuCallback		menuCallback )
    {
    const int	menuColumn= 0;
    const int	menuColspan= 1;
    const int	textColumn= menuColumn+ menuColspan;
    const int	textColspan= 1;
    const int	heightResizable= 0;

    hc->hcEnabled= 1;

    hc->hcRow= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeOptionmenuInRow( &(hc->hcOptionmenu), hc->hcRow,
						menuColumn, menuColspan,
						menuCallback, through );

    appMakeTextInRow( &(hc->hcText), hc->hcRow,
				    textColumn, textColspan, 10, 0 );
    if  ( textCallback )
	{
	appGuiSetGotValueCallbackForText( hc->hcText, textCallback, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the menu in a height chooser.					*/
/*									*/
/************************************************************************/

void tedFormatFillHeightChooser(	HeightChooser *		hc,
					const char *		freeText,
					const char *		atLeastText,
					const char *		exactlyText )
    {
    appEmptyOptionmenu( &(hc->hcOptionmenu) );

    hc->hcMenuItems[HC_FREE]= appAddItemToOptionmenu( &(hc->hcOptionmenu),
								freeText );

    hc->hcMenuItems[HC_AT_LEAST]= appAddItemToOptionmenu( &(hc->hcOptionmenu),
								atLeastText );

    hc->hcMenuItems[HC_EXACTLY]= appAddItemToOptionmenu( &(hc->hcOptionmenu),
								exactlyText );

    appSetOptionmenu( &(hc->hcOptionmenu), HC_FREE );
    hc->hcHow= HC_FREE;

    appOptionmenuRefreshWidth( &(hc->hcOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Initialize.								*/
/*									*/
/************************************************************************/

void tedInitHeightChooser(	HeightChooser *		hc )
    {
    int		i;

    appInitOptionmenu( &(hc->hcOptionmenu) );
    hc->hcText= (APP_WIDGET)0;

    for ( i= 0; i < HC__COUNT; i++ )
	{ hc->hcMenuItems[i]= (APP_WIDGET)0;	}

    hc->hcHow= HC_FREE;
    hc->hcEnabled= 0;
    }

