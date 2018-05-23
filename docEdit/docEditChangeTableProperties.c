/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docEditSetProperties.h"
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle changes to the cell merge fields.				*/
/*									*/
/*  There is no need to also manage the flags in the cellNode itself:	*/
/*  The incremental formatter takes care of reformatting the correct	*/
/*  rows to make sure that the flags are recalculated.			*/
/*									*/
/************************************************************************/

static int docChangeCellMerge(		SetProperties *		setProps,
					BufferItem *		cellNode,
					const PropertyMask *	cpDoneMask,
					const CellProperties *	cpSet )
    {
    BufferItem *		rowNode= cellNode->biParent;
    const BufferItem *		parentNode= rowNode->biParent;
    const TableRectangle *	tr= &(setProps->spTableRectangle);

    int				h0= CELLmergeNONE;
    int				h1= CELLmergeNONE;
    int				v0= CELLmergeNONE;
    int				v1= CELLmergeNONE;

    int				col0= tr->trCol0;
    int				col1= tr->trCol1;
    int				row0= tr->trRow0;
    int				row1= tr->trRow1;

    int				v;
    int				row;

    if  ( PROPmaskISSET( cpDoneMask, CLpropHOR_MERGE ) )
	{
	switch( cpSet->cpHorizontalMerge )
	    {
	    case CELLmergeHEAD:
		if  ( col0 == col1 )
		    { LLDEB(col0,col1);	}
		else{
		    h0= CELLmergeHEAD;
		    h1= CELLmergeFOLLOW;
		    }
		break;
	    case CELLmergeNONE:
		col1= col0+ tr->trCellColspan- 1;
		break;
	    default:
		LDEB(cpSet->cpHorizontalMerge);
		return -1;
	    }
	}

    if  ( PROPmaskISSET( cpDoneMask, CLpropVERT_MERGE ) )
	{
	switch( cpSet->cpVerticalMerge )
	    {
	    case CELLmergeHEAD:
		if  ( row0 == row1 )
		    { LLDEB(row0,row1);	}
		else{
		    v0= CELLmergeHEAD;
		    v1= CELLmergeFOLLOW;
		    }
		break;
	    case CELLmergeNONE:
		row1= row0+ tr->trCellRowspan- 1;
		break;
	    default:
		LDEB(cpSet->cpVerticalMerge);
		return -1;
	    }

	}

    v= v0;

    for ( row= row0; row <= row1; row++ )
	{
	BufferItem *		thisRowNode= parentNode->biChildren[row];
	RowProperties *		rp= &(thisRowNode->biRowProperties);
	CellProperties *	cp= rp->rpCells+ col0;

	int			h= h0;
	int			col;

	for ( col= col0; col <= col1; cp++, col++ )
	    {
	    cp->cpHorizontalMerge= h;
	    cp->cpVerticalMerge= v;

	    h= h1;
	    }

	if  ( col < rp->rpCellCount )
	    {
	    if  ( col+ 1 < rp->rpCellCount			&&
		  cp[1].cpHorizontalMerge == CELLmergeFOLLOW	)
		{ cp->cpHorizontalMerge= CELLmergeHEAD;	}
	    else{ cp->cpHorizontalMerge= CELLmergeNONE;	}
	    }

	v= v1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the properties of a cell that we encounter while scanning a	*/
/*  selection to perform a property change.				*/
/*									*/
/************************************************************************/

int docEditChangeCellProperties( SetProperties *		setProps,
				const DocumentSelection *	ds,
				BufferItem *			cellNode,
				const CellProperties *		cpSet )
    {
    BufferItem *	rowNode= cellNode->biParent;
    RowProperties *	rp= &(rowNode->biRowProperties);
    int			col= cellNode->biNumberInParent;
    CellProperties *	cp= &(rp->rpCells[col]);

    if  ( setProps->sp_cpSetMask )
	{
	PropertyMask	cpDoneMask;

	utilPropMaskClear( &cpDoneMask );

	if  ( docUpdCellProperties( &cpDoneMask, cp, 
					    setProps->sp_cpSetMask, cpSet,
					    setProps->spAttributeMap ) )
	    { LDEB(1); return -1;	}

	if  ( ! utilPropMaskIsEmpty( &cpDoneMask ) )
	    {
	    if  ( PROPmaskISSET( &cpDoneMask, CLpropHOR_MERGE )		||
		  PROPmaskISSET( &cpDoneMask, CLpropVERT_MERGE )	)
		{
		if  ( docChangeCellMerge( setProps, cellNode,
						    &cpDoneMask, cpSet ) )
		    { LDEB(1);	}
		}

	    if  ( PROPmaskISSET( &cpDoneMask, CLpropTOP_BORDER ) )
		{
		}

	    if  ( PROPmaskISSET( &cpDoneMask, CLpropLEFT_BORDER )	&&
		  col > 0						)
		{
		rp->rpCells[col-1].cpRightBorderNumber= 
					rp->rpCells[col].cpLeftBorderNumber;
		}

	    if  ( PROPmaskISSET( &cpDoneMask, CLpropRIGHT_BORDER )	&&
		  col < rp->rpCellCount- 1				)
		{
		rp->rpCells[col+1].cpLeftBorderNumber= 
					rp->rpCells[col].cpRightBorderNumber;
		}

	    if  ( PROPmaskISSET( &cpDoneMask, CLpropBOTTOM_BORDER ) )
		{
		}

	    docEditIncludeNodeInReformatRange( setProps->spEditOperation,
								    rowNode );

	    if  ( setProps->sp_cpDoneMask )
		{
		utilPropMaskOr( setProps->sp_cpDoneMask,
				    setProps->sp_cpDoneMask, &cpDoneMask );
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the properties of a row that we encounter on the way.	*/
/*									*/
/************************************************************************/

int docEditChangeRowProperties(	SetProperties *			setProps,
				const DocumentSelection *	ds,
				BufferItem *			rowNode,
				const RowProperties *		rpSet )
    {
    RowProperties *	rp= &(rowNode->biRowProperties);

    if  ( setProps->sp_rpSetMask )
	{
	PropertyMask	rpSetMask= *setProps->sp_rpSetMask;
	PropertyMask	rpDoneMask;

	DocumentPosition	dp;

	/* prevent crashes */
	if  ( rpSet->rpCellCount != rowNode->biChildCount )
	    { PROPmaskUNSET( &rpSetMask, RPpropCELL_LAYOUT );	}

	utilPropMaskClear( &rpDoneMask );

	if  ( docUpdRowProperties( &rpDoneMask, rp, &rpSetMask, rpSet,
						setProps->spAttributeMap ) )
	    { LDEB(1); return -1;	}

	if  ( ! docHeadPosition( &dp, rowNode ) )
	    {
	    BufferItem *	paraNode= dp.dpNode;

	    if  ( PROPmaskISSET( &rpSetMask, RPprop_KEEPFOLLOW ) )
		{ paraNode->biParaKeepWithNext= rp->rp_Keepfollow;	}
	    }

	if  ( ! utilPropMaskIsEmpty( &rpDoneMask ) )
	    {
	    docEditIncludeNodeInReformatRange( setProps->spEditOperation,
								    rowNode );

	    if  ( setProps->sp_rpDoneMask )
		{
		utilPropMaskOr( setProps->sp_rpDoneMask,
				    setProps->sp_rpDoneMask, &rpDoneMask );
		}
	    }
	}

    return 0;
    }

