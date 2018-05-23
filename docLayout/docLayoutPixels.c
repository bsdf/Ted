/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<geoGrid.h>
#   include	<docDebug.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>

#   include	<appDebugon.h>

/************************************************************************/

int docLayoutYPixels(	const LayoutContext *	lc,
			const LayoutPosition *	lp )
    {
    DocumentRectangle	drPixels;

    docGetPageRectPixels( &drPixels, lc, lp->lpPage );

    return drPixels.drY0+ lc->lcPixelsPerTwip* lp->lpPageYTwips;
    }

int docLayoutXPixels(	const LayoutContext *	lc,
			int			xTwips )
    {
    return lc->lcPixelsPerTwip* xTwips;
    }

/************************************************************************/
/*									*/
/*  Get the pixel rectangle for a rectangle that lives on one page.	*/
/*									*/
/************************************************************************/

void docGetPixelRect(		DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				const DocumentRectangle *	drTwips,
				int				page )
    {
    LayoutPosition		lp0;
    LayoutPosition		lp1;

    lp0.lpPage= lp1.lpPage= page;
    lp0.lpPageYTwips= drTwips->drY0;
    lp1.lpPageYTwips= drTwips->drY1;

    drPixels->drX0= docLayoutXPixels( lc, drTwips->drX0 );
    drPixels->drX1= docLayoutXPixels( lc, drTwips->drX1 );
    drPixels->drY0= docLayoutYPixels( lc, &lp0 );
    drPixels->drY1= docLayoutYPixels( lc, &lp1 );

    return;
    }


void docGetPixelRectForPos(	DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBottom )
    {
    drPixels->drX0= docLayoutXPixels( lc, x0Twips );
    drPixels->drX1= docLayoutXPixels( lc, x1Twips );
    drPixels->drY0= docLayoutYPixels( lc, lpTop );
    drPixels->drY1= docLayoutYPixels( lc, lpBottom );

    return;
    }

/************************************************************************/
/*									*/
/*  Get the pixel rectangle that corrresponds to a particular page.	*/
/*									*/
/*  1)  Find the first section that appears on the page.		*/
/*	If none can be found, the page belongs to document end notes	*/
/*	that are formatted according to the geometry of the last	*/
/*	section.							*/
/*  2)  Skip back to the last section that has its own page geometry:	*/
/*	That is either the first section of the document, or a section	*/
/*	that starts on a fresh page.					*/
/*									*/
/************************************************************************/

void docGetPixelRectangleForPages(
				DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				int				page0,
				int				page1 )
    {
    int			sectNr;
    const BufferItem *	bodyNode= lc->lcDocument->bdBody.dtRoot;
    int			first= 1;

    sectNr= docGetFirstSectionOnPage( lc->lcDocument, page1 );
    if  ( sectNr < 0 )
	{ sectNr= bodyNode->biChildCount- 1;	}

    while( sectNr >= 0 )
	{
	const BufferItem *	sectNode= bodyNode->biChildren[sectNr];

	if  ( sectNr == 0					||
	      ( sectNode->biSectBreakKind != DOCibkNONE	&&
	        sectNode->biSectBreakKind != DOCibkCOL	)	)
	    {
	    DocumentRectangle	drOutside;
	    DocumentRectangle	drInside;
	    int			step;
	    int			topPage= sectNode->biTopPosition.lpPage;

	    docPageRectsPixels( &drOutside, &drInside, lc->lcPixelsPerTwip,
						    sectNode, lc->lcDocument );

	    step= drOutside.drY1- drOutside.drY0+ 1+ lc->lcPageGapPixels;

	    if  ( first )
		{ *drPixels= drOutside; first= 0;	}

	    if  ( page1 > topPage )
		{
		drPixels->drY1 += ( page1- topPage )* step;
		page1= topPage;
		}
	    if  ( page0 > topPage )
		{
		drPixels->drY0 += ( page0- topPage )* step;
		page0= topPage;
		}
	    else{
		if  ( drPixels->drX0 > drOutside.drX0 )
		    { drPixels->drX0=  drOutside.drX0;	}
		if  ( drPixels->drX1 < drOutside.drX1 )
		    { drPixels->drX1=  drOutside.drX1;	}
		}
	    }

	sectNr--;
	}

    return;
    }

void docGetPageRectPixels(	DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				int				page )
    {
    docGetPixelRectangleForPages( drPixels, lc, page, page );
    }

void docPixelRectangleForPositions(
				DocumentRectangle *		drPixels,
				const PositionGeometry *	pgB,
				const PositionGeometry *	pgE,
				const LayoutContext *		lc )
    {
    DocumentRectangle		drPixelsB;
    DocumentRectangle		drPixelsE;

    docGetPixelRectForPos( &drPixelsB, lc,
			    pgB->pgXTwips, pgB->pgXTwips,
			    &(pgB->pgTopPosition),
			    &(pgB->pgBottomPosition) );

    drPixelsB.drX0= drPixelsB.drX1= pgB->pgXPixels; /* Do not trust XTwips yet */


    docGetPixelRectForPos( &drPixelsE, lc,
			    pgE->pgXTwips, pgE->pgXTwips,
			    &(pgE->pgTopPosition),
			    &(pgE->pgBottomPosition) );
    drPixelsE.drX0= drPixelsE.drX1= pgE->pgXPixels; /* Do not trust XTwips yet */

    geoUnionRectangle( drPixels, &drPixelsB, &drPixelsE );

    return;
    }

/************************************************************************/
/*									*/
/*  Derive private rectangles from document properties.			*/
/*									*/
/************************************************************************/

void docPageRectsPixels(	DocumentRectangle *	drOut,
				DocumentRectangle *	drIn,
				double			xfac,
				const BufferItem *	bodySectBi,
				const BufferDocument *	bd )
    {
    const DocumentGeometry *	dg= &(bd->bdProperties.dpGeometry);
    DocumentRectangle		drPage;

    if  ( bodySectBi )
	{
	if  ( bodySectBi->biLevel != DOClevSECT )
	    { SDEB(docLevelStr(bodySectBi->biLevel));	}
	if  ( bodySectBi->biTreeType != DOCinBODY )
	    { SDEB(docTreeTypeStr(bodySectBi->biTreeType));	}

	dg= &(bodySectBi->biSectDocumentGeometry);
	}

    drPage.drX0= 0;
    drPage.drY0= 0;
    drPage.drX1= COORDtoGRID( xfac, dg->dgPageWideTwips );
    drPage.drY1= COORDtoGRID( xfac, dg->dgPageHighTwips );

    if  ( drOut )
	{ *drOut= drPage;	}

    if  ( drIn )
	{
	drIn->drX0= COORDtoGRID( xfac, dg->dgLeftMarginTwips );
	drIn->drY0= COORDtoGRID( xfac, dg->dgTopMarginTwips );
	drIn->drX1= drPage.drX1- COORDtoGRID( xfac, dg->dgRightMarginTwips );
	drIn->drY1= drPage.drY1- COORDtoGRID( xfac, dg->dgBottomMarginTwips );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the page for a particular Y cordinate.				*/
/*									*/
/************************************************************************/

int docGetPageForYPixels(	const LayoutContext *		lc,
				int				yPixels )
    {
    int			sectNr;
    const BufferItem *	bodyNode= lc->lcDocument->bdBody.dtRoot;
    int			candidate= 0;

    DocumentRectangle	drOutside;
    DocumentRectangle	drInside;
    int			step;

    int			pagesBefore= 0;
    int			heightBefore= 0;

    const BufferItem *	sectNode= bodyNode->biChildren[0];

    docPageRectsPixels( &drOutside, &drInside, lc->lcPixelsPerTwip,
						    sectNode, lc->lcDocument );
    step= drOutside.drY1- drOutside.drY0+ 1+ lc->lcPageGapPixels;

    candidate= pagesBefore+ ( yPixels- heightBefore )/ step;

    for ( sectNr= 1; sectNr < bodyNode->biChildCount; sectNr++ )
	{
	sectNode= bodyNode->biChildren[sectNr];

	if  ( sectNode->biSectBreakKind != DOCibkNONE	&&
	      sectNode->biSectBreakKind != DOCibkCOL	)
	    {
	    int			topPage= sectNode->biTopPosition.lpPage;

	    heightBefore += ( topPage- pagesBefore )* step;

	    if  ( heightBefore >= yPixels )
		{ return candidate;	}

	    docPageRectsPixels( &drOutside, &drInside, lc->lcPixelsPerTwip,
						    sectNode, lc->lcDocument );
	    step= drOutside.drY1- drOutside.drY0+ 1+ lc->lcPageGapPixels;

	    pagesBefore= topPage;
	    candidate= pagesBefore+ ( yPixels- heightBefore )/ step;
	    }
	}

    return candidate;
    }

