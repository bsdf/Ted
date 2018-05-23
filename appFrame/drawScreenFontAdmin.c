/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"drawImpl.h"
#   include	"drawScreenFontAdmin.h"
#   include	"drawTextImplX11.h"
#   include	<psFontMetrics.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void drawStartScreenFontList(	NumberedPropertiesList *	sfl )
    {
    utilInitNumberedPropertiesList( sfl );

    utilStartNumberedPropertyList( sfl,

		    DSFkey_COUNT,
		    (NumberedPropertiesGetProperty)drawGetScreenFontKey,

		    sizeof(DrawScreenFont),
		    (InitPagedListItem)drawInitScreenFont,
		    (CleanPagedListItem)drawCleanScreenFont );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

DrawScreenFont * drawGetScreenFontByNumber(
					const NumberedPropertiesList *	sfl,
					int				n )
    {
    void *	vdsf= utilPagedListGetItemByNumber( &(sfl->nplPagedList), n );

    return (DrawScreenFont *)vdsf;
    }

/************************************************************************/
/*									*/
/*  Call a function for all ScreenFont in the list.		*/
/*									*/
/************************************************************************/

void drawForAllScreenFonts(	const NumberedPropertiesList *	sfl,
				ScreenFontFunction	f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(sfl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vdsf= utilPagedListGetItemByNumber( pl, n );

	(*f)( (DrawScreenFont *)vdsf, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the slot for a particular font/attribute combination.		*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*									*/
/************************************************************************/

int drawGetScreenFont(		DrawScreenFont **		pApf,
				int *				pFresh,
				NumberedPropertiesList *	npl,
				const IndexSet *		unicodesUsed,
				const AfmFontInfo *		afi,
				int				pixelSize )
    {
    int					screenFont;

    DrawScreenFont			dsfTemplate;
    DrawScreenFont *			dsfFound;

    drawInitScreenFont( &dsfTemplate );

    if  ( pixelSize < 2 )
	{
	/* LFLDEB(twipsSize,add->addMagnifiedPixelsPerTwip,sizePixels); */
	pixelSize= 2;
	}

    /*  1  */
    drawInitScreenFont( &dsfTemplate );
    dsfTemplate.apfPsFaceNumber= afi->afiFaceNumber;
    dsfTemplate.dsfSizePixels= pixelSize;

    screenFont= utilGetPropertyNumber( npl, 0, &dsfTemplate );
    if  ( screenFont >= 0 )
	{
	dsfFound= drawGetScreenFontByNumber( npl, screenFont );
	if  ( ! dsfFound )
	    { LXDEB(screenFont,dsfFound); return -1;	}

	*pApf= dsfFound; *pFresh= 0; return screenFont;
	}

    /*  KEY  */
    dsfTemplate.apfPsFaceNumber= afi->afiFaceNumber;
    dsfTemplate.dsfSizePixels= pixelSize;

    /*  DEP  */
    dsfTemplate.apfPsFontInfo= afi;
    dsfTemplate.dsfUnicodesUsed= unicodesUsed;

    screenFont= utilGetPropertyNumber( npl, 1, &dsfTemplate );
    if  ( screenFont >= 0 )
	{
	const int		baselinePixels= 0;

	dsfFound= drawGetScreenFontByNumber( npl, screenFont );
	if  ( ! dsfFound )
	    { LXDEB(screenFont,dsfFound); return -1;	}

	/****************************************************************/
	/*  Calculate geometry shortcuts.				*/
	/****************************************************************/

	psStrikethroughGeometry( &(dsfFound->apfStrikethroughPositionPixels),
				&(dsfFound->apfStrikethroughThicknessPixels),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->apfPsFontInfo );

	psUnderlineGeometry( &(dsfFound->apfUnderlinePositionPixels),
				&(dsfFound->apfUnderlineThicknessPixels),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->apfPsFontInfo );

	psGetSuperBaseline( &(dsfFound->apfSuperBaseline),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->apfPsFontInfo );
	psGetSubBaseline( &(dsfFound->apfSubBaseline),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->apfPsFontInfo );

	*pApf= dsfFound; *pFresh= 1; return screenFont;
	}

    LDEB(screenFont); return screenFont;
    }

int drawTextExtentsImpl(	DocumentRectangle *		drText,
				int				x,
				int				y,
				const NumberedPropertiesList *	npl,
				int				screenFont,
				const char *			s,
				int				len )
    {
    DrawScreenFont *			dsf;

    TextProgress			tp;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{ LXDEB(screenFont,dsf); return -1;	}

#   ifdef USE_XFT
    if  ( dsf->dsfXftFont					&&
	  ! drawTextExtentsXft( drText, x, y, dsf, dsf, s, len ) )
	{ return 0;	}
#   endif

    tp.tpX= x;
    tp.tpY= y;

    drawHandleTextSegmentsX11( &tp, dsf, s, len,
		    drawTextSegment8LengthX11, drawTextSegment16LengthX11,
		    &(dsf->dsfEncodedFonts) );

    drText->drX0= x;
    drText->drX1= tp.tpX;

    drawVerticalTextExtentsX11( drText, y, &(dsf->dsfEncodedFonts) );

    return 0;
    }

int drawScapsTextExtentsImpl(	DocumentRectangle *		drText,
				int				x,
				int				y,
				const NumberedPropertiesList * npl,
				int				fullScreenFont,
				int				scapsScreenFont,
				const char *			s,
				int				len )
    {
    DrawScreenFont *			fullDsf;
    DrawScreenFont *			scapsDsf;

    TextProgress			tp;

    fullDsf= drawGetScreenFontByNumber( npl, fullScreenFont );
    if  ( ! fullDsf )
	{ LXDEB(fullScreenFont,fullDsf); return -1;	}

    scapsDsf= drawGetScreenFontByNumber( npl, scapsScreenFont );
    if  ( ! scapsDsf )
	{ LXDEB(scapsScreenFont,scapsDsf); return -1;	}

#   ifdef USE_XFT
    if  ( fullDsf->dsfXftFont						&&
	  scapsDsf->dsfXftFont						&&
	  ! drawTextExtentsXft( drText, x, y,
					scapsDsf, fullDsf, s, len ) )
	{ return 0;	}
#   endif

    tp.tpX= x;
    tp.tpY= y;

    drawHandleTextSegmentsX11( &tp, scapsDsf, s, len,
				    drawTextSegment8LengthX11,
				    drawTextSegment16LengthX11,
				    &(scapsDsf->dsfEncodedFonts) );

    drText->drX0= x;
    drText->drX1= tp.tpX;

    /* note: no scaps here */
    drawVerticalTextExtentsX11( drText, y, &(fullDsf->dsfEncodedFonts) );

    return 0;
    }
