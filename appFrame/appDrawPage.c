/************************************************************************/
/*									*/
/*  Primitives for the page on a page layout tool etc.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>
#   include	<geoUnits.h>

#   include	"guiWidgets.h"
#   include	"guiDrawPage.h"
#   include	"guiDrawingWidget.h"
#   include	<psNup.h>

/************************************************************************/
/*									*/
/*  Draw a text like pattern between some margins.			*/
/*									*/
/************************************************************************/

static void appDrawTextPattern(	DrawingSurface			ds,
				const DocumentRectangle *	drNum,
				const DocumentRectangle *	drText )
    {
    double		wide= drText->drX1- drText->drX0+ 1;
    DocumentRectangle	drWord;

    drWord.drY0= drWord.drY1= drText->drY0+ 2;

    while( drWord.drY0 < drText->drY1- 1 )
	{
	int	wordCount;

	if  ( drNum && drWord.drY0 <= drNum->drY1 )
	    { drWord.drX0= drNum->drX1+ 1;	}
	else{ drWord.drX0= drText->drX0+ 1;	}

	wordCount= (int)( 5+ (75.0* rand() )/ (RAND_MAX+ 1.0 ) );
	while( wordCount > 0 )
	    {
	    int		wordLength;

	    wordLength= (int) ( (wide* rand() )/ (RAND_MAX+ 1.0 ) );
	    wordLength= 2+ wordLength/10;
	    drWord.drX1= drWord.drX0+ wordLength- 1;

	    if  ( drWord.drX1 >= drText->drX1- 1 )
		{
		drWord.drY0 += 2;
		drWord.drY1 += 2;

		if  ( drWord.drY0 >= drText->drY1- 1 )
		    { break;	}

		if  ( drNum && drWord.drY0 <= drNum->drY1 )
		    { drWord.drX0= drNum->drX1+ 1;	}
		else{ drWord.drX0= drText->drX0+ 1;	}
		drWord.drX1= drWord.drX0+ wordLength- 1;
		}

	    drawFillRectangle( ds, &drWord );

	    drWord.drX0 += wordLength+ 2; wordCount--;
	    }

	drWord.drY0 += 4;
	drWord.drY1 += 4;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the margins of a page.						*/
/*									*/
/************************************************************************/

static void appDrawTextPage(	DrawingSurface			ds,
				const RGB8Color	*		backColor,
				const DocumentRectangle *	drText )
    {
    DocumentRectangle	drAround= *drText;

    drAround.drX0--;
    drAround.drY0--;
    drAround.drX1++;
    drAround.drY1++;

    drawSetForegroundColor( ds, backColor );
    drawRectangle( ds, &drAround );

    drawSetForegroundColorBlack( ds );
    appDrawTextPattern( ds, (const DocumentRectangle *)0, drText );

    return;
    }

static void appDrawNupTextPage(	DrawingSurface			ds,
				const RGB8Color	*		backColor,
				int				screenFont,
				const char *			label,
				const DocumentRectangle *	drText )
    {
    if  ( screenFont >= 0 )
	{
	int				l= strlen( label );
	DocumentRectangle		drLabel;
	DocumentRectangle		drAround= *drText;
	int				shift;
	int				desc;

	drAround.drX0--;
	drAround.drY0--;
	drAround.drX1++;
	drAround.drY1++;

	drawGetTextExtents( &drLabel, ds, drText->drX0, drText->drY0,
						    screenFont, label, l );
	geoNormalizeRectangle( &drLabel, &drLabel );

	desc= drLabel.drY1- drText->drY0;
	shift= drText->drY0- drLabel.drY0;
	drLabel.drY0 += shift;
	drLabel.drY1 += shift;

	drLabel.drX1 += 2;
	drLabel.drY1 -= desc;
	drLabel.drY1 += 2;

	drawSetForegroundColor( ds, backColor );
	drawRectangle( ds, &drAround );

	drawSetForegroundColorBlack( ds );
	drawString( ds, drText->drX0+ 1, drText->drY0+ shift,
						    screenFont, label, l );
	appDrawTextPattern( ds, &drLabel, drText );
	}
    else{
	appDrawTextPage( ds, backColor, drText );
	}
    }

/************************************************************************/
/*									*/
/*  Draw the plane of the sheet of paper.				*/
/*									*/
/************************************************************************/

static void appDrawPageSheet(	const DocumentRectangle *	drSheet,
				DrawingSurface			ds )
    {
    DocumentRectangle	drAround= *drSheet;

    drawSetForegroundColorWhite( ds );
    drawFillRectangle( ds, drSheet );

    drAround.drX0--;
    drAround.drY0--;
    drAround.drX1++;
    drAround.drY1++;

    drawSetForegroundColorBlack( ds );
    drawRectangle( ds, &drAround );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of a page.					*/
/*									*/
/*  1)  Perform calculations relative to the centre, to avoid assymetry	*/
/*	from rounding when margins are identical.			*/
/*									*/
/*  pixelsPerTwip is the application pixel density. It is relates to	*/
/*	the applications intention to draw the page at a certain scale.	*/
/*									*/
/************************************************************************/

void appDrawPageDiagram(	APP_WIDGET			w,
				DrawingSurface			ds,
				const RGB8Color	*		backColor,
				double				pixelsPerTwip,
				const DocumentGeometry *	dg )
    {
    int				wide;
    int				high;

    int				sheetWidePixels;
    int				sheetHighPixels;

    DocumentRectangle		drAll;
    DocumentRectangle		drSheet;
    DocumentRectangle		drText;

    guiDrawGetSizeOfWidget( &wide, &high, w );
    drAll.drX0= 0;
    drAll.drY0= 0;
    drAll.drX1= wide- 1;
    drAll.drY1= high- 1;

    drawSetForegroundColor( ds, backColor );
    drawFillRectangle( ds, &drAll );

    sheetWidePixels= pixelsPerTwip* dg->dgPageWideTwips;
    sheetHighPixels= pixelsPerTwip* dg->dgPageHighTwips;

    drSheet.drX0= wide/2- ( sheetWidePixels/ 2 );
    drSheet.drY0= high/2- ( sheetHighPixels/ 2 );
    drSheet.drX1= wide/2+ ( sheetWidePixels/ 2 );
    drSheet.drY1= high/2+ ( sheetHighPixels/ 2 );

    appDrawPageSheet( &drSheet, ds );

    /*  1  */
    drText.drX0= wide/2- pixelsPerTwip*
			    ( dg->dgPageWideTwips/2- dg->dgLeftMarginTwips );
    drText.drX1= wide/2+ pixelsPerTwip*
			    ( dg->dgPageWideTwips/2- dg->dgRightMarginTwips );
    drText.drY0= high/2- pixelsPerTwip*
			    ( dg->dgPageHighTwips/2- dg->dgTopMarginTwips );
    drText.drY1= high/2+ pixelsPerTwip*
			    ( dg->dgPageHighTwips/2- dg->dgBottomMarginTwips );

    appDrawTextPage( ds, backColor, &drText );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw an arrow to show the direction of the paper through the	*/
/*  printer.								*/
/*									*/
/************************************************************************/

#   define	HEADW	5
#   define	HEADL	5
#   define	STEMW	3
#   define	STEML	3

#   define	ARROWD	16

static void appDrawDirectionArrow(	DrawingSurface			ds,
					int			rotate,
					int			x0,
					int			y0 )
    {
    static Point2DI	offsets[8]=
	{
	    { -STEMW,	+STEML,	},
	    { +STEMW,	+STEML,	},
	    { +STEMW,	0,	},
	    { +HEADW,	0,	},
	    { 0,	-HEADL,	},
	    { -HEADW,	0,	},
	    { -STEMW,	0,	},
	    { -STEMW,	+STEML,	},
	};

    Point2DI	points[8];
    int		i;

    if  ( rotate )
	{
	for ( i= 0; i < 8; i++ )
	    {
	    points[i].x= x0- offsets[i].y;
	    points[i].y= y0- offsets[i].x;
	    }
	}
    else{
	for ( i= 0; i < 8; i++ )
	    {
	    points[i].x= x0+ offsets[i].x;
	    points[i].y= y0+ offsets[i].y;
	    }
	}

    drawSetForegroundColorWhite( ds );

    drawFillPolygon( ds, points, 8 );

    drawSetForegroundColorBlack( ds );

    drawLines( ds, points, 8, 0 );
    }

static void appDrawDirectionArrows(	DrawingSurface		ds,
					int			rotate,
					const DocumentRectangle * drSheet )
    {
    if  ( rotate )
	{
	appDrawDirectionArrow( ds, rotate,
				    ( drSheet->drX0+ drSheet->drX1 )/ 2,
				    drSheet->drY0- ARROWD );
	appDrawDirectionArrow( ds, rotate,
				    ( drSheet->drX0+ drSheet->drX1 )/ 2,
				    drSheet->drY1+ ARROWD );
	}
    else{
	appDrawDirectionArrow( ds, rotate,
				    drSheet->drX0- ARROWD,
				    ( drSheet->drY0+ drSheet->drY1 )/ 2 );
	appDrawDirectionArrow( ds, rotate,
				    drSheet->drX1+ ARROWD,
				    ( drSheet->drY0+ drSheet->drY1 )/ 2 );
	}
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of the distribution of pages on a sheet of	*/
/*  paper.								*/
/*									*/
/*  pixelsPerTwip is the application pixel density. It is relates to	*/
/*	the applications intention to draw the page at a certain scale.	*/
/*									*/
/*  1)  When no scaling or moving is required: Use appDrawPageDiagram()	*/
/*	saves work, and is more focused at looking nice.		*/
/*  2)  The vertical orientation of X11 drawing is the opposite of the	*/
/*	mathematical convention that is used in PostScript. So swap	*/
/*	coordinates.							*/
/*  3)  As we rotate the sheet rather than the document pages, swap	*/
/*	coordinates.							*/
/*									*/
/************************************************************************/

void appDrawNupDiagram(	APP_WIDGET			w,
			DrawingSurface			ds,
			const RGB8Color	*		backColor,
			int				screenFont,
			double				pixelsPerTwip,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg )
    {
    int				wide= 0;
    int				high= 0;

    int				sheetWidePixels;
    int				sheetHighPixels;

    int				x0;
    int				x1;
    int				y0;
    int				y1;

    int				rotatePages;
    int				rotateSheet;

    DocumentRectangle		drAll;
    DocumentRectangle		drSheet;
    DocumentRectangle		drText;

    const DocumentGeometry *	dgSheet= &(pg->pgSheetGeometry);

    NupSchema			ns;
    AffineTransform2D		atPage;

    psInitNupSchema( &ns );

    /*  1  */
    if  ( pg->pgGridRows* pg->pgGridCols == 1			&&
	  dgSheet->dgPageWideTwips == dgPage->dgPageWideTwips	&&
	  dgSheet->dgPageHighTwips == dgPage->dgPageHighTwips	)
	{
	appDrawPageDiagram( w, ds, backColor, pixelsPerTwip, dgPage );
	goto ready;
	}

    guiDrawGetSizeOfWidget( &wide, &high, w );
    drAll.drX0= 0;
    drAll.drY0= 0;
    drAll.drX1= wide- 1;
    drAll.drY1= high- 1;

    drawSetForegroundColor( ds, backColor );
    drawFillRectangle( ds, &drAll );

    sheetWidePixels= pixelsPerTwip* dgSheet->dgPageWideTwips+ 0.49;
    sheetHighPixels= pixelsPerTwip* dgSheet->dgPageHighTwips+ 0.49;

    if  ( utilNupGetBaseTranform( &atPage, &rotatePages, pg, dgPage,
							    pixelsPerTwip ) )
	{ LDEB(1); goto ready;	}

    rotateSheet= rotatePages;
    if  ( utilNupSetSchema( &ns, rotateSheet, &atPage, pg,
						    pixelsPerTwip, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); goto ready;	}

    if  ( rotateSheet )
	{
	drSheet.drX0= wide/2- ( sheetHighPixels/ 2 );
	drSheet.drY0= high/2- ( sheetWidePixels/ 2 );
	drSheet.drX1= wide/2+ ( sheetHighPixels/ 2 );
	drSheet.drY1= high/2+ ( sheetWidePixels/ 2 );
	}
    else{
	drSheet.drX0= wide/2- ( sheetWidePixels/ 2 );
	drSheet.drY0= high/2- ( sheetHighPixels/ 2 );
	drSheet.drX1= wide/2+ ( sheetWidePixels/ 2 );
	drSheet.drY1= high/2+ ( sheetHighPixels/ 2 );
	}

    appDrawPageSheet( &drSheet, ds );

    if  ( pg->pgGridRows* pg->pgGridCols == 1 )
	{
	utilNupGetPageTranform( &atPage, &ns, 0 );

	x0= AT2_X( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;
	y0= AT2_Y( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;

	x1= AT2_X( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;
	y1= AT2_Y( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;

	if  ( rotatePages )
	    {
	    /*  3  */
	    drText.drX0= drSheet.drX0+ y0;
	    drText.drX1= drSheet.drX0+ y1;
	    drText.drY0= drSheet.drY0+ x0;
	    drText.drY1= drSheet.drY0+ x1;
	    }
	else{
	    drText.drX0= drSheet.drX0+ x0+ 1;
	    drText.drX1= drSheet.drX0+ x1;
	    /*  2  */
	    drText.drY0= drSheet.drY1- y0;
	    drText.drY1= drSheet.drY1- y1;
	    }

	appDrawTextPage( ds, backColor, &drText );
	}
    else{
	int		i;
	char		scratch[20];

	for ( i= 0; i < pg->pgGridRows* pg->pgGridCols; i++ )
	    {
	    utilNupGetPageTranform( &atPage, &ns, i );

	    sprintf( scratch, "%d", i+ 1 );

	    x0= AT2_X( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;
	    y0= AT2_Y( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;

	    x1= AT2_X( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;
	    y1= AT2_Y( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;

	    if  ( rotatePages )
		{
		/*  3  */
		drText.drX0= drSheet.drX0+ y0;
		drText.drX1= drSheet.drX0+ y1;
		drText.drY0= drSheet.drY0+ x0;
		drText.drY1= drSheet.drY0+ x1;
		}
	    else{
		drText.drX0= drSheet.drX0+ x0+ 1;
		drText.drX1= drSheet.drX0+ x1;
		/*  2  */
		drText.drY0= drSheet.drY1- y0;
		drText.drY1= drSheet.drY1- y1;
		}

	    appDrawNupTextPage( ds, backColor, screenFont, scratch, &drText );
	    }
	}

    appDrawDirectionArrows( ds, rotateSheet, &drSheet );

  ready:

    psCleanNupSchema( &ns );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the location for the schematic drawing of the page.		*/
/*									*/
/*  pixelsPerTwip is the physical pixel density. It bears no relation	*/
/*	to the scale at which we want to draw the page.			*/
/*									*/
/************************************************************************/

APP_WIDGET appMakePageDrawing(	APP_WIDGET			parent,
				double				pixelsPerTwip,
				int				widgetHighMm,
				APP_EVENT_HANDLER_T		redraw,
				void *				through )
    {
    APP_WIDGET		drawing;

    const double	pixPerMM= TWIPS_PER_MM* pixelsPerTwip;
    const int		pixelsWide= -1;
    const int		heightResizable= 1;
    int			pixelsHigh;

    pixelsHigh= ( widgetHighMm* pixPerMM )+ 0.5;

    appGuiMakeDrawingAreaInColumn( &drawing, parent,
					pixelsWide, pixelsHigh,
					heightResizable, redraw, through );

    return drawing;
    }

