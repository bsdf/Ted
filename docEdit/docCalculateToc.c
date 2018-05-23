/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docCalculateToc.h"
#   include	"docTocBookmarks.h"
#   include	"docIntermediaryDocument.h"
#   include	<docTocField.h>
#   include	<docTreeType.h>
#   include	<docSeqField.h>
#   include	<docBookmarkField.h>
#   include	<docTextParticule.h>

#   include	<appDebugon.h>

/************************************************************************/

static void docInitTocEntry(	TocEntry *	te )
    {
    te->teLevel= PPoutlineBODYTEXT;
    te->teField= (const DocumentField *)0;
    docInitDocumentSelection( &(te->teDsInside) );
    docInitDocumentSelection( &(te->teDsAround) );
    te->tePart0= 0;
    te->tePart1= 0;
    te->teMarkName= (const MemoryBuffer *)0;
    te->teNumbered= 1;
    }

/************************************************************************/

void docInitCalculateToc(	CalculateToc *	ct )
    {
    int		i;

    ct->ctBdToc= (BufferDocument *)0;
    ct->ctBdDoc= (BufferDocument *)0;
    ct->ctSectNode= (BufferItem *)0;
    ct->ctDfTocTo= (DocumentField *)0;

    docInitTocField( &(ct->ctTocField) );
    docLayoutInitBlockFrame( &(ct->ctBlockFrame) );
    docInitParagraphProperties( &(ct->ctRefPP) );
    utilInitTextAttribute( &(ct->ctTextAttribute) );
    ct->ctDefaultTextAttributeNumber= -1;
    for ( i= 0; i < PPoutline_COUNT; i++ )
	{
	ct->ctLevelStyles[i]= (const DocumentStyle *)0;
	ct->ctLevelAttributeNumbers[i]= -1;
	}
    utilInitIndexSet( &(ct->ctStyleNumbers) );

    ct->ctEntries= (TocEntry *)0;
    ct->ctEntryCount= 0;
    ct->ctDepth= 0;

    ct->ctStyleLevels= (unsigned char *)0;
    ct->ctStyleLevelCount= 0;
    return;
    }

void docCleanCalculateToc(	CalculateToc *	ct )
    {
    if  ( ct->ctBdToc )
	{ docFreeIntermediaryDocument( ct->ctBdToc );	}

    docCleanTocField( &(ct->ctTocField) );
    docLayoutCleanBlockFrame( &(ct->ctBlockFrame) );
    docCleanParagraphProperties( &(ct->ctRefPP) );

    utilInitIndexSet( &(ct->ctStyleNumbers) );

    if  ( ct->ctEntries )
	{ free( ct->ctEntries );	}

    if  ( ct->ctStyleLevels )
	{ free( ct->ctStyleLevels );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Sort Toc entries. Sort by increasing paragraph number,		*/
/*  decreasing size and increasing position.				*/
/*									*/
/************************************************************************/

static int docCompareTocEntries(	const void *	voidte1,
					const void *	voidte2	)
    {
    const TocEntry *	te1= (const TocEntry *)voidte1;
    const TocEntry *	te2= (const TocEntry *)voidte2;
    int			parts1;
    int			parts2;

    if  ( te1->teField->dfHeadPosition.epParaNr >
	  te2->teField->dfHeadPosition.epParaNr )
	{ return  1;	}

    if  ( te1->teField->dfHeadPosition.epParaNr <
	  te2->teField->dfHeadPosition.epParaNr )
	{ return -1;	}

    parts1= te1->tePart1- te1->tePart0;
    parts2= te2->tePart1- te2->tePart0;
    if  ( parts1 < parts2 )
	{ return  1;	}
    if  ( parts1 > parts2 )
	{ return -1;	}

    if  ( te1->tePart0 > te2->tePart0 )
	{ return  1;	}
    if  ( te1->tePart0 < te2->tePart0 )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Does a field go to the TOC?						*/
/*									*/
/************************************************************************/

static int docGetTocEntry(	TocEntry *		te,
				const CalculateToc *	ct )
    {
    char			flag[2];
    const TocField *		tf= &(ct->ctTocField);

    if  ( te->teField->dfKind == DOCfkBOOKMARK )
	{
	BufferItem *	paraNode;

	if  ( tf->tfType != TOCtypeTOC )
	    { return 1;	}

	if  ( ! tf->tfUseStyleNames		&&
	      ! tf->tfUseStyleLevels		&&
	      ! tf->tfUseOutlineLevels	)
	    { return 1;	}

	if  ( docDelimitFieldInDoc( &(te->teDsInside), &(te->teDsAround),
					    &(te->tePart0), &(te->tePart1),
					    ct->ctBdDoc, te->teField ) )
	    { LDEB(te->teField->dfFieldNumber); return 1; }

	if  ( docIsIBarSelection( &(te->teDsInside) ) )
	    { return 1;	}

	paraNode= te->teDsInside.dsHead.dpNode;
	if  ( te->teDsInside.dsTail.dpNode != paraNode )
	    { return 1;	}

	/*  Holds any text? */
	{
	int			part;
	const TextParticule *	tp;

	tp= te->teDsInside.dsHead.dpNode->biParaParticules+ te->tePart0+ 1;
	for ( part= te->tePart0+ 1; part < te->tePart1; tp++, part++ )
	    {
	    if  ( tp->tpKind == DOCkindSPAN && tp->tpStrlen > 0 )
		{ break;	}
	    }

	if  ( part >= te->tePart1 )
	    { return 1;	}
	}

	if  ( docFieldGetBookmark( &(te->teMarkName), te->teField ) )
	    { LDEB(te->teField->dfFieldNumber); return -1;	}

	if  ( tf->tfType == TOCtypeTOC				&&
	      tf->tfUseStyleLevels				&&
	      paraNode->biParaStyle > 0				&&
	      paraNode->biParaStyle < ct->ctStyleLevelCount	)
	    {
	    te->teLevel= ct->ctStyleLevels[paraNode->biParaStyle];

	    if  ( te->teLevel <= tf->tfStyleLevel1	&&
		  te->teLevel >= tf->tfStyleLevel0	)
		{
		te->teNumbered= te->teLevel < tf->tfNLevel0	||
				te->teLevel > tf->tfNLevel1	;
		return 0;
		}
	    }

	if  ( tf->tfType == TOCtypeTOC				&&
	      tf->tfUseStyleNames				&&
	      utilIndexSetContains( &(ct->ctStyleNumbers),
				      paraNode->biParaStyle )	)
	    {
	    te->teNumbered= 1;
	    return 0;
	    }

	if  ( tf->tfType == TOCtypeTOC				&&
	      tf->tfUseOutlineLevels				&&
	      paraNode->biParaOutlineLevel < PPoutlineBODYTEXT	)
	    {
	    te->teLevel= paraNode->biParaOutlineLevel;

	    te->teNumbered= te->teLevel < tf->tfNLevel0	||
			    te->teLevel > tf->tfNLevel1	;
	    return 0;
	    }

	return 1;
	}

    if  ( tf->tfType == TOCtypeTOC		&&
	  ( te->teField->dfKind == DOCfkTC	||
	    te->teField->dfKind == DOCfkTCN	)	)
	{
	DocumentField *	dfBookmark;

	if  ( ! tf->tfUseTcEntries			||
	      te->teLevel > tf->tfEntryLevel1	||
	      te->teLevel < tf->tfEntryLevel0	)
	    { return 1;	}

	flag[1]= '\0';
	if  ( docFieldGetTc( te->teField, flag,
				    &(te->teLevel), &(te->teNumbered) ) )
	    { LDEB(te->teField->dfFieldNumber); return -1;	}

	dfBookmark= docFindTypedChildField( &(ct->ctBdDoc->bdBody.dtRootFields),
			    &(te->teField->dfHeadPosition), DOCfkBOOKMARK );
	if  ( ! dfBookmark )
	    { /*XDEB(dfBookmark);*/ return 1;	}

	if  ( docDelimitFieldInDoc( &(te->teDsInside), &(te->teDsAround),
					&(te->tePart0), &(te->tePart1),
					ct->ctBdDoc, dfBookmark ) )
	    { LDEB(dfBookmark->dfFieldNumber); return 1; }

	if  ( docIsIBarSelection( &(te->teDsInside) ) )
	    { return 1;	}

	if  ( docFieldGetBookmark( &(te->teMarkName), dfBookmark ) )
	    { LDEB(te->teField->dfFieldNumber); return -1;	}

	te->teNumbered= te->teLevel < tf->tfNLevel0	||
			te->teLevel > tf->tfNLevel1	;

	return 0;
	}

    if  ( te->teField->dfKind == DOCfkSEQ )
	{
	SeqField		sf;
	BufferItem *		paraNode;
	const DocumentField *	dfBookmark;

	if  ( tf->tfType != TOCtypeSEQ )
	    { return 1;	}

	if  ( docGetSeqField( &sf, te->teField ) )
	    { LDEB(1); return 1;	}

	if  ( utilMemoryBufferIsEmpty( &(tf->tfIdentifierName) )	||
	      ! utilEqualMemoryBuffer( &(tf->tfIdentifierName),
						  &(sf.sfIdentifier) )	)
	    { return 1;	}

	paraNode= docGetParagraphByNumber( &(ct->ctBdDoc->bdBody),
					te->teField->dfHeadPosition.epParaNr );
	if  ( ! paraNode )
	    { XDEB(paraNode); return -1; }

	dfBookmark= docGetParaTocBookmark( ct->ctBdDoc,
					    &(ct->ctBdDoc->bdBody), paraNode );
	if  ( ! dfBookmark )
	    { XDEB(dfBookmark); return -1;	}

	if  ( docFieldGetBookmark( &(te->teMarkName), dfBookmark ) )
	    { LDEB(dfBookmark->dfFieldNumber); return -1;	}

	te->teField= dfBookmark;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Collect TOC entries. Relies on the bookmarks that should already	*/
/*  have been made.							*/
/*									*/
/*  Sorting takes care of selecting the most appropriate ones.		*/
/*									*/
/************************************************************************/

static int docCollectTocEntries(	CalculateToc *			ct )
    {
    const DocumentFieldList *	dflDoc= &(ct->ctBdDoc->bdFieldList);
    const int			fieldCount= dflDoc->dflPagedList.plItemCount;
    int				fieldNr;

    int				entryNr= -1;
    int				entryParaNr;

    TocEntry *			te;
    int				to= 0;

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	int			res;
	const DocumentField *	df= docGetFieldByNumber( dflDoc, fieldNr );

	if  ( ! df )
	    { continue;	}
	if  ( df->dfSelectionScope.ssTreeType != DOCinBODY )
	    { continue;	}

	if  ( df->dfKind != DOCfkBOOKMARK	&&
	      df->dfKind != DOCfkSEQ		&&
	      df->dfKind != DOCfkTC		&&
	      df->dfKind != DOCfkTCN		)
	    { continue;	}

	te=  ct->ctEntries+ ct->ctEntryCount;
	te->teField= df;

	res= docGetTocEntry( te, ct );
	if  ( res < 0 )
	    { LLDEB(fieldNr,res); return -1;	}
	if  ( res > 0 )
	    { continue;	}

	if  ( ct->ctDepth < te->teLevel+ 1 )
	    { ct->ctDepth=  te->teLevel+ 1;	}

	ct->ctEntryCount++;
	}

    qsort( ct->ctEntries, ct->ctEntryCount,
				    sizeof(TocEntry), docCompareTocEntries );

    entryParaNr= -1;
    to= 0;
    te= ct->ctEntries;
    for ( entryNr= 0; entryNr < ct->ctEntryCount; te++, entryNr++ )
	{
	if  ( te->teField->dfHeadPosition.epParaNr == entryParaNr )
	    { continue;	}
	entryParaNr= te->teField->dfHeadPosition.epParaNr;

	ct->ctEntries[to++]= *te;
	}

    ct->ctEntryCount= to;

    return 0;
    }

/************************************************************************/

static int docTocCollectTocStyles(	CalculateToc *		ct )
    {
    const TocField *		tf= &(ct->ctTocField);
    const BufferDocument *	bd= ct->ctBdDoc;
    int				i;

    ct->ctStyleLevels= (unsigned char *)malloc(
					bd->bdStyleSheet.dssStyleCount );
    if  ( ! ct->ctStyleLevels )
	{
	LXDEB(bd->bdStyleSheet.dssStyleCount,ct->ctStyleLevels);
	return -1;
	}

    for ( i= 0; i < bd->bdStyleSheet.dssStyleCount; i++ )
	{
	const DocumentStyle *	ds= bd->bdStyleSheet.dssStyles[i];

	ct->ctStyleLevels[i]= PPoutlineBODYTEXT;

	if  ( ds && tf->tfUseStyleLevels )
	    {
	    if  ( ds->dsOutlineLevel <= tf->tfStyleLevel1	&&
		  ds->dsOutlineLevel >= tf->tfStyleLevel0	)
		{ ct->ctStyleLevels[i]= ds->dsOutlineLevel; }
	    }
	}

    if  ( tf->tfUseStyleNames )
	{
	int			j;
	const StyleNameLevel *	snl;

	snl= tf->tfStyleNameLevels;
	for ( j= 0; j < tf->tfStyleNameLevelCount; snl++, j++ )
	    {
	    DocumentStyle *	ds;

	    ds= docGetStyleByName( &(bd->bdStyleSheet), snl->snlStyleName );
	    if  ( ds )
		{
		ct->ctStyleLevels[ds->dsStyleNumber]= snl->snlLevel;
		}

	    if  ( utilIndexSetAdd( &(ct->ctStyleNumbers), ds->dsStyleNumber ) )
		{ LDEB(ds->dsStyleNumber); return -1;	}
	    }
	}

    ct->ctStyleLevelCount= bd->bdStyleSheet.dssStyleCount;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set a TOC bookmark for all paragraphs that are included in the	*/
/*  table of contents because of their style.				*/
/*									*/
/************************************************************************/

static int docTocSetStyleBookmarks(	CalculateToc *		ct )
    {
    const TocField *		tf= &(ct->ctTocField);
    BufferDocument *		bd= ct->ctBdDoc;
    DocumentPosition		dp;

    if  ( docHeadPosition( &dp, bd->bdBody.dtRoot ) )
	{ LDEB(1); return 0;	}
    while( dp.dpNode )
	{
	if  ( dp.dpNode->biParaStyle > 0			&&
	      dp.dpNode->biParaStyle < ct->ctStyleLevelCount	)
	    {
	    int	level= ct->ctStyleLevels[dp.dpNode->biParaStyle];

	    if  ( level <= tf->tfStyleLevel1	&&
		  level >= tf->tfStyleLevel0	)
		{
		/*  Do not check return value:			*/
		/*  No bookmark is set in an empty paragraph	*/

		docGetParaTocBookmark( bd, &(bd->bdBody), dp.dpNode );
		}
	    }

	dp.dpNode= docNextParagraph( dp.dpNode );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect the entries for a TOC field, or for the spine of an EPUB	*/
/*  document.								*/
/*									*/
/************************************************************************/

int docCollectTocInput(	CalculateToc *			ct )
    {
    const TocField *		tf= &(ct->ctTocField);
    int				fieldNr;
    const DocumentFieldList *	dflDoc;
    int				fieldCount;

    dflDoc= &(ct->ctBdDoc->bdFieldList);
    fieldCount= dflDoc->dflPagedList.plItemCount;

    if  ( tf->tfType == TOCtypeTOC				&&
	  ( tf->tfUseStyleNames	|| tf->tfUseStyleLevels	)	)
	{
	if  ( docTocCollectTocStyles( ct ) )
	    { LDEB(1); return -1;	}

	if  ( docTocSetStyleBookmarks( ct ) )
	    { LDEB(1); return -1;	}
	}

    if  ( tf->tfType == TOCtypeTOC	&&
	  tf->tfUseOutlineLevels	)
	{
	if  ( docTocSetOutlineBookmarks( ct->ctBdDoc ) )
	    { LDEB(1); return -1;	}
	}

    ct->ctEntries= (TocEntry *)malloc( fieldCount* sizeof(TocEntry) );
    if  ( ! ct->ctEntries )
	{ LXDEB(fieldCount,ct->ctEntries); return -1;	}
    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{ docInitTocEntry( ct->ctEntries+ fieldNr ); }

    if  ( docCollectTocEntries( ct ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

