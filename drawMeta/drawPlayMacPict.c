#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"drawWinMetaImpl.h"
#   include	"drawMacPictImpl.h"
#   include	<utilMatchFont.h>
#   include	<sioEndian.h>
#   include	<uniLegacyEncoding.h>
#   include	<uniUtf8.h>
#   include	<psDocumentFontStyle.h>
#   include	<bmio.h>

#   include	<math.h>

#   include	<appDebugon.h>

# if 0
#    define	PICTDEB(x)	(x)
#    define	TRANSLATE_NAMES	1
# else
#    define	PICTDEB(x)	/*nothing*/
#    define	TRANSLATE_NAMES	0
# endif

# if TRANSLATE_NAMES
static const char * appMacPictCommentName( int		n )
    {
    static char	scratch[22];

    switch( n )
	{
	case PICTCMT_Proprietary:	return "Proprietary";
	case PICTCMT_MacDrawBegin:	return "MacDrawBegin";
	case PICTCMT_MacDrawEnd:	return "MacDrawEnd";
	case PICTCMT_GroupedBegin:	return "GroupedBegin";
	case PICTCMT_GroupedEnd:	return "GroupedEnd";
	case PICTCMT_BitmapBegin:	return "BitmapBegin";
	case PICTCMT_BitmapEnd:		return "BitmapEnd";
	case PICTCMT_ArrowAtEnd:	return "ArrowAtEnd";
	case PICTCMT_ArrowAtStart:	return "ArrowAtStart";
	case PICTCMT_ArrowBothEnds:	return "ArrowBothEnds";
	case PICTCMT_ArrowEnd:		return "ArrowEnd";
	case PICTCMT_SetGray:		return "SetGray";
	case PICTCMT_TextBegin:		return "TextBegin";
	case PICTCMT_TextEnd:		return "TextEnd";
	case PICTCMT_StringBegin:	return "StringBegin";
	case PICTCMT_StringEnd:		return "StringEnd";
	case PICTCMT_TextCenter:	return "TextCenter";
	case PICTCMT_LineLayoutOff:	return "LineLayoutOff";
	case PICTCMT_LineLayoutOn:	return "LineLayoutOn";
	case PICTCMT_ClientLineLayout:	return "ClientLineLayout";
	case PICTCMT_PolyBegin:		return "PolyBegin";
	case PICTCMT_PolyEnd:		return "PolyEnd";
	case PICTCMT_PolyIgnore:	return "PolyIgnore";
	case PICTCMT_PolySmooth:	return "PolySmooth";
	case PICTCMT_PolyClose:		return "PolyClose";
	case PICTCMT_RotateBegin:	return "RotateBegin";
	case PICTCMT_RotateEnd:		return "RotateEnd";
	case PICTCMT_RotateCenter:	return "RotateCenter";
	case PICTCMT_DashedLine:	return "DashedLine";
	case PICTCMT_DashedStop:	return "DashedStop";
	case PICTCMT_SetLineWidth:	return "SetLineWidth";
	case PICTCMT_PostScriptBegin:	return "PostScriptBegin";
	case PICTCMT_PostScriptEnd:	return "PostScriptEnd";
	case PICTCMT_PostScriptHandle:	return "PostScriptHandle";
	case PICTCMT_PostScriptFile:	return "PostScriptFile";
	case PICTCMT_TextIsPostScript:	return "TextIsPostScript";
	case PICTCMT_ResourcePS:	return "ResourcePS";
	case PICTCMT_PSBeginNoSave:	return "PSBeginNoSave";
	case PICTCMT_FormsPrinting:	return "FormsPrinting";
	case PICTCMT_EndFormsPrinting:	return "EndFormsPrinting";
	case PICTCMT_CMBeginProfile:	return "CMBeginProfile";
	case PICTCMT_CMEndProfile:	return "CMEndProfile";
	case PICTCMT_CMEnableMatching:	return "CMEnableMatching";
	case PICTCMT_CMDisableMatching:	return "CMDisableMatching";
	}

    sprintf( scratch, "%d", n );
    return scratch;
    }
# endif

/************************************************************************/

static int appMacPictGetRect(		DocumentRectangle *	dr,
					SimpleInputStream *	sis )
    {
    int		done= 0;

    dr->drY0= sioEndianGetBeInt16( sis ); done += 2;
    dr->drX0= sioEndianGetBeInt16( sis ); done += 2;
    dr->drY1= sioEndianGetBeInt16( sis ); done += 2;
    dr->drX1= sioEndianGetBeInt16( sis ); done += 2;

    return done;
    }

/************************************************************************/

static int appMacPictGetOvSize(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    dc->dcMacPictRRectOvalHigh= sioEndianGetBeInt16( sis );
    dc->dcMacPictRRectOvalWide= sioEndianGetBeInt16( sis );

    PICTDEB(appDebug( "OvSize( %d, %d )\n",
			    dc->dcMacPictRRectOvalWide,
			    dc->dcMacPictRRectOvalHigh ));

    return 0;
    }

static int appMacPictGetArc(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    double		as;
    double		ae;
    int			x;
    int			y;
    int			dx;
    int			dy;

    appMacPictGetRect( &(dc->dcMacPictArcRect), sis );

    dc->dcMacPictArcA0= sioEndianGetBeInt16( sis );
    dc->dcMacPictArcA1= sioEndianGetBeInt16( sis );

    as= 90.0- dc->dcMacPictArcA0;
    ae= 90.0- ( dc->dcMacPictArcA0+ dc->dcMacPictArcA1 );

    as= ( M_PI* as )/ 180.0;
    ae= ( M_PI* ae )/ 180.0;

    x= ( dc->dcMacPictArcRect.drX0+ dc->dcMacPictArcRect.drX1 )/ 2;
    y= ( dc->dcMacPictArcRect.drY0+ dc->dcMacPictArcRect.drY1 )/ 2;
    dx= dc->dcMacPictArcRect.drX1- dc->dcMacPictArcRect.drX0;
    dy= dc->dcMacPictArcRect.drY1- dc->dcMacPictArcRect.drY0;

    dc->dcMacPictArcXs= x+ 0.5* dx* cos( as );
    dc->dcMacPictArcYs= y+ 0.5* dy* sin( as );
    dc->dcMacPictArcXe= x+ 0.5* dx* cos( ae );
    dc->dcMacPictArcYe= y+ 0.5* dy* sin( ae );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve a color.							*/
/*									*/
/************************************************************************/

static int appMacPictGetColor(	RGB8Color *		rgb8,
				SimpleInputStream *	sis )
    {
    unsigned int	r= sioEndianGetBeUint16( sis );
    unsigned int	g= sioEndianGetBeUint16( sis );
    unsigned int	b= sioEndianGetBeUint16( sis );

    rgb8->rgb8Red= r/ 256;
    rgb8->rgb8Green= g/ 256;
    rgb8->rgb8Blue= b/ 256;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a pattern.							*/
/*									*/
/************************************************************************/

static int appMacPictGetPattern(	DeviceContext *		dc,
					SimpleInputStream *	sis,
					RGB8Color *		pSolidColor,
					int *			pIsSolid,
					unsigned char		pattern[8] )
    {
    int		i;
    int		isSolid;
    /*
    int		gray= 0xff;
    */

    pattern[0]= sioInGetByte( sis );
    isSolid= ( pattern[0] == 0x00 || pattern[0] == 0xff );

    /*
    if  ( pattern[0] == 0xff )
	{ gray= 0x00;	}
    */

    for ( i= 1; i < 8; i++ )
	{
	pattern[i]= sioInGetByte( sis );

	if  ( pattern[i] != pattern[0] )
	    { isSolid= 0;	}
	}

    if  ( isSolid )
	{
	if  ( pattern[0] == 0xff )
	    { *pSolidColor= dc->dcMacPictForeColor;	}
	else{
	    if  ( pattern[0] == 0x00 )
		{ *pSolidColor= dc->dcMacPictBackColor;	}
	    else{ XDEB(pattern[0]);			}
	    }
	}

    *pIsSolid= isSolid;
    return 0;
    }

static int appMacPictGetShortLine(	int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			y0= sioEndianGetBeInt16( sis );
    int			x0= sioEndianGetBeInt16( sis );
    int			y1;
    int			x1;
    int			c;

    c= sioInGetByte( sis );
    if  ( c & 0x80 )
	{ x1= x0+ c- 256;	}
    else{ x1= x0+ c;		}

    c= sioInGetByte( sis );
    if  ( c & 0x80 )
	{ y1= y0+ c- 256;	}
    else{ y1= y0+ c;		}

    PICTDEB(appDebug( "ShortLine( %d+ %d, %d+ %d )\n",
				    x0, x1- x0, y0, y1- y0 ));

    dc->dcX= x1;
    dc->dcY= y1;

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

static int appMacPictGetShortLineFrom(	int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			y0= dc->dcY;
    int			x0= dc->dcX;
    int			y1;
    int			x1;
    int			c;

    c= sioInGetByte( sis );
    if  ( c & 0x80 )
	{ x1= x0+ c- 256;	}
    else{ x1= x0+ c;		}

    c= sioInGetByte( sis );
    if  ( c & 0x80 )
	{ y1= y0+ c- 256;	}
    else{ y1= y0+ c;		}

    PICTDEB(appDebug( "ShortLineFrom( %d+ %d, %d+ %d )\n",
				    x0, x1- x0, y0, y1- y0 ));

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

static int appMacPictGetLineFrom(	int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int		y0= sioEndianGetBeInt16( sis );
    int		x0= sioEndianGetBeInt16( sis );
    int		y1= dc->dcY;
    int		x1= dc->dcX;

    PICTDEB(appDebug( "LineFrom( %d, %d )\n", x0, y0 ));

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

static int appMacPictGetLine(	int *			pX0,
				int *			pY0,
				int *			pX1,
				int *			pY1,
				DeviceContext *		dc,
				SimpleInputStream *	sis )
    {
    int			y0= sioEndianGetBeInt16( sis );
    int			x0= sioEndianGetBeInt16( sis );
    int			y1= sioEndianGetBeInt16( sis );
    int			x1= sioEndianGetBeInt16( sis );

    PICTDEB(appDebug( "Line( %d, %d, %d, %d )\n", x0, y0, x1, y1 ));

    dc->dcX= x1;
    dc->dcY= y1;

    *pX0= x0;
    *pY0= y0;
    *pX1= x1;
    *pY1= y1;

    return 0;
    }

/************************************************************************/

static int appMacPictAddFont(		DeviceContext *		dc,
					const char *		name )
    {
    int			rval= 0;

    int			fontNum;
    int			fontStyle;

    DocumentFont	dfNew;

    docInitDocumentFont( &dfNew );

    if  ( docFontSetFamilyName( &dfNew, name ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fontStyle= psFontFamilyStyle( name );
    if  ( docFontSetFamilyStyle( &dfNew, fontStyle ) )
	{ SLDEB(name,fontStyle); rval= -1; goto ready; }

    fontNum= docMergeFontIntoFontlist( &(dc->dcFontList), &dfNew );
    if  ( fontNum < 0 )
	{ SLDEB(name,fontNum); rval= -1; goto ready; }

    if  ( dc->dcFont.lfTextAttribute.taFontNumber != fontNum )
	{
	dc->dcFont.lfTextAttribute.taFontNumber= fontNum;
	dc->dcFont.lfPrivateFont= -1;
	dc->dcAfi= (AfmFontInfo *)0;
	}

  ready:
    docCleanDocumentFont( &dfNew );

    return rval;
    }

static int appMacPictGetFontName(	DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			rval= 0;

    int			dataLength= sioEndianGetBeInt16( sis );
    int			oldFontId= sioEndianGetBeInt16( sis );
    int			nameLength= sioInGetByte( sis );
    int			nbytes= 5;

    int			i;
    char *		to;
    char *		fontName= (char *)0;

    oldFontId= oldFontId; /*  Avoid compiler warning */

    fontName= to= (char *)malloc( nameLength+ 1 );
    if  ( ! fontName || ! to )
	{ LXDEB(nameLength,fontName); rval= -1; goto ready;	}

    for ( i= 0; i < nameLength; to++, i++ )
	{ *to= sioInGetByte( sis ); nbytes++; }
    *to= '\0';

    PICTDEB(appDebug( "FontName( %d, %d, %d, \"%s\" )\n",
	    dataLength, oldFontId, nameLength, fontName ));

    if  ( dataLength > 3+ nameLength )
	{ LLDEB(dataLength,3+nameLength); }

    for ( i= 3+ nameLength; i < dataLength; i++ )
	{ (void) sioInGetByte( sis ); nbytes++;	}

    if  ( dc->dcMacPictVersion > 1 && nbytes % 2 )
	{ (void)sioInGetByte( sis ); nbytes++;	}

    if  ( appMacPictAddFont( dc, fontName ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( fontName )
	{ free( fontName );	}

    return rval;
    }

static int appMacPictGetTxFont(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			rval= 0;

    int			val= sioEndianGetBeInt16( sis );
    const char *	name= "?";

    switch( val )
	{
	case 0:		name= "Chicago";	break;
	case 1:		name= "Helvetica";	break;
	case 2:		name= "Times";		break;
	case 3:		name= "Helvetica";	break;
	case 4:		name= "Courier";	break;
	case 20:	name= "Times";		break;
	case 21:	name= "Helvetica";	break;
	case 22:	name= "Courier";	break;
	case 23:	name= "Symbol";		break;
	case 15011:	name= "Avantgarde";	break;

	default:
	    PICTDEB(appDebug( "TxFont( %d )\n", val ));
	    goto ready;
	}

    PICTDEB(appDebug( "TxFont( %d -> \"%s\" )\n", val, name ));

    if  ( appMacPictAddFont( dc, name ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int appMacPictGetTxFace(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    unsigned int	val= sioInGetByte( sis );

    int			isBold= 0;
    int			isSlanted= 0;

    if  ( val & 1 )
	{ isBold= 1; val &= ~1;	}
    if  ( val & 2 )
	{ isSlanted= 1; val &= ~2;	}

    if(val)XDEB(val);

    PICTDEB(appDebug( "TxFace( 0x%02x )%s%s\n", val,
					isBold?"":" BOLD",
					isSlanted?"":" SLANTED" ));

    if  ( dc->dcMacPictVersion > 1 )
	{ (void)sioInGetByte( sis );	}

    if  ( dc->dcFont.lfTextAttribute.taFontIsBold != isBold )
	{
	dc->dcFont.lfTextAttribute.taFontIsBold= isBold;
	dc->dcFont.lfPrivateFont= -1;
	dc->dcAfi= (AfmFontInfo *)0;
	}

    if  ( dc->dcFont.lfTextAttribute.taFontIsSlanted != isSlanted )
	{
	dc->dcFont.lfTextAttribute.taFontIsSlanted= isSlanted;
	dc->dcFont.lfPrivateFont= -1;
	dc->dcAfi= (AfmFontInfo *)0;
	}

    return 0;
    }

static int appMacPictGetGlyphState(	DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int	dataLength= sioEndianGetBeInt16( sis );
    int	i;

#   if 0
    int	outlinePreferred= 0;
    int	preserveGlyph= 0;
    int	fractionalWidth= 0;
    int	scalingDisabled= 0;
#   endif

    for ( i= 0; i < dataLength; i++ )
	{
	(void) /* int c= */ sioInGetByte( sis );

#	if 0
	if  ( i == 0 )
	    { outlinePreferred= c;	}
	if  ( i == 1 )
	    { preserveGlyph= c;	}
	if  ( i == 2 )
	    { fractionalWidth= c;	}
	if  ( i == 3 )
	    { scalingDisabled= c;	}
#	endif
	}

    PICTDEB(appDebug( "GlyphState( %d:%x,%x,%x,%x )\n",
				dataLength,
				outlinePreferred,
				preserveGlyph,
				fractionalWidth,
				scalingDisabled ));

    dc->dcX= 0;
    dc->dcY= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve a text string.						*/
/*									*/
/************************************************************************/

static int appMacPictGetString(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			done;
    int			count= sioInGetByte( sis );

    int			nbytes= 1;

    utilEmptyMemoryBuffer( &(dc->dcCollectedText) );

    for ( done= 0; done < count; nbytes++, done++ )
	{
	int		step;
	char		scratch[6+1];
	int		c= sioInGetByte( sis );
	int		unicode;

	if  ( c == 0 )
	    { break;		}

	if  ( c == '\r' )
	    { unicode= '\n';				}
	else{ unicode= uniMacRomanGlyphUnicodes[c];	}

	if  ( unicode < 0 || unicode == 0xffff )
	    { LLDEB(c,unicode); unicode= '?';	}

	step= uniPutUtf8( scratch, unicode );
	if  ( step < 1 || step > 3 )
	    { XLDEB(unicode,step); break;	}

	if  ( utilMemoryBufferAppendBytes( &(dc->dcCollectedText),
				    (const unsigned char *)scratch, step ) )
	    { LDEB(step); return -1;	}
	}

    while( done < count )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(1); return -1;	}

	nbytes++; done++;
	}

    return nbytes;
    }

static int appMacPictGetDHText(	DeviceContext *		dc,
				int *			pDh,
				SimpleInputStream *	sis )
    {
    int		dh= sioInGetByte( sis );
    int		nbytes= 1;
    int		step;

    step= appMacPictGetString( dc, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    nbytes += step;

    if  ( dc->dcMacPictVersion > 1 && nbytes % 2 )
	{ (void)sioInGetByte( sis ); nbytes++;	}

    PICTDEB(appDebug( "DHText( %d, %d: \"%.*s\" )\n", dh,
				dc->dcCollectedText.mbSize,
				dc->dcCollectedText.mbSize,
				(char *)dc->dcCollectedText.mbBytes ));

    *pDh= dh;

    return 0;
    }

static int appMacPictGetDVText(	DeviceContext *		dc,
				int *			pDv,
				SimpleInputStream *	sis )
    {
    int		dv= sioInGetByte( sis );
    int		nbytes= 1;
    int		step;

    step= appMacPictGetString( dc, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    nbytes += step;

    if  ( dc->dcMacPictVersion > 1 && nbytes % 2 )
	{ (void)sioInGetByte( sis ); nbytes++;	}

    PICTDEB(appDebug( "DVText( %d, %d: \"%.*s\" )\n", dv,
				dc->dcCollectedText.mbSize,
				dc->dcCollectedText.mbSize,
				(char *)dc->dcCollectedText.mbBytes ));

    *pDv= dv;

    return 0;
    }

static int appMacPictGetDHDVText(	DeviceContext *		dc,
					int *			pDh,
					int *			pDv,
					SimpleInputStream *	sis )
    {
    int		dh= sioInGetByte( sis );
    int		dv= sioInGetByte( sis );
    int		nbytes= 2;
    int		step;

    step= appMacPictGetString( dc, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    nbytes += step;

    if  ( dc->dcMacPictVersion > 1 && nbytes % 2 )
	{ (void)sioInGetByte( sis ); nbytes++;	}

    PICTDEB(appDebug( "DHDVText( %d, %d, %d: \"%.*s\" )\n", dh, dv,
				dc->dcCollectedText.mbSize,
				dc->dcCollectedText.mbSize,
				(char *)dc->dcCollectedText.mbBytes ));

    *pDh= dh;
    *pDv= dv;

    return 0;
    }

static int appMacPictGetLongText(	DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int		y= sioEndianGetBeInt16( sis );
    int		x= sioEndianGetBeInt16( sis );
    int		nbytes= 4;
    int		step;

    step= appMacPictGetString( dc, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    nbytes += step;

    if  ( dc->dcMacPictVersion > 1 && nbytes % 2 )
	{ (void)sioInGetByte( sis ); nbytes++;	}

    PICTDEB(appDebug( "LongText( %d, %d, %d: \"%.*s\" )\n", x, y,
				dc->dcCollectedText.mbSize,
				dc->dcCollectedText.mbSize,
				(char *)dc->dcCollectedText.mbBytes ));

    dc->dcX= x;
    dc->dcY= y;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the points of a polyline/polygon.				*/
/*									*/
/************************************************************************/

static int appMacPictGetPoly(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			count;
    int			done= 0;
    int			step;
    int			bytes;

    Point2DI *		xp;
    int			i;

    bytes= sioEndianGetBeInt16( sis ); done += 2;

    step= appMacPictGetRect( &(dc->dcMacPictPolyRect), sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    {
    int		left= bytes- done;

    if  ( left % 4 || left <= 4 )
	{ LDEB(left); return -1;	}

    count= left/ 4;
    }

    xp= (Point2DI *)realloc( dc->dcPoints, (count+ 1)* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(count,xp); return -1;	}

    dc->dcPoints= xp;
    dc->dcPointCount= count;

    for ( i= 0; i < count; xp++, i++ )
	{
	int	x, y;

	y= sioEndianGetBeInt16( sis ); done += 2;
	x= sioEndianGetBeInt16( sis ); done += 2;

	xp->x= DC_xViewport( x, y, dc );
	xp->y= DC_yViewport( x, y, dc );
	}

    *xp= dc->dcPoints[0];

    return done;
    }

static int appMacPictGetLongComment(	DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int		i;

    int		kind= sioEndianGetBeInt16( sis );
    int		size= sioEndianGetBeInt16( sis );

    kind= kind;

    PICTDEB(appDebug( "LongComment( %d:%s, %d, .. )\n",
	    kind, appMacPictCommentName( kind ), size ));

    switch( kind )
	{
	case PICTCMT_TextBegin:
	    {
	    if  ( size != 12 )
		{ goto defaultCase;	}

	    (void) /* justify= */ sioEndianGetBeInt16( sis );
	    (void) /* flip= */ sioEndianGetBeInt16( sis );
	    (void) /* angle= */ sioEndianGetBeInt16( sis );
	    (void) /* line= */ sioEndianGetBeInt16( sis );
	    (void) /* cmnt= */ sioEndianGetBeInt16( sis );
	    (void) /* angleFixed= */ sioEndianGetBeInt16( sis );

	    PICTDEB(appDebug(
		"%s: justify=%d, flip=%d, angle=%d, angleFixed=%d\n",
		    appMacPictCommentName( kind ),
		    justify, flip, angle, angleFixed ));
	    }
	    break;

	case  PICTCMT_DashedLine:
	    dc->dcObjects[MACPICThandleFRAME_PEN].mfoLogicalPen.lpStyle= PS_DASH;
	    for ( i= 0; i < size; i++ )
		{ (void) /* c= */ sioInGetByte( sis ); PICTDEB(LDEB(c)); }
	    break;

	case  PICTCMT_DashedStop:
	    if  ( size != 0 )
		{ LDEB(size); goto defaultCase;	}
	    dc->dcObjects[MACPICThandleFRAME_PEN].mfoLogicalPen.lpStyle= PS_SOLID;
	    break;

	case  PICTCMT_SetLineWidth:
	    if  ( size != 4 )
		{ LDEB(size); goto defaultCase;	}
	    {
	    (void) /* int x= */ sioEndianGetBeInt16( sis );
	    (void) /* int y= */ sioEndianGetBeInt16( sis );
	    PICTDEB(appDebug( "LongComment( %d:%s, %d bytes, %d/%d )\n",
		    kind, appMacPictCommentName( kind ), size, x, y ));
	    }
	    break;

	default:
	defaultCase:
	    for ( i= 0; i < size; i++ )
		{ (void) /* c= */ sioInGetByte( sis );	}
	    break;
	}

    if  ( dc->dcMacPictVersion > 1 && size % 2 )
	{ (void)sioInGetByte( sis );	}

    return 0;
    }

/************************************************************************/

static int appMacPictGetTxSize(		DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int		size= sioEndianGetBeInt16( sis );

    PICTDEB(appDebug( "TxSize( %d -> %d )\n", dc->dcFont.lfHeight, size ));

    if  ( dc->dcFont.lfHeight != size )
	{
	dc->dcFont.lfPrivateFont= -1;
	dc->dcAfi= (AfmFontInfo *)0;
	dc->dcFont.lfHeight= size;
	dc->dcFont.lfWidth= ( 65* size )/ 100;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a string.							*/
/*									*/
/************************************************************************/

static int appMacPictDrawString(	DeviceContext *		dc,
					int			dh,
					int			dv,
					void *			through )
    {
    const char *	s= (const char *)dc->dcCollectedText.mbBytes;
    int			count= dc->dcCollectedText.mbSize;

    int			x0;
    int			y0;

    while( count > 0 && isspace( s[count-1] ) )
	{ count--;	}

    if  ( count == 0 )
	{ return 0;	}

    dc->dcX += dh;
    dc->dcY += dv;

    x0= DC_xViewport( dc->dcX, dc->dcY, dc );
    y0= DC_yViewport( dc->dcX, dc->dcY, dc );

    if  ( ! dc->dcAfi )
	{
	if  ( (*dc->dcSelectFontObject)( dc, through, &(dc->dcFont) ) )
	    { LDEB(1); return -1;	}
	}

    if  ( (*dc->dcDrawString)( dc, through, x0, y0, &(dc->dcCollectedText) ) )
	{ LDEB(count); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  MACPICT_PackBitsRect and similar images.				*/
/*									*/
/*  1)  Initialise bitmap.						*/
/*  2)  Get image.							*/
/*  12) Draw image.							*/
/*  13) Ready: clean image related data.				*/
/*									*/
/************************************************************************/

static int appMacPictDrawImage(		DeviceContext *		dc,
					void *			through,
					SimpleInputStream *	sis,
					int			packed,
					int			direct,
					int			clipRegion )
    {
    int			rval= 0;
    int			step;

    DocumentRectangle	drSrc;
    DocumentRectangle	drDest;

    RasterImage	abi;
    DocumentRectangle	drOutput;

    /*  1  */
    bmInitRasterImage( &abi );

    /*  2  */
    step= bmMacPictGetImage( &abi, &drSrc, &drDest,
					    sis, packed, direct, clipRegion );
    if  ( step < 0 )
	{ LDEB(step); rval= -1; goto ready;	}

    /*  11  */
    if  ( dc->dcMacPictVersion > 1 && step % 2 )
	{ (void)sioInGetByte( sis ); step++;	}


    appWinMetaTransformRectangle( &drDest, dc, &drDest );

    if  ( ! geoIntersectRectangle( &drOutput, &drDest, &(dc->dcOutputRect) ) )
	{
	RECTDEB(&drDest);
	RECTDEB(&(dc->dcOutputRect));
	return 0;
	}

    /*  12  */
    if  ( (*dc->dcDrawRasterImage)( dc, through, &abi, &drSrc, &drDest ) )
	{ LDEB(1); return -1;	}

  ready:

    /*  13  */
    bmCleanRasterImage( &abi );

    return rval;
    }

static int appMacPictDrawLine(	DeviceContext *		dc,
				void *			through,
				int			x0,
				int			y0,
				int			x1,
				int			y1 )
    {
    int		count= 2;
    Point2DI	points[2];

    points[0].x= DC_xViewport( x0, y0, dc );
    points[0].y= DC_yViewport( x0, y0, dc );
    points[1].x= DC_xViewport( x1, y1, dc );
    points[1].y= DC_yViewport( x1, y1, dc );

    if  ( (*dc->dcDrawPolyPolygon)( dc, through,
				    1, &count, points, 0, 1, 0 ) )
	{ LDEB(count); return -1;	}

    return 0;
    }

/************************************************************************/

static int appMacPictSetPatternBrush(	DeviceContext *		dc,
					PatternBrush *		pb,
					const unsigned char *	pattern )
    {
    int			foreBlack;
    int			foreWhite;
    int			backBlack;
    int			backWhite;

    foreBlack=	dc->dcMacPictForeColor.rgb8Red == 0 &&
		dc->dcMacPictForeColor.rgb8Green == 0 &&
		dc->dcMacPictForeColor.rgb8Blue == 0;
    foreWhite=	dc->dcMacPictForeColor.rgb8Red == 255 &&
		dc->dcMacPictForeColor.rgb8Green == 255 &&
		dc->dcMacPictForeColor.rgb8Blue == 255;
    backBlack=	dc->dcMacPictBackColor.rgb8Red == 0 &&
		dc->dcMacPictBackColor.rgb8Green == 0 &&
		dc->dcMacPictBackColor.rgb8Blue == 0;
    backWhite=	dc->dcMacPictBackColor.rgb8Red == 255 &&
		dc->dcMacPictBackColor.rgb8Green == 255 &&
		dc->dcMacPictBackColor.rgb8Blue == 255;

    pb->pbAbi= (RasterImage *)malloc( sizeof(RasterImage) );
    if  ( ! pb->pbAbi )
	{ XDEB(pb->pbAbi); return -1;	}
    bmInitRasterImage( pb->pbAbi );
    pb->pbAbi->riBytes= (unsigned char *)malloc( 8 );
    if  ( ! pb->pbAbi->riBytes )
	{ XDEB(pb->pbAbi->riBytes); return -1;	}
    memcpy( pb->pbAbi->riBytes, pattern, 8 );

    pb->pbAbi->riDescription.bdPixelsWide= 8;
    pb->pbAbi->riDescription.bdPixelsHigh= 8;
    pb->pbAbi->riDescription.bdBitsPerSample= 1;
    pb->pbAbi->riDescription.bdXResolution= 1;
    pb->pbAbi->riDescription.bdYResolution= 1;
    pb->pbAbi->riDescription.bdUnit= BMunPIXEL;

    if  ( foreBlack && backWhite )
	{
	pb->pbAbi->riDescription.bdSamplesPerPixel= 1;
	pb->pbAbi->riDescription.bdColorEncoding= BMcoBLACKWHITE;
	}
    else{
	if  ( foreWhite && backBlack )
	    {
	    pb->pbAbi->riDescription.bdSamplesPerPixel= 1;
	    pb->pbAbi->riDescription.bdColorEncoding= BMcoWHITEBLACK;
	    }
	else{
	    ColorPalette *	cp= &(pb->pbAbi->riDescription.bdPalette);

	    pb->pbAbi->riDescription.bdSamplesPerPixel= 3;
	    pb->pbAbi->riDescription.bdColorEncoding= BMcoRGB8PALETTE;

	    if  ( utilPaletteSetCount( cp, 2 ) )
		{ LDEB(2); return -1;	}

	    cp->cpColors[0]= dc->dcMacPictBackColor;
	    cp->cpColors[1]= dc->dcMacPictForeColor;
	    }
	}

    bmCalculateSizes( &(pb->pbAbi->riDescription) );
    pb->pbType= 5;
    pb->pbUsage= 0;
    return 0;
    }

static int appMacPictSetPattern(	DeviceContext *		dc,
					void *			through,
					int			brushHandle,
					int			penHandle,
					int			solid,
					const RGB8Color *	solidColor,
					const unsigned char *	pattern )
    {
    MetaFileObject *	mfo;

    if  ( solid )
	{
	if  ( brushHandle != MACPICThandle_NONE )
	    {
	    mfo= &(dc->dcObjects[brushHandle]);
	    (*dc->dcCleanObject)( dc, through,  mfo );
	    appMetaCleanObject( mfo );

	    mfo->mfoType= MFtypeBRUSH;
	    mfo->mfoLogicalBrush.lbStyle= BS_SOLID;
	    mfo->mfoLogicalBrush.lbColor= *solidColor;
	    mfo->mfoLogicalBrush.lbTilePixmap= (struct DrawingSurface *)0;
	    mfo->mfoLogicalBrush.lbTileNumber= -1;
	    }

	if  ( penHandle != MACPICThandle_NONE )
	    {
	    mfo= &(dc->dcObjects[penHandle]);
	    (*dc->dcCleanObject)( dc, through,  mfo );
	    appMetaCleanObject( mfo );

	    mfo->mfoType= MFtypePEN;
	    mfo->mfoLogicalPen.lpStyle= PS_SOLID;
	    mfo->mfoLogicalPen.lpColor= *solidColor;
	    }
	}
    else{
	if  ( brushHandle != MACPICThandle_NONE )
	    {
	    mfo= &(dc->dcObjects[brushHandle]);
	    (*dc->dcCleanObject)( dc, through,  mfo );
	    appMetaCleanObject( mfo );

	    mfo->mfoType= MFtypePATTERNBRUSH;
	    mfo->mfoPatternBrush.pbTilePixmap= (struct DrawingSurface *)0;
	    mfo->mfoPatternBrush.pbTileNumber= -1;
	    if  ( appMacPictSetPatternBrush( dc, &(mfo->mfoPatternBrush),
						dc->dcMacPictPenPattern ) ) 
		{
		appMetaCleanObject( mfo );
		mfo->mfoType= MFtypeFREE;
		return -1;
		}
	    }
	}

    return 0;
    }

/************************************************************************/

static int appMacPictRefreshDrawingGear(	DeviceContext *	dc,
						void *		through )
    {
    if  ( appMacPictSetPattern( dc, through,
				    MACPICThandlePAINT_BRUSH,
				    MACPICThandleFRAME_PEN,
				    dc->dcMacPictPenIsSolid,
				    &(dc->dcMacPictPenSolid),
				    dc->dcMacPictPenPattern ) )
	{ LDEB(1); return -1;	}

    if  ( appMacPictSetPattern( dc, through,
				    MACPICThandleFILL_BRUSH,
				    MACPICThandle_NONE,
				    dc->dcMacPictFillIsSolid,
				    &(dc->dcMacPictFillSolid),
				    dc->dcMacPictFillPattern ) )
	{ LDEB(1); return -1;	}

    if  ( appMacPictSetPattern( dc, through,
				    MACPICThandleERASE_BRUSH,
				    MACPICThandle_NONE,
				    dc->dcMacPictBackIsSolid,
				    &(dc->dcMacPictBackSolid),
				    dc->dcMacPictBackPattern ) )
	{ LDEB(1); return -1;	}

    return 0;
    }


static int appMacPictChooseGear(	DeviceContext *	dc,
					void *		through,
					int		brushHandle,
					int		penHandle )
    {
    if  ( brushHandle != MACPICThandle_NONE )
	{
	if  ( appWinMetaSelectObject( dc, through, brushHandle ) )
	    { LDEB(brushHandle); return -1;	}
	}
    if  ( penHandle != MACPICThandle_NONE )
	{
	if  ( appWinMetaSelectObject( dc, through, penHandle ) )
	    { LDEB(brushHandle); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Play a macintosh pict file.						*/
/*									*/
/************************************************************************/

int appMacPictPlayPict( DeviceContext *			dc,
			void *				through )
    {
    SimpleInputStream *	sis= dc->dcPlayer->mpInputStream;

    int			rval= 0;

    dc->dcMacPictRRectOvalWide= 20;
    dc->dcMacPictRRectOvalHigh= 20;

    dc->dcFont.lfPrivateFont= -1;
    dc->dcFont.lfHeight= 12;
    dc->dcTextAlignment= TA_LEFT|TA_BASELINE;

    if  ( appMacPictRefreshDrawingGear( dc, through ) )
	{ LDEB(1); return -1;	}

    dc->dcObjects[MACPICThandleFRAME_PEN].mfoLogicalPen.lpWidth= 1;
    dc->dcObjects[MACPICThandleDUMMY_PEN].mfoLogicalPen.lpWidth= 0;

    {
    MetaFileObject *	mfo= &(dc->dcObjects[MACPICThandleDUMMY_BRUSH]);

    mfo->mfoType= MFtypeBRUSH;
    mfo->mfoLogicalBrush.lbStyle= BS_NULL;
    mfo->mfoLogicalBrush.lbTilePixmap= (struct DrawingSurface *)0;
    mfo->mfoLogicalBrush.lbTileNumber= -1;
    }

    {
    MetaFileObject *	mfo= &(dc->dcObjects[MACPICThandleDUMMY_PEN]);

    mfo->mfoType= MFtypePEN;
    mfo->mfoLogicalPen.lpStyle= PS_NULL;
    }

    for (;;)
	{
	unsigned int	opcode;
	int		bytes;

	switch( dc->dcMacPictVersion )
	    {
	    case 1:
		opcode= sioInGetByte( sis );
		break;
	    case 2:
		opcode= sioEndianGetBeUint16( sis );
		break;
	    default:
		LDEB(dc->dcMacPictVersion); return -1;
	    }

	if  ( opcode == EOF )
	    { LDEB(opcode); return -1;	}

	bytes= -1;
	if  ( opcode >= 0x100 && opcode <= 0x7fff )
	    {
	    bytes= 2* ( ( opcode >> 8 ) & 0xff );
	    opcode= opcode & 0xff;
	    }

	switch( opcode )
	    {
	    case MACPICT_NOP:
		PICTDEB(appDebug( "NOP() bytes= %d\n", bytes ));
		continue;

	    case MACPICT_PnSize:
		{
		int w, h;

		h= sioEndianGetBeInt16( sis );
		w= sioEndianGetBeInt16( sis );
		PICTDEB(appDebug( "PnSize( %d, %d )\n", w, h ));

		dc->dcObjects[MACPICThandleFRAME_PEN].mfoLogicalPen.lpWidth=
								( w+ h )/ 2;
		}
		continue;

	    case MACPICT_OvSize:
		if  ( appMacPictGetOvSize( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_DefHilite:
		PICTDEB(appDebug( "DefHilite()\n" ));
		continue;

	    case MACPICT_Clip:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	skip;

		PICTDEB(appDebug( "Clip( %d )\n", regionBytes/ 2 ));

		for ( skip= 2; skip < regionBytes; skip += 2 )
		    {
		    (void) /* regionCoord= */ sioEndianGetBeInt16( sis );
		    }
		}
		continue;

	    case MACPICT_PnMode:
		dc->dcMacPictPenMode= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnMode(%d)\n", dc->dcMacPictPenMode ));

		continue;

	    case MACPICT_PnLocHFrac:
		{
		(void) /* val= */ sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnLocHFrac(%d)\n", val ));

		}
		continue;

	    case MACPICT_RGBFgCol:
		if  ( appMacPictGetColor( &(dc->dcMacPictForeColor), sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBFgCol(%d,%d,%d)\n",
					dc->dcMacPictForeColor.rgb8Red,
					dc->dcMacPictForeColor.rgb8Green,
					dc->dcMacPictForeColor.rgb8Blue ));

		if  ( appMacPictRefreshDrawingGear( dc, through ) )
		    { LDEB(1); return -1;	}

		continue;

	    case MACPICT_RGBBkCol:
		if  ( appMacPictGetColor( &(dc->dcMacPictBackColor), sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBBkCol(%d,%d,%d)\n",
					dc->dcMacPictBackColor.rgb8Red,
					dc->dcMacPictBackColor.rgb8Green,
					dc->dcMacPictBackColor.rgb8Blue ));

		if  ( appMacPictRefreshDrawingGear( dc, through ) )
		    { LDEB(1); return -1;	}

		continue;

	    case MACPICT_PnPat:
		if  ( appMacPictGetPattern( dc, sis, &(dc->dcMacPictPenSolid),
						&(dc->dcMacPictPenIsSolid),
						dc->dcMacPictPenPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"PnPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    dc->dcMacPictPenPattern[0],
					    dc->dcMacPictPenPattern[1],
					    dc->dcMacPictPenPattern[2],
					    dc->dcMacPictPenPattern[3],
					    dc->dcMacPictPenPattern[4],
					    dc->dcMacPictPenPattern[4],
					    dc->dcMacPictPenPattern[6],
					    dc->dcMacPictPenPattern[7] ));

		if  ( appMacPictSetPattern( dc, through,
						MACPICThandlePAINT_BRUSH,
						MACPICThandleFRAME_PEN,
						dc->dcMacPictPenIsSolid,
						&(dc->dcMacPictPenSolid),
						dc->dcMacPictPenPattern ) )
		    { LDEB(1); return -1;	}
		continue;

	    case MACPICT_FillPat:
		if  ( appMacPictGetPattern( dc, sis, &(dc->dcMacPictFillSolid),
						&(dc->dcMacPictFillIsSolid),
						dc->dcMacPictFillPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"FillPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    dc->dcMacPictFillPattern[0],
					    dc->dcMacPictFillPattern[1],
					    dc->dcMacPictFillPattern[2],
					    dc->dcMacPictFillPattern[3],
					    dc->dcMacPictFillPattern[4],
					    dc->dcMacPictFillPattern[4],
					    dc->dcMacPictFillPattern[6],
					    dc->dcMacPictFillPattern[7] ));

		if  ( appMacPictSetPattern( dc, through,
						MACPICThandlePAINT_BRUSH,
						MACPICThandle_NONE,
						dc->dcMacPictFillIsSolid,
						&(dc->dcMacPictFillSolid),
						dc->dcMacPictFillPattern ) )
		    { LDEB(1); return -1;	}

		continue;

	    case MACPICT_BkPat:
		if  ( appMacPictGetPattern( dc, sis, &(dc->dcMacPictBackSolid),
						&(dc->dcMacPictBackIsSolid),
						dc->dcMacPictBackPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"BkPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    dc->dcMacPictBackPattern[0],
					    dc->dcMacPictBackPattern[1],
					    dc->dcMacPictBackPattern[2],
					    dc->dcMacPictBackPattern[3],
					    dc->dcMacPictBackPattern[4],
					    dc->dcMacPictBackPattern[4],
					    dc->dcMacPictBackPattern[6],
					    dc->dcMacPictBackPattern[7] ));

		if  ( appMacPictSetPattern( dc, through,
						MACPICThandleERASE_BRUSH,
						MACPICThandle_NONE,
						dc->dcMacPictBackIsSolid,
						&(dc->dcMacPictBackSolid),
						dc->dcMacPictBackPattern ) )
		    { LDEB(1); return -1;	}
		continue;

	    case MACPICT_OpColor:
		{
		dc->dcMacPictRedMax= sioEndianGetBeUint16( sis );
		dc->dcMacPictGreenMax= sioEndianGetBeUint16( sis );
		dc->dcMacPictBlueMax= sioEndianGetBeUint16( sis );

		PICTDEB(appDebug( "OpColor(%u,%u,%u)\n",
						dc->dcMacPictRedMax,
						dc->dcMacPictGreenMax,
						dc->dcMacPictBlueMax ));
		}
		continue;

	    case MACPICT_TxRatio:
		{
		(void) /* y0= */ sioEndianGetBeInt16( sis );
		(void) /* x0= */ sioEndianGetBeInt16( sis );
		(void) /* y1= */ sioEndianGetBeInt16( sis );
		(void) /* x1= */ sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "TxRatio( %d, %d, %d, %d )\n",
							x0, y0, x1, y1 ));
		}
		continue;

	    case MACPICT_FrameOval:
		appMacPictGetRect( &(dc->dcMacPictOvalRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_FrameSameOval:
		{
		PICTDEB(appDebug( "FrameOval([%d..%d x %d..%d)\n",
		    dc->dcMacPictOvalRect.drX0, dc->dcMacPictOvalRect.drX1,
		    dc->dcMacPictOvalRect.drY0, dc->dcMacPictOvalRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleDUMMY_BRUSH,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawEllipse( dc, through,
				    &(dc->dcMacPictOvalRect),
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintOval:
		appMacPictGetRect( &(dc->dcMacPictOvalRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_PaintSameOval:
		{
		PICTDEB(appDebug( "PaintOval([%d..%d x %d..%d)\n",
		    dc->dcMacPictOvalRect.drX0, dc->dcMacPictOvalRect.drX1,
		    dc->dcMacPictOvalRect.drY0, dc->dcMacPictOvalRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandlePAINT_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawEllipse( dc, through,
				    &(dc->dcMacPictOvalRect),
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillOval:
		appMacPictGetRect( &(dc->dcMacPictOvalRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_FillSameOval:
		{
		PICTDEB(appDebug( "FillOval([%d..%d x %d..%d)\n",
		    dc->dcMacPictOvalRect.drX0, dc->dcMacPictOvalRect.drX1,
		    dc->dcMacPictOvalRect.drY0, dc->dcMacPictOvalRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleFILL_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawEllipse( dc, through,
				    &(dc->dcMacPictOvalRect),
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseOval:
		appMacPictGetRect( &(dc->dcMacPictOvalRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_EraseSameOval:
		{
		PICTDEB(appDebug( "EraseOval([%d..%d x %d..%d)\n",
		    dc->dcMacPictOvalRect.drX0, dc->dcMacPictOvalRect.drX1,
		    dc->dcMacPictOvalRect.drY0, dc->dcMacPictOvalRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleERASE_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawEllipse( dc, through,
				    &(dc->dcMacPictOvalRect),
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FramePoly:
		{
		if  ( appMacPictGetPoly( dc, sis ) < 0 )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "FramePoly(,%d)\n", dc->dcPointCount ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleDUMMY_BRUSH,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1,
				    &(dc->dcPointCount), dc->dcPoints,
				    dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_PaintPoly:
		{
		if  ( appMacPictGetPoly( dc, sis ) < 0 )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "PaintPoly(,%d)\n", dc->dcPointCount ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandlePAINT_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1,
				    &dc->dcPointCount, dc->dcPoints,
				    dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    /* case MACPICT_FillPoly: does not exist */

	    case MACPICT_ErasePoly:
		/* Format different from documented? */
		if  ( bytes > 0 )
		    {
		    PICTDEB(appDebug( "ErasePoly() bytes= %d\n", bytes ));
		    goto skipBytes;
		    }
		{
		if  ( appMacPictGetPoly( dc, sis ) < 0 )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "ErasePoly(,%d)\n", dc->dcPointCount ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleERASE_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1,
				    &(dc->dcPointCount), dc->dcPoints,
				    dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_ShortLine:
		{
		int			x0;
		int			y0;
		int			x1;
		int			y1;

		if  ( appMacPictGetShortLine( &x0, &y0, &x1, &y1, dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandle_NONE,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appMacPictDrawLine( dc, through, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_ShortLineFrom:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetShortLineFrom( &x0, &y0, &x1, &y1,
								dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandle_NONE,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appMacPictDrawLine( dc, through, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LineFrom:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetLineFrom( &x0, &y0, &x1, &y1, dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandle_NONE,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appMacPictDrawLine( dc, through, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_Line:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetLine( &x0, &y0, &x1, &y1, dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandle_NONE,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appMacPictDrawLine( dc, through, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameRRect:
		appMacPictGetRect( &(dc->dcMacPictRRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_FrameSameRRect:
		{
		DocumentRectangle	dr;
		int			h;
		int			w;

		PICTDEB(appDebug( "FrameRRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRRectRect.drX0, dc->dcMacPictRRectRect.drX1,
		    dc->dcMacPictRRectRect.drY0, dc->dcMacPictRRectRect.drY1 ));

		appWinMetaTransformRectangle( &dr, dc,
						&(dc->dcMacPictRRectRect) );
		h= DC_hViewport( dc->dcMacPictRRectOvalHigh, dc );
		w= DC_wViewport( dc->dcMacPictRRectOvalWide, dc );

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleDUMMY_BRUSH,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawRoundedRectangle)( dc, through, &dr, w, h,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_PaintRRect:
		appMacPictGetRect( &(dc->dcMacPictRRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_PaintSameRRect:
		{
		DocumentRectangle	dr;
		int			h;
		int			w;


		PICTDEB(appDebug( "PaintRRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRRectRect.drX0, dc->dcMacPictRRectRect.drX1,
		    dc->dcMacPictRRectRect.drY0, dc->dcMacPictRRectRect.drY1 ));

		appWinMetaTransformRectangle( &dr, dc,
						&(dc->dcMacPictRRectRect) );
		h= DC_hViewport( dc->dcMacPictRRectOvalHigh, dc );
		w= DC_wViewport( dc->dcMacPictRRectOvalWide, dc );

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandlePAINT_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawRoundedRectangle)( dc, through, &dr, w, h,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_FillRRect:
		appMacPictGetRect( &(dc->dcMacPictRRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_FillSameRRect:
		{
		DocumentRectangle	dr;
		int			h;
		int			w;


		PICTDEB(appDebug( "FillRRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRRectRect.drX0, dc->dcMacPictRRectRect.drX1,
		    dc->dcMacPictRRectRect.drY0, dc->dcMacPictRRectRect.drY1 ));

		appWinMetaTransformRectangle( &dr, dc,
						&(dc->dcMacPictRRectRect) );
		h= DC_hViewport( dc->dcMacPictRRectOvalHigh, dc );
		w= DC_wViewport( dc->dcMacPictRRectOvalWide, dc );

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleFILL_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawRoundedRectangle)( dc, through, &dr, w, h,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_EraseRRect:
		appMacPictGetRect( &(dc->dcMacPictRRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_EraseSameRRect:
		{
		DocumentRectangle	dr;
		int			h;
		int			w;


		PICTDEB(appDebug( "EraseRRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRRectRect.drX0, dc->dcMacPictRRectRect.drX1,
		    dc->dcMacPictRRectRect.drY0, dc->dcMacPictRRectRect.drY1 ));

		appWinMetaTransformRectangle( &dr, dc,
						&(dc->dcMacPictRRectRect) );
		h= DC_hViewport( dc->dcMacPictRRectOvalHigh, dc );
		w= DC_wViewport( dc->dcMacPictRRectOvalWide, dc );

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleERASE_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawRoundedRectangle)( dc, through, &dr, w, h,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_FrameRect:
		appMacPictGetRect( &(dc->dcMacPictRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_FrameSameRect:
		{
		PICTDEB(appDebug( "FrameRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRectRect.drX0, dc->dcMacPictRectRect.drX1,
		    dc->dcMacPictRectRect.drY0, dc->dcMacPictRectRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleDUMMY_BRUSH,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawRectangle( dc, through,
					&(dc->dcMacPictRectRect),
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintRect:
		appMacPictGetRect( &(dc->dcMacPictRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_PaintSameRect:
		{
		PICTDEB(appDebug( "PaintRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRectRect.drX0, dc->dcMacPictRectRect.drX1,
		    dc->dcMacPictRectRect.drY0, dc->dcMacPictRectRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandlePAINT_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawRectangle( dc, through,
					&(dc->dcMacPictRectRect),
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillRect:
		appMacPictGetRect( &(dc->dcMacPictRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_FillSameRect:
		{
		PICTDEB(appDebug( "FillRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRectRect.drX0, dc->dcMacPictRectRect.drX1,
		    dc->dcMacPictRectRect.drY0, dc->dcMacPictRectRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleFILL_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawRectangle( dc, through,
					&(dc->dcMacPictRectRect),
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseRect:
		appMacPictGetRect( &(dc->dcMacPictRectRect), sis );
		/*FALLTHROUGH*/
	    case MACPICT_EraseSameRect:
		{
		PICTDEB(appDebug( "EraseRect([%d..%d x %d..%d])\n",
		    dc->dcMacPictRectRect.drX0, dc->dcMacPictRectRect.drX1,
		    dc->dcMacPictRectRect.drY0, dc->dcMacPictRectRect.drY1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleERASE_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( appWinMetaDrawRectangle( dc, through,
					&(dc->dcMacPictRectRect),
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameArc:
		if  ( appMacPictGetArc( dc, sis ) )
		    { LDEB(1); return -1;	}
		/*FALLTHROUGH*/
	    case MACPICT_FrameSameArc:
		{
		PICTDEB(appDebug( "FrameArc([%d..%d x %d..%d], %d -> %d)\n",
		    dc->dcMacPictArcRect.drX0, dc->dcMacPictArcRect.drX1,
		    dc->dcMacPictArcRect.drY0, dc->dcMacPictArcRect.drY1,
		    dc->dcMacPictArcA0, dc->dcMacPictArcA1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandleDUMMY_BRUSH,
						    MACPICThandleFRAME_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawArc)( dc, through, &(dc->dcMacPictArcRect),
				    dc->dcMacPictArcXs, dc->dcMacPictArcYs,
				    dc->dcMacPictArcXe, dc->dcMacPictArcYe,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintArc:
		if  ( appMacPictGetArc( dc, sis ) )
		    { LDEB(1); return -1;	}
		/*FALLTHROUGH*/
	    case MACPICT_PaintSameArc:
		{
		PICTDEB(appDebug( "PaintArc([%d..%d x %d..%d], %d -> %d)\n",
		    dc->dcMacPictArcRect.drX0, dc->dcMacPictArcRect.drX1,
		    dc->dcMacPictArcRect.drY0, dc->dcMacPictArcRect.drY1,
		    dc->dcMacPictArcA0, dc->dcMacPictArcA1 ));

		if  ( appMacPictChooseGear( dc, through,
						    MACPICThandlePAINT_BRUSH,
						    MACPICThandleDUMMY_PEN ) )
		    { LDEB(1); return -1;	}

		if  ( (*dc->dcDrawArc)( dc, through, &(dc->dcMacPictArcRect),
				    dc->dcMacPictArcXs, dc->dcMacPictArcYs,
				    dc->dcMacPictArcXe, dc->dcMacPictArcYe,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FontName:
		if  ( appMacPictGetFontName( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_TxFont:
		if  ( appMacPictGetTxFont( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_TxFace:
		if  ( appMacPictGetTxFace( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_GlyphState:
		if  ( appMacPictGetGlyphState( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_DHText:
		{
		int		dh;
		const int	dv= 0;

		if  ( appMacPictGetDHText( dc, &dh, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawString( dc, dh, dv, through ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DVText:
		{
		const int	dh= 0;
		int		dv;

		if  ( appMacPictGetDVText( dc, &dv, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawString( dc, dh, dv, through ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DHDVText:
		{
		int		dh;
		int		dv;

		if  ( appMacPictGetDHDVText( dc, &dh, &dv, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawString( dc, dh, dv, through ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongText:
		{
		const int	dh= 0;
		const int	dv= 0;

		if  ( appMacPictGetLongText( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawString( dc, dh, dv, through ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongComment:
		if  ( appMacPictGetLongComment( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_ShortComment:
		{
		int	kind= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "ShortComment( %d:%s )\n",
				kind, appMacPictCommentName( kind ) ));

		kind= kind;
		}
		continue;

	    case MACPICT_FrameRgn:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	skip;

		PICTDEB(appDebug( "FrameRgn( %d )\n", regionBytes ));

		for ( skip= 2; skip < regionBytes; skip++ )
		    {
		    (void) /* regionCoord= */ sioInGetByte( sis );
		    }
		}
		continue;

	    case MACPICT_PaintRgn:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	skip;

		PICTDEB(appDebug( "PaintRgn( %d )\n", regionBytes ));

		for ( skip= 2; skip < regionBytes; skip++ )
		    {
		    (void) /* regionCoord= */ sioInGetByte( sis );
		    }
		}
		continue;

	    case MACPICT_BitsRect:
		{
		const int	packed= 0;
		const int	direct= 0;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "BitsRect( .. ) %d bytes\n", bytes ));

		if  ( appMacPictDrawImage( dc, through, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRect:
		{
		const int	packed= 1;
		const int	direct= 0;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "PackBitsRect( .. ) %d bytes\n", bytes ));

		if  ( appMacPictDrawImage( dc, through, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRgn:
		{
		const int	packed= 1;
		const int	direct= 0;
		const int	clipRegion= 1;

		PICTDEB(appDebug( "PackBitsRgn( .. ) %d bytes\n", bytes ));

		if  ( appMacPictDrawImage( dc, through, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DirectBitsRect:
		{
		const int	packed= 0;
		const int	direct= 1;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "DirectBitsRect( .. ) %d bytes\n", bytes ));

		if  ( appMacPictDrawImage( dc, through, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DirectBitsRgn:
		{
		const int	packed= 0;
		const int	direct= 1;
		const int	clipRegion= 1;

		PICTDEB(appDebug( "DirectBitsRgn( .. )\n" ));

		if  ( appMacPictDrawImage( dc, through, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_CompressedQuickTime:
		{
		long	qtbytes= sioEndianGetBeInt32( sis );
		int	skip;

		PICTDEB(appDebug( "CompressedQuickTime( %ld )\n",
							    qtbytes ));

		for ( skip= 0; skip < qtbytes; skip++ )
		    {
		    (void) /* qtdata= */ sioInGetByte( sis );
		    }

		}
		continue;

	    case MACPICT_UncompressedQuickTime:
		{
		long	qtbytes= sioEndianGetBeInt32( sis );
		int	skip;

		PICTDEB(appDebug( "UncompressedQuickTime( %ld )\n",
							    qtbytes ));

		for ( skip= 0; skip < qtbytes; skip++ )
		    {
		    (void) /* qtdata= */ sioInGetByte( sis );
		    }

		}
		continue;

	    case MACPICT_TxSize:
		if  ( appMacPictGetTxSize( dc, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_Origin:
		{
		int	dh= sioEndianGetBeUint16( sis );
		int	dv= sioEndianGetBeUint16( sis );

		dh= dh; dv= dv;

		PICTDEB(appDebug( "Origin(%d,%d)\n", dh, dv ));
		}
		continue;

	    case 0x66:
		XLDEB(opcode,bytes);
		bytes= 12;
		goto skipBytes;

	    default:
		XDEB(opcode);
	    skipBytes:
		if  ( bytes >= 0 )
		    {
		    int		skip;
		    XLDEB(opcode,bytes);

		    for ( skip= 0; skip < bytes; skip++ )
			{ (void) /* c= */ sioInGetByte( sis );	}

		    continue;
		    }
		XDEB(opcode); rval= -1; goto ready;

	    case MACPICT_OpEndPic:
		PICTDEB(appDebug( "OpEndPic()\n" ));
		break;
	    }

	break;
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read the header of a macpict metafile.				*/
/*									*/
/*  2)  Read picture size.						*/
/*  3)  Read picture frame.						*/
/*  4)  Read version.							*/
/*									*/
/************************************************************************/

int appMacPictGetDeviceHeader(	MacPictHeader *		mph,
				SimpleInputStream *	sis )
    {
    int			c;
    int			i;

    /*  2  */
    mph->mphPictureSize= sioEndianGetBeInt16( sis );

    /*  3  */
    mph->mphFrame.drY0= sioEndianGetBeInt16( sis );
    mph->mphFrame.drX0= sioEndianGetBeInt16( sis );
    mph->mphFrame.drY1= sioEndianGetBeInt16( sis );
    mph->mphFrame.drX1= sioEndianGetBeInt16( sis );

    /*  4  */
    c= sioInGetByte( sis );
    switch( c )
	{
	case 0:
	    c= sioInGetByte( sis );
	    if  ( c != 0x11 )
		{ XDEB(c); return -1;	}

	    mph->mphVersion= sioInGetByte( sis );
	    if  ( mph->mphVersion != 2 )
		{ LDEB(mph->mphVersion); return -1;	}

	    c= sioInGetByte( sis );
	    if  ( c != 0xff )
		{ XDEB(c); return -1;	}

	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0xc00 )
		{ XDEB(c); return -1;	}

	    (void) /* l= */ sioEndianGetBeInt32( sis );
	    /*
	    if  ( l != -1 )
		{ LDEB(l); return -1;	}
	    */

	    for ( i= 0; i < 8; i++ )
		{ c= sioEndianGetBeInt16( sis ); }

	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0x00 )
		{ XDEB(c); /*return -1;*/	}
	    c= sioEndianGetBeInt16( sis );
	    if  ( c != 0x00 )
		{ XDEB(c); /*return -1;*/	}
	    break;

	case 0x11:
	    mph->mphVersion= sioInGetByte( sis );
	    if  ( mph->mphVersion != 1 )
		{ LDEB(mph->mphVersion); return -1;	}
	    break;

	default:
	    XDEB(c); return -1;
	}

    return 0;
    }

