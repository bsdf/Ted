/************************************************************************/
/*									*/
/*  Get headers and footers for a certain page/position.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	<docTreeType.h>

/************************************************************************/
/*									*/
/*  Kinds of Headers/Footers.						*/
/*									*/
/*  The arrays are a mapping from the options to the header/footer data	*/
/*  structure that is used in the implementation.			*/
/*									*/
/************************************************************************/

const int DOC_HeaderScopes[PAGES__COUNT]=
    {
    DOCinFIRST_HEADER,	/*  PAGES_FIRST_PAGE		*/
    DOCinRIGHT_HEADER,	/*  PAGES_SUBSEQUENT_PAGES	*/
    DOCinRIGHT_HEADER,	/*  PAGES_ALL_PAGES		*/
    DOCinRIGHT_HEADER,	/*  PAGES_ODD_PAGES		*/
    DOCinLEFT_HEADER,	/*  PAGES_EVEN_PAGES		*/
    };

const int DOC_FooterScopes[PAGES__COUNT]=
    {
    DOCinFIRST_FOOTER,	/*  PAGES_FIRST_PAGE		*/
    DOCinRIGHT_FOOTER,	/*  PAGES_SUBSEQUENT_PAGES	*/
    DOCinRIGHT_FOOTER,	/*  PAGES_ALL_PAGES		*/
    DOCinRIGHT_FOOTER,	/*  PAGES_ODD_PAGES		*/
    DOCinLEFT_FOOTER,	/*  PAGES_EVEN_PAGES		*/
    };

/************************************************************************/
/*									*/
/*  Determine what header/footer applies for a certain page.		*/
/*									*/
/************************************************************************/

static int docCheckTreeIsEmpty(	int *			pIsEmpty,
				const BufferDocument *	bd,
				const DocumentTree *	dt )
    {
    int			isEmpty= 1;

    if  ( dt && dt->dtRoot )
	{
	DocumentPosition		dpBegin;
	DocumentPosition		dpNext;

	if  ( docHeadPosition( &dpBegin, dt->dtRoot ) )
	    { LDEB(1); docInitDocumentPosition( &dpBegin );	}

	dpNext= dpBegin;
	if  ( ! docNextPosition( &dpNext ) )
	    { isEmpty= 0;	}
	else{
	    const ParagraphProperties *	pp= &(dpBegin.dpNode->biParaProperties);

	    if  ( pp->ppTableNesting > 0 )
		{ isEmpty= 0;	}

	    if  ( docBorderNumberIsBorder( bd, pp->ppTopBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppBottomBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppLeftBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppRightBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppBetweenBorderNumber ) )
		{ isEmpty= 0;	}
	    if  ( docBorderNumberIsBorder( bd, pp->ppBarNumber ) )
		{ isEmpty= 0;	}

	    if  ( docShadingNumberIsShading( bd, pp->ppShadingNumber ) )
		{ isEmpty= 0;	}
	    }
	}

    if  ( pIsEmpty )
	{ *pIsEmpty= isEmpty;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Decide what header should go to this page.				*/
/*									*/
/************************************************************************/

int docWhatPageHeader(	DocumentTree **		pTree,
			int *			pIsEmpty,
			const BufferItem *	bodySectNode,
			int			page,
			const BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const SectionProperties *	sp= &(bodySectNode->biSectProperties);

    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;
    DocumentTree *		tree= (DocumentTree *)0;

    if  ( ! shf )
	{ XDEB(shf); return -1;	}

    if  ( page == bodySectNode->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	tree= &(shf->shfFirstPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinFIRST_HEADER;
	}

    if  ( page % 2 && dp->dpHasFacingPages )
	{
	tree= &(shf->shfLeftPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinLEFT_HEADER;
	}
    else{
	tree= &(shf->shfRightPageHeader);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinRIGHT_HEADER;
	}
    }

int docWhatPageFooter(	DocumentTree **		pTree,
			int *			pIsEmpty,
			const BufferItem *	bodySectNode,
			int			page,
			const BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const SectionProperties *	sp= &(bodySectNode->biSectProperties);

    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;
    DocumentTree *		tree= (DocumentTree *)0;

    if  ( ! shf )
	{ XDEB(shf); return -1;	}

    if  ( page == bodySectNode->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	tree= &(shf->shfFirstPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinFIRST_FOOTER;
	}

    if  ( page % 2 && dp->dpHasFacingPages )
	{
	tree= &(shf->shfLeftPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinLEFT_FOOTER;
	}
    else{
	tree= &(shf->shfRightPageFooter);
	docCheckTreeIsEmpty( pIsEmpty, bd, tree );
	if  ( pTree )
	    { *pTree= tree;	}
	return DOCinRIGHT_FOOTER;
	}
    }

/************************************************************************/
/*									*/
/*  Decide what footer should go to this page.				*/
/*									*/
/************************************************************************/

int docWhatPagesForHeaderFooter( const DocumentProperties *	dp,
				const SectionProperties *	sp,
				int				treeType )
    {
    switch( treeType )
	{
	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    return PAGES_FIRST_PAGE;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    return PAGES_EVEN_PAGES;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    if  ( dp->dpHasFacingPages )
		{ return PAGES_ODD_PAGES;		}
	    else{
		if  ( sp->spHasTitlePage )
		    { return PAGES_SUBSEQUENT_PAGES;	}
		else{ return PAGES_ALL_PAGES;		}
		}
	    /*unreachable*/

	default:
	    LDEB(treeType);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Return a particular header or footer by scope.			*/
/*									*/
/************************************************************************/

DocumentTree *	docSectionHeaderFooter(
				const BufferItem *		bodySectNode,
				unsigned char *			pApplies,
				const DocumentProperties *	dp,
				int				treeType )
    {
    const SectionProperties *	sp= &(bodySectNode->biSectProperties);
    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;

    if  ( ! shf )
	{ XDEB(shf); return (DocumentTree *)0;	}

    switch( treeType )
	{
	case DOCinBODY:
	    LDEB(treeType); return (DocumentTree *)0;

	case DOCinFIRST_HEADER:
	    if  ( pApplies )
		{ *pApplies= sp->spHasTitlePage;	}
	    return &(shf->shfFirstPageHeader);

	case DOCinLEFT_HEADER:
	    if  ( pApplies )
		{ *pApplies= dp->dpHasFacingPages;	}
	    return &(shf->shfLeftPageHeader);

	case DOCinRIGHT_HEADER:
	    if  ( pApplies )
		{ *pApplies= 1;	}
	    return &(shf->shfRightPageHeader);

	case DOCinFIRST_FOOTER:
	    if  ( pApplies )
		{ *pApplies= sp->spHasTitlePage;	}
	    return &(shf->shfFirstPageFooter);

	case DOCinLEFT_FOOTER:
	    if  ( pApplies )
		{ *pApplies= dp->dpHasFacingPages;	}
	    return &(shf->shfLeftPageFooter);

	case DOCinRIGHT_FOOTER:
	    if  ( pApplies )
		{ *pApplies= 1;	}
	    return &(shf->shfRightPageFooter);

	default:
	    LDEB(treeType); return (DocumentTree *)0;
	}
    }

int docSectionHasHeaderFooter(	const BufferItem *		bodySectNode,
				unsigned char *			pApplies,
				const DocumentProperties *	dp,
				int				treeType )
    {
    DocumentTree *	dt;

    dt= docSectionHeaderFooter( bodySectNode, pApplies, dp, treeType );
    if  ( ! dt || ! dt->dtRoot )
	{ return 0;	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Return the first page where a particular kind of header/footer can	*/
/*  be used in a document. The existence of the tree is checked by the	*/
/*  caller.								*/
/*									*/
/************************************************************************/

int docSectionHeaderFooterFirstPage(
				int *				pUsedByDocument,
				const BufferItem *		bodySectNode,
				int				treeType,
				const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectNode->biSectProperties);
    int				topPage= bodySectNode->biTopPosition.lpPage;
    int				belowPage= bodySectNode->biBelowPosition.lpPage;
    int				page;

    const BufferItem *		prevBi= (const BufferItem *)0;

    if  ( bodySectNode->biNumberInParent > 0 )
	{
	prevBi= bodySectNode->biParent->biChildren[
					    bodySectNode->biNumberInParent- 1];

	if  ( prevBi->biBelowPosition.lpPage >= belowPage )
	    { *pUsedByDocument= 0; return -1;	}
	}

    switch( treeType )
	{
	case DOCinBODY:
	    LDEB(treeType); *pUsedByDocument= 0; return -1;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    if  ( ! sp->spHasTitlePage )
		{ *pUsedByDocument= 0; return -1;	}

	    if  ( prevBi && prevBi->biBelowPosition.lpPage >= topPage )
		{ *pUsedByDocument= 0; return topPage;
		}
	    page= topPage;
	    break;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ *pUsedByDocument= 0; return -1;	}
	    if  ( topPage % 2 )
		{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 2;	}
		else{ page= topPage;	}
		}
	    else{ page= topPage+ 1;	}
	    break;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    if  ( dp->dpHasFacingPages )
		{
		if  ( topPage % 2 == 0 )
		    {
		    if  ( sp->spHasTitlePage )
			{ page= topPage+ 2;	}
		    else{ page= topPage;	}
		    }
		else{ page= topPage+ 1;	}
		}
	    else{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 1;	}
		else{ page= topPage;	}
		}
	    break;

	default:
	    LDEB(treeType); *pUsedByDocument= 0; return -1;
	}

    *pUsedByDocument= page <= bodySectNode->biBelowPosition.lpPage;

    return page;
    }

/************************************************************************/
/*									*/
/*  Return the Header/Footer corrsponding to 'treeType'. If the		*/
/*  selection itself is in an external tree, go to the section in the	*/
/*  body to get the header or footer.					*/
/*									*/
/************************************************************************/

int docGetHeaderFooter(		DocumentTree **			pEi,
				BufferItem **			pBodySectNode,
				const DocumentPosition *	dp,
				BufferDocument *		bd,
				int				treeType )
    {
    DocumentTree *	eiDp;
    DocumentTree *	eiHdFt;
    BufferItem *	bodySectNode;
    unsigned char	applies= 1;

    if  ( docGetTreeOfNode( &eiDp, &bodySectNode, bd, dp->dpNode ) )
	{ LDEB(1); return -1;	}

    eiHdFt= docSectionHeaderFooter( bodySectNode, &applies,
					    &(bd->bdProperties), treeType );
    if  ( ! eiHdFt )
	{ XDEB(eiHdFt); return -1;	}

    *pEi= eiHdFt; *pBodySectNode= bodySectNode; return 0;
    }

/************************************************************************/
/*									*/
/*  Determine whether that document has headers and footers at all.	*/
/*									*/
/************************************************************************/

int docInquireHeadersFooters(	int *			pDocHasHeaders,
				int *			pDocHasFooters,
				const BufferDocument *	bd )
    {
    int			i;
    int			hasPageHeader= 0;
    int			hasPageFooter= 0;

    const BufferItem *	bodyBi= bd->bdBody.dtRoot;

    for ( i= 0; i < bodyBi->biChildCount; i++ )
	{
	int			j;
	BufferItem *		sectBi= bodyBi->biChildren[i];

	for ( j= 0; j < PAGES__COUNT; j++ )
	    {
	    DocumentTree *	dt;
	    unsigned char	applies;

	    applies= 1;
	    dt= docSectionHeaderFooter( sectBi, &applies,
				    &(bd->bdProperties), DOC_HeaderScopes[j] );
	    if  ( dt && dt->dtRoot && applies )
		{ hasPageHeader= 1;	}

	    applies= 1;
	    dt= docSectionHeaderFooter( sectBi, &applies,
				    &(bd->bdProperties), DOC_FooterScopes[j] );
	    if  ( dt && dt->dtRoot && applies )
		{ hasPageFooter= 1;	}
	    }
	}

    *pDocHasHeaders= hasPageHeader;
    *pDocHasFooters= hasPageFooter;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the page, nearest to the current page that can hold a		*/
/*  particular kinfd of header or footer.				*/
/*									*/
/************************************************************************/

static int DOC_TryPageOffsets[]= { 0, 1, -1, 2 };

int docHeaderFooterPage(		const BufferDocument *	bd,
					const BufferItem *	bodySectNode,
					int			currentPage,
					int			treeType )
    {
    switch( treeType )
	{
	int		isEmpty;
	int		i;
	DocumentTree *	eiTry;
	int		ttForPg;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    return bodySectNode->biTopPosition.lpPage;
	    break;

	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	    for ( i= 0; i < sizeof(DOC_TryPageOffsets)/sizeof(int); i++ )
		{
		int pg= currentPage+ DOC_TryPageOffsets[i];

		if  ( pg < bodySectNode->biTopPosition.lpPage	||
		      pg > bodySectNode->biBelowPosition.lpPage	)
		    { continue;	}

		ttForPg= docWhatPageHeader( &eiTry, &isEmpty,
							bodySectNode, pg, bd );
		if  ( ttForPg == treeType )
		    { return pg;	}
		}
	    break;

	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    for ( i= 0; i < sizeof(DOC_TryPageOffsets)/sizeof(int); i++ )
		{
		int pg= currentPage+ DOC_TryPageOffsets[i];

		if  ( pg < bodySectNode->biTopPosition.lpPage	||
		      pg > bodySectNode->biBelowPosition.lpPage	)
		    { continue;	}

		ttForPg= docWhatPageFooter( &eiTry, &isEmpty,
							bodySectNode, pg, bd );
		if  ( ttForPg == treeType )
		    { return pg;	}
		}
	    break;

	default:
	    LDEB(treeType); return -1;
	}

    return -1;
    }


/************************************************************************/
/*									*/
/*  Force recalculation of header/fooet layout.				*/
/*									*/
/************************************************************************/

void docInvalidateSectHeaderFooterLayout(	BufferItem *	sectBi )
    {
    SectHeadersFooters *	shf= sectBi->biSectHeadersFooters;

    if  ( shf )
	{
	docInvalidateTreeLayout( &(shf->shfFirstPageHeader) );
	docInvalidateTreeLayout( &(shf->shfLeftPageHeader) );
	docInvalidateTreeLayout( &(shf->shfRightPageHeader) );

	docInvalidateTreeLayout( &(shf->shfFirstPageFooter) );
	docInvalidateTreeLayout( &(shf->shfLeftPageFooter) );
	docInvalidateTreeLayout( &(shf->shfRightPageFooter) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Bookkeeping of section headers and footers.				*/
/*									*/
/************************************************************************/

void docCleanSectHeadersFooters( BufferDocument *		bd,
				SectHeadersFooters *		shf )
    {
    docCleanDocumentTree( bd, &(shf->shfFirstPageHeader) );
    docCleanDocumentTree( bd, &(shf->shfLeftPageHeader) );
    docCleanDocumentTree( bd, &(shf->shfRightPageHeader) );

    docCleanDocumentTree( bd, &(shf->shfFirstPageFooter) );
    docCleanDocumentTree( bd, &(shf->shfLeftPageFooter) );
    docCleanDocumentTree( bd, &(shf->shfRightPageFooter) );

    return;
    }

void docInitSectHeadersFooters( SectHeadersFooters *		shf )
    {
    docInitDocumentTree( &(shf->shfFirstPageHeader) );
    docInitDocumentTree( &(shf->shfLeftPageHeader) );
    docInitDocumentTree( &(shf->shfRightPageHeader) );

    docInitDocumentTree( &(shf->shfFirstPageFooter) );
    docInitDocumentTree( &(shf->shfLeftPageFooter) );
    docInitDocumentTree( &(shf->shfRightPageFooter) );
    }
