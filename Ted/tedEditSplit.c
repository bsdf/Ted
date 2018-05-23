/************************************************************************/
/*									*/
/*  Editor functionality: Split nodes in the document tree.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docParaParticules.h>
#   include	<docRtfTrace.h>
#   include	<docLayout.h>
#   include	<appFrame.h>
#   include	<docEditCommand.h>
#   include	"tedEdit.h"
#   include	"tedDocFront.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  1)  Split in the buffer administration.				*/
/*  2)  Recalculate the geometry from the start of the selection to the	*/
/*	end of the paragraph.						*/
/*  3)  Redraw the affected part of the paragraph.			*/
/*  4)  Cause the new paragraph to be reformatted in this edit		*/
/*	operation.							*/
/*  5)  If the new paragraph is part of a numbered/bulleted list,	*/
/*	insert a list text field at the head.				*/
/*									*/
/************************************************************************/

static int tedSplitParaNode(	TedEditOperation *		teo,
				int				splitLevel,
				int				onNewPage )
    {
    EditOperation *		eo= &(teo->teoEo);
    BufferItem *		newParaNode;
    const LayoutContext *	lc= &(teo->teoLayoutContext);

    DocumentRectangle		drPara;

    docNodeRectangle( &drPara, eo->eoTailDp.dpNode, lc,
						    (const BlockOrigin *)0 );

    /*  1  */
    if  ( docSplitParaNode( &newParaNode, &(teo->teoEo), splitLevel ) )
	{ LDEB(1); return -1;	}

    newParaNode->biParaBreakKind= DOCibkNONE;
    if  ( newParaNode->biParaTableNesting == 0 && onNewPage )
	{ newParaNode->biParaBreakKind= DOCibkPAGE;	}
    
    /*  3  */
    geoUnionRectangle( &(teo->teoChangedRect),
				    &(teo->teoChangedRect), &drPara );

    if  ( docHeadPosition( &(eo->eoTailDp), newParaNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

void tedDocSplitParagraph(	EditDocument *		ed,
				int			onNewPage,
				int			traced )
    {
    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpBefore;
    DocumentSelection		dsTraced;

    int				command= EDITcmdREPLACE;
    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( onNewPage )
	{ command= EDITcmdREPLACE_BODY_LEVEL;	}

    if  ( tedEditStartReplace( &dsTraced, &teo, command, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    if  ( tedEditDeleteSelection( &teo ) )
	{ LDEB(1); goto ready;	}

    if  ( tedSplitParaNode( &teo, DOClevPARA, onNewPage ) )
	{ LDEB(1); goto ready;	}

    dpBefore= eo->eoTailDp;
    if  ( docPrevPosition( &dpBefore  ) )
	{ LDEB(1); goto ready;	}

    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:
    tedCleanEditOperation( &teo );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a section break in the document.				*/
/*									*/
/*  We do not want to insert a section break immediately before or	*/
/*  after a paragraph break: In those cases, let the section break	*/
/*  replace the paragraph break.					*/
/*									*/
/************************************************************************/

int tedDocInsertSectBreak(	EditDocument *		ed,
				int			traced )
    {
    int				rval= 0;
    const int			onNewPage= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				part;
    int				partFlags;

    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( docFindParticuleOfPosition( &part, &partFlags,
					    &(eo->eoHeadDp), PARAfindFIRST ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( ( partFlags & POSflagPARA_HEAD )	&&
	  ! docPrevPosition( &(eo->eoHeadDp)  )	)
	{
	docSetEditPosition( &(eo->eoReformatRange.erHead), &(eo->eoHeadDp) );
	eo->eoSelectedRange.erHead= eo->eoReformatRange.erHead;
	eo->eoAffectedRange.erHead= eo->eoReformatRange.erHead;
	}

    if  ( docFindParticuleOfPosition( &part, &partFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( ( partFlags & POSflagPARA_TAIL )	&&
	  ! docNextPosition( &(eo->eoTailDp)  )	)
	{
	eo->eoLastDp= eo->eoTailDp;
	docSetEditPosition( &(eo->eoReformatRange.erTail), &(eo->eoTailDp) );
	eo->eoSelectedRange.erTail= eo->eoReformatRange.erTail;
	eo->eoAffectedRange.erTail= eo->eoReformatRange.erTail;
	}

    if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdSPLIT_SECT, DOClevSPAN, 0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditDeleteSelection( &teo ) )
	{ rval= -1; goto ready;	}

    if  ( docFindParticuleOfPosition( &part, &partFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! ( partFlags & (POSflagPARA_HEAD|POSflagPARA_TAIL) ) )
	{
	if  ( tedSplitParaNode( &teo, DOClevSECT, onNewPage ) )
	    { LDEB(1); rval= -1; goto ready;	}

	eo->eoSectionsAdded++;

	/*  7,8  */
	tedEditFinishSelectionTail( &teo );
	}
    else{
	const int		after= ( partFlags & POSflagPARA_HEAD ) == 0;
	DocumentPosition	dpBefore;
	DocumentPosition	dpAfter;
	DocumentSelection	dsSplit;

	const int		split= 1;
	const int		direction= 1;

	if  ( tedEditInsertSection( &dpBefore, &dpAfter, &teo, split, after ) )
	    { LDEB(after); rval= -1; goto ready;	}

	docSetRangeSelection( &dsSplit, &dpBefore, &dpAfter, direction );
	docSetEditRange( &(eo->eoAffectedRange), &dsSplit );
	eo->eoSelectedRange= eo->eoAffectedRange;

	/*  7,8  */
	tedEditFinishSelectionTail( &teo );
	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }
