#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<appMatchFont.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docScreenLayout.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph because its contents have changed	*/
/*  during editing.							*/
/*									*/
/************************************************************************/

int tedAdjustParagraphLayout(	TedEditOperation *		teo,
				DocumentTree *			dt )
    {
    EditOperation *		eo= &(teo->teoEo);
    int				paraNr= eo->eoParaAdjustParagraphNumber;
    int				stroffFrom= eo->eoParaAdjustStroffFrom;
    int				stroffShift= eo->eoParaAdjustStroffShift;
    int				stroffUpto= eo->eoParaAdjustStroffUpto;

    EditRange *			er= &(eo->eoReformatRange);
    struct BufferItem *		paraNode;

    LayoutJob			lj;

    if  ( er->erHead.epParaNr != paraNr )
	{ LLDEB(er->erHead.epParaNr,paraNr); }
    if  ( er->erTail.epParaNr != paraNr )
	{ LLDEB(er->erTail.epParaNr,paraNr); }

    paraNode= docGetParagraphByNumber( dt, paraNr );
    if  ( ! paraNode )
	{ LXDEB(paraNr,paraNode); return -1;	}

    if  ( ! teo->teoChangedRectSet )
	{ LDEB(teo->teoChangedRectSet);	}

    docInitLayoutJob( &lj );

    lj.ljContext= teo->teoLayoutContext;
    docSetScreenLayoutFunctions( &lj );
    lj.ljChangedRectanglePixels= &(teo->teoChangedRect);
    lj.ljChangedNode= paraNode;

    lj.ljBodySectNode= eo->eoBodySectNode;

    if  ( docAdjustParaLayout( paraNode, ! eo->eoIBarSelectionOld,
				stroffFrom, stroffShift, stroffUpto, &lj ) )
	{ LDEB(1); return -1;	}

    if  ( tedOpenNodeObjects( paraNode, &(teo->teoLayoutContext) ) )
	{ LDEB(1); return -1;	}

    if  ( lj.ljReachedDocumentBottom )
	{ teo->teoRefreshScreenRectangle= 1;	}

    return 0;
    }

/************************************************************************/

void tedSetDocumentLayoutContext( LayoutContext *		lc,
				DrawingSurface			ds,
				const PostScriptFontList *	psfl,
				struct TedDocument *		td )
    {
    lc->lcDrawingSurface= ds;
    lc->lcPostScriptFontList= psfl;
    lc->lcDocument= td->tdDocument;
    lc->lcAttributeToScreenFont= &(td->tdAttributeToScreenFont);

    lc->lcCloseObject= docScreenCloseObject;

    lc->lcGetFontForAttribute= appGetFontInfoForAttribute;

    lc->lcPixelsPerTwip= td->tdPixelsPerTwip;
    lc->lcPageGapPixels= td->tdPageGapPixels;
    }

void tedSetScreenLayoutContext(	LayoutContext *		lc,
				EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedSetDocumentLayoutContext( lc, ed->edDrawingSurface,
					    &(ea->eaPostScriptFontList), td );

    lc->lcDrawingSurface= ed->edDrawingSurface;
    lc->lcOx= ed->edVisibleRect.drX0;
    lc->lcOy= ed->edVisibleRect.drY0;

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the eight blocks around a selected object.			*/
/*									*/
/************************************************************************/

void tedGetObjectRectangle(	DocumentRectangle *		drObject,
				Point2DI *			xp,
				const InsertedObject *		io,
				const PositionGeometry *	pg,
				const LayoutContext *		lc,
				const EditDocument *		ed )
    {
    const TedDocument *		td= (TedDocument *)ed->edPrivateData;

    docGetPixelRectForPos( drObject, lc,
			    pg->pgXTwips, pg->pgXTwips+ io->ioTwipsWide,
			    &(pg->pgTopPosition), &(pg->pgBasePosition) );

    /* Do not trust pg->pgXTwips yet */
    drObject->drX0= pg->pgXPixels;
    drObject->drX1= drObject->drX0+ io->ioPixelsWide- 1;

    switch( td->tdObjectResizeCorner )
	{
	case -1:
	    break;

	case RESIZE_BOTTOM_LEFT:
	case RESIZE_MIDDLE_LEFT:
	case RESIZE_TOP_LEFT:
	    drObject->drX0 += td->tdObjectCornerMovedX;
	    break;

	case RESIZE_BOTTOM_RIGHT:
	case RESIZE_MIDDLE_RIGHT:
	case RESIZE_TOP_RIGHT:
	    drObject->drX1 += td->tdObjectCornerMovedX;
	    break;

	case RESIZE_BOTTOM_MIDDLE:
	case RESIZE_TOP_MIDDLE:
	    break;

	default:
	    LDEB(td->tdObjectResizeCorner);
	    break;
	}

    if  ( drObject->drX1 <= drObject->drX0 )
	{ drObject->drX1= drObject->drX0+ 1;	}

    switch( td->tdObjectResizeCorner )
	{
	case -1:
	    break;

	case RESIZE_TOP_LEFT:
	case RESIZE_TOP_MIDDLE:
	case RESIZE_TOP_RIGHT:
	    drObject->drY0 += td->tdObjectCornerMovedY;
	    break;

	case RESIZE_BOTTOM_LEFT:
	case RESIZE_BOTTOM_MIDDLE:
	case RESIZE_BOTTOM_RIGHT:
	    drObject->drY1 += td->tdObjectCornerMovedY;
	    break;

	case RESIZE_MIDDLE_LEFT:
	case RESIZE_MIDDLE_RIGHT:
	    break;

	default:
	    LDEB(td->tdObjectResizeCorner);
	    break;
	}

    if  ( drObject->drY1 <= drObject->drY0 )
	{ drObject->drY1= drObject->drY0+ 1;	}

    if  ( xp )
	{
	int		xl, xm, xr;
	int		yb, ym, yt;

	xl= drObject->drX0;
	xm= ( drObject->drX0+ drObject->drX1 )/ 2- RESIZE_BLOCK/2;
	xr= drObject->drX1- RESIZE_BLOCK;

	yt= drObject->drY0;
	ym= ( drObject->drY0+ drObject->drY1 )/ 2- RESIZE_BLOCK/2;
	yb= drObject->drY1- RESIZE_BLOCK;

	xp[RESIZE_BOTTOM_LEFT].x= xl;
	xp[RESIZE_BOTTOM_LEFT].y= yb;

	xp[RESIZE_BOTTOM_MIDDLE].x= xm;
	xp[RESIZE_BOTTOM_MIDDLE].y= yb;

	xp[RESIZE_BOTTOM_RIGHT].x= xr;
	xp[RESIZE_BOTTOM_RIGHT].y= yb;

	xp[RESIZE_MIDDLE_LEFT].x= xl;
	xp[RESIZE_MIDDLE_LEFT].y= ym;

	xp[RESIZE_MIDDLE_RIGHT].x= xr;
	xp[RESIZE_MIDDLE_RIGHT].y= ym;

	xp[RESIZE_TOP_LEFT].x= xl;
	xp[RESIZE_TOP_LEFT].y= yt;

	xp[RESIZE_TOP_MIDDLE].x= xm;
	xp[RESIZE_TOP_MIDDLE].y= yt;

	xp[RESIZE_TOP_RIGHT].x= xr;
	xp[RESIZE_TOP_RIGHT].y= yt;
	}

    return;
    }

