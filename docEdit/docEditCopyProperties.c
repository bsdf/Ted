/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<stdlib.h>

#   include	"docEdit.h"
#   include	<docTreeScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docEditSetProperties.h"

#   include	<appDebugon.h>

/************************************************************************/

typedef struct CollectNodes
    {
    int			cnNodeCount;
    BufferItem **	cnNodes;
    } CollectNodes;

static void docInitCollectNodes(	CollectNodes *	cn )
    {
    cn->cnNodeCount= 0;
    cn->cnNodes= (BufferItem **)0;

    return;
    }

static void docCleanCollectNodes(	CollectNodes *	cn )
    {
    if  ( cn->cnNodes )
	{ free( cn->cnNodes );	}

    return;
    }

static int docCopyPropsCollectNode(
				BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				through )
    {
    CollectNodes *	cn= (CollectNodes *)through;

    if  ( cn->cnNodes )
	{ cn->cnNodes[cn->cnNodeCount++]= node;	}
    else{ cn->cnNodeCount++;			}

    return ADVICEtsOK;
    }

static int docCopyPropsCollectNodes(	CollectNodes *			cn,
					BufferDocument *		bd,
					DocumentTree *			tree,
					const DocumentSelection *	ds )
    {
    const int			flags= FLAGtsSCAN_MERGED_CELLS;

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
			docCopyPropsCollectNode, (NodeVisitor)0,
			flags, (void *)cn ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( bd, tree,
			docCopyPropsCollectNode, (NodeVisitor)0,
			flags, (void *)cn ) < 0 )
	    { LDEB(1); return -1;	}
	}

    if  ( cn->cnNodeCount > 0 )
	{
	cn->cnNodes= (BufferItem **)malloc(
				    cn->cnNodeCount* sizeof(BufferItem *) );
	if  ( ! cn->cnNodes )
	    { LXDEB(cn->cnNodeCount,cn->cnNodes); return -1;	}

	cn->cnNodeCount= 0;

	if  ( ds )
	    {
	    if  ( docScanSelection( bd, ds,
			docCopyPropsCollectNode, (NodeVisitor)0,
			flags, (void *)cn ) < 0 )
		{ LDEB(cn->cnNodeCount); return -1;	}
	    }
	else{
	    if  ( docScanTree( bd, tree,
			docCopyPropsCollectNode, (NodeVisitor)0,
			flags, (void *)cn ) < 0 )
		{ LDEB(cn->cnNodeCount); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy the properties of an individual node.				*/
/*									*/
/************************************************************************/

static int docCopyNodeProperties(
				SetProperties *			setProps,
				const DocumentSelection *	dsTo,
				BufferItem *			nodeTo,
				const BufferItem *		nodeFrom )
    {
    switch( nodeTo->biLevel )
	{
	case DOClevPARA:
	    setProps->spGotPara++;

	    if  ( docEditChangeParaProperties( setProps, dsTo, nodeTo,
					(const TextAttribute *)0,
					&(nodeFrom->biParaProperties ) ) )
		{ LDEB(nodeTo->biLevel); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( nodeTo->biParent ) )
		{
		const RowProperties *	rpFrom;
		const CellProperties *	cpFrom;

		setProps->spGotCell++;

		cpFrom= docGetCellProperties( &rpFrom, nodeFrom );
		if  ( ! cpFrom )
		    { XDEB(cpFrom); return -1;	}

		if  ( ! CELL_MERGED( cpFrom ) )
		    {
		    if  ( docEditChangeCellProperties( setProps, dsTo,
							    nodeTo, cpFrom ) )
			{ LDEB(nodeTo->biLevel); return -1;	}
		    }
		}
	    break;
	case DOClevROW:
	    if  ( docIsRowNode( nodeTo ) )
		{
		setProps->spGotRow++;

		if  ( docEditChangeRowProperties( setProps, dsTo, nodeTo,
					    &(nodeFrom->biRowProperties ) ) )
		    { LDEB(nodeTo->biLevel); return -1;	}
		}
	    break;
	case DOClevSECT:
	    if  ( setProps->sp_spSetMask )
		{
		EditOperation *	eo= setProps->spEditOperation;

		if  ( docEditUpdSectProperties( eo, setProps->sp_spDoneMask,
					    nodeTo, setProps->sp_spSetMask,
					    &(nodeFrom->biSectProperties),
					    setProps->spAttributeMap ) )
		    { XDEB(setProps->sp_spSetMask); return -1;	}
		}
	    setProps->spGotSect++;
	    break;
	default:
	     SDEB(docLevelStr(nodeTo->biLevel));
	     return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int docCopySelectionProperties(
			DocumentCopyJob *		dcj,
			const DocumentSelection *	dsTo,
			const DocumentSelection *	dsFrom,

			PropertyMask *			selPpDoneMask,
			const PropertyMask *		ppSetMask,

			PropertyMask *			selCpDoneMask,
			const PropertyMask *		cpSetMask,

			PropertyMask *			selRpDoneMask,
			const PropertyMask *		rpSetMask,

			PropertyMask *			selSpDoneMask,
			const PropertyMask *		spSetMask,

			PropertyMask *			docDpDoneMask,
			const PropertyMask *		dpSetMask )

    {
    int				rval= 0;
    EditOperation *		eo= dcj->dcjEditOperation;
    SetProperties		setProps;

    CollectNodes		cnTo;
    CollectNodes		cnFrom;

    docInitCollectNodes( &cnTo );
    docInitCollectNodes( &cnFrom );

    if  ( ! dsFrom->dsHead.dpNode || ! dsFrom->dsTail.dpNode )
	{
	XXDEB(dsFrom->dsHead.dpNode,dsFrom->dsTail.dpNode);
	rval= -1; goto ready;
	}
    if  ( ! dsTo->dsHead.dpNode || ! dsTo->dsTail.dpNode )
	{
	XXDEB(dsTo->dsHead.dpNode,dsTo->dsTail.dpNode);
	rval= -1; goto ready;
	}

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
    setProps.spAttributeMap= &(dcj->dcjAttributeMap);

    setProps.spGotPara= 0;
    setProps.spGotCell= 0;
    setProps.spGotRow= 0;
    setProps.spGotSect= 0;

    setProps.sp_taDoneMask= (PropertyMask *)0;
    setProps.sp_taSetMask= (const PropertyMask *)0;
    setProps.sp_taSet= (const TextAttribute *)0;

    setProps.sp_ppDoneMask= selPpDoneMask;
    setProps.sp_ppSetMask= ppSetMask;
    setProps.sp_ppSet= (const ParagraphProperties *)0;

    setProps.sp_cpDoneMask= selCpDoneMask;
    setProps.sp_cpSetMask= cpSetMask;
    setProps.sp_cpSet= (const CellProperties *)0;

    setProps.sp_rpDoneMask= selRpDoneMask;
    setProps.sp_rpSetMask= rpSetMask;
    setProps.sp_rpSet= (const RowProperties *)0;

    setProps.sp_spDoneMask= selSpDoneMask;
    setProps.sp_spSetMask= spSetMask;
    setProps.sp_spSet= (const SectionProperties *)0;

    if  ( cpSetMask || rpSetMask )
	{
	if  ( docGetTableRectangle( &(setProps.spTableRectangle), dsTo ) )
	    { XXDEB(cpSetMask,rpSetMask); rval= -1; goto ready;	}
	}
    else{
	docInitTableRectangle( &(setProps.spTableRectangle) );
	}

    if  ( dpSetMask							&&
	  docChangeDocumentProperties( eo, docDpDoneMask,
			    dpSetMask, &(dcj->dcjSourceDocument->bdProperties),
			    setProps.spAttributeMap )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ppSetMask || cpSetMask || rpSetMask || spSetMask )
	{
	int		n;

	if  ( docCopyPropsCollectNodes( &cnFrom, dcj->dcjSourceDocument,
						dcj->dcjSourceTree, dsFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docCopyPropsCollectNodes( &cnTo, eo->eoDocument,
							eo->eoTree, dsTo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( cnFrom.cnNodeCount != cnTo.cnNodeCount )
	    {
	    LLDEB(cnFrom.cnNodeCount,cnTo.cnNodeCount);
	    rval= -1; goto ready;
	    }

	for ( n= 0; n < cnTo.cnNodeCount; n++ )
	    {
	    if  ( cnTo.cnNodes[n]->biLevel != cnFrom.cnNodes[n]->biLevel )
		{
		SDEB(docLevelStr(cnTo.cnNodes[n]->biLevel));
		SDEB(docLevelStr(cnFrom.cnNodes[n]->biLevel));
		rval= -1; goto ready;
		}

	    if  ( docCopyNodeProperties( &setProps, dsTo,
					cnTo.cnNodes[n], cnFrom.cnNodes[n] ) )
		{ LDEB(n); rval= -1; goto ready;	}
	    }
	}

    if  ( cpSetMask && ! setProps.spGotCell )
	{
	BufferItem *	cellNodeTo= docGetCellNode( dsTo->dsHead.dpNode );
	BufferItem *	cellNodeFrom= docGetCellNode( dsFrom->dsHead.dpNode );

	if  ( ! cellNodeTo || ! cellNodeFrom )
	    { XXDEB(cellNodeTo,cellNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodeProperties( &setProps, dsTo,
						cellNodeTo, cellNodeFrom ) )
	    { XDEB(cpSetMask); rval= -1; goto ready;	}
	}

    if  ( rpSetMask && ! setProps.spGotRow )
	{
	BufferItem *	rowNodeTo= docGetRowNode( dsTo->dsHead.dpNode );
	BufferItem *	rowNodeFrom= docGetRowNode( dsFrom->dsHead.dpNode );

	if  ( ! rowNodeTo || ! rowNodeFrom )
	    { XXDEB(rowNodeTo,rowNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodeProperties( &setProps, dsTo,
						rowNodeTo, rowNodeFrom ) )
	    { XDEB(rpSetMask); rval= -1; goto ready;	}
	}

    if  ( spSetMask && ! setProps.spGotSect )
	{
	BufferItem *	sectNodeTo= docGetSectNode( dsTo->dsHead.dpNode );
	BufferItem *	sectNodeFrom= docGetSectNode( dsFrom->dsHead.dpNode );

	if  ( ! sectNodeTo || ! sectNodeFrom )
	    { XXDEB(sectNodeTo,sectNodeFrom); rval= -1; goto ready;	}

	if  ( docCopyNodeProperties( &setProps, dsTo,
						sectNodeTo, sectNodeFrom ) )
	    { XDEB(spSetMask); rval= -1; goto ready;	}
	}

  ready:

    docCleanCollectNodes( &cnTo );
    docCleanCollectNodes( &cnFrom );

    return rval;
    }

