/************************************************************************/
/*									*/
/*  Ted: Table manipulation.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"
#   include	<docIntermediaryDocument.h>
#   include	<docRtfTrace.h>
#   include	<docPageGrid.h>
#   include	<docTreeNode.h>
#   include	<docParaParticules.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a table into the document.					*/
/*									*/
/*  0)  Also expose the table borders.					*/
/*  1)  If a range of data was selected, first discard it.		*/
/*  2)  If the selection was at the end of a paragraph, try to move to	*/
/*	the beginning of the next one.					*/
/*  3)  If the IBar is in the middle of a paragraph, split it.		*/
/*	tedSplitParaNode() forces the layout of both halves.		*/
/*  4)  If the paragraph is not the first one in its parent, split the	*/
/*	the parent. This is part of a legacy decision to have some	*/
/*	kind of fake rows and cells even outside a table.		*/
/*  5)  If the 'cell' is not the first one in the 'row' that contains	*/
/*	it, split the row. (Legacy again).				*/
/*									*/
/************************************************************************/

int tedInsertTable(		EditDocument *		ed,
				int			rows,
				int			columns,
				int			traced )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    BufferDocument *		bdTable= (BufferDocument *)0;
    BufferItem *		sectNodeTable;
    int				textAttrNr;
    int				wide;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentCopyJob		dcj;

    int				headFlags;
    int				tailFlags;
    int				posWhere= SELposHEAD;

    const int			fullWidth= 1;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( docFindParticuleOfPosition( (int *)0, &headFlags,
					    &(eo->eoHeadDp), PARAfindFIRST ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( docFindParticuleOfPosition( (int *)0, &tailFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ( headFlags & POSflagPARA_HEAD )	&&
	  ( tailFlags & POSflagPARA_TAIL )	&&
	  ! docNextPosition( &(eo->eoTailDp)  )	)
	{
	eo->eoLastDp= eo->eoTailDp;
	docSetEditPosition( &(eo->eoReformatRange.erTail), &(eo->eoTailDp) );
	eo->eoSelectedRange.erTail= eo->eoReformatRange.erTail;
	eo->eoAffectedRange.erTail= eo->eoReformatRange.erTail;
	}

    if  ( headFlags & POSflagPARA_HEAD )
	{ posWhere= SELposHEAD;	}
    else{ posWhere= SELposNEXT;	}

    {
    DocumentRectangle		dr;

    docParaBlockFrameRectangle( &dr, eo->eoHeadDp.dpNode,
				eo->eoDocument,
				eo->eoHeadDp.dpNode->biTopPosition.lpPage,
				eo->eoHeadDp.dpNode->biTopPosition.lpColumn );

    wide= dr.drX1- dr.drX0;
    }

    bdTable= docIntermediaryDocument( &sectNodeTable, eo->eoDocument );
    if  ( ! bdTable )
	{ XDEB(bdTable); rval= -1; goto ready;	}

    textAttrNr= docMapTextAttributeNumberFromTo( bdTable, eo->eoDocument,
			    td->tdSelectionDescription.sdTextAttributeNumber );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); rval= -1; goto ready;	}

    if  ( docFillTableDocument( bdTable, sectNodeTable, textAttrNr,
						    wide, rows, columns ) )
	{ LLDEB(rows,columns); rval= -1; goto ready;	}

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdTable, &(bdTable->bdBody),
						    &(ed->edFilename), -1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  0  */
    teo.teoChangedRect.drY0--;

    if  ( tedEditIncludeDocument( &teo, &dcj,
					EDITcmdINSERT_TABLE, posWhere ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    tedCleanEditOperation( &teo );

    if  ( bdTable )
	{ docFreeIntermediaryDocument( bdTable );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Roll rows in a table.						*/
/*									*/
/************************************************************************/

int tedDocRollRowsInTableImpl(	TedEditOperation *		teo,
				const TableRectangle  *		tr,
				int				move,
				int				rowsdown )
    {
    EditOperation *		eo= &(teo->teoEo);
    int				rval= 0;

    BufferItem *		parentNode;

    int				col;
    int				row;
    int				tabRow0;
    int				tabRow1;

    DocumentPosition		dp;

    EditRange			selectedRange= eo->eoSelectedRange;
    int				paraNr;
    int				paraOff0;
    int				paraOff1;
    int				selRow0= tr->trRow0;
    int				selRow1= tr->trRow1;

    const int			recursively= 0;

    int				row0= selRow0;
    int				row1= selRow1;
    int				rowsHigh;

    if  ( move )
	{
	if  ( rowsdown < 0 )
	    { row0 += rowsdown;	}
	if  ( rowsdown > 0 )
	    { row1 += rowsdown;	}
	}

    rowsHigh= row1- row0+ 1;

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
					    &col, &tabRow0, &row, &tabRow1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditIncludeRowsInRedraw( teo, parentNode, row0, row1 );

    if  ( rowsdown < 0 )
	{ rowsdown= rowsHigh+ rowsdown;	}

    if  ( row0 >= row1 )
	{ LLDEB(row0,row1); rval= -1; goto ready;	}
    if  ( row0 < tabRow0 || row0 > tabRow1 )
	{ LLLDEB(tabRow0,row0,tabRow1); rval= -1; goto ready;	}
    if  ( row1 < tabRow0 || row1 > tabRow1 )
	{ LLLDEB(tabRow0,row1,tabRow1); rval= -1; goto ready;	}
    if  ( rowsdown < 1 || rowsdown >= rowsHigh )
	{ LLDEB(rowsdown,rowsHigh); rval= -1; goto ready;	}

    docInitDocumentPosition( &dp );
    if  ( docHeadPosition( &dp, parentNode->biChildren[selRow0] ) )
	{ LDEB(selRow0); rval= -1; goto ready;	}
    paraNr= docNumberOfParagraph( dp.dpNode );
    paraOff0= selectedRange.erHead.epParaNr- paraNr;
    if  ( docHeadPosition( &dp, parentNode->biChildren[selRow1] ) )
	{ LDEB(selRow1); rval= -1; goto ready;	}
    paraNr= docNumberOfParagraph( dp.dpNode );
    paraOff1= selectedRange.erTail.epParaNr- paraNr;
    if  ( paraOff0 < 0 || paraOff1 < 0 )
	{ LLDEB(paraOff0,paraOff1); rval= -1; goto ready;	}

    if  ( docRollNodeChildren( eo, parentNode, row0, row1+ 1, rowsdown ) )
	{ LLLDEB(row0,row1,rowsdown); rval= -1; goto ready;	}

    docEditIncludeNodeInReformatRange( eo, parentNode );

    docDelimitTables( parentNode, recursively );

    selRow0 += rowsdown;
    if  ( selRow0 > row1 )
	{ selRow0 -= rowsHigh;	}
    selRow1 += rowsdown;
    if  ( selRow1 > row1 )
	{ selRow1 -= rowsHigh;	}

    if  ( docHeadPosition( &dp, parentNode->biChildren[selRow0] ) )
	{ LDEB(selRow0); rval= -1; goto ready;	}
    paraNr= docNumberOfParagraph( dp.dpNode );
    selectedRange.erHead.epParaNr= paraNr+ paraOff0;
    if  ( docHeadPosition( &dp, parentNode->biChildren[selRow1] ) )
	{ LDEB(selRow1); rval= -1; goto ready;	}
    paraNr= docNumberOfParagraph( dp.dpNode );
    selectedRange.erTail.epParaNr= paraNr+ paraOff1;
    if  ( selectedRange.erHead.epParaNr < 0	||
	  selectedRange.erTail.epParaNr < 0	)
	{ LDEB(1); rval= -1; goto ready;	}

    eo->eoSelectedRange= selectedRange;
    eo->eoAffectedRange= selectedRange;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

static int tedSplitColumnInRows(	TedEditOperation *	teo,
					DocumentPosition *	dpNew,
					BufferItem *		sectBi,
					int			row0,
					int			row,
					int			row1,
					int			col,
					int			after )
    {
    EditOperation *		eo= &(teo->teoEo);
    BufferItem *		newParaBi= (BufferItem *)0;
    const int			recursively= 0;

    tedEditIncludeRowsInRedraw( teo, sectBi, row0, row1 );

    if  ( docSplitColumnInRows( &newParaBi, eo,
				    sectBi, row0, row, row1, col, after ) )
	{ LDEB(1); return -1;	}

    docDelimitTables( sectBi, recursively );

    if  ( docHeadPosition( dpNew, newParaBi ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int tedDocAddColumnToTable(	EditDocument *		ed,
				int			after,
				int			traced )
    {
    int				rval= 0;
    BufferItem *		sectBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpNew;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( after )
	{
	if  ( tedEditStartStep( &teo, EDITcmdAPPEND_COLUMN ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdINSERT_COLUMN ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &sectBi,
					    &col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedSplitColumnInRows( &teo, &dpNew,
				    sectBi, row0, row, row1, col, after ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
    docSetIBarRange( &(eo->eoSelectedRange), &dpNew );
    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	const SelectionScope * const s0= (const SelectionScope *)0;

	if  ( after )
	    { docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

