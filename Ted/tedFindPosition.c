/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/*  Find the object in the document for an x,y position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<limits.h>
#   include	<math.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	<docParaString.h>
#   include	<docDraw.h>
#   include	<docParaString.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docParaParticules.h>

#   include	<appDebugon.h>

/************************************************************************/

typedef struct PositionFindJob
    {
    DocumentPosition		pfjDocumentPosition;
    PositionGeometry		pfjPositionGeometry;
    DocumentRectangle		pfjPageRect;
    DrawingContext		pfjDrawingContext;
    int				pfjPageFound;
    int				pfjColumnFound;

    int				pfjFound;
    int				pfjDistance;

    int				pfjDocX;
    int				pfjDocY;
    int				pfjPage;
    } PositionFindJob;

/************************************************************************/
/*									*/
/*  Probe a line to find out whether the position is inside the line.	*/
/*  If it is, remember the documentposition.				*/
/*									*/
/*  0)  Avoid shifted table headers.					*/
/*  1)  Determine rectangle (in pixels) for the line.			*/
/*	As pfjDocX,Y have been corrected for scrolling, scrolling is	*/
/*	irrelevant here. The shifts are for table headers &c that are	*/
/*	repeated on a different page.					*/
/*  2)  x, y inside rectangle?						*/
/*  3)  Above line, if near.. select head of line.			*/
/*									*/
/************************************************************************/

static void tedSetPositionFound(	PositionFindJob *	pfj,
					const TextLine *	tl,
					int			distance )
    {
    pfj->pfjPageFound= tl->tlTopPosition.lpPage;
    pfj->pfjColumnFound= tl->tlTopPosition.lpColumn;

    pfj->pfjFound= 1;
    pfj->pfjDistance= distance;

    return;
    }

static int tedFindPositionInTextLine(
				BufferItem *			paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLinePixels,
				void *				vpfj,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    PositionFindJob *		pfj= (PositionFindJob *)vpfj;

    const TextLine *		tl= paraBi->biParaLines+ line;

    /*  0  */
    if  ( bo->boOverrideFrame )
	{ return tl->tlParticuleCount;	}

    /*  2  */
    if  ( geo2DIXYInBox( pfj->pfjDocX, pfj->pfjDocY, drLinePixels ) )
	{
	const int	d= 0;

	if  ( tedFindPositionInLine( &(pfj->pfjDocumentPosition),
					&(pfj->pfjPositionGeometry),
					lc, paraBi, line, pfj->pfjDocX ) )
	    { LDEB(pfj->pfjDocX);			}
	else{ tedSetPositionFound( pfj, tl, d );	}

	return tl->tlParticuleCount;
	}

    if  ( paraBi->biTreeType == DOCinBODY )
	{
	int		yc= ( drLinePixels->drY0+ drLinePixels->drY1 )/ 2;
	int		dx;
	int		dy;
	int		d;

	DocumentRectangle	drTry;

	if  ( pfj->pfjDocY > yc )
	    { dy= pfj->pfjDocY- drLinePixels->drY0;	}
	else{ dy= drLinePixels->drY1- pfj->pfjDocY;	}

	/*  left */
	drTry.drX0= 2* drLinePixels->drX0- 1* drLinePixels->drX1;
	drTry.drX1= drLinePixels->drX0;
	drTry.drY0= 2* drLinePixels->drY0- 1* drLinePixels->drY1;
	drTry.drY1= 2* drLinePixels->drY1- 1* drLinePixels->drY0;

	dx= drLinePixels->drX0- pfj->pfjDocX;
	d= (int)sqrt( (double)(dx*dx)+ (double)(dy*dy) );

	if  ( d < pfj->pfjDistance					&&
	      geo2DIXYInBox( pfj->pfjDocX, pfj->pfjDocY, &drTry ) 	)
	    {
	    pfj->pfjDocumentPosition.dpNode= (BufferItem *)paraBi;
	    pfj->pfjDocumentPosition.dpStroff= tl->tlStroff;

	    tedPositionGeometry( &(pfj->pfjPositionGeometry),
					    &(pfj->pfjDocumentPosition),
					    PARAfindLAST, lc );

	    tedSetPositionFound( pfj, tl, d );
	    return tl->tlParticuleCount;
	    }

	/*  right */
	drTry.drX0= drLinePixels->drX1;
	drTry.drX1= 2* drLinePixels->drX1- drLinePixels->drX0;
	drTry.drY0= 2* drLinePixels->drY0- 1* drLinePixels->drY1;
	drTry.drY1= 2* drLinePixels->drY1- 1* drLinePixels->drY0;

	dx= pfj->pfjDocX- drLinePixels->drX1;
	d= (int)sqrt( (double)(dx*dx)+ (double)(dy*dy) );

	if  ( d < pfj->pfjDistance					&&
	      geo2DIXYInBox( pfj->pfjDocX, pfj->pfjDocY, &drTry ) 	)
	    {
	    pfj->pfjDocumentPosition.dpNode= (BufferItem *)paraBi;
	    pfj->pfjDocumentPosition.dpStroff= tl->tlStroff+ tl->tlStrlen;

	    tedPositionGeometry( &(pfj->pfjPositionGeometry),
					    &(pfj->pfjDocumentPosition),
					    PARAfindFIRST, lc );

	    tedSetPositionFound( pfj, tl, d );
	    return tl->tlParticuleCount;
	    }

	/*  above, below */
	drTry.drX0= drLinePixels->drX0;
	drTry.drX1= drLinePixels->drX1;
	drTry.drY0= 2* drLinePixels->drY0- 1* drLinePixels->drY1;
	drTry.drY1= 2* drLinePixels->drY1- 1* drLinePixels->drY0;

	d= dy;

	if  ( d < pfj->pfjDistance					&&
	      geo2DIXYInBox( pfj->pfjDocX, pfj->pfjDocY, &drTry ) 	)
	    {
	    if  ( tedFindPositionInLine( &(pfj->pfjDocumentPosition),
					    &(pfj->pfjPositionGeometry),
					    lc, paraBi, line, pfj->pfjDocX ) )
		{ LDEB(pfj->pfjDocX);			}
	    else{ tedSetPositionFound( pfj, tl, d );	}

	    return tl->tlParticuleCount;
	    }
	}

    return tl->tlParticuleCount;
    }

/************************************************************************/
/*									*/
/*  Find the document position corresponding to the x coordinate in	*/
/*  pixels in a line of text.						*/
/*  Initially, we try a binary search for a string offset that results	*/
/*  in an X position <= docXPixels. But instead of the final comparison	*/
/*  we compare the X to the center of the character.			*/
/*									*/
/*  NOTE: As docXPixels has been corrected for the scrollbars,		*/
/*	scrolling is irrelevant.					*/
/*									*/
/************************************************************************/

int tedFindPositionInLine(	DocumentPosition *	dp,
				PositionGeometry *	pg,
				const LayoutContext *	lc,
				const BufferItem *	paraBi,
				int			line,
				int			docXPixels )
    {
    const TextLine *		tl= paraBi->biParaLines+ line;
    int				upto= docParaStrlen( paraBi );

    int				l= tl->tlStroff;
    int				r= tl->tlStroff+ tl->tlStrlen;
    int				m= (l+ r)/ 2;

    DocumentPosition		dpM;
    DocumentPosition		dpR;
    PositionGeometry		pgM;
    PositionGeometry		pgR;

    docInitPositionGeometry( &pgM );
    docInitPositionGeometry( &pgR );

    dpM.dpNode= (BufferItem *)paraBi; /* remove const */
    dpM.dpStroff= l;

    while( l != m )
	{
	dpM.dpStroff= m= docParaFixStroff( paraBi, m );
	if  ( l == m )
	    { break;	}

	tedPositionGeometry( &pgM, &dpM, PARAfindFIRST, lc );

	if  ( pgM.pgXPixels < docXPixels )
	    { l= m;	}
	else{ r= m;	}

	m= (l+ r)/ 2;
	}

    dpM.dpStroff= docParaFixStroff( paraBi, m );

    if  ( dpM.dpStroff == tl->tlStroff )
	{ tedPositionGeometry( &pgM, &dpM, PARAfindLAST, lc );	}
    else{ tedPositionGeometry( &pgM, &dpM, PARAfindFIRST, lc );	}

    dpR= dpM;
    pgR= pgM;

    if  ( dpR.dpStroff < upto )
	{
	dpR.dpStroff= docParaNextStroff( paraBi, dpR.dpStroff );
	if  ( dpR.dpStroff < 0 )
	    { LDEB(dpR.dpStroff); 	}
	else{
	    tedPositionGeometry( &pgR, &dpR, PARAfindFIRST, lc );

	    if  ( docXPixels > ( pgM.pgXPixels+ pgR.pgXPixels )/ 2 )
		{
		*dp= dpR;
		*pg= pgR;
		return 0;
		}
	    }
	}

    *dp= dpM;
    *pg= pgM;
    return 0;
    }

/************************************************************************/

static void tedSetFindPosition(	PositionFindJob *		pfj,
				const LayoutContext *		lc,
				int				docXPixels,
				int				docY )

    {
    DrawingContext *	dc= &(pfj->pfjDrawingContext);

    docInitDrawingContext( dc );

    pfj->pfjPage= docGetPageForYPixels( lc, docY );
    docGetPageRectPixels( &(pfj->pfjPageRect), lc, pfj->pfjPage );

    dc->dcLayoutContext= *lc;
    dc->dcDrawTableGrid= 0;
    dc->dcClipRect= &(pfj->pfjPageRect);

    dc->dcFirstPage= pfj->pfjPage;
    dc->dcLastPage= pfj->pfjPage;

    docInitDocumentPosition( &(pfj->pfjDocumentPosition) );
    docInitPositionGeometry( &(pfj->pfjPositionGeometry) );
    dc->dcDrawTextLine= tedFindPositionInTextLine;
    dc->dcDrawExternalItems= 0;
    dc->dcPostponeHeadersFooters= 0;

    dc->dcCurrentColorSet= 0;
    dc->dcCurrentTextAttributeSet= 0;

    pfj->pfjPageFound= -1;
    pfj->pfjColumnFound= -1;
    pfj->pfjFound= 0;
    pfj->pfjDistance= INT_MAX;

    pfj->pfjDocX= docXPixels;
    pfj->pfjDocY= docY;

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a Window coordinate to a position in a text buffer.	*/
/*									*/
/************************************************************************/

int tedFindPosition(	DocumentPosition *		dp,
			PositionGeometry *		pg,
			int *				pPage,
			int *				pColumn,
			BufferItem *			selRootNode,
			const LayoutContext *		lc,
			int				docXPixels,
			int				docY )
    {
    PositionFindJob	pfj;

    tedSetFindPosition( &pfj, lc, docXPixels, docY );

    if  ( docDrawNode( (LayoutPosition *)0, selRootNode,
				(void *)&pfj, &(pfj.pfjDrawingContext) ) )
	{ LDEB(1); return 1;	}
    if  ( ! pfj.pfjDocumentPosition.dpNode )
	{ return 1;		}

    *dp= pfj.pfjDocumentPosition;
    *pg= pfj.pfjPositionGeometry;
    *pPage= pfj.pfjPageFound;
    *pColumn= pfj.pfjColumnFound;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a node for a particular position.				*/
/*									*/
/************************************************************************/

int tedFindRootForPosition(		DocumentPosition *	dpFound,
					PositionGeometry *	pgFound,
					DocumentTree **		pTree,
					BufferItem **		pSelSectNode,
					int *			pPage,
					int *			pColumn,
					EditDocument *		ed,
					int			docXPixels,
					int			docY )
    {
    LayoutContext		lc;
    PositionFindJob		pfj;

    tedSetScreenLayoutContext( &lc, ed );

    tedSetFindPosition( &pfj, &lc, docXPixels, docY );

    if  ( docDrawPageRange( &(pfj.pfjDrawingContext), (void *)&pfj ) )
	{ LDEB(1); return -1;	}

    if  ( pfj.pfjFound )
	{
	BufferItem *	selSectNode;

	selSectNode= docGetSectNode( pfj.pfjDocumentPosition.dpNode );

	if  ( pTree )
	    {
	    if  ( docGetRootOfSelectionScope( pTree, (BufferItem **)0,
		    lc.lcDocument, &(selSectNode->biSectSelectionScope) ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( dpFound )
	    { *dpFound= pfj.pfjDocumentPosition;	}
	if  ( pgFound )
	    { *pgFound= pfj.pfjPositionGeometry;	}
	if  ( pSelSectNode )
	    { *pSelSectNode= selSectNode;		}
	if  ( pPage )
	    { *pPage= pfj.pfjPageFound;			}
	if  ( pColumn )
	    { *pColumn= pfj.pfjColumnFound;		}

	return 0;
	}

    return 1;
    }
