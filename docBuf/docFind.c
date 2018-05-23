/************************************************************************/
/*									*/
/*  Find utility routines.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docTreeNode.h"
#   include	"docFind.h"
#   include	"docNodeTree.h"
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>

/************************************************************************/
/*									*/
/*  Look for something in a document.					*/
/*									*/
/*  The 'Find Next' action scans the document in the following order:	*/
/*									*/
/*  1)  The body of the document.					*/
/*  2)  The external items of the sections of the document in the order	*/
/*	of the sections. [No attempt is made to start with a particular	*/
/*	section.]							*/
/*  3)  Per section, the headers and footers are tried in the order of	*/
/*	the 'DOC_FindHeaderFooters[]' array.				*/
/*  4)  Then the foot/endnotes are tried in the order in which they	*/
/*	appear in the internal administration of the document.		*/
/*									*/
/*  A search wraps (And will wrap indefinitely) when the tree in which	*/
/*  it started is left for the second time.				*/
/*									*/
/************************************************************************/

typedef int (*STEP_TREE)(	DocumentPosition *	dp,
				int *			pPage,
				int *			pColumn,
				int			samePhase,
				const BufferDocument *	bd );

typedef BufferItem * (*STEP_PARA)(	BufferItem *	bi );

typedef int (*START_PARA)(	DocumentPosition *	dp,
				BufferItem *		bi );

typedef struct DocumentFindJob
    {
    DocumentSelection		dfjStartSelection;
    DocumentPosition		dfjCurrentPosition;

    int				dfjExternalItemPage;
    int				dfjExternalItemColumn;

    int				dfjWrapCount;
    int				dfjReverse;

    STEP_TREE			dfjStepTree;
    STEP_PARA			dfjStepPara;
    START_PARA			dfjStartPara;
    PARA_FIND_STRING		dfjFindString;

    BufferDocument *		dfjDocument;
    void *			dfjThrough;
    } DocumentFindJob;

/**/

static int DOC_FindDocNoteItems[]=
    {
    DOCinFTNSEP,	DOCinFTNSEPC,	DOCinFTNCN,
    DOCinAFTNSEP,	DOCinAFTNSEPC,	DOCinAFTNCN,
    };

static int DOC_FindDocNoteItemCount= 
			sizeof(DOC_FindDocNoteItems)/sizeof(int);

/************************************************************************/

static int docFindGetHeaderFooterIndex(	int	treeType )
    {
    int		i;

    for ( i= 0; i < DOC_HeaderFooterTypeCount; i++ )
	{
	if  ( DOC_HeaderFooterTypes[i] == treeType )
	    { return i;	}
	}

    LDEB(treeType); return -1;
    }

static int docFindGetDocNoteItemIndex(	int	treeType )
    {
    int		i;

    for ( i= 0; i < DOC_FindDocNoteItemCount; i++ )
	{
	if  ( DOC_FindDocNoteItems[i] == treeType )
	    { return i;	}
	}

    LDEB(treeType); return -1;
    }

/************************************************************************/

static DocumentTree * docFindGetHeaderFooter(
				BufferItem *			bodySectNode,
				const DocumentProperties *	dp,
				int				hf )
    {
    unsigned char		applies= 1;
    DocumentTree *		dt;

    dt= docSectionHeaderFooter( bodySectNode, &applies, dp,
						DOC_HeaderFooterTypes[hf] );

    if  ( ! dt )
	{ XDEB(dt); return (DocumentTree *)0;	}
    if  ( ! applies )
	{ return (DocumentTree *)0;	}

    return dt;
    }

/************************************************************************/
/*									*/
/*  Initialize a find job.						*/
/*									*/
/*  NOTE that a document without a body is not always handeled		*/
/*	correctly. solve this in the caller by making an empty body. It	*/
/*	is not worth the hassle to find a work around here.		*/
/*									*/
/************************************************************************/

static int docFindInitFindJob(	DocumentFindJob *		dfj,
				int				reverse,
				const DocumentPosition *	dpFrom,
				const BufferDocument *		bd )
    {
    dfj->dfjExternalItemPage= -1;
    dfj->dfjExternalItemColumn= -1;
    dfj->dfjReverse= reverse;

    if  ( dpFrom->dpNode )
	{
	dfj->dfjCurrentPosition= *dpFrom;
	if  ( reverse )
	    { docPrevPosition( &(dfj->dfjCurrentPosition) );	}

	if  ( dpFrom->dpNode->biTreeType != DOCinBODY )
	    {
	    DocumentTree *	dt;
	    BufferItem *	bodySectNode;

	    if  ( docGetTreeOfNode( &dt, &bodySectNode, (BufferDocument *)bd,
							    dpFrom->dpNode ) )
		{ LDEB(1); return -1;	}

	    dfj->dfjExternalItemPage= dt->dtPageSelectedUpon;
	    dfj->dfjExternalItemColumn= dt->dtColumnSelectedIn;
	    }
	}
    else{
	BufferItem *	bodyBi= bd->bdBody.dtRoot;

	if  ( reverse )
	    {
	    if  ( docTailPosition( &(dfj->dfjCurrentPosition), bodyBi ) )
		{ LDEB(1); return 1;	}
	    }
	else{
	    if  ( docHeadPosition( &(dfj->dfjCurrentPosition), bodyBi ) )
		{ LDEB(1); return 1;	}
	    }
	}

    docInitDocumentSelection( &(dfj->dfjStartSelection) );
    docSetIBarSelection( &(dfj->dfjStartSelection),
					    &(dfj->dfjCurrentPosition) );

    dfj->dfjWrapCount= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the next header or footer to look in for a find action.		*/
/*									*/
/************************************************************************/

static int docFindNextHeaderFooter(	DocumentPosition *	dpNew,
					int *			pExtItPage,
					int *			pExtItColumn,
					const BufferDocument *	bd,
					int			sectFrom,
					int			hfFrom )
    {
    BufferItem *		bodyBi= bd->bdBody.dtRoot;
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				sect;
    BufferItem *		bodySectNode;
    int				hf;
    DocumentTree *		dt;

    for ( sect= sectFrom; sect < bodyBi->biChildCount; sect++ )
	{
	bodySectNode= bodyBi->biChildren[sect];

	for ( hf= hfFrom; hf < DOC_HeaderFooterTypeCount; hf++ )
	    {
	    int			page;
	    int			usedInDoc;

	    dt= docFindGetHeaderFooter( bodySectNode, dp, hf );
	    if  ( ! dt || ! dt->dtRoot )
		{ continue;	}

	    page= docSectionHeaderFooterFirstPage( &usedInDoc, bodySectNode,
					    DOC_HeaderFooterTypes[hf], dp );

	    if  ( page < 0 || ! usedInDoc )
		{ continue;	}

	    if  ( docHeadPosition( dpNew, dt->dtRoot ) )
		{
		SDEB(docTreeTypeStr(DOC_HeaderFooterTypes[hf]));
		continue;
		}

	    *pExtItPage= page;
	    *pExtItColumn= 0; /* irrelevant */
	    return 0;
	    }

	hfFrom= 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next note to look in for a find action.			*/
/*									*/
/************************************************************************/

static int docFindNextNote(		DocumentPosition *	dp,
					const BufferDocument *	bd,
					DocumentField *		dfNote )
    {
    DocumentNote *	dn;

    dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 );
    while( dfNote )
	{
	if  ( dn->dnDocumentTree.dtRoot )
	    {
	    if  ( docHeadPosition( dp, dn->dnDocumentTree.dtRoot ) )
		{ LDEB(1); return -1;	}

	    return 0;
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 );
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next note item to look in for a find action.		*/
/*									*/
/************************************************************************/

static int docFindNextSeparator(	DocumentPosition *	dp,
					int *			pExtItPage,
					int *			pExtItColumn,
					const BufferDocument *	bd,
					int			sepFrom )
    {
    int			sep;

    for ( sep= sepFrom; sep < DOC_FindDocNoteItemCount; sep++ )
	{
	DocumentTree *		dt;
	DocumentNote *		dn;
	int			page= -1;
	int			column= -1;

	dt= docDocumentNoteSeparator( (BufferDocument *)bd,
						DOC_FindDocNoteItems[sep] );
	if  ( ! dt || ! dt->dtRoot )
	    { continue;	}

	switch( DOC_FindDocNoteItems[sep] )
	    {
	    case DOCinFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) )
		    { continue;	}
		if  ( dn->dnReferringPage < 0 )
		    { continue;	}
		if  ( dn->dnReferringColumn < 0 )
		    { continue;	}
		page= dn->dnReferringPage;
		column= dn->dnReferringColumn;
		break;

	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    case DOCinAFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) )
		    { continue;	}
		page= dt->dtRoot->biTopPosition.lpPage;
		column= dt->dtRoot->biTopPosition.lpColumn;
		break;

	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    default:
		LDEB(DOC_FindDocNoteItems[sep]); return -1;
	    }

	if  ( docHeadPosition( dp, dt->dtRoot ) )
	    { LDEB(1); return -1;	}

	*pExtItPage= page;
	*pExtItColumn= column;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next tree to look in in a find action.			*/
/*									*/
/*  It is the responsibility of the caller to avoid loops. This routine	*/
/*  just returns the next tree indefinitely. (It wraps from the end to	*/
/*  the beginning.)							*/
/*									*/
/************************************************************************/

static int docFindNextNextTree(		DocumentPosition *	dp,
					int *			pPage,
					int *			pColumn,
					int			samePhase,
					const BufferDocument *	bd )
    {
    int				phaseTo;
    int				phaseFrom;

    int				res;
    int				hfFrom;
    DocumentField *		dfNote= (DocumentField *)0;
    int				sectFrom;
    int				sepFrom;

    const BufferItem *		selSectBi;
    const SelectionScope *	ss;

    selSectBi= docGetSectNode( dp->dpNode );
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}
    ss= &(selSectBi->biSectSelectionScope);

    phaseTo= 0;
    sectFrom= 0; hfFrom= 0; sepFrom= 0;
    switch( ss->ssTreeType )
	{
	case DOCinBODY:

	    phaseFrom= 0;
	    phaseTo= 1;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:

	    phaseFrom= 1;
	    phaseTo= 1;
	    sectFrom= ss->ssOwnerSectNr;
	    hfFrom= docFindGetHeaderFooterIndex( selSectBi->biTreeType );
	    if  ( hfFrom < 0 )
		{ LLDEB(selSectBi->biTreeType,hfFrom); return -1;	}
	    hfFrom++;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    phaseFrom= 2;
	    phaseTo= 2;
	    dfNote= docGetFieldByNumber( &(bd->bdFieldList),
							ss->ssOwnerNumber );

	    break;

	case DOCinFTNSEP:	case DOCinAFTNSEP:
	case DOCinFTNSEPC:	case DOCinAFTNSEPC:
	case DOCinFTNCN:	case DOCinAFTNCN:

	    phaseFrom= 3;
	    phaseTo= 3;
	    sepFrom= docFindGetDocNoteItemIndex( selSectBi->biTreeType );
	    if  ( sepFrom < 0 )
		{ LLDEB(selSectBi->biTreeType,sepFrom); return -1; }
	    sepFrom++;

	    break;

	default:
	    LDEB(ss->ssTreeType);
	    LDEB(selSectBi->biTreeType);
	    return -1;
	}

    if  ( phaseTo <= 0 || phaseTo > 3 )
	{ LDEB(phaseTo); return -1;	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 1 )
	{
	res= docFindNextHeaderFooter( dp,
				    pPage, pColumn, bd, sectFrom, hfFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 2 )
	{
	res= docFindNextNote( dp, bd, dfNote );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 3 )
	{
	res= docFindNextSeparator( dp, pPage, pColumn, bd, sepFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( docDocumentHead( dp, (BufferDocument *)bd ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Prepare for the next 'Find Prev' attempt.				*/
/*									*/
/************************************************************************/

static int docFindPrevHeaderFooter(	DocumentPosition *	dpNew,
					int *			pExtItPage,
					int *			pExtItColumn,
					const BufferDocument *	bd,
					int			sectFrom,
					int			hfFrom )
    {
    BufferItem *		bodyBi= bd->bdBody.dtRoot;
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				sect;
    BufferItem *		bodySectNode;
    int				hf;
    DocumentTree *		dt;

    for ( sect= sectFrom; sect >= 0; sect-- )
	{
	bodySectNode= bodyBi->biChildren[sect];

	for ( hf= hfFrom; hf >= 0; hf-- )
	    {
	    int			page;
	    int			usedInDoc;

	    dt= docFindGetHeaderFooter( bodySectNode, dp, hf );
	    if  ( ! dt || ! dt->dtRoot )
		{ continue;	}

	    page= docSectionHeaderFooterFirstPage( &usedInDoc, bodySectNode,
					    DOC_HeaderFooterTypes[hf], dp );

	    if  ( page < 0 || ! usedInDoc )
		{ continue;	}

	    if  ( docTailPosition( dpNew, dt->dtRoot ) )
		{ LDEB(1); return -1;	}

	    *pExtItPage= page;
	    *pExtItColumn= 0; /* irrelevant */
	    return 0;
	    }

	hfFrom= DOC_HeaderFooterTypeCount- 1;
	}

    return 1;
    }

static int docFindPrevNote(		DocumentPosition *	dp,
					const BufferDocument *	bd,
					DocumentField *		dfNote )
    {
    DocumentNote *	dn;

    dfNote= docGetPrevNoteInDocument( &dn, bd, dfNote, -1 );
    while( dfNote )
	{
	if  ( dn->dnDocumentTree.dtRoot )
	    {
	    if  ( docTailPosition( dp, dn->dnDocumentTree.dtRoot ) )
		{ LDEB(1); return -1;	}

	    return 0;
	    }

	dfNote= docGetPrevNoteInDocument( &dn, bd, dfNote, -1 );
	}

    return 1;
    }

static int docFindPrevSeparator(	DocumentPosition *	dp,
					int *			pExtItPage,
					int *			pExtItColumn,
					const BufferDocument *	bd,
					int			sepFrom )
    {
    int			sep;

    for ( sep= sepFrom; sep >= 0; sep-- )
	{
	DocumentTree *		dt;
	DocumentNote *		dn;

	dt= docDocumentNoteSeparator( (BufferDocument *)bd, DOC_FindDocNoteItems[sep] );
	if  ( ! dt || ! dt->dtRoot )
	    { continue;	}

	switch( DOC_FindDocNoteItems[sep] )
	    {
	    case DOCinFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) )
		    { continue;	}
		if  ( dn->dnReferringPage < 0 )
		    { continue;	}
		if  ( dn->dnReferringColumn < 0 )
		    { continue;	}
		break;

	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    case DOCinAFTNSEP:
		if  ( ! docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) )
		    { continue;	}
		break;

	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    default:
		LDEB(DOC_FindDocNoteItems[sep]); return -1;
	    }

	if  ( docTailPosition( dp, dt->dtRoot ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    return 1;
    }

static int docFindPrevPrevTree(		DocumentPosition *	dp,
					int *			pPage,
					int *			pColumn,
					int			samePhase,
					const BufferDocument *	bd )
    {
    int				phaseTo;
    int				phaseFrom;

    int				res;
    int				hfFrom;
    DocumentField *		dfNote= (DocumentField *)0;
    int				sectFrom;
    int				sepFrom;

    BufferItem *		bodyBi= bd->bdBody.dtRoot;
    const BufferItem *		selSectBi;
    const SelectionScope *	ss;

    selSectBi= docGetSectNode( dp->dpNode );
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}
    ss= &(selSectBi->biSectSelectionScope);

    sectFrom= bodyBi->biChildCount- 1;
    hfFrom= DOC_HeaderFooterTypeCount- 1;
    sepFrom= DOC_FindDocNoteItemCount- 1;

    switch( ss->ssTreeType )
	{
	case DOCinBODY:

	    phaseFrom= 0;
	    phaseTo= 3;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:

	    phaseFrom= 1;
	    phaseTo= 1;
	    sectFrom= ss->ssOwnerSectNr;
	    hfFrom= docFindGetHeaderFooterIndex( selSectBi->biTreeType );
	    if  ( hfFrom < 0 )
		{ LLDEB(selSectBi->biTreeType,hfFrom); return -1;	}

	    hfFrom--;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    phaseFrom= 2;
	    phaseTo= 2;
	    dfNote= docGetFieldByNumber(
				&(bd->bdFieldList), ss->ssOwnerNumber );
	    break;

	case DOCinFTNSEP:	case DOCinAFTNSEP:
	case DOCinFTNSEPC:	case DOCinAFTNSEPC:
	case DOCinFTNCN:	case DOCinAFTNCN:

	    phaseFrom= 3;
	    phaseTo= 3;
	    sepFrom= docFindGetDocNoteItemIndex( selSectBi->biTreeType );
	    if  ( sepFrom < 0 )
		{ LLDEB(selSectBi->biTreeType,sepFrom); return -1; }
	    sepFrom--;

	    break;

	default:
	    LDEB(ss->ssTreeType);
	    LDEB(selSectBi->biTreeType);
	    return -1;
	}

    if  ( phaseTo <= 0 || phaseTo > 3 )
	{ LDEB(phaseTo); return -1;	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 3 )
	{
	res= docFindPrevSeparator( dp, pPage, pColumn, bd, sepFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 2 )
	{
	res= docFindPrevNote( dp, bd, dfNote );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 1 )
	{
	res= docFindPrevHeaderFooter( dp, pPage, pColumn, bd, sectFrom, hfFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( docTailPosition( dp, bodyBi ) )
	{ LDEB(1); return 1;	}

    return 0;
    }

static int docFindInCurrentTree(
				DocumentSelection *		ds,
				const DocumentFindJob *		dfj )
    {
    BufferItem *		bi= dfj->dfjCurrentPosition.dpNode;

    int				ret;

    DocumentPosition		dpHere;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return -1;	}

    dpHere= dfj->dfjCurrentPosition;

    for (;;)
	{
	ret= (*dfj->dfjFindString)( ds, bi,
				dfj->dfjDocument, &dpHere, dfj->dfjThrough );

	if  ( ret == 0 )
	    { return ret;		}
	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	bi= (*dfj->dfjStepPara)( bi );
	if  ( ! bi )
	    { return 1;	}
	if  ( (*dfj->dfjStartPara)( &dpHere, bi ) )
	    { LDEB(1); return 1;	}
	}
    }

/************************************************************************/

typedef int (*FIND_IN_TREE)(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through );

static int docFindStepInDocument(
				DocumentSelection *		ds,
				DocumentFindJob *		dfj )
    {
    int				ret= -1;
    DocumentSelection		dsNew;

    const int			samePhase= 0;

    docInitDocumentSelection( &dsNew );

    for (;;)
	{
	ret= docFindInCurrentTree( &dsNew, dfj );
	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	if  ( ret == 0 )
	    {
	    if  ( dfj->dfjReverse )
		{
		dsNew.dsDirection=  -1;
		dsNew.dsAnchor= dsNew.dsTail;
		}
	    else{
		dsNew.dsDirection=  1;
		dsNew.dsAnchor= dsNew.dsHead;
		}

	    docSetSelectionScope( &dsNew );

	    if  ( dsNew.dsHead.dpNode->biTreeType != DOCinBODY )
		{
		DocumentTree *	dt;
		BufferItem *	bodySectNode;

		if  ( docGetTreeOfNode( &dt, &bodySectNode,
				(BufferDocument *)dfj->dfjDocument,
				dsNew.dsHead.dpNode ) )
		    { LDEB(1); return -1;	}

		dt->dtPageSelectedUpon= dfj->dfjExternalItemPage;
		dt->dtColumnSelectedIn= dfj->dfjExternalItemColumn;
		}

	    *ds= dsNew;

	    return 0;
	    }

	if  ( docSelectionSameRoot( &(dfj->dfjStartSelection),
					    dfj->dfjCurrentPosition.dpNode ) )
	    {
	    if  ( dfj->dfjWrapCount > 0 )
		{ break;	}

	    dfj->dfjWrapCount++;
	    }

	ret= (*dfj->dfjStepTree)( &(dfj->dfjCurrentPosition),
				    &(dfj->dfjExternalItemPage),
				    &(dfj->dfjExternalItemColumn),
				    samePhase, dfj->dfjDocument );
	if  ( ret < 0 )
	    { LDEB(ret); return -1;	}
	if  ( ret > 0 )
	    { return ret;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Look for the next occurrence of something in a whole document	*/
/*  including the section headers and footers.				*/
/*									*/
/************************************************************************/

int docFindFindNextInDocument(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through )
    {
    const int			reverse= 0;
    DocumentFindJob		dfj;

    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    dfj.dfjStepTree= docFindNextNextTree;
    dfj.dfjStepPara= docNextParagraph;
    dfj.dfjStartPara= docHeadPosition;
    dfj.dfjFindString= findNext;

    dfj.dfjDocument= bd;
    dfj.dfjThrough= through;

    return docFindStepInDocument( ds, &dfj );
    }

int docFindFindPrevInDocument(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findPrev,
				void *				through )
    {
    const int			reverse= 1;
    DocumentFindJob		dfj;

    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    dfj.dfjStepTree= docFindPrevPrevTree;
    dfj.dfjStepPara= docPrevParagraph;
    dfj.dfjStartPara= docTailPosition;
    dfj.dfjFindString= findPrev;

    dfj.dfjDocument= bd;
    dfj.dfjThrough= through;

    return docFindStepInDocument( ds, &dfj );
    }

/************************************************************************/
/*									*/
/*  Find something forward/backward in one branch of a tree.		*/
/*									*/
/*  This can be done by walking over the leaf nodes of the tree.	*/
/*									*/
/************************************************************************/

int docFindFindNextInCurrentTree(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through )
    {
    const int			reverse= 0;
    DocumentFindJob		dfj;

    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    dfj.dfjStepTree= docFindNextNextTree;
    dfj.dfjStepPara= docNextParagraph;
    dfj.dfjStartPara= docHeadPosition;
    dfj.dfjFindString= findNext;

    dfj.dfjDocument= bd;
    dfj.dfjThrough= through;

    return docFindInCurrentTree( ds, &dfj );
    }

/************************************************************************/
/*									*/
/*  Navigation betweem external items with the PageUp/PageDown keys.	*/
/*									*/
/************************************************************************/

int docNextSimilarRoot(		DocumentPosition *	dp,
				int *			pPage,
				int *			pColumn,
				BufferDocument *	bd )
    {
    int		page= dp->dpNode->biTopPosition.lpPage;
    int		column= dp->dpNode->biTopPosition.lpColumn;
    const int	samePhase= 1;

    int ret= docFindNextNextTree( dp, &page, &column, samePhase, bd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    *pPage= page;
    *pColumn= column;
    return ret;
    }

int docPrevSimilarRoot(		DocumentPosition *	dp,
				int *			pPage,
				int *			pColumn,
				BufferDocument *	bd )
    {
    int		page= dp->dpNode->biTopPosition.lpPage;
    int		column= dp->dpNode->biTopPosition.lpColumn;
    const int	samePhase= 1;

    int ret= docFindPrevPrevTree( dp, &page, &column, samePhase, bd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    *pPage= page;
    *pColumn= column;
    return ret;
    }
