/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to preserve links when the	*/
/*  PostScript is converted to PDF.					*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<psPrint.h>
#   include	<docCalculateToc.h>
#   include	<docEvalField.h>
#   include	"docPsPrintImpl.h"
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Leave a trace in the PDF document information.			*/
/*									*/
/************************************************************************/

static void psSaveInfo(		const char *		tag,
				const char *		info,
				PrintingState *		ps )
    {
    if  ( ! info )
	{ return;	}

    sioOutPrintf( ps->psSos, "  %s (", tag );
    psPrintString( ps->psSos, (unsigned char *)info, strlen( info ),
								ps->ps7Bits );
    sioOutPrintf( ps->psSos, ")\n" );
    }

static void psSaveMbInfo(	const char *		tag,
				const MemoryBuffer *	info,
				PrintingState *		ps )
    {
    if  ( utilMemoryBufferIsEmpty( info ) )
	{ return;	}

    sioOutPrintf( ps->psSos, "  %s (", tag );
    psPrintString( ps->psSos, (unsigned char *)info->mbBytes, info->mbSize,
								ps->ps7Bits );
    sioOutPrintf( ps->psSos, ")\n" );
    }

static void psSaveDate(		const char *		tag,
				const struct tm *	tm,
				PrintingState *		ps )
    {
    char	scratch[40+1];

    if  ( tm->tm_mday == 0 )
	{ return;	}

    if  ( strftime( scratch, sizeof(scratch)- 1, "D:%Y%m%d%H%M", tm ) < 1 )
	{ LDEB(1); return;	}

    psSaveInfo( tag, scratch, ps );

    return;
    }

int docPsDocinfoPdfmark( PrintingState *		ps,
			const char *			applicationName,
			const char *			applicationReference,
			const DocumentProperties *	dp )
    {
    MemoryBuffer	scratch;

    utilInitMemoryBuffer( &scratch );

    utilMemoryBufferPrintf( &scratch, "%s: %s",
				applicationName, applicationReference );

    sioOutPrintf( ps->psSos, "[\n" );

    psSaveMbInfo( "/Title",		&(dp->dpTitle), ps );
    psSaveMbInfo( "/Author",		&(dp->dpAuthor), ps );
    psSaveMbInfo( "/Subject",		&(dp->dpSubject), ps );
    psSaveMbInfo( "/Keywords",		&(dp->dpKeywords), ps );
    psSaveMbInfo( "/Creator",		&scratch, ps );
    psSaveMbInfo( "/Producer",		&scratch, ps );

    psSaveDate( "/ModDate",		&(dp->dpRevtim), ps );
    psSaveDate( "/CreationDate",	&(dp->dpCreatim), ps );

    sioOutPrintf( ps->psSos, "/DOCINFO pdfmark\n\n" );

    utilCleanMemoryBuffer( &scratch );

    return 0;
    }

/************************************************************************/

static int docPsOutlinePdfmark(	PrintingState *		ps,
				BufferDocument *	bd,
				const TocEntry *	te,
				int			closeLevel,
				int			count )
    {
    int			rval= 1;
    int			childCount= 0;

    MemoryBuffer	title;

    utilInitMemoryBuffer( &title );

    if  ( count > 1 && te[1].teLevel > te[0].teLevel )
	{
	int	c;

	childCount++;

	for ( c= 2; c < count; c++ )
	    {
	    if  ( te[c].teLevel <  te[1].teLevel )
		{ rval= c; break;	}
	    if  ( te[c].teLevel == te[1].teLevel )
		{ childCount++;		}
	    }
	}

    utilEmptyMemoryBuffer( &title );
    if  ( docCalculateRefFieldValue( &title, te->teMarkName, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    psOutlinePdfmark( ps, childCount,
			te->teLevel >= closeLevel, &title, te->teMarkName );

    if  ( childCount > 0 )
	{
	int	c;

	for ( c= 1; c < rval; c++ )
	    {
	    if  ( te[c].teLevel == te[1].teLevel )
		{
		docPsOutlinePdfmark( ps, bd, te+ c, closeLevel, count- c );
		}
	    }
	}

  ready:

    utilCleanMemoryBuffer( &title );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit a PDF outline for the document.				*/
/*									*/
/************************************************************************/

int docPsOutlinePdfmarks(	PrintingState *		ps,
				BufferDocument *	bd )
    {
    int			rval= 0;

    int			done;
    int			level;

    CalculateToc	ct;
    int			levelCount[PPoutline_COUNT];

    docInitCalculateToc( &ct );

    for ( level= 0; level < PPoutline_COUNT; level++ )
	{ levelCount[level]= 0;	}

    ct.ctBdDoc= bd;
    ct.ctTocField.tfType= TOCtypeTOC;
    ct.ctTocField.tfUseStyleLevels= 1;
    ct.ctTocField.tfStyleLevel0= 0;
    ct.ctTocField.tfStyleLevel1= PPoutlineDEEPEST;
    ct.ctTocField.tfUseOutlineLevels= 1;

    if  ( docCollectTocInput( &ct ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( level= 0; level < ct.ctEntryCount; level++ )
	{
	if  ( ct.ctEntries[level].teLevel >= 0			&&
	      ct.ctEntries[level].teLevel < PPoutline_COUNT	)
	    {
	    levelCount[ct.ctEntries[level].teLevel]++;
	    }
	}

    for ( level= 1; level < PPoutline_COUNT; level++ )
	{ levelCount[level] += levelCount[level-1];	}
    level= 0;
    while( level < PPoutlineDEEPEST && levelCount[level] < 20 )
	{ level++;	}

    psPageModePdfmark( ps->psSos, "UseOutlines" );

    done= 0;
    while( done < ct.ctEntryCount )
	{
	int		step;

	step= docPsOutlinePdfmark( ps, bd, ct.ctEntries+ done,
					    level, ct.ctEntryCount- done );
	if  ( step <= 0 )
	    { LDEB(step); rval= -1; goto ready;	}

	done += step;
	}

  ready:

    docCleanCalculateToc( &ct );

    return rval;
    }

