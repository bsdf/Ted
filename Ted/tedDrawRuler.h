/************************************************************************/
/*									*/
/*  Ruler drawing primitives.						*/
/*									*/
/************************************************************************/

#   include	<drawDrawingSurface.h>
#   include	<appRuler.h>
#   include	<docTabStop.h>
#   include	<docTabStopList.h>
#   include	"tedColumnSeparator.h"

typedef struct TedTopRuler
    {
    RulerData		ttrRulerData;
    double		ttrMagnification;

				/**
				 *  Area that is accessible for column 
				 *  separators. It is not possible to shift
				 *  a column separator out of this area.
				 */
    int			ttrBlockFrameX0;
    int			ttrBlockFrameX1;
				/**
				 *  Area that can be used to set tabs
				 *  It is not possible to drag a tab
				 *  out of this area or to create a tab 
				 *  outside it.
				 */
    int			ttrParaFrameX0;
    int			ttrParaFrameX1;
				 /**
				  *  Page margins from document or section.
				  */
    int			ttrMarginX0;
    int			ttrMarginX1;
					/********************************/
					/*  Describe the area that is	*/
					/*  accessible as values.	*/
					/********************************/

    int			ttrFirstIndent;		/*  pixels		*/
    int			ttrLeftIndent;		/*  pixels		*/
    int			ttrRightIndent;		/*  pixels		*/

    TabStopList		ttrTabStopList;
    int			ttrTabKind;		/************************/
						/*  alignment for fresh	*/
						/*  tabs.		*/
						/************************/
    int			ttrColumnCount;
    ColumnSeparator *	ttrColumns;
    } TedTopRuler;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedTopRulerDrawTicks(	TedTopRuler *		ttr,
					int			ox,
					int			c0,
					int			c1 );

extern void tedHangingButtonRect(	DocumentRectangle *	drButton,
					int			value,
					const RulerData *	rd );

extern void tedRulerDrawHangingButton(	int			value,
					const RulerData *	rd );

extern void tedStandingButtonRect(	DocumentRectangle *	drButton,
					int			value,
					const RulerData *	rd );

extern void tedRulerDrawStandingButton(	int			value,
					const RulerData *	rd );

extern void tedRulerColumnControlRect(	DocumentRectangle *	drButton,
					int			x0,
					int			x1,
					const RulerData *	rd );

extern void tedRulerDrawColumnControl(	int			x0,
					int			x1,
					int			ox,
					const RulerData *	rd );

extern void tedTabSymbolRect(		DocumentRectangle *	drButton,
					int			tabX,
					const RulerData *	rd );

extern void tedRulerDrawTab(		const RulerData *	rd,
					int			tabX,
					int			kind );

extern void tedTabButtonRectangle(	DocumentRectangle *	drButton,
					int			x,
					const RulerData *	rd );

extern void tedDrawTabButton(		const RulerData *	rd,
					int			kind );


extern void tedRulerActiveRect(		DocumentRectangle *	drActive,
					const TedTopRuler * 	ttr );

extern int tedRulerMakeDrawingSurface(	TedTopRuler *		ttr,
					APP_WIDGET		w );

