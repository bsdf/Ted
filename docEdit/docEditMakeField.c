/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docTreeScanner.h>
#   include	<docParaParticules.h>
#   include	"docEdit.h"

int docEditSurroundTextSelectionByField(
				EditOperation *			eo,
				DocumentField **		pDf,
				DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				int *				pHeadPart,
				int *				pTailPart,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				int				fieldKind,
				const FieldInstructions *	fi )
    {
    BufferDocument *	bd= eo->eoDocument;
    DocumentTree *	dt= eo->eoTree;
    DocumentField *	df;

    DocumentSelection	ds;

    docEditOperationGetSelection( &ds, eo );

    if  ( docSurroundTextSelectionByField( bd, dt, &ds, &df,
				dsInside, dsAround, pHeadPart, pTailPart,
				taSetMask, taSet, fieldKind, fi ) )
	{ LDEB(fieldKind); return -1;	}

    docSetEditPosition( &(eo->eoSelectedRange.erTail), &(dsAround->dsTail) );
    docSetEditPosition( &(eo->eoAffectedRange.erTail), &(dsAround->dsTail) );

    *pDf= df; return 0;
    }

/************************************************************************/

static int docEditForgetFields(	BufferItem *			node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voideo )
    {
    if  ( node->biLevel == DOClevPARA )
	{
	int		partFrom;
	int		partUpto;
	int		headFlags;
	int		tailFlags;

	int		part;
	TextParticule *	tp;

	DocumentSelection	dsPara;

	if  ( docIntersectSelectionWithParagraph( &dsPara,
				    &partFrom, &partUpto,
				    &headFlags, &tailFlags,
				    node, ds ) < 0 )
	    { LDEB(1); return -1;	}

	if  ( headFlags & POSflagPART_TAIL )
	    { partFrom++;	}

	tp= node->biParaParticules+ partFrom;
	for ( part= partFrom; part < partUpto; tp++, part++ )
	    {
	    if  ( tp->tpKind == DOCkindFIELDHEAD	||
		  tp->tpKind == DOCkindFIELDTAIL	)
		{
		tp->tpObjectNumber= -1;
		}
	    }
	}

    return ADVICEtsOK;
    }

int docDeleteFieldsInRange(	EditOperation *		eo,
				DocumentSelection *	dsBalanced )
    {
    int			updateFlags= 0;
    int			flags= 0;

    EditRange		erBalanced;

    DocumentField *	dfLeft;
    DocumentField *	dfRight;

    int			headMoved= 0;
    int			tailMoved= 0;

    int * const		pHeadPart= (int *)0;
    int * const		pTailPart= (int *)0;

    if  ( docBalanceFieldSelection( &dfLeft, &dfRight,
			    &headMoved, &tailMoved, pHeadPart, pTailPart,
			    &(dsBalanced->dsHead), &(dsBalanced->dsTail),
			    eo->eoTree, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    docSetEditRange( &erBalanced, dsBalanced );

    if  ( docDeleteFieldRange( &updateFlags, eo->eoDocument,
				    &erBalanced, &(eo->eoTree->dtRootFields),
				    dfLeft, dfRight ) )
	{ LDEB(1);	}

    eo->eoFieldUpdate |= updateFlags;

    docScanSelection( eo->eoDocument, dsBalanced,
				    docEditForgetFields, (NodeVisitor)0,
				    flags, (void *)eo );

    return 0;
    }

/************************************************************************/
/*									*/
/*  A set of branches in the document tree, rather than a balanced	*/
/*  selection is deleted. Make sure that no partial fields remain.	*/
/*									*/
/*  This code relies on the fact that a field is deleted if its		*/
/*  contents are fully selected.					*/
/*									*/
/************************************************************************/

int docEditDeleteFieldsForBlockDelete(	EditOperation *			eo,
					const DocumentSelection *	ds )
    {
    int			rval= 0;
    DocumentSelection	dsBalanced= *ds;

    dsBalanced.dsHead.dpStroff= 0;
    dsBalanced.dsTail.dpStroff= docParaStrlen( dsBalanced.dsTail.dpNode );

    if  ( docDeleteFieldsInRange( eo, &dsBalanced ) )
	{ LDEB(1); return -1;	}

    return rval;
    }

