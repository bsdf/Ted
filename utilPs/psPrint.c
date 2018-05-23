/************************************************************************/
/*									*/
/*  Print Postscript.							*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"psPrint.h"
#   include	<geo2DInteger.h>
#   include	<sioGeneral.h>
#   include	<uniUtf8.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialisation for 'nup' printing.					*/
/*									*/
/************************************************************************/

int psSetNupSchema(	PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter )
    {
    const double		fac= 0.05;
    int				rotatePages;

    AffineTransform2D		at1Page;

    if  ( utilNupGetBaseTranform( &at1Page, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); return -1;	}

    ps->psRotateSheetGrid= rotatePages;
    ps->psPrintGeometry= *pg;

    if  ( utilNupSetSchema( &(ps->psNupSchema),
			ps->psRotateSheetGrid, &at1Page, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); return -1;	}

    if  ( ps->psRotateSheetGrid )
	{ ps->psOrientation= "Landscape";	}
    else{ ps->psOrientation= "Portrait";	}

    psNupSheetBoundingBox( &(ps->psSheetBoundingBox), &(ps->psNupSchema),
					dgPage, hasPageHeader, hasPageFooter );

    return 0;
    }

void psRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage )
    {
    const double		fac= 0.05;
    const PrintGeometry *	pg= &(ps->psPrintGeometry);
    AffineTransform2D		at1Page;

    int				rotatePages; /* ignored */

    if  ( utilNupGetBaseTranform( &at1Page, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); return;	}

    if  ( utilNupSetSchema( &(ps->psNupSchema),
			ps->psRotateSheetGrid, &at1Page, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); return;	}

    /* ?
    psNupSheetBoundingBox( &(ps->psSheetBoundingBox), &(ps->psNupSchema),
					dgPage, hasPageHeader, hasPageFooter );
    */

    return;
    }

/************************************************************************/
/*									*/
/*  Issue a character string.						*/
/*									*/
/************************************************************************/

int psPrintString(	SimpleOutputStream *	sos,
			const unsigned char *	s,
			int			len,
			int			sevenBits )
    {
    int				i;

    for ( i= 0; i < len; s++, i++ )
	{
	if  ( *s == '(' || *s == ')' || *s == '\\' )
	    {
	    if  ( sioOutPutByte( '\\', sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( *s, sos ) < 0 )
		{ return -1;	}
	    continue;
	    }

	if  ( *s == '\r' )
	    {
	    if  ( sioOutPutByte( '\\', sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( 'r', sos ) < 0 )
		{ return -1;	}
	    continue;
	    }

	if  ( *s == '\n' )
	    {
	    if  ( sioOutPutByte( '\\', sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( 'n', sos ) < 0 )
		{ return -1;	}
	    continue;
	    }

	if  ( (   isascii( *s ) && ! isprint( *s )	)	||
	      ( ! isascii( *s ) && sevenBits		)	)
	    {
	    sioOutPrintf( sos, "\\%03o", *s );
	    continue;
	    }

	if  ( sioOutPutByte( *s, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

int psPrintStringValue(	PrintingState *		ps,
			const unsigned char *	s,
			int			len )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, ps->ps7Bits ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( ')', sos ) < 0 )
	{ return -1;	}

    return 0;
    }

static int psPrintUnicodeStringValue(	PrintingState *	ps,
					const char *	s,
					int		len )
    {
    SimpleOutputStream *	sos= ps->psSos;
    int				off;

    if  ( sioOutPutString( "<FEFF", sos ) < 0 )
	{ return -1;	}

    off= 0;
    while( off < len )
	{
	unsigned short		uni;
	int			step= uniGetUtf8( &uni, s+ off );

	if  ( step < 1 )
	    { XXLDEB(s[off],s[off+1],step); return -1;	}

	sioOutPrintf( sos, "%04X", uni );

	off += step;
	}

    if  ( sioOutPutByte( '>', sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int psPrintPdfMarkStringValue(	PrintingState *		ps,
				const unsigned char *	s,
				int			len )
    {
    int		i;

    for ( i= 0; i < len; i++ )
	{
	if  ( s[i] >= 127 )
	    { return psPrintUnicodeStringValue( ps, (const char *)s, len ); }
	}

    return psPrintStringValue( ps, s, len );
    }

int psMoveShowString(	PrintingState *		ps,
			const unsigned char *	s,
			int			len,
			int			x,
			int			y )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, ps->ps7Bits ) < 0 )
	{ return -1;	}

    sioOutPrintf( sos, ") %d %d mvs\n", x, y );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

int psShowString(	PrintingState *		ps,
			const unsigned char *	s,
			int			len )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, ps->ps7Bits ) < 0 )
	{ return -1;	}

    sioOutPrintf( sos, ") utf8show " );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the font, this depends on the assumption that fonts have been	*/
/*  defined for the different fonts in the document.			*/
/*									*/
/*  NOTE that 'smallcaps' is never set for characters that are not to	*/
/*  be scaled.								*/
/*									*/
/************************************************************************/

int psSetFont(	PrintingState *			ps,
		const AfmFontInfo *		afi,
		const TextAttribute *		ta )
    {
    SimpleOutputStream *	sos= ps->psSos;
    int				fontSizeTwips= 10* ta->taFontSizeHalfPoints;
    char			fontName[40];

    if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT	||
	  ta->taSuperSub == TEXTvaSUBSCRIPT	)
	{ fontSizeTwips= SUPERSUB_SIZE( fontSizeTwips ); }

    if  ( ta->taSmallCaps )
	{ fontSizeTwips= ( 8* fontSizeTwips )/ 10;	}

    if  ( fontSizeTwips == 0 )
	{ LDEB(fontSizeTwips); fontSizeTwips= 1;	}

    psSetFontName( fontName, afi );
    sioOutPrintf( sos, "%d %s\n", fontSizeTwips, fontName );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialise a printing session.					*/
/*									*/
/************************************************************************/

void psInitPrintingState(	PrintingState *	ps )
    {
    ps->psSos= (SimpleOutputStream *)0;

    ps->psLastPageMarked= -1;
    ps->psLastSheetMarked= -1;
    ps->psPagesPrinted= 0;
    ps->psSheetsPrinted= 0;
    ps->psSheetsStarted= 0;

    psInitPrintGeometry( &(ps->psPrintGeometry) );

    psInitNupSchema( &(ps->psNupSchema) );
    ps->psRotateSheetGrid= 0;
    geoIdentityAffineTransform2D( &(ps->psCurrentTransform) );

    ps->psOrientation= (char *)0;

    geoInitRectangle( &(ps->psSheetBoundingBox) );

    ps->psInsideLink= 0;

    utilInitMemoryBuffer( &(ps->psNoteRef) );
    utilInitMemoryBuffer( &(ps->psNoteDef) );

    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= -1;
    utilInitMemoryBuffer( &(ps->psLinkFile) );
    utilInitMemoryBuffer( &(ps->psLinkMark) );

    ps->psUsePostScriptFilters= 1;
    ps->psUsePostScriptIndexedImages= 1;
    ps->ps7Bits= 0;

    return;
    }

void psCleanPrintingState(	PrintingState *	ps )
    {
    psCleanNupSchema( &(ps->psNupSchema) );

    psCleanPrintGeometry( &(ps->psPrintGeometry) );

    utilCleanMemoryBuffer( &(ps->psNoteRef) );
    utilCleanMemoryBuffer( &(ps->psNoteDef) );
    utilCleanMemoryBuffer( &(ps->psLinkMark) );
    utilCleanMemoryBuffer( &(ps->psLinkFile) );

    return;
    }

/************************************************************************/
/*									*/
/*  Set the color for subsequent drawing operations.			*/
/*									*/
/************************************************************************/

int psSetRgb8Color(	PrintingState *		ps,
			const RGB8Color *	rgb8 )
    {
    if  ( rgb8->rgb8Red == rgb8->rgb8Green	&&
	  rgb8->rgb8Green == rgb8->rgb8Blue	)
	{
	if  ( sioOutPrintf( ps->psSos, "%g setgray\n",
					    rgb8->rgb8Red/ 255.0 ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos, "%g %g %g setrgbcolor\n",
						rgb8->rgb8Red/ 255.0,
						rgb8->rgb8Green/ 255.0,
						rgb8->rgb8Blue/ 255.0 ) < 0 )
	    { LDEB(1); return -1;	}
	}

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Issue a 2D affine transform in PS notation.				*/
/*									*/
/************************************************************************/

void psTransformMatrix(	SimpleOutputStream *		sos,
			const AffineTransform2D *	at )
    {
    sioOutPrintf( sos, "[ %g %g %g %g %g %g ]",
				    at->at2Axx, at->at2Axy,
				    at->at2Ayx, at->at2Ayy,
				    at->at2Tx, at->at2Ty );
    return;
    }

/************************************************************************/
/*									*/
/*  Define a procedure from an array of lines of PostScript code.	*/
/*  Empty lines and lines completely consisting of a comment are	*/
/*  skipped.								*/
/*									*/
/************************************************************************/

void psDefineProcedure(	SimpleOutputStream *	sos,
				const char **		lines,
				int			count )
    {
    int		i;

    for ( i= 0; i < count; lines++, i++ )
	{
	const char *	s= *lines;

	while( isspace( *s ) )
	    { s++;	}

	if  ( ! *s || *s == '%' )
	    { continue;	}

	sioOutPrintf( sos, "%s\n", *lines );
	}

    sioOutPrintf( sos, "\n" );
    }

/************************************************************************/
/*									*/
/*  Fill a rectangle							*/
/*									*/
/************************************************************************/

void psFillRectangle(		PrintingState *		ps,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n", x, y, wide, high );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

void psRectanglePath(		PrintingState *		ps,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    sioOutPrintf( ps->psSos, "newpath %d %d moveto ", x, y );
    sioOutPrintf( ps->psSos, "%d 0 rlineto ", wide );
    sioOutPrintf( ps->psSos, "0 %d rlineto ", high );
    sioOutPrintf( ps->psSos, "%d 0 rlineto\n", -wide );
    sioOutPrintf( ps->psSos, "0 %d rlineto closepath\n", -high );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the destination of a pdfmark jump in the Printout.		*/
/*									*/
/************************************************************************/

int psDestPdfmark(		PrintingState *		ps,
				int			lineTop,
				const MemoryBuffer *	mbRef )
    {
    psEmitDestPdfmark( ps->psSos, &(ps->psCurrentTransform), lineTop, mbRef );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

void psFlushLink(		PrintingState *		ps,
				int			x0,
				int			wide,
				int			lineTop,
				int			lineHeight )
    {
    if  ( ps->psLinkParticulesDone > 0 )
	{
	DocumentRectangle	drLink;

	drLink.drX0= ps->psLinkRectLeft;
	drLink.drY0= lineTop+ lineHeight;
	drLink.drX1= x0+ wide;
	drLink.drY1= lineTop;

	psSourcePdfmark( ps->psSos, &drLink,
				    &(ps->psLinkFile), &(ps->psLinkMark) );

	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= x0;
	}

    return;
    }

