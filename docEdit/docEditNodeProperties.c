/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	"docEdit.h"
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>
#   include	<docParaParticules.h>
#   include	<docListDepth.h>

/************************************************************************/
/*									*/
/*  Set the list number field at the head of a fresh paragraph.		*/
/*									*/
/*  We have an issue with the text attributes of the fresh field:	*/
/*  -	If we copy it from the particles in the paragraph, we		*/
/*	contaminate the text attributes of the listttext field.		*/
/*									*/
/************************************************************************/

static int docInsertListtextField( DocumentField **	pField,
				DocumentSelection *	dsInsideField,
				DocumentSelection *	dsAroundField,
				int *			pPartBegin,
				int *			pPartEnd,
				BufferItem *		paraNode,
				DocumentTree *		dt,
				BufferDocument *	bd )
    {
    int				rval= 0;

    int				paraNr= docNumberOfParagraph( paraNode );
    int				textAttrNr= 0;

#   if 1
    if  ( paraNode->biParaParticuleCount > 0 )
	{
	const TextParticule *		tp= paraNode->biParaParticules;

	textAttrNr= tp->tpTextAttrNr;
	}
#   endif

    if  ( docInsertParaHeadField( pField, dsInsideField, dsAroundField,
			    pPartBegin, pPartEnd,
			    paraNode, bd, dt, DOCfkLISTTEXT, textAttrNr ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docListNumberTreesInsertParagraph( &(dt->dtListNumberTrees),
					paraNode->biParaListOverride,
					paraNode->biParaListLevel, paraNr ) )
	{ LDEB(paraNr); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Update paragraph properties and do additional administration.	*/
/*									*/
/************************************************************************/

int docEditUpdParaProperties(	EditOperation *			eo,
				PropertyMask *			pPpDoneMask,
				BufferItem *			paraNode,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,
				const DocumentAttributeMap *	dam )
    {
    BufferDocument *		bd= eo->eoDocument;

    int				wasInList= 0;
    int				isInList= 0;
    int				listChange= 0;

    int				ls= -1;
    int				ilvl= 0;

    int				olevel;

    PropertyMask		ppDoneMask;

    DocumentField *		dfBullet= (DocumentField *)0;
    DocumentSelection		dsInsideBullet;
    DocumentSelection		dsAroundBullet;
    int				partBulletHead= -1;
    int				partBulletTail= -1;

    int				paraNr= -1;

    utilPropMaskClear( &ppDoneMask );

    /*  A  */
    if  ( paraNode->biParaListOverride > 0 )
	{
	wasInList= 1;
	ls= paraNode->biParaListOverride;
	ilvl= paraNode->biParaListLevel;

	if  ( docDelimitParaHeadField( &dfBullet,
			    &dsInsideBullet, &dsAroundBullet,
			    &partBulletHead, &partBulletTail, paraNode, bd ) )
	    { /* LDEB(paraNode->biParaListOverride); */	}
	}

    olevel= paraNode->biParaOutlineLevel;

    if  ( docUpdParaProperties( &ppDoneMask, &(paraNode->biParaProperties),
						    ppSetMask, ppSet, dam ) )
	{ LDEB(1); return -1;	}

    if  ( paraNode->biParaListOverride > 0 )
	{ isInList= 1;	}

    if  ( wasInList						||
	  isInList						||
	  olevel < PPoutlineBODYTEXT				||
	  paraNode->biParaOutlineLevel < PPoutlineBODYTEXT	)
	{ paraNr= docNumberOfParagraph( paraNode );	}

    if  ( wasInList && ! isInList )
	{
	DocumentPosition	dpTail;

	if  ( docListNumberTreesDeleteParagraph(
				&(eo->eoTree->dtListNumberTrees), ls, paraNr ) )
	    { /*LLDEB(override,paraNr);*/	}

	if  ( dfBullet )
	    {
	    if  ( docDeleteField( &dsInsideBullet, eo, paraNode, paraNode,
			    partBulletHead, partBulletTail, dfBullet ) )
		{ LLDEB(partBulletHead,partBulletTail);	}

	    if  ( docParaDeleteText( eo, paraNr, &dpTail,
					    &(dsInsideBullet.dsHead),
					    dsInsideBullet.dsTail.dpStroff ) )
		{ LDEB(1); return -1;	}
	    }

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	}

    if  ( ! wasInList && isInList )
	{
	if  ( docInsertListtextField( &dfBullet,
					&dsInsideBullet, &dsAroundBullet,
					&partBulletHead, &partBulletTail,
					paraNode, eo->eoTree, bd ) )
	    { LDEB(1); return -1;	}

	listChange= 1;
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	}

    if  ( wasInList					&&
	  isInList					&&
	  ( paraNode->biParaListOverride != ls	||
	    paraNode->biParaListLevel != ilvl	)	)
	{
	if  ( docListNumberTreesDeleteParagraph(
				&(eo->eoTree->dtListNumberTrees), ls, paraNr ) )
	    { LLDEB(ls,paraNr);	}

	ls= paraNode->biParaListOverride;
	ilvl= paraNode->biParaListLevel;

	if  ( docListNumberTreesInsertParagraph(
			&(eo->eoTree->dtListNumberTrees), ls, ilvl, paraNr ) )
	    { LLLDEB(ls,ilvl,paraNr);	}

	listChange= 1;
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	}

    if  ( paraNode->biParaOutlineLevel != olevel )
	{
	if  ( olevel < PPoutlineBODYTEXT )
	    {
	    if  ( docListNumberTreeDeleteParagraph(
					&(eo->eoTree->dtOutlineTree), paraNr ) )
		{ LDEB(paraNr);	}
	    }

	if  ( paraNode->biParaOutlineLevel < PPoutlineBODYTEXT )
	    {
	    if  ( docListNumberTreeInsertParagraph(
					&(eo->eoTree->dtOutlineTree),
					paraNode->biParaOutlineLevel, paraNr ) )
		{ LDEB(paraNr);	}
	    }
	}

    if  ( listChange )
	{
	int		indentChanged= 0;

	if  ( docAdaptParagraphToListLevel( &indentChanged, paraNode, bd ) )
	    { LDEB(1);		}
	}

    if  ( paraNode->biParaTableNesting > 0	&&
	  paraNode->biNumberInParent == 0		)
	{ docEditAdaptRowPropertiesToFirstChild( eo, paraNode );	}

    if  ( ! utilPropMaskIsEmpty( &ppDoneMask ) )
	{ docEditIncludeNodeInReformatRange( eo, paraNode );	}

    if  ( pPpDoneMask )
	{ *pPpDoneMask= ppDoneMask;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Invalidate the layout of the notes in a section. This will force	*/
/*  them to be formatted later on in the edit operation.		*/
/*									*/
/************************************************************************/

static void docInvalidateNoteLayoutForSection(	BufferDocument *	bd,
						int			sect )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;
    const int		treeType= -1;

    if  ( sect >= 0 )
	{
	dfNote= docGetFirstNoteOfSection( &dn, bd, sect, treeType );
	}
    else{
	dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
	}

    for ( ;
	  dfNote;
	  dfNote= docGetNextNoteInSection( &dn, bd, sect, dfNote, treeType ) )
	{
	dn->dnReferringPage= -1;
	dn->dnReferringColumn= -1;

	docInvalidateTreeLayout( &(dn->dnDocumentTree) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change properties of a section.					*/
/*									*/
/*  For sections in the body, invalidate the layout of notes and the	*/
/*  headers and footers if that is needed.				*/
/*									*/
/*  Most properties will also cause a redraw of the section. Tell an	*/
/*  interested caller.							*/
/*									*/
/************************************************************************/

int docEditUpdSectProperties(	EditOperation *			eo,
				PropertyMask *			pSpDoneMask,
				BufferItem *			sectNode,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		spDoneMask;

    utilPropMaskClear( &spDoneMask );

    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}

    if  ( spSetMask )
	{
	if  ( docUpdSectProperties( &spDoneMask, &(sectNode->biSectProperties),
							spSetMask, spNew ) )
	    { LDEB(1); return -1;	}
	}

    if  ( sectNode->biTreeType == DOCinBODY )
	{
	/*
	if  ( PROPmaskISSET( &spDoneMask, SPpropTITLEPG )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_STYLE )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_HYPHEN )	||
	      PROPmaskISSET( &spDoneMask, SPpropPAGE_RESTART )	||
	      PROPmaskISSET( &spDoneMask, SPpropSTART_PAGE )	)
	    { redraw= 1;	}
	*/

	if  ( PROPmaskISSET( &spDoneMask, SPpropNUMBER_STYLE )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_HYPHEN )	||
	      PROPmaskISSET( &spDoneMask, SPpropPAGE_RESTART )	||
	      PROPmaskISSET( &spDoneMask, SPpropSTART_PAGE )	||
	      PROPmaskISSET( &spDoneMask, DGpropPAGE_WIDTH )	||
	      PROPmaskISSET( &spDoneMask, DGpropLEFT_MARGIN )	||
	      PROPmaskISSET( &spDoneMask, DGpropRIGHT_MARGIN )	)
	    { docInvalidateSectHeaderFooterLayout( sectNode );	}

	if  ( PROPmaskISSET( &spDoneMask, SPpropNUMBER_STYLE )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_HYPHEN )	||
	      PROPmaskISSET( &spDoneMask, SPpropPAGE_RESTART )	||
	      PROPmaskISSET( &spDoneMask, SPpropSTART_PAGE )	||
	      PROPmaskISSET( &spDoneMask, DGpropPAGE_WIDTH )	||
	      PROPmaskISSET( &spDoneMask, DGpropLEFT_MARGIN )	||
	      PROPmaskISSET( &spDoneMask, DGpropRIGHT_MARGIN )	||

	      PROPmaskISSET( &spDoneMask, SPpropCOLUMN_COUNT )	||
	      PROPmaskISSET( &spDoneMask, SPpropCOLUMN_SPACING ) ||
	      PROPmaskISSET( &spDoneMask, SPpropCOLUMNS )	)
	    {
	    docInvalidateNoteLayoutForSection( eo->eoDocument,
						sectNode->biNumberInParent );
	    }
	}

    if  ( ! utilPropMaskIsEmpty( &spDoneMask ) )
	{ docEditIncludeNodeInReformatRange( eo, sectNode );	}

    if  ( pSpDoneMask )
	{ utilPropMaskOr( pSpDoneMask, pSpDoneMask, &spDoneMask );	}

    return 0;
    }

void docEditAdaptRowPropertiesToFirstChild(
					EditOperation *		eo,
					const BufferItem *	paraNode )
    {
    if  ( paraNode->biParaTableNesting > 0	&&
	  paraNode->biNumberInParent == 0		)
	{
	BufferItem *	cellNode= paraNode->biParent;

	if  ( cellNode->biNumberInParent == 0 )
	    {
	    BufferItem *	rowNode= cellNode->biParent;

	    if  ( rowNode->biRow_Keepfollow != paraNode->biParaKeepWithNext )
		{
		rowNode->biRow_Keepfollow= paraNode->biParaKeepWithNext;
		docEditIncludeNodeInReformatRange( eo, rowNode );
		}
	    }
	}

    return;
    }

