/************************************************************************/
/*									*/
/*  Print PostScript: Borders and Shading.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"docDraw.h"
#   include	"docPsPrintImpl.h"
#   include	<psShading.h>

#   include	<appDebugon.h>

#   define	SHOW_PAGE_GRID	0

/************************************************************************/
/*									*/
/*  Printing of borders.						*/
/*									*/
/*  1)  The border is around paragraphs. Both the border and its	*/
/*      are outside the frame.						*/
/*									*/
/************************************************************************/

static void psSolidBorderProc(	SimpleOutputStream *		sos,
				const char *			name )
    {
    sioOutPrintf( sos, "%% x y w h\n" );

    sioOutPrintf( sos, "/%s\n", name );
    sioOutPrintf( sos, "  {\n" );
    sioOutPrintf( sos, "  rectfill\n" );
    sioOutPrintf( sos, "  } bind def\n" );
    return;
    }

static void psDashHorBorderProc( SimpleOutputStream *		sos,
				const char *			name,
				const unsigned char *		dashes,
				int				dashCount )
    {
    int		i;
    int		dashLength= 0;

    sioOutPrintf( sos, "%% x y w h\n" );

    sioOutPrintf( sos, "/%s\n", name );
    sioOutPrintf( sos, "  {\n" );

    sioOutPrintf( sos, "  gsave\n" );

    for ( i= 0; i < dashCount; i++ )
	{ dashLength += dashes[i];	}

    /* dx y th x0 */
    if  ( dashLength > 0 )
	{
	sioOutPrintf( sos, "  dup %d mod ", dashLength );
	/* x y w h offset */
	}
    else{
	sioOutPrintf( sos, "  " );
	}

    sioOutPrintf( sos, "[" );
    for ( i= 0; i < dashCount; i++ )
	{ sioOutPrintf( sos, " %d", dashes[i] ); }
    sioOutPrintf( sos, " ]" );

    if  ( dashLength > 0 )
	{
	/* x y w h offset [] */
	sioOutPrintf( sos, " exch setdash\n" );
	}
    else{
	/* x y w h [] */
	sioOutPrintf( sos, " 0 setdash\n" );
	}

    /* x y w h */
    sioOutPrintf( sos, "  dup setlinewidth 0 setlinecap\n" );
    /* x y w h -> x w y h -> x w y+h/2 ->  w x y+h/2 */
    sioOutPrintf( sos, "  3 -1 roll exch 1 add 2 div add 3 -1 roll exch\n" );

    /* dx y x0 */
    sioOutPrintf( sos, "  newpath moveto " );

    /* dx */
    sioOutPrintf( sos, " 0 rlineto stroke\n" );

    sioOutPrintf( sos, "  grestore\n" );
    sioOutPrintf( sos, "  } bind def\n" );
    }

static void psDashVerBorderProc( SimpleOutputStream *		sos,
				const char *			name,
				const unsigned char *		dashes,
				int				dashCount )
    {
    int		i;
    int		dashLength= 0;

    sioOutPrintf( sos, "%% x y w h\n" );

    sioOutPrintf( sos, "/%s\n", name );
    sioOutPrintf( sos, "  {\n" );

    sioOutPrintf( sos, "  gsave\n" );

    for ( i= 0; i < dashCount; i++ )
	{ dashLength += dashes[i];	}

    /* x y w h */
    if  ( dashLength > 0 )
	{
	sioOutPrintf( sos, "  dup %d mod ", dashLength );
	/* x y w h offset */
	}
    else{
	sioOutPrintf( sos, "  " );
	}

    sioOutPrintf( sos, "[" );
    for ( i= 0; i < dashCount; i++ )
	{ sioOutPrintf( sos, " %d", dashes[i] ); }
    sioOutPrintf( sos, " ]" );

    if  ( dashLength > 0 )
	{
	/* x y w h offset [] */
	sioOutPrintf( sos, " exch setdash\n" );
	}
    else{
	/* x y w h [] */
	sioOutPrintf( sos, " 0 setdash\n" );
	}

    /* x y w h */
    sioOutPrintf( sos, "  exch dup setlinewidth 0 setlinecap\n" );
    /* x y h w -> y h x w -> y h x+w/2 -> h x+w/2 y */
    sioOutPrintf( sos, "  4 -1 roll exch 1 add 2 div add 3 -1 roll\n" );

    /* h x+w/2 y */
    sioOutPrintf( sos, "  newpath moveto " );

    /* h */
    sioOutPrintf( sos, " 0 exch rlineto stroke\n" );

    sioOutPrintf( sos, "  grestore\n" );
    sioOutPrintf( sos, "  } bind def\n" );
    }

static const unsigned char PSborderDASH[]=	{ 60, 60 };
static const unsigned char PSborderDOT[]=	{ 20, 20 };
static const unsigned char PSborderDASHD[]=	{ 60, 40, 20, 40 };
static const unsigned char PSborderDASHDD[]=	{ 60, 40, 20, 40, 20, 40 };

void psDefineBorderProcs(	SimpleOutputStream *	sos )
    {
    psSolidBorderProc( sos, "h-brdrs" );

    psDashHorBorderProc( sos, "h-brdrdash",
				    PSborderDASH, sizeof(PSborderDASH) );
    psDashHorBorderProc( sos, "h-brdrdot",
				    PSborderDOT, sizeof(PSborderDOT) );
    psDashHorBorderProc( sos, "h-brdrdashd",
				    PSborderDASHD, sizeof(PSborderDASHD) );
    psDashHorBorderProc( sos, "h-brdrdashdd",
				    PSborderDASHDD, sizeof(PSborderDASHDD) );

    psSolidBorderProc( sos, "v-brdrs" );

    psDashVerBorderProc( sos, "v-brdrdash",
				    PSborderDASH, sizeof(PSborderDASH) );
    psDashVerBorderProc( sos, "v-brdrdot",
				    PSborderDOT, sizeof(PSborderDOT) );
    psDashVerBorderProc( sos, "v-brdrdashd",
				    PSborderDASHD, sizeof(PSborderDASHD) );
    psDashVerBorderProc( sos, "v-brdrdashdd",
				    PSborderDASHDD, sizeof(PSborderDASHDD) );

    }

/************************************************************************/
/*									*/
/*  Draw a vertical border.						*/
/*									*/
/************************************************************************/

static int docPsPrintVerticalBorder(
				const BorderProperties *	bpVert,
				const DocumentRectangle *	dr,
				PrintingState *			ps,
				struct DrawingContext *		dc )
    {
    int		wide= dr->drX1- dr->drX0+ 1;
    int		high= dr->drY1- dr->drY0+ 1;

    docDrawSetColorNumber( dc, (void *)ps, bpVert->bpColor );

    switch( bpVert->bpStyle )
	{
	case DOCbsDASH:
	    sioOutPrintf( ps->psSos, "%d %d %d %d v-brdrdash ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	case DOCbsDOT:
	    sioOutPrintf( ps->psSos, "%d %d %d %d v-brdrdot ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	case DOCbsDASHD:
	    sioOutPrintf( ps->psSos, "%d %d %d %d v-brdrdashd ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	case DOCbsDASHDD:
	    sioOutPrintf( ps->psSos, "%d %d %d %d v-brdrdashdd ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	default:
	    sioOutPrintf( ps->psSos, "%d %d %d %d v-brdrs ",
					     dr->drX0, dr->drY0, wide, high );
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw an horizontal border.						*/
/*									*/
/************************************************************************/

static int docPsPrintHorizontalBorder(
				const BorderProperties *	bpHor,
				const DocumentRectangle *	dr,
				PrintingState *			ps,
				struct DrawingContext *		dc )
    {
    int		wide= dr->drX1- dr->drX0+ 1;
    int		high= dr->drY1- dr->drY0+ 1;

    docDrawSetColorNumber( dc, (void *)ps, bpHor->bpColor );

    switch( bpHor->bpStyle )
	{
	case DOCbsDASH:
	    sioOutPrintf( ps->psSos, "%d %d %d %d h-brdrdash ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	case DOCbsDOT:
	    sioOutPrintf( ps->psSos, "%d %d %d %d h-brdrdot ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	case DOCbsDASHD:
	    sioOutPrintf( ps->psSos, "%d %d %d %d h-brdrdashd ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	case DOCbsDASHDD:
	    sioOutPrintf( ps->psSos, "%d %d %d %d h-brdrdashdd ",
					     dr->drX0, dr->drY0, wide, high );
	    break;

	default:
	    sioOutPrintf( ps->psSos, "%d %d %d %d h-brdrs ",
					     dr->drX0, dr->drY0, wide, high );
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw background ornaments.						*/
/*									*/
/************************************************************************/

int docPsPrintOrnaments(	const BlockOrnaments *		bo,
				int				page,
				const DocumentRectangle *	drOutside,
				const DocumentRectangle *	drInside,
				void *				through,
				struct DrawingContext *		dc )
    {
    PrintingState *		ps= (PrintingState *)through;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				done= 0;

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawSHADE ) )
	{
	int			isFilled= 0;
	RGB8Color		rgb8;

	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8,
						bd, &(bo->boShading) ) )
	    { LDEB(1);	}

	if  ( isFilled )
	    {
	    docDrawSetColorRgb( dc, (void *)ps, &rgb8 );

	    psFillRectangle( ps, 
			    drInside->drX0, drInside->drY0,
			    drInside->drX1- drInside->drX0+ 1,
			    drInside->drY1- drInside->drY0+ 1 );

	    dc->dcCurrentColorSet= 0;
	    }

	if  ( bo->boShading.isPattern != DOCspSOLID )
	    {
	    RGB8Color			cf;
	    SimpleOutputStream *	sos= ps->psSos;

	    utilInitRGB8Color( &cf );
	    cf.rgb8Red= cf.rgb8Green= cf.rgb8Blue= 0;

	    if  ( bo->boShading.isForeColor > 0			&&
		  bo->boShading.isForeColor < dp->dpColorPalette->cpColorCount )
		{ cf= dp->dpColorPalette->cpColors[bo->boShading.isForeColor]; }

	    docDrawSetColorRgb( dc, (void *)ps, &cf );

	    psRectanglePath( ps, 
			    drInside->drX0, drInside->drY0,
			    drInside->drX1- drInside->drX0+ 1,
			    drInside->drY1- drInside->drY0+ 1 );

	    sioOutPrintf( sos, "%s\n",
				PsShadingNames[bo->boShading.isPattern] );
	    }
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_BORDER ) )
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY1= drInside->drY0- 1;

	docPsPrintHorizontalBorder( &(bo->boTopBorder), &drBorder, ps, dc );
	done= 1;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_BORDER ) )
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY0= drInside->drY0;
	drBorder.drY1= drInside->drY1;
	drBorder.drX1= drInside->drX0- 1;

	docPsPrintVerticalBorder( &(bo->boLeftBorder), &drBorder, ps, dc );
	done= 1;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_BORDER ) )
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY0= drInside->drY0;
	drBorder.drY1= drInside->drY1;
	drBorder.drX0= drInside->drX1+ 1;

	docPsPrintVerticalBorder( &(bo->boRightBorder), &drBorder, ps, dc );
	done= 1;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_BORDER ) )
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY0= drInside->drY1+ 1;

	docPsPrintHorizontalBorder( &(bo->boBottomBorder), &drBorder, ps, dc );
	done= 1;
	}

#   if 0
    if  ( ! utilPropMaskIsEmpty( &(bo->boPropMask) ) )
	{
	sioOutPrintf( ps->psSos, " gsave 1 0 0 setrgbcolor " );
	sioOutPrintf( ps->psSos, " newpath %d %d moveto ",
					drInside->drX0, drInside->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto stroke grestore ",
					drInside->drX1, drInside->drY1 );

	sioOutPrintf( ps->psSos, " gsave 0 0 1 setrgbcolor " );
	sioOutPrintf( ps->psSos, " newpath %d %d moveto ",
					drInside->drX0, drInside->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto stroke grestore ",
					drInside->drX1, drInside->drY0 );
	}
#   endif

    if  ( done )
	{ sioOutPrintf( ps->psSos, "\n" ); }

    return 0;
    }

