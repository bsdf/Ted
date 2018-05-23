/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTreeScanner.h>
#   include	<docTreeType.h>
#   include	"docEditSetProperties.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the properties of a node that we encounter on the way.	*/
/*									*/
/************************************************************************/

static int docSetPropsEnterNode( BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				through )
    {
    SetProperties *	setProps= (SetProperties *)through;
    EditOperation *	eo= setProps->spEditOperation;


    switch( node->biLevel )
	{
	case DOClevPARA:
	    setProps->spGotPara++;

	    if  ( docEditChangeParaProperties( setProps, ds, node,
				    setProps->sp_taSet, setProps->sp_ppSet ) )
		{ LDEB(node->biLevel); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{
		setProps->spGotCell++;

		if  ( docEditChangeCellProperties( setProps, ds, node,
							setProps->sp_cpSet ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		setProps->spGotRow++;

		if  ( docEditChangeRowProperties( setProps, ds, node,
							setProps->sp_rpSet ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevSECT:
	    setProps->spGotSect++;

	    if  ( setProps->sp_spSetMask && node->biTreeType == DOCinBODY )
		{
		if  ( docEditUpdSectProperties( eo, setProps->sp_spDoneMask,
				      node, setProps->sp_spSetMask,
				      setProps->sp_spSet,
				      setProps->spAttributeMap ) )
		    { XDEB(setProps->sp_spSetMask); return -1;	}
		}
	    break;

	case DOClevBODY:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return ADVICEtsOK;
    }

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int docChangeSelectionProperties(
			EditOperation *			eo,
			const DocumentSelection *	ds,

			PropertyMask *			selTaDoneMask,
			const PropertyMask *		taSetMask,
			const TextAttribute *		taSet,

			PropertyMask *			selPpDoneMask,
			const PropertyMask *		ppSetMask,
			const ParagraphProperties *	ppSet,

			PropertyMask *			selCpDoneMask,
			const PropertyMask *		cpSetMask,
			const CellProperties *		cpSet,

			PropertyMask *			selRpDoneMask,
			const PropertyMask *		rpSetMask,
			const RowProperties *		rpSet,

			PropertyMask *			selSpDoneMask,
			const PropertyMask *		spSetMask,
			const SectionProperties *	spSet,

			PropertyMask *			docDpDoneMask,
			const PropertyMask *		dpSetMask,
			const DocumentProperties *	dpSet )

    {
    const int			flags= 0; /* NOT FLAGtsSCAN_MERGED_CELLS */
    SetProperties		setProps;

    if  ( ds && docIsIBarSelection( ds ) )
	{ taSetMask= (const PropertyMask *)0;	}

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    setProps.spEditOperation= eo;
    setProps.spRedoParaLayout= 1;
    setProps.spAttributeMap= (const DocumentAttributeMap *)0;

    setProps.spGotPara= 0;
    setProps.spGotCell= 0;
    setProps.spGotRow= 0;
    setProps.spGotSect= 0;

    setProps.sp_taDoneMask= selTaDoneMask;
    setProps.sp_taSetMask= taSetMask;
    setProps.sp_taSet= taSet;

    setProps.sp_ppDoneMask= selPpDoneMask;
    setProps.sp_ppSetMask= ppSetMask;
    setProps.sp_ppSet= ppSet;

    setProps.sp_cpDoneMask= selCpDoneMask;
    setProps.sp_cpSetMask= cpSetMask;
    setProps.sp_cpSet= cpSet;

    setProps.sp_rpDoneMask= selRpDoneMask;
    setProps.sp_rpSetMask= rpSetMask;
    setProps.sp_rpSet= rpSet;

    setProps.sp_spDoneMask= selSpDoneMask;
    setProps.sp_spSetMask= spSetMask;
    setProps.sp_spSet= spSet;

    if  ( cpSetMask || rpSetMask )
	{
	if  ( docGetTableRectangle( &(setProps.spTableRectangle), ds ) )
	    { XXDEB(cpSetMask,rpSetMask); return -1;	}
	}
    else{
	docInitTableRectangle( &(setProps.spTableRectangle) );
	}

    if  ( dpSetMask							&&
	  docChangeDocumentProperties( eo, docDpDoneMask,
			    dpSetMask, dpSet, setProps.spAttributeMap )	)
	{ LDEB(1); return -1;	}

    if  ( ! ds )
	{
	if  ( docScanTree( eo->eoDocument, eo->eoTree,
				    docSetPropsEnterNode, (NodeVisitor)0,
				    flags, (void *)&setProps ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( ( ! ds->dsHead.dpNode || ! ds->dsTail.dpNode ) )
	    { XXDEB(ds->dsHead.dpNode,ds->dsTail.dpNode); return -1; }

	if  ( docSelectionInsideParagraph( ds ) &&
	      ! ppSetMask			&&
	      ! cpSetMask			&&
	      ! rpSetMask			&&
	      ! spSetMask			&&
	      ! dpSetMask			)
	    {
	    const int	stroffShift= 0;

	    setProps.spRedoParaLayout= 0;

	    docSetParagraphAdjust( eo, ds->dsHead.dpNode,
					    stroffShift, ds->dsTail.dpStroff );
	    }

	if  ( docScanSelection( eo->eoDocument, ds,
				    docSetPropsEnterNode, (NodeVisitor)0,
				    flags, (void *)&setProps ) < 0 )
	    { LDEB(1); return -1;	}

	if  ( cpSetMask && ! setProps.spGotCell )
	    {
	    BufferItem *	cellNode= docGetCellNode( ds->dsHead.dpNode );

	    if  ( ! cellNode )
		{ XDEB(cellNode); return -1;	}

	    if  ( docSetPropsEnterNode( cellNode, ds,
				eo->eoBodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(cpSetMask); return -1;	}
	    }

	if  ( rpSetMask && ! setProps.spGotRow )
	    {
	    BufferItem *	rowNode= docGetRowNode( ds->dsHead.dpNode );

	    if  ( ! rowNode )
		{ XDEB(rowNode); return -1;	}

	    if  ( docSetPropsEnterNode( rowNode, ds,
				eo->eoBodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(rpSetMask); return -1;	}
	    }

	if  ( spSetMask && ! setProps.spGotSect )
	    {
	    BufferItem *	sectNode= docGetSectNode( ds->dsHead.dpNode );

	    if  ( ! sectNode )
		{ XDEB(sectNode); return -1;	}

	    if  ( docSetPropsEnterNode( sectNode, ds,
				eo->eoBodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(spSetMask); return -1;	}
	    }

	if  ( spSetMask && ds->dsHead.dpNode->biTreeType != DOCinBODY )
	    {
	    BufferItem *	bodySectNode;

	    bodySectNode= docGetBodySectNode( ds->dsHead.dpNode,
							    eo->eoDocument );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}

	    if  ( docSetPropsEnterNode( bodySectNode, ds,
				    bodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(spSetMask); return -1;	}
	    }
	}

    return 0;
    }

