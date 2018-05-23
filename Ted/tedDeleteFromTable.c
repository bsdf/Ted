/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedSelect.h"
#   include	"tedDocFront.h"
#   include	<docRtfTrace.h>
#   include	<docEditImpl.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Delete the current row.						*/
/*									*/
/************************************************************************/

static int tedEditDeleteSelectedRows(
				DocumentPosition *		dpNew,
				int *				pSide,
				const DocumentSelection *	dsRows,
				BufferItem *			rowNode0,
				BufferItem *			rowNode1,
				TedEditOperation *		teo )
    {
    int				rval= 0;
    EditOperation *		eo= &(teo->teoEo);

    BufferItem *		parentNode;
    int				child0;
    int				child1;

    int				side;

    if  ( docEditDeleteFieldsForBlockDelete( eo, dsRows ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditFindPositionOutsideBlockDelete( &side, dpNew, dsRows ) )
	{ LDEB(1); rval= -1; goto ready;	}

    parentNode= rowNode0->biParent;
    child0= rowNode0->biNumberInParent;
    child1= rowNode1->biNumberInParent;

    if  ( child0 <= child1 )
	{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	tedEditIncludeNodeInRedraw( teo, rowNode0 );

	docEditDeleteNodes( eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentNode, child0, child1- child0+ 1 );

	docEditIncludeNodeInReformatRange( eo, parentNode );
	eo->eoHeadDp= *dpNew;
	eo->eoTailDp= *dpNew;
	}

    *pSide= side;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete rows from a table.						*/
/*									*/
/************************************************************************/

static int tedDeleteRowsFromTableOperation(
					TedEditOperation *	teo,
					int			command,
					int			row0,
					int			row1 )
    {
    EditOperation *		eo= &(teo->teoEo);

    int				rval= 0;

    BufferItem *		parentNode;

    int				col;
    int				row;
    int				tabRow0;
    int				tabRow1;

    DocumentPosition		dpNew;

    int				allChildren= 0;

    BufferItem *		rowNode0;
    BufferItem *		rowNode1;

    DocumentSelection		dsTraced;

    docInitDocumentPosition( &dpNew );

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
					    &col, &tabRow0, &row, &tabRow1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( row0 > row1 )
	{ LLDEB(row0,row1); rval= -1; goto ready;	}
    if  ( row0 < tabRow0 || row0 > tabRow1 )
	{ LLLDEB(tabRow0,row0,tabRow1); rval= -1; goto ready;	}
    if  ( row1 < tabRow0 || row1 > tabRow1 )
	{ LLLDEB(tabRow0,row1,tabRow1); rval= -1; goto ready;	}

    rowNode0= parentNode->biChildren[row0];
    rowNode1= parentNode->biChildren[row1];

    if  ( rowNode0->biNumberInParent == 0 )
	{
	BufferItem *	parentNode= rowNode0->biParent;

	if  ( rowNode1->biNumberInParent == parentNode->biChildCount -1 )
	    { allChildren= 1;	}
	}

    if  ( allChildren )
	{
	if  ( tedEditStartReplace( &dsTraced, teo, command, DOClevSECT, 0 ) )
	    { LDEB(1); goto ready;	}

	/*  Fraudulous HACK */
	if  ( docHeadPosition( &(eo->eoHeadDp), rowNode0 ) )
	    { LDEB(1); goto ready;	}
	eo->eoHeadDp.dpStroff= 0;
	if  ( docTailPosition( &(eo->eoTailDp), rowNode1 ) )
	    { LDEB(1); goto ready;	}
	eo->eoLastDp= eo->eoTailDp;

	if  ( tedDeleteSelectionImpl( teo ) )
	    { LDEB(allChildren); goto ready;	}

	docFlattenRow( eo->eoTailDp.dpNode );
	}
    else{
	int			side;
	const int		direction= 1;
	const int		recursively= 0;

	DocumentSelection	dsRows;
	DocumentPosition	dpHead;
	DocumentPosition	dpTail;

	docInitDocumentSelection( &dsRows );
	if  ( docHeadPosition( &dpHead, rowNode0 ) )
	    { LDEB(0); return -1;	}
	dpHead.dpStroff= 0;

	if  ( docTailPosition( &dpTail, rowNode1 ) )
	    { LDEB(1); return -1;	}
	docSetRangeSelection( &dsRows, &dpHead, &dpTail, direction );

	if  ( tedEditStartStep( teo, command ) )
	    { LDEB(1); return -1;	}

	if  ( teo->teoEditTrace )
	    {
	    if  ( docRtfTraceOldContentsLow( eo, &dsRows, 0 ) )
		{ LDEB(1); goto ready;	}
	    }

	if  ( tedEditDeleteSelectedRows( &dpNew, &side, &dsRows,
						rowNode0, rowNode1, teo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docDelimitTables( parentNode, recursively );

	docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
	docSetIBarRange( &(eo->eoSelectedRange), &dpNew );
	tedEditFinishSelectionTail( teo );

	if  ( teo->teoEditTrace )
	    {
	    const SelectionScope * const s0= (const SelectionScope *)0;

	    if  ( side > 0 )
		{ docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	    else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	    }

	tedFinishEditOperation( teo );
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete rows from a table.						*/
/*									*/
/************************************************************************/

static int tedDeleteRowsFromTable(	EditDocument *		ed,
					int			command,
					int			row0,
					int			row1,
					int			traced )
    {
    int				rval= 0;
    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedDeleteRowsFromTableOperation( &teo, command, row0, row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete a table.							*/
/*									*/
/************************************************************************/

int tedDocDeleteTable(	EditDocument *		ed,
			int			traced )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed  ) )
	{ LDEB(1); return -1;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return -1;	}

    if  ( tedDeleteRowsFromTable( ed, EDITcmdDELETE_TABLE,
					    tr.trRow00, tr.trRow11, traced ) )
	{ LLDEB(tr.trRow00,tr.trRow11); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete a row.							*/
/*									*/
/************************************************************************/

int tedDocDeleteRow(	EditDocument *		ed,
			int			traced )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed  ) )
	{ LDEB(1); return -1;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return -1;	}

    if  ( tedDeleteRowsFromTable( ed, EDITcmdDELETE_ROW,
					    tr.trRow0, tr.trRow1, traced ) )
	{ LLDEB(tr.trRow00,tr.trRow11); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete columns: Implementation.					*/
/*									*/
/************************************************************************/

static int tedDeleteColumnsFromRowsOperation(
					TedEditOperation *	teo,
					int			delRow0,
					int			delRow1,
					int			delCol0,
					int			delCol1 )
    {
    EditOperation *		eo= &(teo->teoEo);

    BufferItem *		parentNode;
    BufferItem *		rowBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    int				side= 1;

    DocumentPosition		dpNew;
    DocumentSelection		dsTraced;

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
						&col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    if  ( tedEditStartReplace( &dsTraced, teo,
				    EDITcmdDELETE_COLUMN, DOClevCOLUMN, 0 ) )
	{ LDEB(1); goto ready;	}

    tedEditIncludeRowsInRedraw( teo, parentNode, delRow0, delRow1 );

    docInitDocumentPosition( &dpNew );
    rowBi= parentNode->biChildren[delRow1];
    if  ( delCol1 >= rowBi->biChildCount- 1			||
	  docTailPosition( &dpNew, rowBi->biChildren[delCol1] )	||
	  docNextPosition( &dpNew )				)
	{
	docInitDocumentPosition( &dpNew );
	side= -1;
	rowBi= parentNode->biChildren[delRow0];
	if  ( delCol0 <= 0						||
	      docHeadPosition( &dpNew, rowBi->biChildren[delCol0] )	||
	      docPrevPosition( &dpNew )					)
	    { docInitDocumentPosition( &dpNew ); }
	}

    if  ( docDeleteColumnsFromRows( eo, parentNode,
				    delRow0, delRow1, delCol0, delCol1 ) )
	{ LDEB(1); return -1;	}

    eo->eoHeadDp= dpNew;
    eo->eoTailDp= dpNew;

    docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
    tedEditFinishSelectionTail( teo );

    if  ( teo->teoEditTrace )
	{
	const SelectionScope * const s0= (const SelectionScope *)0;

	if  ( side > 0 )
	    { docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	}

    tedFinishEditOperation( teo );

  ready:

    return 0;
    }


/************************************************************************/
/*									*/
/*  Delete a column.							*/
/*									*/
/************************************************************************/

int tedDocDeleteColumn(	EditDocument *		ed,
			int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (BufferItem **)0, ed  ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedDeleteColumnsFromRowsOperation( &teo,
			    tr.trRow00, tr.trRow11, tr.trCol0, tr.trCol1 ) )
	{ LLDEB(tr.trRow00,tr.trRow11); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete a slice from a table. Either horizontal or vertical or both:	*/
/*  the table as a whole.						*/
/*									*/
/*  1)  See whether the current selection is a table slice.		*/
/*  2)  If it is a row slice (Whole rows) delete the rows. This covers	*/
/*	deleteing the whole table as well. [ Handling it in the column	*/
/*	branch would leave empty rows.]					*/
/*  3)  If it is a column slice (Whole columns) delete the columns.	*/
/*  4)  Impossible!							*/
/*									*/
/************************************************************************/

int tedDeleteTableSliceSelection(	EditDocument *		ed,
					int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				ret;

    const int			fullWidth= 1;

    if  ( ed->edIsReadonly )
	{ return 1;	}

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    /*  1  */
    if  ( ! sd.sdInOneTable || ! sd.sdTableRectangle.trIsTableSlice )
	{ rval= 1; goto ready;	}

    /*  2  */
    if  ( sd.sdTableRectangle.trIsRowSlice )
	{
	ret= tedDeleteRowsFromTableOperation( &teo, EDITcmdDELETE_ROW,
						sd.sdTableRectangle.trRow0,
						sd.sdTableRectangle.trRow1 );
	if  ( ret )
	    { LDEB(ret); rval= -1; goto ready;	}

	goto ready;
	}

    /*  3  */
    if  ( sd.sdTableRectangle.trIsColSlice )
	{
	ret= tedDeleteColumnsFromRowsOperation( &teo,
						sd.sdTableRectangle.trRow0,
						sd.sdTableRectangle.trRow1,
						sd.sdTableRectangle.trCol0,
						sd.sdTableRectangle.trCol1 );
	if  ( ret )
	    { LDEB(ret); rval= -1; goto ready;	}

	goto ready;
	}

    /*  4  */
    LLDEB(sd.sdTableRectangle.trIsRowSlice,sd.sdTableRectangle.trIsColSlice);
    rval= -1;

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

