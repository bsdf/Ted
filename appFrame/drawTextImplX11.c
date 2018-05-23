/************************************************************************/
/*									*/
/*  Text Drawing primitives for X11.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<uniUtf8.h>
#   include	"drawScreenFontImpl.h"
#   include	"drawTextImplX11.h"

#   include	<appDebugon.h>

#   ifdef USE_X11_FONTS

static XChar2b *	ws;
static int		wc= 0;

static unsigned char *	bs;
static int		bc= 0;

/************************************************************************/
/*									*/
/*  Actual actions.							*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK

void drawTextSegment8X11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const char *		s,
				int			l )
    {
    gdk_draw_text( dsf->dsfDrawable, xfs, dsf->dsfGc,
					tp->tpX, tp->tpY, s, l );
    }

void drawTextSegment16X11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const XChar2b *		ws,
				int			wl )
    {
    gdk_draw_text( dsf->dsfDrawable, xfs, dsf->dsfGc,
					tp->tpX, tp->tpY, (gchar *)ws, 2* wl );
    }

void drawTextSegment8LengthX11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const char *		s,
				int			l )
    {
    tp->tpX += gdk_text_width( xfs, s, l );
    }

void drawTextSegment16LengthX11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const XChar2b *		ws,
				int			wl )
    {
    tp->tpX += gdk_text_width( xfs, (gchar *)ws, 2* wl );
    }

#   endif

/************************************************************************/
/*									*/
/*  Actual actions.							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF

void drawTextSegment8X11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const char *		s,
				int			l )
    {
    XSetFont( dsf->dsfDisplay, dsf->dsfGc, xfs->fid );

    XDrawString( dsf->dsfDisplay, dsf->dsfDrawable, dsf->dsfGc,
						    tp->tpX, tp->tpY, s, l );
    }

void drawTextSegment16X11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const XChar2b *		ws,
				int			wl )
    {
    XSetFont( dsf->dsfDisplay, dsf->dsfGc, xfs->fid );

    XDrawString16( dsf->dsfDisplay, dsf->dsfDrawable, dsf->dsfGc,
						tp->tpX, tp->tpY, ws, wl );
    }

void drawTextSegment8LengthX11(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const char *		s,
				int			l )
    {
    tp->tpX += XTextWidth( xfs, s, l );
    }

void drawTextSegment16LengthX11( TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const XChar2b *		ws,
				int			wl )
    {
    tp->tpX += XTextWidth16( xfs, ws, wl );
    }

#   endif

/************************************************************************/
/*									*/
/*  Convert to 16 bit string.						*/
/*									*/
/************************************************************************/

static int appDrawX11SetWS(	int *			pUsed,
				const char *		ss,
				int			len )
    {
    int				wl= 0;
    int				used= 0;

    if  ( wc < len )
	{
	XChar2b *	fresh= (XChar2b *)realloc( ws, len* sizeof( XChar2b ) );

	if  ( ! fresh )
	    { LXDEB(len,fresh);	return -1;	}

	ws= fresh; wc= len;
	}

    while( len > 0 )
	{
	unsigned short	sym;
	int		step;

	step= uniGetUtf8( &sym, ss );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	ws[wl].byte1= sym/ 256; ws[wl].byte2= sym% 256; wl++;
	used += step; ss += step; len -= step;
	}

    *pUsed= used; return wl;
    }

/************************************************************************/
/*									*/
/*  Convert to 8 bit string.						*/
/*									*/
/************************************************************************/

static int appDrawX11SetBS(	int *			pUsed,
				const IndexMapping *	im,
				const char *		ss,
				int			len )
    {
    int				bl= 0;
    int				used= 0;

    if  ( ! im )
	{ XDEB(im); return -1;	}

    if  ( bc < len )
	{
	unsigned char *	fresh= (unsigned char *)realloc( bs, len* sizeof( unsigned char ) );

	if  ( ! fresh )
	    { LXDEB(len,fresh);	return -1;	}

	bs= fresh; bc= len;
	}

    while( len > 0 )
	{
	unsigned short	sym;
	int		step;
	int		byte;

	step= uniGetUtf8( &sym, ss );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	byte= utilIndexMappingGetU( im, sym );
	if  ( byte < 0 )
	    { break;	}

	bs[bl]= byte; bl++;
	used += step; ss += step; len -= step;
	}

    *pUsed= used; return bl;
    }

void drawHandleTextSegmentsX11( TextProgress *		tp,
			DrawScreenFont *		dsf,
			const char *			s,
			int				len,
			HandleText8			handleText8,
			HandleText16			handleText16,
			const EncodedScreenFontList *	esfl )
    {
    int				used= 0;

    if  ( esfl->esfFontCount == 0 )
	{ LDEB(esfl->esfFontCount); return;	}

    while( len > 0 )
	{
	const EncodedScreenFont *	efs;
	int				font;
	int				step;
	unsigned short			sym;

	step= uniGetUtf8( &sym, s );
	if  ( step < 1 )
	    { LLDEB(len,step); return;	}

	font= utilIndexMappingGetU( &(esfl->esflSymbolToFont), sym );
	if  ( font < 0 )
	    { /*XLDEB(sym,font);*/ len -= step; s += step; continue;	}
	efs= esfl->esfFonts+ font;

	if  ( efs->esfIsTwoByte )
	    {
	    int	wl= appDrawX11SetWS( &used, s, len );

	    if  ( wl < 0 )
		{ LLDEB(len,wl); return;	}

	    (*handleText16)( tp, dsf, efs->esfFontStruct, ws, wl );
	    }
	else{
	    const IndexMapping *	s2b= efs->esfSymbolToByteMapping;
	    int				bl;

	    bl= appDrawX11SetBS( &used, s2b, s, len );
	    if  ( bl < 0 )
		{ SLLDEB(efs->esfFontName,len,bl); return;	}

	    (*handleText8)( tp, dsf, efs->esfFontStruct, (char *)bs, bl );
	    }

	if  ( used < 1 )
	    { LDEB(used); return;	}

	len -= used; s += used;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine text extents. NOTE that in the scaps case, the regular	*/
/*  font is used for the vertical dimensions and the scaps variant for	*/
/*  the horizontal dimensions.						*/
/*									*/
/************************************************************************/

void drawVerticalTextExtentsX11( DocumentRectangle *		drText,
				int				y,
				const EncodedScreenFontList *	esfl )
    {
    const EncodedScreenFont *	esf;
    int				i;
    int				ascent= 0;
    int				descent= 0;

    esf= esfl->esfFonts;
    for ( i= 0; i < esfl->esfFontCount; esf++, i++ )
	{
	if  ( ascent < esf->esfFontStruct->ascent )
	    { ascent=  esf->esfFontStruct->ascent;	}
	if  ( descent < esf->esfFontStruct->descent )
	    { descent=  esf->esfFontStruct->descent;	}
	}

    drText->drY0= y- descent;
    drText->drY1= y- ascent;
    }

#   endif
