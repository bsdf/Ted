/************************************************************************/
/*									*/
/*  Drawing functionality: Utility functions.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docDraw.h"

#   include	<docTextLine.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a DrawingContext						*/
/*									*/
/************************************************************************/

void docInitDrawingContext(	DrawingContext *	dc )
    {
    dc->dcCurrentTextAttributeSet= 0;
    utilInitTextAttribute( &(dc->dcCurrentTextAttribute) );

    dc->dcCurrentColorSet= 0;
    utilInitRGB8Color( &(dc->dcCurrentColor) );

    layoutInitContext( &(dc->dcLayoutContext) );

    dc->dcClipRect= (DocumentRectangle *)0;
    dc->dcSelection= (DocumentSelection *)0;
    dc->dcSelectionGeometry= (SelectionGeometry *)0;
    dc->dcFirstPage= -1;
    dc->dcLastPage= -1;
    dc->dcDrawExternalItems= 0;
    dc->dcPostponeHeadersFooters= 0;
    dc->dcDocHasPageHeaders= 0;
    dc->dcDocHasPageFooters= 0;

    dc->dcDrawTableGrid= 0;

    dc->dcSetColorRgb= (SET_COLOR_RGB)0;
    dc->dcSetFont= (SET_FONT)0;
    dc->dcDrawShape= (DRAW_SHAPE)0;
    dc->dcDrawObject= (DRAW_OBJECT)0;
    dc->dcStartField= (START_FIELD)0;
    dc->dcFinishField= (FINISH_FIELD)0;
    dc->dcDrawTab= (DRAW_TAB)0;
    dc->dcDrawFtnsep= (DRAW_FTNSEP)0;
    dc->dcDrawUnderline= (DRAW_UNDERLINE)0;
    dc->dcDrawStrikethrough= (DRAW_STRIKETHROUGH)0;
    dc->dcDrawSpan= (DRAW_SPAN)0;

    dc->dcDrawTextLine= (DRAW_TEXT_LINE)0;
    dc->dcDrawOrnaments= (DRAW_ORNAMENTS)0;

    dc->dcFinishPage= (FINISH_PAGE)0;
    dc->dcStartPage= (START_PAGE)0;
    dc->dcInitLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;

    return;
    }

/************************************************************************/

void docInitDrawTextLine(	DrawTextLine *	dtl )
    {
    dtl->dtlThrough= (void *)0;
    dtl->dtlDrawingContext= (DrawingContext *)0;

    docInitLayoutPosition( &(dtl->dtlShiftedTop) );
    geoInitRectangle( &(dtl->dtlLineRectangle) );
    docInitLayoutPosition( &(dtl->dtlBaselinePosition) );
    docInitLayoutPosition( &(dtl->dtlShiftedBaselinePosition) );
    dtl->dtlXShift= 0;
    dtl->dtlYShift= 0;
    dtl->dtlPartUpto= 0;
    dtl->dtlParticuleData= (const ParticuleData *)0;
    dtl->dtlTextLine= (const struct TextLine *)0;
    dtl->dtlParaNode= (struct BufferItem *)0;
    dtl->dtlParagraphFrame= (ParagraphFrame *)0;
    dtl->dtlDrawParticulesSeparately= 0;
    }

void docSetDrawTextLine(	DrawTextLine *			dtl,
				void *				through,
				DrawingContext *		dc,
				const TextLine *		tl,
				const struct BufferItem *	paraBi,
				const BlockOrigin *		bo,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine )
    {
    dtl->dtlThrough= through;
    dtl->dtlDrawingContext= dc;

    dtl->dtlTextLine= tl;
    dtl->dtlPartUpto= tl->tlFirstParticule+ tl->tlParticuleCount;
    dtl->dtlBaselinePosition= tl->tlTopPosition;
    dtl->dtlBaselinePosition.lpPageYTwips += TL_BASELINE( tl );
    dtl->dtlParaNode= paraBi;
    dtl->dtlXShift= bo->boXShift;
    dtl->dtlYShift= bo->boYShift;
    docShiftPosition( &(dtl->dtlShiftedTop), bo, &(tl->tlTopPosition) );
    dtl->dtlParagraphFrame= pf;
    dtl->dtlLineRectangle= *drLine;
    dtl->dtlShiftedBaselinePosition= dtl->dtlShiftedTop;
    dtl->dtlShiftedBaselinePosition.lpPageYTwips += TL_BASELINE( tl );

    return;
    }

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

void docDrawSetColorRgb(	DrawingContext *	dc,
				void *			through,
				const RGB8Color *	rgb8 )
    {
    if  ( ! dc->dcCurrentColorSet				||
	  dc->dcCurrentColor.rgb8Red != rgb8->rgb8Red		||
	  dc->dcCurrentColor.rgb8Green != rgb8->rgb8Green	||
	  dc->dcCurrentColor.rgb8Blue != rgb8->rgb8Blue		)
	{
	if  ( ! dc->dcSetColorRgb			||
	      (*dc->dcSetColorRgb)( dc, through, rgb8 )	)
	    { LDEB(1); return;	}

	dc->dcCurrentColor= *rgb8;
	dc->dcCurrentColorSet= 1;
	}

    return;
    }

void docDrawSetColorNumber(	DrawingContext *	dc,
				void *			through,
				int			colorNumber )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    RGB8Color			rgb8;

    docGetColorByNumber( &rgb8, lc->lcDocument, colorNumber );
    docDrawSetColorRgb( dc, through, &rgb8 );

    return;
    }

void docDrawSetFont(		DrawingContext *	dc,
				void *			through,
				int			textAttr,
				const TextAttribute *	newTa )
    {
    TextAttribute *	curTa= &(dc->dcCurrentTextAttribute);

    if  ( ! dc->dcCurrentTextAttributeSet				||
	  curTa->taFontNumber != newTa->taFontNumber			||
	  curTa->taFontSizeHalfPoints != newTa->taFontSizeHalfPoints	||
	  curTa->taFontIsBold != newTa->taFontIsBold			||
	  curTa->taFontIsSlanted != newTa->taFontIsSlanted		||
	  curTa->taSmallCaps != newTa->taSmallCaps			||
	  curTa->taSuperSub != newTa->taSuperSub			)
	{
	if  ( ! dc->dcSetFont					||
	      (*dc->dcSetFont)( dc, through, textAttr, newTa )	)
	    { LDEB(1); return;	}

	dc->dcCurrentTextAttributeSet= 1;
	*curTa= *newTa;
	}

    return;
    }

