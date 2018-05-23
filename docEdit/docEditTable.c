/************************************************************************/
/*									*/
/*  Table editing.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<limits.h>

#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	"docEditImpl.h"
#   include	<docTextParticule.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Step in insert row actions: Copy attributes.			*/
/*									*/
/*  NOTE: Copying the text attribute numbers means that the rows MUST	*/
/*	be in the same document. The same applies for the color numbers	*/
/*	in the paragraph properties.					*/
/*									*/
/************************************************************************/

static int docCopyRowColumnAttributes(	BufferItem *		rowBi,
					const BufferItem *	refRowBi )
    {
    int		colCount;
    int		col;

    if  ( rowBi->biChildCount < refRowBi->biChildCount )
	{ colCount= rowBi->biChildCount;	}
    else{ colCount= refRowBi->biChildCount;	}

    for ( col= 0; col < colCount; col++ )
	{
	DocumentPosition	dp;
	DocumentPosition	dpRef;

	TextParticule *		tp;
	const TextParticule *	tpRef;

	PropertyMask		ppSetMask;

	if  ( docHeadPosition( &dp, rowBi->biChildren[col] ) )
	    { continue;	}
	if  ( docHeadPosition( &dpRef, refRowBi->biChildren[col] ) )
	    { continue;	}

	utilPropMaskClear( &ppSetMask );
	utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

	PROPmaskUNSET( &ppSetMask, PPpropLISTOVERRIDE );
	PROPmaskUNSET( &ppSetMask, PPpropOUTLINELEVEL );
	PROPmaskUNSET( &ppSetMask, PPpropLISTLEVEL );

	if  ( docUpdParaProperties( (PropertyMask *)0,
				&(dp.dpNode->biParaProperties),
				&ppSetMask, &(dpRef.dpNode->biParaProperties),
				(const DocumentAttributeMap *)0 ) )
	    { LDEB(1); return -1;	}

	tp= dp.dpNode->biParaParticules;
	tpRef= dpRef.dpNode->biParaParticules;

	tp->tpTextAttrNr= tpRef->tpTextAttrNr;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a series of rows in a table.					*/
/*									*/
/************************************************************************/

int docInsertTableRows(		DocumentSelection *	dsRows,
				EditOperation *		eo,
				BufferItem *		parentBi,
				const BufferItem *	refRowBi,
				const RowProperties *	rp,
				int			textAttributeNumber,
				int			n,
				int			paraNr,
				int			rows )
    {
    int			row;
    int			row0= n;

    const int		recursively= 0;

    for ( row= 0; row < rows; n++, row++ )
	{
	BufferItem *		rowBi= (BufferItem *)0;

	const int		sectShift= 0;
	const int		stroffShift= 0;
	const int		paraShift= rp->rpCellCount;

	rowBi= docInsertRowNode( eo->eoDocument, parentBi, n, rp,
						    textAttributeNumber );
	if  ( ! rowBi )
	    { XDEB(rowBi); return -1;	}

	if  ( paraNr == 1 )
	    {
	    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, 0,
				    sectShift, paraShift, stroffShift );
	    }
	else{
	    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr- 1, INT_MAX,
				    sectShift, paraShift, stroffShift );
	    }

	paraNr += rp->rpCellCount;

	if  ( refRowBi						&&
	      docCopyRowColumnAttributes( rowBi, refRowBi )	)
	    { LDEB(1); return -1;	}

	docEditIncludeNodeInReformatRange( eo, rowBi );
	}

    docDelimitTables( parentBi, recursively );

    {
    const int		direction= 1;

    DocumentSelection	dsNew;
    EditRange		erRows;

    if  ( docHeadPosition( &(dsNew.dsHead), parentBi->biChildren[row0] ) )
	{ LDEB(row0);	}
    if  ( docTailPosition( &(dsNew.dsTail), parentBi->biChildren[n-1] ) )
	{ LDEB(n-1);	}

    docSetRangeSelection( dsRows, &(dsNew.dsHead), &(dsNew.dsTail), direction );

    docSetEditRange( &erRows, &dsNew );
    docUnionEditRanges( &(eo->eoAffectedRange),
					&(eo->eoAffectedRange), &erRows );
    }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

static BufferItem * docInsertCellInRow(
				BufferDocument *	bd,
				BufferItem *		rowBi,
				int			col,
				const CellProperties *	cp,
				int			textAttributeNumber )
    {
    RowProperties *	rp;

    BufferItem *	cellBi;
    BufferItem *	paraBi;

    const int		shiftTail= 0; /* Do not shift others */

    rp= &(rowBi->biRowProperties);

    if  ( docInsertRowColumn( rp, col, shiftTail,
				    cp, (const DocumentAttributeMap *)0 ) )
	{ LDEB(col); return (BufferItem *)0;	}

    cellBi= docInsertNode( bd, rowBi, col, DOClevCELL );
    if  ( ! cellBi )
	{ LXDEB(col,cellBi); return (BufferItem *)0;	}

    paraBi= docInsertEmptyParagraph( bd, cellBi, textAttributeNumber );
    if  ( ! paraBi )
	{ XDEB(paraBi); return (BufferItem *)0;	}

    docSetParaTableNesting( paraBi );

    return paraBi;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

int docSplitColumnInRows(	BufferItem **		pNewParaBi,
				EditOperation *		eo,
				BufferItem *		parentBi,
				int			row0,
				int			row,
				int			row1,
				int			col,
				int			after )
    {
    BufferDocument *		bd= eo->eoDocument;

    BufferItem *		newParaBi= (BufferItem *)0;

    BufferItem *		rowBi;

    const int			recursively= 0;

    if  ( after )
	{ after= 1;	}

    while( row0 <= row1 )
	{
	RowProperties *		rp;

	CellProperties *	oldCp;
	CellProperties		newCp;

	int			left;
	int			middle;

	BufferItem *		cellBi;
	BufferItem *		paraBi;

	int			textAttributeNumber;

	rowBi= parentBi->biChildren[row0];
	rp= &(rowBi->biRowProperties);

	oldCp= rp->rpCells+ col;

	left= docColumnLeft( rp, col );
	middle= ( rp->rpCells[col].cpRightBoundaryTwips+ left )/ 2;

	newCp= *oldCp;
	if  ( after )
	    { oldCp->cpRightBoundaryTwips= middle;	}
	else{ newCp. cpRightBoundaryTwips= middle;	}

	cellBi= rowBi->biChildren[col];
	paraBi= cellBi->biChildren[0];
	textAttributeNumber= paraBi->biParaParticules[0].tpTextAttrNr;

	paraBi= docInsertCellInRow( bd, rowBi, col+ after, &newCp,
							textAttributeNumber );
	if  ( row0 == row )
	    { newParaBi= paraBi;	}

	{
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	sectShift= 0;

	const int	paraNr= docNumberOfParagraph( paraBi );
	const int	paraShift= 1;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
					paraNr, stroffFrom,
					sectShift, paraShift, stroffShift );
	}

	docEditIncludeNodeInReformatRange( eo, rowBi );

	row0++;
	}

    docDelimitTables( parentBi, recursively );

    *pNewParaBi= newParaBi;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete columns from a table.					*/
/*									*/
/************************************************************************/

int docDeleteColumnsFromRows(	EditOperation *	eo,
				BufferItem *	parentBi,
				int		delRow0,
				int		delRow1,
				int		delCol0,
				int		delCol1 )
    {
    int				rval= 0;
    int				count= delCol1- delCol0+ 1;
    int				row;

    const int			recursively= 0;

    for ( row= delRow0; row <= delRow1; row++ )
	{
	BufferItem *		rowBi= parentBi->biChildren[row];
	DocumentSelection	dsCols;

	docInitDocumentSelection( &dsCols );
	if  ( docHeadPosition( &dsCols.dsHead, rowBi->biChildren[delCol0] ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docTailPosition( &dsCols.dsTail, rowBi->biChildren[delCol1] ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docEditDeleteFieldsForBlockDelete( eo, &dsCols ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    for ( row= delRow0; row <= delRow1; row++ )
	{
	BufferItem *	rowBi= parentBi->biChildren[row];

	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	docEditDeleteNodes( eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					rowBi, delCol0, count );

	docEditIncludeNodeInReformatRange( eo, rowBi );
	}

    docDelimitTables( parentBi, recursively );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Flatten a row in a table.						*/
/*									*/
/************************************************************************/

static void docRefreshTableNesting(	BufferItem *	node )
    {
    if  ( docIsParaNode( node ) )
	{ docSetParaTableNesting( node );	}
    else{
	int		ch;

	for ( ch= 0; ch < node->biChildCount; ch++ )
	    { docRefreshTableNesting( node->biChildren[ch] );	}
	}
    }

void docFlattenRow(		BufferItem *		node )
    {
    /* nearest row. NOT a real one! */
    node= docGetRowLevelNode( node );

    if  ( ! node )
	{ XDEB(node);	}
    else{
	docCleanRowProperties( &(node->biRowProperties) );
	docInitRowProperties( &(node->biRowProperties) );

	docRefreshTableNesting( node );
	}
    }

/************************************************************************/
/*									*/
/*  Build a table to be inserted in another document.			*/
/*									*/
/************************************************************************/

int docFillTableDocument(	BufferDocument *	bdTable,
				BufferItem *		parentNode,
				int			textAttrNr,
				int			wide,
				int			rows,
				int			columns )
    {
    int				rval= 0;
    int				row;

    TextAttribute		ta;

    RowProperties		rp;

    docInitRowProperties( &rp );

    docGetTextAttributeByNumber( &ta, bdTable, textAttrNr );

    if  ( docEqualWidthColumns( &rp, columns, wide, ta.taFontSizeHalfPoints ) )
	{ LDEB(columns); rval= -1; goto ready;	}

    for ( row= 0; row < rows; row++ )
	{
	BufferItem *		rowNode= (BufferItem *)0;

	rowNode= docInsertRowNode( bdTable, parentNode, row, &rp,
							    textAttrNr );
	if  ( ! rowNode )
	    { XDEB(rowNode); rval= -1; goto ready;	}
	}

  ready:

    docCleanRowProperties( &rp );

    return rval;
    }

