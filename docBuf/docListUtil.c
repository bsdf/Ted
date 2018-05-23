/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/*  Levels in the tree correspond to the ilvl value of the paragraphs.	*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docTreeScanner.h"
#   include	<docDocumentList.h>
#   include	<docListOverride.h>
#   include	<docDocumentNote.h>

/************************************************************************/
/*									*/
/*  Collect list related information for a paragraph.			*/
/*									*/
/*  1)  Paranoia: check list number.					*/
/*  2)  Get override and number tree.					*/
/*  3)  Link override to list if nexessary.				*/
/*  4)  Paranoia again: Is it an existing list?				*/
/*									*/
/************************************************************************/

int docGetListOfParagraph(	ListOverride **		pLo,
				DocumentList **		pDl,
				int			ls,
				const BufferDocument *	bd )
    {
    return docGetListForStyle( pLo, pDl, ls, bd->bdProperties.dpListAdmin );
    }

int docGetListLevelOfParagraph(	int *				startPath,
				int *				formatPath,
				ListOverride **			pLo,
				DocumentList **			pDl,
				const ListLevel **		pLl,
				const ParagraphProperties *	pp,
				const BufferDocument *		bd )
    {
    ListOverride *	lo;
    DocumentList *	dl;
    const ListLevel *	ll;

    if  ( docGetListForStyle( &lo, &dl, pp->ppListOverride,
					    bd->bdProperties.dpListAdmin ) )
	{ LDEB(pp->ppListOverride); return -1;	}

    if  ( docListGetFormatPath( startPath, formatPath, &ll,
						pp->ppListLevel, dl, lo ) )
	{ LDEB(pp->ppListLevel); return -1;	}

    if  ( pLo )
	{ *pLo= lo;	}
    if  ( pDl )
	{ *pDl= dl;	}
    if  ( pLl )
	{ *pLl= ll;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove a paragraph from its list.					*/
/*									*/
/************************************************************************/

int docRemoveParagraphFromList(		BufferItem *		paraNode,
					DocumentTree *		dt,
					BufferDocument *	bd )
    {
    int				rval= 0;
    int				paraNr= docNumberOfParagraph( paraNode );

    if  ( paraNode->biParaListOverride > 0 )
	{
	if  ( docListNumberTreesDeleteParagraph( &(dt->dtListNumberTrees),
					paraNode->biParaListOverride, paraNr ) )
	    { LDEB(paraNr); rval= -1;	}

	paraNode->biParaListOverride= 0;
	paraNode->biParaListLevel= 0;
	}

    if  ( paraNode->biParaOutlineLevel < PPoutlineBODYTEXT )
	{
	if  ( docListNumberTreeDeleteParagraph(
					&(dt->dtOutlineTree), paraNr ) )
	    { LDEB(paraNr); rval= -1;	}

	paraNode->biParaOutlineLevel= PPoutlineBODYTEXT;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the first list in the selection.				*/
/*									*/
/************************************************************************/

typedef struct FindList
    {
    int	fl_ls;
    int	fl_level;
    int	fl_paraNr;
    int	fl_multiList;
    int	fl_multiLevel;
    } FindList;

static int docFindListEnterNode( BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				through )
    {
    FindList *	fl= (FindList *)through;

    if  ( node->biLevel == DOClevPARA )
	{
	const ParagraphProperties *	pp;

	pp= &(node->biParaProperties);
	if  ( fl->fl_ls < 0		&&
	      pp->ppListOverride >= 1	)
	    {
	    fl->fl_paraNr= docNumberOfParagraph( node );
	    fl->fl_ls= pp->ppListOverride;
	    }
	if  ( fl->fl_ls < 0 || pp->ppListOverride != fl->fl_ls )
	    { fl->fl_multiList= 1;	}

	if  ( fl->fl_ls > 0			&&
	      pp->ppListOverride == fl->fl_ls	)
	    {
	    if  ( fl->fl_level < 0 )
		{ fl->fl_level= pp->ppListLevel;	}
	    else{
		if  ( fl->fl_level != pp->ppListLevel )
		    { fl->fl_multiLevel= 1;	}
		}
	    }
	}

    return ADVICEtsOK;
    }

int docFindListOfSelection(	int *				pLs,
				int *				pLevel,
				int *				pMultiList,
				int *				pMultiLevel,
				int *				pParaNr,
				const DocumentSelection *	ds,
				DocumentTree *			tree,
				BufferDocument *		bd )
    {
    const int	flags= 0;
    FindList	fl;

    fl.fl_ls= -1;
    fl.fl_level= -1;
    fl.fl_paraNr= -1;
    fl.fl_multiList= 0;
    fl.fl_multiLevel= 0;

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
		    docFindListEnterNode, (NodeVisitor)0, flags, &fl ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( bd, tree,
		    docFindListEnterNode, (NodeVisitor)0, flags, &fl ) < 0 )
	    { LDEB(1); return -1;	}
	}

    if  ( fl.fl_paraNr < 0 )
	{ return 1;	}

    *pLs= fl.fl_ls;
    *pLevel= fl.fl_level;
    *pMultiList= fl.fl_multiList;
    *pMultiLevel= fl.fl_multiLevel;
    *pParaNr= fl.fl_paraNr;

    return 0;
    }

/************************************************************************/
/*									*/
/*  The ruler of a list was changed: transfer properties to the		*/
/*  paragraphs in the list.						*/
/*									*/
/*  NOTE: The implementation relies on the fact that the root of the	*/
/*	tree is not a paragraph that has to be adjusted.		*/
/*									*/
/************************************************************************/

static int docAdaptParagraphPropsToListLevel(
					int *			pChanged,
					ParagraphProperties *	pp,
					const ListLevel *	ll )
    {
    int		changed= 0;

    if  ( pp->ppFirstIndentTwips != ll->llFirstIndentTwips )
	{
	pp->ppFirstIndentTwips= ll->llFirstIndentTwips;
	changed= 1;
	}

    if  ( pp->ppLeftIndentTwips != ll->llLeftIndentTwips )
	{
	pp->ppLeftIndentTwips= ll->llLeftIndentTwips;
	changed= 1;
	}

    if  ( pp->ppTabStopListNumber != ll->llTabStopListNumber )
	{
	pp->ppTabStopListNumber= ll->llTabStopListNumber;
	changed= 1;
	}

    *pChanged= changed;
    return 0;
    }

int docAdaptParagraphToListLevel(	int *			pChanged,
					BufferItem *		paraNode,
					const BufferDocument *	bd )
    {
    int				indentChanged= 0;

    ListOverride *		lo;
    DocumentList *		dl;
    const ListLevel *		ll;

    int * const			startPath= (int *)0;
    int * const			formatPath= (int *)0;
    const ParagraphProperties *	pp= &(paraNode->biParaProperties);

    if  ( docGetListLevelOfParagraph( startPath, formatPath,
						    &lo, &dl, &ll, pp, bd ) )
	{ LLDEB(paraNode->biParaListOverride,paraNode->biParaListLevel);	}
    else{
	if  ( docAdaptParagraphPropsToListLevel( &indentChanged,
					&(paraNode->biParaProperties), ll ) )
	    { LDEB(1);		}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Apply the changes to a list to the affected paragraphs at a 	*/
/*  cetrain level of the list.						*/
/*									*/
/************************************************************************/

static int docApplyListRulerLevel(
				const ListNumberTreeNode *	lntn,
				int				level,
				const DocumentList *		dl,
				const ListOverride *		lo,
				const DocumentTree *		dt )
    {
    const ListLevel *		ll= (const ListLevel *)0;
    const ListOverrideLevel *	lol= (const ListOverrideLevel *)0;

    int				rval= 0;

    int				i;

    if  ( level < lo->loLevelCount )
	{ lol= &(lo->loLevels[level]);	}
    if  ( level < dl->dlLevelCount )
	{ ll= &(dl->dlLevels[level]);	}

    if  ( lol && lol->lolOverrideFormat )
	{ ll= &(lol->lolListLevel);	}

    for ( i= 0; i < lntn->lntnChildCount; i++ )
	{
	BufferItem *		paraNode;
	int			changed= 0;
	ListNumberTreeNode *	child= lntn->lntnChildren[i];

	if  ( docApplyListRulerLevel( child, level+ 1, dl, lo, dt ) )
	    { LLDEB(level,i); rval= -1;	}

	if  ( child->lntnParaNr < 0 )
	    { continue;	}

	paraNode= docGetParagraphByNumber( dt, child->lntnParaNr );
	if  ( ! paraNode )
	    { LXDEB(child->lntnParaNr,paraNode); rval= -1; continue; }

	if  ( paraNode->biParaListLevel != level )
	    {
	    LLLDEB(child->lntnParaNr,paraNode->biParaListLevel,level);
	    rval= -1;
	    }

	if  ( ! ll )
	    { XDEB(ll); continue;		}

	if  ( docAdaptParagraphPropsToListLevel( &changed,
					&(paraNode->biParaProperties), ll ) )
	    { LDEB(1); return -1;	}

	if  ( changed )
	    { docInvalidateParagraphLayout( paraNode ); }
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply the changes to a list to the affected paragraphs in one of	*/
/*  the document trees.							*/
/*									*/
/************************************************************************/

static int docApplyListRulerToTree(	const DocumentList *		dl,
					const ListOverride *		lo,
					BufferDocument *		bd,
					const DocumentTree *		dt )
    {
    if  ( ! dt->dtRoot						||
	  dt->dtListNumberTrees.lntTreeCount <= lo->loIndex	)
	{ return 0;	}

    return docApplyListRulerLevel( dt->dtListNumberTrees.lntTrees+ lo->loIndex,
								0, dl, lo, dt );
    }

/************************************************************************/
/*									*/
/*  Apply the changes to a list to the affected paragraphs. We only	*/
/*  expect paragraphs in the document body, but it does no harm to	*/
/*  cope with them in the other trees as well.				*/
/*									*/
/************************************************************************/

int docApplyListRuler(	const DocumentList *		dl,
			const ListOverride *		lo,
			BufferDocument *		bd )
    {
    int			i;
    int			rval= 0;
    BufferItem *	bodyBi;
    DocumentNote *	dn;

    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdBody) ) )
	{ LDEB(1); rval= -1;	}

    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdEiFtnsep) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdEiFtnsepc) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdEiFtncn) ) )
	{ LDEB(1); rval= -1;	}

    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdEiAftnsep) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdEiAftnsepc) ) )
	{ LDEB(1); rval= -1;	}
    if  ( docApplyListRulerToTree( dl, lo, bd, &(bd->bdEiAftncn) ) )
	{ LDEB(1); rval= -1;	}

    bodyBi= bd->bdBody.dtRoot;
    for ( i= 0; i < bodyBi->biChildCount; i++ )
	{
	BufferItem *		sectBi= bodyBi->biChildren[i];
	SectHeadersFooters *	shf= sectBi->biSectHeadersFooters;

	if  ( shf )
	    {
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfFirstPageHeader) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfLeftPageHeader) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfRightPageHeader) ) )
		{ LDEB(1); rval= -1;	}

	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfFirstPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfLeftPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    if  ( docApplyListRulerToTree( dl, lo, bd,
					    &(shf->shfRightPageFooter) ) )
		{ LDEB(1); rval= -1;	}
	    }
	}

    dn= bd->bdNotesList.nlNotes;
    for ( i= 0; i < bd->bdNotesList.nlNoteCount; dn++, i++ )
	{
	if  ( NOTE_IS_DELETED( dn ) )
	    { continue;	}

	if  ( docApplyListRulerToTree( dl, lo, bd, &(dn->dnDocumentTree) ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }

