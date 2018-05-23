/************************************************************************/
/*									*/
/*  Buffer administration for 'ExternalItems' I.E. headers/footers and	*/
/*  foonotes/endnotes.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docShape.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>

/************************************************************************/
/*									*/
/*  Initialise an DocumentTree.						*/
/*									*/
/************************************************************************/

void docInitDocumentTree(	DocumentTree *	dt )
    {
    dt->dtRoot= (BufferItem *)0;
    dt->dtPageFormattedFor= -1;
    dt->dtColumnFormattedFor= -1;

    dt->dtY0UsedTwips= 0;
    dt->dtY1UsedTwips= 0;
    dt->dtY0ReservedTwips= 0;
    dt->dtY1ReservedTwips= 0;

    dt->dtPageSelectedUpon= -1;
    dt->dtColumnSelectedIn= -1;

    docInitListNumberTrees( &(dt->dtListNumberTrees) );
    docInitListNumberTreeNode( &(dt->dtOutlineTree) );

    docInitChildFields( &(dt->dtRootFields) );
    return;
    }

void docCleanDocumentTree(	BufferDocument *	bd,
				DocumentTree *		dt )
    {
    int		updateFlags= 0;

    if  ( dt->dtRoot )
	{ docDeleteDocumentTree( bd, dt );	}

    docDeleteChildFields( &updateFlags, bd, &(dt->dtRootFields) );

    docCleanListNumberTrees( &(dt->dtListNumberTrees) );
    docCleanListNumberTreeNode( &(dt->dtOutlineTree) );

    return;
    }

void docInvalidateTreeLayout(	DocumentTree *	dt )
    {
    dt->dtPageFormattedFor= -1;
    dt->dtColumnFormattedFor= -1;
    }

/************************************************************************/
/*									*/
/*  Delete the contents of a document tree.				*/
/*									*/
/*  Leave some of the geometry intact to permit the formatter to see	*/
/*  things have changed and hence cause a redraw of its original	*/
/*  rectangle.								*/
/*									*/
/************************************************************************/

void docEraseDocumentTree(	BufferDocument *	bd,
				DocumentTree *		dt )
    {
    int		resY0= dt->dtY0ReservedTwips;
    int		resY1= dt->dtY1ReservedTwips;

    docCleanDocumentTree( bd, dt );
    docInitDocumentTree( dt );

    dt->dtY0ReservedTwips= resY0;
    dt->dtY1ReservedTwips= resY1;

    return;
    }

/************************************************************************/
/*  									*/
/*  Keep track of the hierarchy of fields in this tree.			*/
/*  									*/
/************************************************************************/

int docAddRootFieldToTree(	DocumentTree *		dt,
				DocumentField *		dfCh )
    {
    if  ( docInsertChildField( (DocumentField *)0,
					    &(dt->dtRootFields), dfCh ) )
	{ LDEB(1); return -1;	}

    dfCh->dfParent= (DocumentField *)0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a new 'External Item' E.G. A section Header/Footer or a	*/
/*  Footnote/Endnote.							*/
/*									*/
/************************************************************************/

int docMakeDocumentTree(	BufferDocument *		bd,
				DocumentTree *			dt,
				const SelectionScope *		ss,
				const SectionProperties *	sp )
    {
    BufferItem *	bi= (BufferItem *)0;
    const int		numberInParent= 0;

    bi= (BufferItem *)malloc( sizeof(BufferItem) );
    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    docInitNode( bi, (BufferItem *)0, bd, numberInParent,
					DOClevSECT, ss->ssTreeType );

    if  ( docCopySectionProperties( &(bi->biSectProperties), sp ) )
	{ LDEB(1); docFreeNode( bd, dt, bi ); return -1; }

    bi->biSectBreakKind= DOCibkNONE;
    bi->biSectSelectionScope= *ss;
    dt->dtRoot= bi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a document tree and insert an empty paragraph into it.		*/
/*									*/
/************************************************************************/

BufferItem * docMakeExternalParagraph(
			BufferDocument *		bd,
			DocumentTree *			dt,
			int				treeType,
			const BufferItem *		bodyNode,
			int				ownerNumber,
			int				txtAttrNr )
    {
    BufferItem *		paraNode= (BufferItem *)0;
    BufferItem *		sectNode= (BufferItem *)0;

    sectNode= docMakeTreeRoot( bd, dt, bodyNode, ownerNumber, treeType );
    if  ( ! sectNode )
	{ XDEB(sectNode); return (BufferItem *)0;	}

    paraNode= docInsertEmptyParagraph( bd, dt->dtRoot, txtAttrNr );
    if  ( ! paraNode )
	{
	XDEB(paraNode);
	docCleanDocumentTree( bd, dt );
	docInitDocumentTree( dt );
	}

    return paraNode;
    }

BufferItem * docMakeTreeRoot(
			BufferDocument *		bd,
			DocumentTree *			dt,
			const BufferItem *		bodyNode,
			int				ownerNumber,
			int				treeType )
    {
    SectionProperties		spDef;

    int				numberInParent= -1;
    SelectionScope		ss;

    docInitSelectionScope( &ss );
    docInitSectionProperties( &spDef );

    switch( treeType )
	{
	case DOCinBODY:
	    LDEB(treeType); goto ready;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    bodyNode= docGetSectNode( (BufferItem *)bodyNode );
	    if  ( ! bodyNode )
		{ XDEB(bodyNode); goto ready;	}

	    numberInParent= bodyNode->biNumberInParent;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    bodyNode= docGetSectNode( (BufferItem *)bodyNode );
	    if  ( ! bodyNode )
		{ XDEB(bodyNode); goto ready;	}

	    numberInParent= bodyNode->biNumberInParent;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:

	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    break;

	default:
	    LDEB(treeType); goto ready;
	}


    ss.ssTreeType= treeType;
    ss.ssSectNr= 0;
    ss.ssOwnerSectNr= numberInParent;
    ss.ssOwnerNumber= ownerNumber;

    if  ( docMakeDocumentTree( bd, dt, &ss, &spDef ) )
	{ XDEB(dt->dtRoot); goto ready;	}

    docInvalidateTreeLayout( dt );

    dt->dtPageSelectedUpon= -1;
    dt->dtColumnSelectedIn= -1;

  ready:

    docCleanSectionProperties( &spDef );

    return dt->dtRoot;
    }

/************************************************************************/
/*									*/
/*  Get the tree in the document that a node is contained in.		*/
/*									*/
/************************************************************************/

int docGetRootOfSelectionScope(	DocumentTree **		pTree,
				BufferItem **		pBodySectNode,
				BufferDocument *	bd,
				const SelectionScope *	ss )
    {
    const DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int			fieldCount= dfl->dflPagedList.plItemCount;
    BufferItem *		bodySectNode= (BufferItem *)0;
    DocumentTree *		dt;

    DocumentField *		dfNote;
    DocumentNote *		dn;

    switch( ss->ssTreeType )
	{
	case DOCinBODY:
	    if  ( pBodySectNode )
		{ bodySectNode= docGetBodySectNodeOfScope( ss, bd );	}
	    dt= &(bd->bdBody);
	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    bodySectNode= docGetBodySectNodeOfScope( ss, bd );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}

	    dt= docSectionHeaderFooter( bodySectNode, (unsigned char *)0,
					&(bd->bdProperties), ss->ssTreeType );
	    if  ( ! dt )
		{ LXDEB(ss->ssTreeType,dt); return -1;	}

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    if  ( ss->ssOwnerNumber < 0			||
		  ss->ssOwnerNumber >= fieldCount	)
		{
		LLDEB(ss->ssOwnerNumber,fieldCount);
		return -1;
		}
	    dfNote= docGetFieldByNumber( &(bd->bdFieldList),
							ss->ssOwnerNumber );
	    dn= docGetNoteOfField( dfNote, bd );
	    if  ( ! dn )
		{ XDEB(dn); return -1;	}
	    dt= &(dn->dnDocumentTree);

	    bodySectNode= docGetBodySectNodeOfScope( ss, bd );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    dt= docDocumentNoteSeparator( bd, ss->ssTreeType );
	    if  ( ! dt )
		{ LXDEB(ss->ssTreeType,dt); return -1;	}
	    bodySectNode= (BufferItem *)0;
	    /* HACK */
	    if  ( bd->bdBody.dtRoot->biChildCount > 0 )
		{ bodySectNode= bd->bdBody.dtRoot->biChildren[0];	}
	    break;

	case DOCinSHPTXT:
	    bodySectNode= docGetBodySectNodeOfScope( ss, bd );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}
	    {
	    DrawingShape *	ds;

	    ds= docGetShapeByNumber( &(bd->bdShapeList), ss->ssOwnerNumber );
	    if  ( ! ds )
		{ LPDEB(ss->ssOwnerNumber,ds); return -1;	}
	    dt= &(ds->dsDocumentTree);
	    }
	    break;

	default:
	    LDEB(ss->ssTreeType);
	    return -1;
	}

    if  ( pTree )
	{ *pTree= dt;		}
    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode; }

    return 0;
    }

