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
#   include	<docParaParticules.h>
#   include	<docNodeTree.h>
#   include	"docEdit.h"
#   include	"docEditImpl.h"
#   include	<docTreeType.h>

#   define	DEB_PARTICULES		0
#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Finish a step in the editing process: The next step starts where	*/
/*  this one ended.							*/
/*									*/
/************************************************************************/

void docEditFinishStep(	EditOperation *			eo )
    {
    eo->eoHeadDp= eo->eoTailDp;

    eo->eoCol0= -1;
    eo->eoCol1= -1;

    return;
    }

/************************************************************************/
/*									*/
/*  Make sure that the ranges on the EditOperation do not include the	*/
/*  deleted paragraphs.							*/
/*									*/
/************************************************************************/

void docEditAvoidDeletedParagraphs(	EditOperation *		eo,
					const SelectionScope *	ssRoot,
					int			paraFrom,
					int			paraUpto )
    {
    int			prune= 0;

    if  ( ! docSelectionSameScope( &(eo->eoSelectionScope), ssRoot ) )
	{ return;	}

    if  ( eo->eoSelectedRange.erHead.epParaNr >= paraFrom	&&
	  eo->eoSelectedRange.erHead.epParaNr <  paraUpto	)
	{ prune= 1;	}
    if  ( eo->eoSelectedRange.erTail.epParaNr >= paraFrom	&&
	  eo->eoSelectedRange.erTail.epParaNr <  paraUpto	)
	{ prune= 1;	}

    if  ( eo->eoAffectedRange.erHead.epParaNr >= paraFrom	&&
	  eo->eoAffectedRange.erHead.epParaNr <  paraUpto	)
	{ prune= 1;	}
    if  ( eo->eoAffectedRange.erTail.epParaNr >= paraFrom	&&
	  eo->eoAffectedRange.erTail.epParaNr <  paraUpto	)
	{ prune= 1;	}

    if  ( eo->eoReformatRange.erHead.epParaNr >= paraFrom	&&
	  eo->eoReformatRange.erHead.epParaNr <  paraUpto	)
	{ prune= 1;	}
    if  ( eo->eoReformatRange.erTail.epParaNr >= paraFrom	&&
	  eo->eoReformatRange.erTail.epParaNr <  paraUpto	)
	{ prune= 1;	}

    if  ( ! prune )
	{ return;	}

    if  ( paraFrom <= 1 )
	{
	EditPosition		epAfter;

	epAfter.epParaNr= paraUpto;
	epAfter.epStroff= 0;

	if  ( eo->eoSelectedRange.erHead.epParaNr >= paraFrom	&&
	      eo->eoSelectedRange.erHead.epParaNr <  paraUpto	)
	    { eo->eoSelectedRange.erHead= epAfter;	}
	if  ( eo->eoSelectedRange.erTail.epParaNr >= paraFrom	&&
	      eo->eoSelectedRange.erTail.epParaNr <  paraUpto	)
	    { eo->eoSelectedRange.erTail= epAfter;	}

	if  ( eo->eoAffectedRange.erHead.epParaNr >= paraFrom	&&
	      eo->eoAffectedRange.erHead.epParaNr <  paraUpto	)
	    { eo->eoAffectedRange.erHead= epAfter;	}
	if  ( eo->eoAffectedRange.erTail.epParaNr >= paraFrom	&&
	      eo->eoAffectedRange.erTail.epParaNr <  paraUpto	)
	    { eo->eoAffectedRange.erTail= epAfter;	}

	if  ( eo->eoReformatRange.erHead.epParaNr >= paraFrom	&&
	      eo->eoReformatRange.erHead.epParaNr <  paraUpto	)
	    { eo->eoReformatRange.erHead= epAfter;	}
	if  ( eo->eoReformatRange.erTail.epParaNr >= paraFrom	&&
	      eo->eoReformatRange.erTail.epParaNr <  paraUpto	)
	    { eo->eoReformatRange.erTail= epAfter;	}
	}
    else{
	DocumentPosition	dpBefore;
	EditPosition		epBefore;

	dpBefore.dpNode= docGetParagraphByNumber( eo->eoTree, paraFrom- 1 );
	if  ( ! dpBefore.dpNode || docTailPosition( &dpBefore, dpBefore.dpNode ) )
	    { LDEB(paraFrom); return;	}
	epBefore.epParaNr= paraFrom- 1;
	epBefore.epStroff= dpBefore.dpStroff;

	if  ( eo->eoSelectedRange.erHead.epParaNr >= paraFrom	&&
	      eo->eoSelectedRange.erHead.epParaNr <  paraUpto	)
	    { eo->eoSelectedRange.erHead= epBefore;	}
	if  ( eo->eoSelectedRange.erTail.epParaNr >= paraFrom	&&
	      eo->eoSelectedRange.erTail.epParaNr <  paraUpto	)
	    { eo->eoSelectedRange.erTail= epBefore;	}

	if  ( eo->eoAffectedRange.erHead.epParaNr >= paraFrom	&&
	      eo->eoAffectedRange.erHead.epParaNr <  paraUpto	)
	    { eo->eoAffectedRange.erHead= epBefore;	}
	if  ( eo->eoAffectedRange.erTail.epParaNr >= paraFrom	&&
	      eo->eoAffectedRange.erTail.epParaNr <  paraUpto	)
	    { eo->eoAffectedRange.erTail= epBefore;	}

	if  ( eo->eoReformatRange.erHead.epParaNr >= paraFrom	&&
	      eo->eoReformatRange.erHead.epParaNr <  paraUpto	)
	    { eo->eoReformatRange.erHead= epBefore;	}
	if  ( eo->eoReformatRange.erTail.epParaNr >= paraFrom	&&
	      eo->eoReformatRange.erTail.epParaNr <  paraUpto	)
	    { eo->eoReformatRange.erTail= epBefore;	}
	}
    }

/************************************************************************/
/*									*/
/*  Shift references after an insertion.				*/
/*									*/
/*  1)  If a paragraph is split, only shift the list number references	*/
/*	from the next paragraph.					*/
/*									*/
/************************************************************************/

void docEditShiftReferences(		EditOperation *		eo,
					const SelectionScope *	ssRoot,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    DocumentTree *		dt= eo->eoTree;
    int				sectFrom= -1;

    if  ( docGetRootOfSelectionScope( &dt, (BufferItem **)0,
						eo->eoDocument, ssRoot ) )
	{ SDEB(docTreeTypeStr(ssRoot->ssTreeType)); return;	}

#   if SHOW_SELECTION_RANGE
    appDebug( "REFS FROM %3d:%-3d BY %2d:%d:%-2d <%s>\n",
			    paraFrom, stroffFrom,
			    sectShift, paraShift, stroffShift,
			    docTreeTypeStr(ssRoot->ssTreeType) );
#   endif

    if  ( docSelectionSameScope( &(eo->eoSelectionScope), ssRoot ) )
	{
#	if SHOW_SELECTION_RANGE
	appDebug( "> SEL     %3d:%-3d .. %3d:%-3d\n",
			    eo->eoSelectedRange.erHead.epParaNr,
			    eo->eoSelectedRange.erHead.epStroff,
			    eo->eoSelectedRange.erTail.epParaNr,
			    eo->eoSelectedRange.erTail.epStroff );
	appDebug( "> AFF     %3d:%-3d .. %3d:%-3d\n",
			    eo->eoAffectedRange.erHead.epParaNr,
			    eo->eoAffectedRange.erHead.epStroff,
			    eo->eoAffectedRange.erTail.epParaNr,
			    eo->eoAffectedRange.erTail.epStroff );
	appDebug( "> FORMAT  %3d:%-3d .. %3d:%-3d\n",
			    eo->eoReformatRange.erHead.epParaNr,
			    eo->eoReformatRange.erHead.epStroff,
			    eo->eoReformatRange.erTail.epParaNr,
			    eo->eoReformatRange.erTail.epStroff );
#	endif

	if  ( eo->eoReformatNeeded == REFORMAT_ADJUST_PARAGRAPH	&&
	      eo->eoParaAdjustParagraphNumber >= 0		&&
	      eo->eoParaAdjustParagraphNumber >= paraFrom	)
	    { eo->eoParaAdjustParagraphNumber += paraShift;	}

	docShiftEditRange( &(eo->eoReformatRange),
			    paraFrom, stroffFrom, paraShift, stroffShift );
	docShiftEditRange( &(eo->eoSelectedRange),
			    paraFrom, stroffFrom, paraShift, stroffShift );

	if  ( eo->eoSplitAtHead )
	    {
	    docAdjustEditPositionE( &(eo->eoAffectedRange.erTail),
			    paraFrom, stroffFrom, paraShift, stroffShift );
	    }
	else{
	    docShiftEditRange( &(eo->eoAffectedRange),
			    paraFrom, stroffFrom, paraShift, stroffShift );
	    }

#	if SHOW_SELECTION_RANGE
	appDebug( "< SEL     %3d:%-3d .. %3d:%-3d\n",
			    eo->eoSelectedRange.erHead.epParaNr,
			    eo->eoSelectedRange.erHead.epStroff,
			    eo->eoSelectedRange.erTail.epParaNr,
			    eo->eoSelectedRange.erTail.epStroff );
	appDebug( "< AFF     %3d:%-3d .. %3d:%-3d\n",
			    eo->eoAffectedRange.erHead.epParaNr,
			    eo->eoAffectedRange.erHead.epStroff,
			    eo->eoAffectedRange.erTail.epParaNr,
			    eo->eoAffectedRange.erTail.epStroff );
	appDebug( "< FORMAT  %3d:%-3d .. %3d:%-3d\n",
			    eo->eoReformatRange.erHead.epParaNr,
			    eo->eoReformatRange.erHead.epStroff,
			    eo->eoReformatRange.erTail.epParaNr,
			    eo->eoReformatRange.erTail.epStroff );
#	endif
	}

    if  ( ssRoot->ssTreeType == DOCinBODY )
	{ sectFrom= ssRoot->ssSectNr;	}

    if  ( docShiftFieldReferences( dt, sectFrom, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift ) )
	{ LDEB(paraFrom);	}

    if  ( ssRoot->ssTreeType == DOCinBODY )
	{
	if  ( paraShift != 0 )
	    {
	    int				listParaFrom= paraFrom;

	    /*  1  */
	    if  ( paraShift > 0					&&
		  ( stroffFrom > 0 || eo->eoSplitAtHead )	)
		{ listParaFrom++;	}

	    docShiftListTreeReferences( &(dt->dtListNumberTrees),
						    listParaFrom, paraShift );
	    docShiftListNodeReferences( &(dt->dtOutlineTree),
						    listParaFrom, paraShift );
	    }
	}

    eo->eoSplitAtHead= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Shift offsets in particules, selection and notes to reflect a	*/
/*  change in the paragraph string.					*/
/*									*/
/************************************************************************/

int docEditShiftParticuleOffsets(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			paraNr,
					int			partFrom,
					int			partUpto,
					int			stroffFrom,
					int			stroffShift )
    {
    BufferDocument *		bd= eo->eoDocument;
    int				rval;

    docAdjustEditPositionOffsetB( &(eo->eoSelectedRange.erHead),
					    paraNr, stroffFrom, stroffShift );
    docAdjustEditPositionOffsetE( &(eo->eoSelectedRange.erTail),
					    paraNr, stroffFrom, stroffShift );

    docAdjustEditPositionOffsetB( &(eo->eoAffectedRange.erHead),
					    paraNr, stroffFrom, stroffShift );
    docAdjustEditPositionOffsetE( &(eo->eoAffectedRange.erTail),
					    paraNr, stroffFrom, stroffShift );

    rval= docShiftParticuleOffsets( bd, paraBi,
					    partFrom, partUpto, stroffShift );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete all fields that are embedded in the content that we will	*/
/*  replace in this edit operation.					*/
/*									*/
/************************************************************************/

int docEditDeleteReplacedFields(	EditOperation *		eo )
    {
    DocumentSelection	dsBalanced;

    dsBalanced.dsHead= eo->eoHeadDp;
    dsBalanced.dsTail= eo->eoTailDp;

    if  ( docDeleteFieldsInRange( eo, &dsBalanced ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a position outside the deleted block of nodes.			*/
/*									*/
/************************************************************************/

int docEditFindPositionOutsideBlockDelete(
				int *				pSide,
				DocumentPosition *		dpNew,
				const DocumentSelection *	dsDel )
    {
    int			side;
    BufferItem *	newBi;

    side= 1;
    newBi= docNextParagraph( dsDel->dsTail.dpNode );
    if  ( ! newBi || docHeadPosition( dpNew, newBi ) )
	{
	side= -1;
	newBi= docPrevParagraph( dsDel->dsHead.dpNode );

	if  ( ! newBi || docTailPosition( dpNew, newBi ) )
	    { LDEB(1); return -1;	}
	}

    *pSide= side;
    return 0;
    }

