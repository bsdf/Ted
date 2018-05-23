/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docDebug.h>
#   include	<docParaString.h>
#   include	<docNodeTree.h>
#   include	"docCopyNode.h"
#   include	"docEdit.h"
#   include	"docEditImpl.h"
#   include	<docField.h>
#   include	<docParaParticules.h>

#   define	DEB_PARTICULES		0
#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Replace a selection with a piece of text.				*/
/*									*/
/*  1)  If the selection spans more than one paragraph...		*/
/*  2)  Replace the tail of the first paragraph with the new text.	*/
/*  2b) Make sure that the bullet of the last paragraph is not included	*/
/*	in the tail that is to be appended to the first paragraph.	*/
/*  3)  Merge the two paragraphs. I.E: append what remains of the last	*/
/*	paragraph to the first paragraph.				*/
/*  4)  Erase all paragraphs after the beginning of the selection	*/
/*	upto and including the end of the selection.			*/
/*  5)  Otherwise just replace the text inside the selected paragraph.	*/
/*									*/
/************************************************************************/

int docReplaceSelection(	EditOperation *			eo,
				const char *			addedString,
				int				addedStrlen,
				int				addedAttrNr )
    {
    int			rval= 0;
    DocumentCopyJob	dcjTail;
    int			paraNrBegin;

    paraNrBegin= docNumberOfParagraph( eo->eoHeadDp.dpNode );

    docInitDocumentCopyJob( &dcjTail );

    if  ( docEditDeleteReplacedFields( eo ) )
	{ LDEB(1);	}

    if  ( eo->eoHeadDp.dpStroff == 0			&&
	  eo->eoHeadDp.dpNode->biParaListOverride > 0	)
	{
	if  ( docRemoveParagraphFromList( eo->eoHeadDp.dpNode,
						eo->eoTree, eo->eoDocument ) )
	    { LDEB(1);	}
	}

    /*  1  */
    if  ( ! docPositionsInsideParagraph( &(eo->eoHeadDp), &(eo->eoTailDp) ) )
	{
	DocumentPosition	tailDp= eo->eoTailDp;

	if  ( eo->eoHeadDp.dpNode->biParaTableNesting > 0		&&
	      eo->eoTailDp.dpNode->biParaTableNesting == 0	)
	    { docFlattenRow( eo->eoHeadDp.dpNode );	}

	/*  2  */
	if  ( docParaReplaceText( eo, paraNrBegin, &tailDp,
			&(eo->eoHeadDp), docParaStrlen( eo->eoHeadDp.dpNode ),
			addedString, addedStrlen, addedAttrNr ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	/*  3  */
	if  ( eo->eoTailDp.dpStroff < docParaStrlen( eo->eoTailDp.dpNode ) )
	    {
	    const int	copyFields= 1;

	    if  ( docSet1DocumentCopyJob( &dcjTail, eo, copyFields ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    /*  tail of EO is after insert */
	    if  ( docParaInsertTail( &dcjTail, paraNrBegin,
				&(eo->eoTailDp), &tailDp, &(eo->eoTailDp) ) )
		{ LDEB(eo->eoTailDp.dpStroff); rval= -1; goto ready;	}
	    }
	else{
	    eo->eoTailDp= tailDp;
	    }

	docSetEditPosition( &(eo->eoSelectedRange.erTail), &tailDp );
	docSetEditPosition( &(eo->eoAffectedRange.erTail), &tailDp );

	/*  4  */
	if  ( docRemoveSelectionTail( eo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	eo->eoTailDp= tailDp;

	docCheckNoBreakAsLast( eo, eo->eoHeadDp.dpNode );

	docEditIncludeNodeInReformatRange( eo, eo->eoHeadDp.dpNode );
	}
    else{
	int		tailStroffOld= eo->eoTailDp.dpStroff;
	int		tailStroffNew;

	/*  5  */
	if  ( docParaReplaceText( eo, paraNrBegin, &(eo->eoTailDp),
			&(eo->eoHeadDp), eo->eoTailDp.dpStroff,
			addedString, addedStrlen, addedAttrNr ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	docCheckNoBreakAsLast( eo, eo->eoHeadDp.dpNode );

	tailStroffNew= eo->eoTailDp.dpStroff;
	docSetParagraphAdjust( eo, eo->eoHeadDp.dpNode,
				tailStroffNew- tailStroffOld, tailStroffNew );
	}

    docEditFinishStep( eo );

  ready:

    docCleanDocumentCopyJob( &dcjTail );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete the selection: Replace it with noting.			*/
/*									*/
/************************************************************************/

int docDeleteSelection(	EditOperation *			eo )
    {
    const char * const	addedString= (const char *)0;
    int			addedStrlen= 0;
    const int		addedAttrNr= -1;

    return docReplaceSelection( eo, addedString, addedStrlen, addedAttrNr );
    }

/************************************************************************/
/*									*/
/*  Clean particules that will no longer be needed.			*/
/*									*/
/************************************************************************/

int docEditCleanParticules(	EditOperation *		eo,
				BufferItem *		paraBi,
				int			partFrom,
				int			partUpto )
    {
    int			part;
    TextParticule *	tp;

    /*  11  */
    tp= paraBi->biParaParticules+ partFrom;
    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
#	if DEB_PARTICULES
	const int	indent= 0;
	docListParticule( indent, "OLD", part, paraBi, tp );
#	endif

	if  ( eo->eoCloseObject )
	    { (*eo->eoCloseObject)( eo->eoDocument, tp ); }

	docCleanParticuleObject( eo->eoDocument, tp );

	if  ( tp->tpKind == DOCkindFIELDHEAD	||
	      tp->tpKind == DOCkindFIELDTAIL	)
	    {
	    /* Not yet deleted? */
	    if  ( tp->tpObjectNumber >= 0 )
		{
		const DocumentField *	df;
		df= docGetFieldByNumber( &(eo->eoDocument->bdFieldList),
						    tp->tpObjectNumber );
		if  ( df && df->dfFieldNumber >= 0 )
		    {
		    LSLDEB(part,docKindStr(tp->tpKind),df->dfFieldNumber);
		    docListFieldParticule( 0, "FLD", part, paraBi, tp, df );
		    return -1;
		    }
		}
	    }

	tp->tpObjectNumber= -1;
	tp->tpKind= DOCkindUNKNOWN;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is this indentation at the head of the paragraph? If so.. skip it	*/
/*  while merging.							*/
/*									*/
/************************************************************************/

static int docIsIndentationParticule(	const BufferItem *	bi,
					const TextParticule *	tp )
    {
    const unsigned char *	s= docParaString( bi, tp->tpStroff );
    int				i;

    if  ( tp->tpKind == DOCkindTAB )
	{ return 1;	}

    if  ( tp->tpKind != DOCkindSPAN )
	{ return 0;	}

    for ( i= 0; i < tp->tpStrlen; i++ )
	{
	if  ( *(s++) != ' ' )
	    { return 0;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Merge the selection into one paragraph.				*/
/*									*/
/************************************************************************/

int docMergeParagraphsInSelection(	EditOperation *	eo )
    {
    int				rval= 0;
    BufferDocument *		bd= eo->eoDocument;

    BufferItem *		biTo;
    BufferItem *		biFrom;

    DocumentCopyJob		dcj;
    const int			copyFields= 0;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet1DocumentCopyJob( &dcj, eo, copyFields ) )
	{ LDEB(1); rval= -1; goto ready;	}

    biTo= eo->eoHeadDp.dpNode;
    if  ( ! biTo )
	{ XDEB(biTo); rval= -1; goto ready;	}
    biFrom= docNextParagraph( biTo );
    if  ( ! biFrom )
	{ XDEB(biFrom); rval= -1; goto ready;	}

    for (;;)
	{
	int	partFrom= 0;

	int	particulesInserted= 0;
	int	charactersCopied= 0;

	if  ( biFrom->biParaListOverride > 0 )
	    {
	    DocumentField *	dfHead= (DocumentField *)0;
	    DocumentSelection	dsInsideHead;
	    DocumentSelection	dsAroundHead;
	    int			partBegin= -1;
	    int			partEnd= -1;

	    if  ( docDelimitParaHeadField( &dfHead,
					&dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, biFrom, bd ) )
		{ LDEB(biFrom->biParaListOverride);	}
	    else{
		if  ( partFrom <= partEnd )
		    { partFrom= partEnd+ 1;	}
		}
	    }

	while( partFrom < biFrom->biParaParticuleCount- 1		&&
	       docIsIndentationParticule( biFrom,
				biFrom->biParaParticules+ partFrom )	)
	    { partFrom++; }

	if  ( partFrom < biFrom->biParaParticuleCount )
	    {
	    int			toCount= biTo->biParaParticuleCount;
	    int			toStrlen= docParaStrlen( biTo );
	    const unsigned char *
				toString= docParaString( biTo, 0 ); /*SHAME*/

	    if  ( toCount > 0						&&
		  biTo->biParaParticules[toCount-1].tpKind ==
							DOCkindSPAN	&&
		  toString[toStrlen-1] != ' '				)
		{
		int		stroffShift= 0;

		if  ( docParaStringReplace( &stroffShift, biTo,
					    toStrlen, toStrlen, " ", 1 ) )
		    { LDEB(toStrlen); rval= -1; goto ready;	}

		biTo->biParaParticules[toCount-1].tpStrlen++;
		}

	    if  ( docCopyParticules( &dcj, biTo, biFrom,
			biTo->biParaParticuleCount, partFrom,
			biFrom->biParaParticuleCount- partFrom,
			&particulesInserted, &charactersCopied ) )
		{ LDEB(biFrom->biParaParticuleCount); rval= -1; goto ready; }
	    }

	if  ( biFrom == eo->eoTailDp.dpNode )
	    { break;	}

	biFrom= docNextParagraph( biFrom );
	if  ( ! biFrom )
	    { XDEB(biFrom); rval= -1; goto ready;	}
	}

    biTo= eo->eoHeadDp.dpNode;
    if  ( ! biTo )
	{ XDEB(biTo); rval= -1; goto ready;	}

    if  ( docTailPosition( &(eo->eoTailDp), eo->eoHeadDp.dpNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRemoveSelectionTail( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilIndexSetUnion( &(eo->eoNoteFieldsAdded), &(eo->eoNoteFieldsAdded),
					    &(dcj.dcjNoteFieldsCopied) );
    if  ( dcj.dcjBulletsCopied > 0 )
	{ eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}

    docEditIncludeNodeInReformatRange( eo, biTo );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

