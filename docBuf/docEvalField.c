/************************************************************************/
/*									*/
/*  Evaluate fields+ the list of kinds of fields.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docField.h"
#   include	"docParaString.h"
#   include	"docEvalField.h"
#   include	"docParaParticules.h"
#   include	"docDebug.h"
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>
#   include	"docTreeScanner.h"
#   include	"docDebug.h"
#   include	"docRecalculateFields.h"

/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

void docInitRecalculateFields(   RecalculateFields *     rf )
    {
    rf->rfDocument= (BufferDocument *)0;
    rf->rfTree= (DocumentTree *)0;
    rf->rfSelectedTree= (DocumentTree *)0;
    rf->rfMergeValueTree= (void *)0;
    rf->rfCloseObject= (DOC_CLOSE_OBJECT)0;
    rf->rfFieldsUpdated= 0;
    rf->rfUpdateFlags= 0;

    rf->rfBodySectNode= (const BufferItem *)0;
    rf->rfBodySectPage= -1;

    docInitEditPosition( &(rf->rfSelHead) );
    docInitEditPosition( &(rf->rfSelTail) );

    rf->rfFieldDataProvider= (FieldDataProvider)0;
    rf->rfInstanceStreamProvider= (InstanceStreamProvider)0;
    rf->rfMergeThrough= (void *)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Replace the previous contents of a field with new contents.		*/
/*									*/
/************************************************************************/

int docFieldReplaceContents(
			int *				pStroff,
			int *				pStroffShift,
			int *				pTextAttrNr,
			BufferItem *			paraNode,
			int				part,
			int				partCount,
			int				stroffShift,
			const char *			addedString,
			int				addedStrlen,
			const RecalculateFields *	rf )
    {
    int			i;
    TextParticule *	tp= paraNode->biParaParticules+ part;

    int			textAttributeNumber= tp[1].tpTextAttrNr;
    int			past= tp[1+partCount].tpStroff+ stroffShift;
    int			stroff= tp[1].tpStroff+ stroffShift;

    int			d= 0;

    if  ( docParaStringReplace( &d, paraNode, stroff, past,
					    addedString, addedStrlen ) )
	{ LDEB(addedStrlen); return -1;	}

    if  ( partCount > 0 )
	{
	tp= paraNode->biParaParticules+ part+ 1;
	for ( i= 0; i < partCount; tp++, i++ )
	    {
	    if  ( rf->rfCloseObject )
		{ (*rf->rfCloseObject)( rf->rfDocument, tp );	}
	    docCleanParticuleObject( rf->rfDocument, tp );
	    }

	docDeleteParticules( paraNode, part+ 1, partCount );
	}

    *pStroff= stroff;
    *pStroffShift= d;
    *pTextAttrNr= textAttributeNumber;
    return 0;
    }

/************************************************************************/

int docRecalculateFieldParticulesFromString(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraNode,
				int				part,
				int				partCount,
				const MemoryBuffer *		mbResult,
				const RecalculateFields *	rf )
    {
    int					d;

    TextParticule *			tp= paraNode->biParaParticules+ part;
    int					textAttributeNumber;

    int					past;
    int					stroff;
    int					partsMade;

    int					i;

    if  ( tp[1].tpStrlen == mbResult->mbSize				&&
	  ! memcmp( docParaString( paraNode, tp[1].tpStroff+ *pStroffShift ),
				    mbResult->mbBytes, mbResult->mbSize ) )
	{
	*pCalculated= 0;
	*pPartShift= 0;
	/* NO! *pStroffShift= 0; */
	return 0;
	}

    textAttributeNumber= tp[1].tpTextAttrNr;
    past= tp[1+partCount].tpStroff+ *pStroffShift;
    stroff= tp[1].tpStroff+ *pStroffShift;

    if  ( docParaStringReplace( &d, paraNode, stroff, past,
				(char *)mbResult->mbBytes, mbResult->mbSize ) )
	{ LDEB(mbResult->mbSize); return -1;	}

    tp= paraNode->biParaParticules+ part+ 1;
    for ( i= 0; i < partCount; tp++, i++ )
	{
	if  ( rf->rfCloseObject )
	    { (*rf->rfCloseObject)( rf->rfDocument, tp ); }

	docCleanParticuleObject( rf->rfDocument, tp );
	}

    partsMade= docRedivideStringInParticules( paraNode, stroff,
				    mbResult->mbSize,
				    part+ 1, partCount, textAttributeNumber );
    if  ( partsMade < partCount )
	{
	docDeleteParticules( paraNode,
				part+ 1+ partsMade, partCount- partsMade );
	}

    *pCalculated= 1;
    *pPartShift= partsMade- partCount;
    *pStroffShift += d;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Substitute an individual text field in a paragraph.			*/
/*									*/
/*  NOTE: This routine is not recursive. Nested fields are handeled	*/
/*	by the caller.							*/
/*									*/
/************************************************************************/

int docRecalculateParaStringTextParticules(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraNode,
				int				part,
				int				partCount,
				DocumentField *			df,
				const RecalculateFields *	rf )
    {
    int					rval= 0;
    MemoryBuffer			mbResult;
    int					calculated= 0;

    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    utilInitMemoryBuffer( &mbResult );

    if  ( (*fki->fkiCalculateTextString)( &calculated, &mbResult, df, rf ) )
	{ SDEB(fki->fkiLabel); rval= -1; goto ready;	}

    if  ( ! calculated )
	{
	*pCalculated= 0;
	*pPartShift= 0;
	/* NO! *pStroffShift= 0; */
	goto ready;
	}

    if  ( docRecalculateFieldParticulesFromString(
				    pCalculated, pPartShift, pStroffShift,
				    paraNode, part, partCount, &mbResult, rf ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &mbResult );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Substitute the text fields in a paragraph.				*/
/*									*/
/*  1)  For all fields in range...					*/
/*	NOTE that the end of the range may have been shifted by the	*/
/*	calculation of field results.					*/
/*  2)  Shift this particule by what previous calculations have changed	*/
/*	in size.							*/
/*  3)  Not the beginning of a field.. Irrelevant.			*/
/*  4)  Retrieve the field. Do some sanity checks: Only text level	*/
/*	fields are evaluated here.					*/
/*  5)  Count the number of particules currently in the field.		*/
/*  6)  When the field is to be recalculated.. do so.			*/
/*	NOTE that this may shift both the array of particules and the	*/
/*	paragraph text.							*/
/*	Otherwise just shift the offsets of the particules inside the	*/
/*	field.								*/
/*  7)  When there is sufficient space inside the field to contain	*/
/*	yet another one.. go into recursion.				*/
/*	NOTE that this may shift both the array of particules and the	*/
/*	paragraph text.							*/
/*  8)  Shift the end particule of the field.				*/
/*  9)  Set the current position in the loop to the end particule. The	*/
/*	loop increment will move to the particule after the field.	*/
/*									*/
/************************************************************************/

static int docRecalculateParaTextFields(
				RecalculateFields *	rf,
				int *			pPartShift,
				int *			pStroffShift,
				BufferItem *		paraNode,
				int			part,
				int			partUpto )
    {
    BufferDocument *	bd= rf->rfDocument;
    int			fieldsUpdated= 0;
    int			paraNr= -1;

    /*  1  */
    for ( part= part; part < partUpto+ *pPartShift; part++ )
	{
	TextParticule *			tp= paraNode->biParaParticules+ part;

	DocumentField *			df;
	const FieldKindInformation *	fki;

	int				tailPart;
	int				partCount;
	int				closed;
	int				stroffTail;

	/*  2  */
	if  ( docShiftParticuleOffsets( rf->rfDocument, paraNode, part, part+ 1,
							    *pStroffShift ) )
	    { LDEB(*pStroffShift);	}

	/*  3  */
	if  ( tp->tpKind != DOCkindFIELDHEAD )
	    { continue;	}

	if  ( paraNr <= 0					&&
	      ( rf->rfSelHead.epParaNr > 0	||
	        rf->rfSelTail.epParaNr > 0	)	)
	    { paraNr= docNumberOfParagraph( paraNode );	}

	/*  4  */
	df= docGetFieldByNumber( &(bd->bdFieldList), tp->tpObjectNumber );
	if  ( ! df )
	    {
	    LXDEB(tp->tpObjectNumber,df);
	    docListNode(0,paraNode,0);
	    docListFieldTree(rf->rfDocument,rf->rfTree);
	    continue;
	    }
	if  ( df->dfKind >= DOC_FieldKindCount )
	    { LLDEB(df->dfKind,DOC_FieldKindCount); continue;	}

	fki= DOC_FieldKinds+ df->dfKind;

	/*  5  */
	partCount= docCountParticulesInField( paraNode, &closed, part,
						    partUpto+ *pPartShift );
	if  ( partCount < 0 )
	    { SLDEB(docFieldKindStr(df->dfKind),partCount); continue;	}
	tailPart= part+ 1+ partCount;
	if  ( tailPart < paraNode->biParaParticuleCount )
	    { stroffTail= tp[1+partCount].tpStroff;	}
	else{ stroffTail= docParaStrlen( paraNode );	}

	/*  6  */
	if  ( closed						&&
	      fki->fkiLevel == DOClevSPAN			&&
	      ( fki->fkiCalculateWhen & rf->rfUpdateFlags )	&&
	      fki->fkiCalculateTextParticules			)
	    {
	    int			partShift= 0;
	    int			calculated= 0;
	    int			oldStroffShift= *pStroffShift;

	    if  ( (*fki->fkiCalculateTextParticules)( &calculated,
					&partShift, pStroffShift,
					paraNode, part, partCount, df, rf ) )
		{ LDEB(1); return -1;	}

	    if  ( calculated )
		{
		int	stroffShift= *pStroffShift- oldStroffShift;

		if  ( rf->rfTree == rf->rfSelectedTree )
		    {
		    docAdjustEditPositionOffsetB( &(rf->rfSelHead),
					paraNr, stroffTail, stroffShift );
		    docAdjustEditPositionOffsetE( &(rf->rfSelTail),
					paraNr, stroffTail, stroffShift );
		    }

		fieldsUpdated++; rf->rfFieldsUpdated++;
		}
	    else{
		if  ( docShiftParticuleOffsets( rf->rfDocument, paraNode,
					part+ 1, tailPart, *pStroffShift ) )
		    { LDEB(*pStroffShift);	}
		}

	    tailPart += partShift;
	    *pPartShift += partShift;
	    }
	else{
	    if  ( docShiftParticuleOffsets( rf->rfDocument, paraNode,
					part+ 1, tailPart, *pStroffShift ) )
		{ LDEB(*pStroffShift);	}
	    }

	/*  7  */
	if  ( tailPart- part >= 2 )
	    {
	    int			partShift= 0;
	    int			stroffShift= 0;

	    if  ( docRecalculateParaTextFields( rf,
		    &partShift, &stroffShift, paraNode, part+ 1, tailPart ) )
		{ LDEB(1); return -1;	}

	    tailPart += partShift;
	    *pPartShift += partShift;
	    *pStroffShift += stroffShift;
	    }

	/*  8   */
	if  ( closed )
	    {
	    tp= paraNode->biParaParticules+ tailPart;
	    if  ( tp->tpKind != DOCkindFIELDTAIL )
		{ LDEB(tp->tpKind);	}

	    tp->tpStroff += *pStroffShift;
	    df->dfTailPosition.epStroff= tp->tpStroff;
	    }

	/*  9  */
	part= tailPart;
	}

    if  ( fieldsUpdated )
	{ docInvalidateParagraphLayout( paraNode );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Substitute the text fields in a document tree.			*/
/*									*/
/*  1)	Unexpected recursion?						*/
/*									*/
/************************************************************************/

int docRecalculateTextLevelFieldsInDocumentTree(
					RecalculateFields *	rf,
					DocumentTree *		dt,
					const BufferItem *	bodySectNode,
					int			page )
    {
    int			rval= 0;
    int			ret;
    int			saveFieldsUpdated= rf->rfFieldsUpdated;
    DocumentTree *	saveTree= rf->rfTree;
    const BufferItem *	saveBodySectNode= rf->rfBodySectNode;
    int			saveBodySectPage= rf->rfBodySectPage;

    if  ( ! dt->dtRoot )
	{ return 0;	}

    if  ( dt->dtRoot->biTreeType != DOCinBODY			&&
          ( ! bodySectNode				||
            bodySectNode->biTreeType != DOCinBODY	)	)
	{ XDEB(rf->rfBodySectNode); return -1;	}

    rf->rfBodySectNode= bodySectNode;
    rf->rfBodySectPage= page;

    rf->rfFieldsUpdated= 0;
    rf->rfTree= dt;

    ret= docRecalculateTextLevelFields( rf, dt->dtRoot );
    if  ( ret )
	{ LDEB(ret); rval= -1;	}

    if  ( rf->rfFieldsUpdated > 0 )
	{ docInvalidateTreeLayout( dt );	}

    rf->rfFieldsUpdated += saveFieldsUpdated;

    rf->rfTree= saveTree;
    rf->rfBodySectNode= saveBodySectNode;
    rf->rfBodySectPage= saveBodySectPage;

    return rval;
    }

static int docRecalculateTextLevelFieldsInSeparators(
					RecalculateFields *	rf,
					const BufferItem *	bodySectNode )
    {
    const int		page= -1;
    BufferDocument *	bd= rf->rfDocument;

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdEiFtnsep), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdEiFtnsepc), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdEiFtncn), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdEiAftnsep), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdEiAftnsepc), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdEiAftncn), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRecalculateTextLevelFieldsInSectHdFt(
					RecalculateFields *	rf,
					const BufferItem *	bodySectNode )
    {
    const int			page= -1;
    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;

    rf->rfBodySectNode= bodySectNode;

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfFirstPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfLeftPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfRightPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}


    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfFirstPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfLeftPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfRightPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRecalculateTextLevelFieldsInSectNotes(
					RecalculateFields *	rf,
					const BufferItem *	bodySectNode )
    {
    BufferDocument *	bd= rf->rfDocument;

    DocumentField *	dfNote;
    DocumentNote *	dn;
    const int		page= -1;
    const int		treeType= -1;

    dfNote= docGetFirstNoteOfSection( &dn, bd,
				bodySectNode->biNumberInParent, treeType );
    while( dfNote )
	{
	DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( ! dt->dtRoot )
	    { continue;	}

	if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
						dt, bodySectNode, page ) )
	    { LDEB(1); return -1;	}

	dfNote= docGetNextNoteInSection( &dn, bd,
			bodySectNode->biNumberInParent, dfNote, treeType );
	}

    return 0;
    }

static int docRecalculateTextLevelFieldsLeaveNode(
				    struct BufferItem *		node,
				    const DocumentSelection *	ds,
				    const struct BufferItem *	bodySectNode,
				    void *			voidrf )
    {
    RecalculateFields *	rf= (RecalculateFields *)voidrf;
    int			rval= 0;

    const BufferItem *	saveBodySectNode= rf->rfBodySectNode;

    switch( node->biLevel )
	{
	case DOClevBODY:
	    if  ( docRecalculateTextLevelFieldsInSeparators( rf,
						    node->biChildren[0] ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    break;

	case DOClevSECT:
	    if  ( node->biTreeType == DOCinBODY )
		{
		SectHeadersFooters *	shf= node->biSectHeadersFooters;

		if  ( ! shf )
		    { XDEB(shf); rval= -1; goto ready;	}

		if  ( docRecalculateTextLevelFieldsInSectHdFt( rf, node ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( docRecalculateTextLevelFieldsInSectNotes( rf, node ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}

	    break;

	case DOClevPARA:

	    {
	    int		partShift= 0;
	    int		stroffShift= 0;

	    if  ( docRecalculateParaTextFields( rf, &partShift, &stroffShift,
					node, 0, node->biParaParticuleCount ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	    break;

	default:
	    break;
	}

  ready:

    rf->rfBodySectNode= saveBodySectNode;

    return rval;
    }

int docRecalculateTextLevelFields(	RecalculateFields *	rf,
					BufferItem *		node )
    {
    const int		flags= 0;

    if  ( docScanTreeNode( rf->rfDocument, node,
		    (NodeVisitor)0, docRecalculateTextLevelFieldsLeaveNode,
		    flags, (void *)rf ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
