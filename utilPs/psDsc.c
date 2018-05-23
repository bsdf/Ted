/************************************************************************/
/*									*/
/*  Issue Postscript DSC Comments. (Document Structuring Convention)	*/
/*									*/
/*  See:								*/
/*  http://partners.adobe.com/public/developer/en/ps/5001.DSC_Spec.pdf	*/
/*									*/
/*  Ghostview has a kind of checker:					*/
/*  - Download								*/
/*    http://mirror.cs.wisc.edu/pub/mirrors/ghost/ghostgum/gsview-4.9.tar.gz */
/*  - In the gsview-4.9/src direcory compile: (Ignore warnings)		*/
/*	gcc -o dscparse -DSTANDALONE -DUNIX dscutil.c dscparse.c	*/
/*  - Running 'dscparse some.ps' produces a lot of output with the	*/
/*    diagnostics at the head.						*/
/*  - dscparse does not know %%DocumentFonts. That is a shortcoming of	*/
/*    dscparse.								*/
/*  - dscparse complains about the conventional comments in included	*/
/*    fonts. That is a shortcoming of dscparse. See also:		*/
/*    http://partners.adobe.com/public/developer/en/font/T1_SPEC.PDF	*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<time.h>

#   include	"psPrint.h"
#   include	<geo2DInteger.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a bounding box.						*/
/*									*/
/*  %%BoundingBox <llx> <lly> <urx> <ury>				*/
/*									*/
/************************************************************************/

static void psBoundingBoxComment(
			    const PrintingState *	ps,
			    const char *		comment,
			    const char *		orientationComment )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "%%%%%s: %d %d %d %d\n",
			    comment,
			    ps->psSheetBoundingBox.drX0,
			    ps->psSheetBoundingBox.drY0,
			    ps->psSheetBoundingBox.drX1,
			    ps->psSheetBoundingBox.drY1 );

    sioOutPrintf( sos, "%%%%%s: %s\n",
				    orientationComment, ps->psOrientation );
    }

/************************************************************************/
/*									*/
/*  Write a DSC comment. Fold the value, not to exceed the maximum line	*/
/*  width.								*/
/*									*/
/************************************************************************/

static int psFoldComment(	SimpleOutputStream *	sos,
				const char *		name,
				const char *		value )
    {
    int         done;
    int		len= strlen( value );

    int		tl= strlen( name );

    while( len > 0 )
	{
	int     l;

	l= len;

	if  ( len+ tl > 72 )
	    {
	    int         ll;

	    l= ll= 0;
	    while( ll+ tl < 72 && ll < len )
		{
		if  ( isspace( value[ll] ) )
		    { l= ll+ 1; }

		ll++;
		}

	    if  ( l == 0 )
		{ l= ll;        }
	    else{
		while( isspace( value[l] ) && l < len )
		    { l++;      }
		}
	    }

	sioOutPrintf( sos, "%%%%%s ", name );
	name= "+"; tl= 1;

	for ( done= 0; done < l; done++ )
	    {
	    if  ( sioOutPutByte( value[done], sos ) < 0 )
		{ return -1;	}
	    }

	len -= l; value += l;
	sioOutPrintf( sos, "\n" );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a DSC convention document.					*/
/*									*/
/************************************************************************/

void psStartDSCDocument( const PrintingState *		ps,
			const PostScriptTypeList *	pstl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "%%!PS-Adobe-2.0\n" );
    if  ( title )
	{ psFoldComment( sos, "Title:", title );	}

    psBoundingBoxComment( ps, "BoundingBox", "Orientation" );

    sioOutPrintf( sos, "%%%%Creator: %s: %s\n",
				creatorName, creatorReference );
    sioOutPrintf( sos, "%%%%Pages: (atend)\n" );
    sioOutPrintf( sos, "%%%%PageOrder: Ascend\n" );

    if  ( pstl )
	{ psDscListFontNames( sos, ps->psPrintGeometry.pgEmbedFonts, pstl ); }

    sioOutPrintf( sos, "%%%%EndComments\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Go to the next page.						*/
/*									*/
/************************************************************************/

static int psPageOperator(		const char *		psOperator,
					const PrintingState *	ps,
					int			documentPage )
    {
    if  ( sioOutPrintf( ps->psSos, "%s %% Page doc:%d -> print:%d Sheet %d\n",
				    psOperator,
				    documentPage+ 1,
				    ps->psPagesPrinted+ 1,
				    ps->psSheetsPrinted+ 1 ) < 0 )
	{ LDEB(documentPage); return -1;	}

    return 0;
    }

int psStartPage(	PrintingState *			ps,
			int				documentPage )
    {
    int				nup= ps->psNupSchema.nsNup;
    int				firstOnSheet= 0;

    if  ( nup == 1			||
	  ps->psPagesPrinted % nup == 0	)
	{ firstOnSheet= 1;	}

    if  ( ps->psSheetsStarted > ps->psSheetsPrinted )
	{ firstOnSheet= 0;	}

    if  ( firstOnSheet )
	{
	if  ( nup == 1 )
	    {
	    sioOutPrintf( ps->psSos, "%%%%Page: %d %d\n",
				    documentPage+ 1, ps->psSheetsPrinted+ 1 );
	    }
	else{
	    sioOutPrintf( ps->psSos, "%%%%Page: (%d ..) %d\n",
				    documentPage+ 1, ps->psSheetsPrinted+ 1 );
	    }

	psBoundingBoxComment( ps, "PageBoundingBox", "PageOrientation" );

	sioOutPrintf( ps->psSos, "%%%%BeginPageSetup\n" );
	ps->psSheetsStarted= ps->psSheetsPrinted+ 1;
	}

    utilNupGetPageTranform( &(ps->psCurrentTransform),
				    &(ps->psNupSchema), ps->psPagesPrinted );

    if  ( psPageOperator( "gsave", ps, documentPage ) )
	{ LDEB(documentPage); return -1;	}

#   if 0
    if  ( firstOnSheet )
	{
	const DocumentRectangle *	dr= &(ps->psSheetBoundingBox);

	sioOutPrintf( ps->psSos, "newpath" );
	sioOutPrintf( ps->psSos, " %d %d moveto", dr->drX0, dr->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX1, dr->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX1, dr->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX0, dr->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX0, dr->drY0 );

	sioOutPrintf( ps->psSos, " stroke\n" );
	}
#   endif

    psTransformMatrix( ps->psSos, &(ps->psCurrentTransform) );
    sioOutPrintf( ps->psSos, " concat\n" );

    if  ( firstOnSheet )
	{ sioOutPrintf( ps->psSos, "%%%%EndPageSetup\n" );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish the page.							*/
/*									*/
/*  1)  Print something invisible on absolutely empty pages: Some	*/
/*	versions of acroread (5 for windows) do not like the pdf that	*/
/*	ghostscript generates for an absolutely empty page.		*/
/*									*/
/************************************************************************/

/*  1  */
static void psDrawSometingInvisible(	PrintingState *	ps )
    {
    sioOutPrintf( ps->psSos,
	    "1 setgray 0 0 moveto 1 0 rlineto stroke"
	    "  %% Avoid an empty page\n" );
    }

int psFinishPage(	PrintingState *		ps,
			int			documentPage,
			int			asLast )
    {
    int			nup= ps->psNupSchema.nsNup;
    int			pageIsEmpty;
    int			sheetIsEmpty;

    pageIsEmpty= ps->psLastPageMarked < ps->psPagesPrinted;
    sheetIsEmpty= ps->psLastSheetMarked < ps->psSheetsPrinted;

    if  ( ! pageIsEmpty )
	{
	ps->psLastSheetMarked= ps->psSheetsPrinted;
	sheetIsEmpty= 0;
	}

    if  ( asLast				||
	  nup == 1				||
	  ( ps->psPagesPrinted+ 1 ) % nup == 0	)
	{
	/*  1  */
	if  ( sheetIsEmpty )
	    { psDrawSometingInvisible( ps );	}

	if  ( psPageOperator( "showpage grestore", ps, documentPage ) )
	    { LDEB(documentPage); return -1;	}

	sioOutPrintf( ps->psSos, "%%%%PageTrailer\n" );

	ps->psPagesPrinted++;
	ps->psSheetsPrinted++;
	}
    else{
	if  ( psPageOperator( "grestore", ps, documentPage ) )
	    { LDEB(documentPage); return -1;	}

	ps->psPagesPrinted++;
	}

    if  ( asLast )
	{
	sioOutPrintf( ps->psSos, "%%%%Trailer\n" );
	sioOutPrintf( ps->psSos, "%%%%Pages: %d\n", ps->psSheetsPrinted );
	sioOutPrintf( ps->psSos, "%%%%EOF\n" );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Abort a page as there is a reason not to print it. Usually, this is	*/
/*  because it is empty.						*/
/*									*/
/************************************************************************/

void psAbortPage(	PrintingState *		ps,
			int			documentPage,
			int			asLast )
    {
    if  ( asLast					&&
	  ps->psLastSheetMarked == ps->psSheetsPrinted	)
	{ psPageOperator( "showpage grestore", ps, documentPage );	}
    else{ psPageOperator( "grestore", ps, documentPage );		}

    if  ( asLast )
	{
	sioOutPrintf( ps->psSos, "%%%%Trailer\n" );
	sioOutPrintf( ps->psSos, "%%%%Pages: %d\n", ps->psSheetsPrinted );
	sioOutPrintf( ps->psSos, "%%%%EOF\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Write the header for an EPS file.					*/
/*									*/
/************************************************************************/

void psStartEpsFile(	SimpleOutputStream *	sos,
				const char *		creator,
				const char *		title,
				int			pointsWide,
				int			pointsHigh )
    {
    time_t			now;

    now= time( (time_t *)0 );

    sioOutPrintf( sos, "%%!PS-Adobe-3.0 EPSF-3.0\n" );

    if  ( creator )
	{ psFoldComment( sos, "Creator:", creator );	}

    if  ( title )
	{ psFoldComment( sos, "Title:", title );	}

    sioOutPrintf( sos, "%%%%CreationDate: %s", ctime(&now) );
    sioOutPrintf( sos, "%%%%BoundingBox: 0 0 %d %d\n", pointsWide, pointsHigh );
    sioOutPrintf( sos, "%%%%EndComments\n");

    return;
    }

void psFinishEpsFile(	SimpleOutputStream *	sos )
    {
    /*  6  */
    sioOutPrintf( sos, "showpage\n");

    /*  7  */
    sioOutPrintf( sos, "%%%%Trailer\n" );
    sioOutPrintf( sos, "%%%%EOF\n" );

    return;
    }

