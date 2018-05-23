/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	"docEdit.h"
#   include	"docIntermediaryDocument.h"
#   include	<docTocField.h>
#   include	"docCalculateToc.h"
#   include	<docStripFrame.h>
#   include	<docPageGrid.h>
#   include	<docTabStop.h>
#   include	<docParaRulerAdmin.h>
#   include	<docHyperlinkField.h>
#   include	<docRefField.h>
#   include	<docPagerefField.h>
#   include	"docRecalculateTocField.h"

/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*  This is a two level process: First build the TOC from REF and	*/
/*  PAGEREF fields, then let those be evaluated by the text field	*/
/*  evaluator.								*/
/*									*/
/*  Expertimenting with MS-Word shows that the format of the table of	*/
/*  contents is controlled by the format of the first entry: the	*/
/*  paragraph in which the TOC field starts. Things are not THAT simple	*/
/*  however: To be investigated!					*/
/*									*/
/************************************************************************/

static int docParaInsertTocEntry(	CalculateToc *		ct,
					int			makeHyperlinks,
					const TocEntry *	te,
					const SelectionScope *	ss,
					BufferItem *		paraBi )
    {
    int			rval= 0;

    int			textAttrNr= ct->ctLevelAttributeNumbers[te->teLevel];
    DocumentField *	dfParent= ct->ctDfTocTo;
    DocumentField *	dfHyper= (DocumentField *)0;
    DocumentField *	dfRef= (DocumentField *)0;
    DocumentField *	dfPageref= (DocumentField *)0;

    int			stroffShift;

    TextParticule *	tp;

    DocumentFieldList *	dfl= &(ct->ctBdToc->bdFieldList);

    EditPosition	ep;

    HyperlinkField	hf;
    RefField		rf;
    PagerefField	pf;

    docInitHyperlinkField( &hf );
    docInitRefField( &rf );
    docInitPagerefField( &pf );

    if  ( ! te->teMarkName						||
	  utilCopyMemoryBuffer( &(hf.hfBookmark), te->teMarkName )	||
	  utilCopyMemoryBuffer( &(rf.rfBookmark), te->teMarkName )  	||
	  utilCopyMemoryBuffer( &(pf.pfBookmark), te->teMarkName )	)
	{ XDEB(te->teMarkName); rval= -1; goto ready;	}

    docDeleteEmptySpan( paraBi );

    ep.epParaNr= docNumberOfParagraph( paraBi );
    ep.epStroff= docParaStrlen( paraBi );

    /* HYPERLINK */
    if  ( makeHyperlinks )
	{
	dfHyper= docClaimField( dfl );
	if  ( ! dfHyper )
	    { XDEB(dfHyper); rval= -1; goto ready;	}

	tp= docMakeSpecialParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    DOCkindFIELDHEAD, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}

	tp->tpObjectNumber= dfHyper->dfFieldNumber;
	dfHyper->dfHeadPosition= ep;
	dfHyper->dfSelectionScope= *ss;

	dfHyper->dfKind= DOCfkHYPERLINK;
	if  ( docSetHyperlinkField( &(dfHyper->dfInstructions), &hf ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ep.epStroff += tp->tpStrlen;
	dfParent= dfHyper;
	}

    /* REF */
    dfRef= docClaimField( dfl );
    if  ( ! dfRef )
	{ XDEB(dfRef); rval= -1; goto ready;	}

    tp= docMakeSpecialParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    DOCkindFIELDHEAD, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); rval= -1; goto ready;	}

    tp->tpObjectNumber= dfRef->dfFieldNumber;
    dfRef->dfHeadPosition= ep;
    dfRef->dfSelectionScope= *ss;

    if  ( docSetRefField( dfRef, &rf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ep.epStroff += tp->tpStrlen;

    if  ( docParaStringReplace( &stroffShift, paraBi, ep.epStroff, ep.epStroff,
								    "x", 1 ) )
	{ LDEB(1); rval= -1; goto ready; }
    tp= docInsertTextParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    stroffShift, DOCkindSPAN, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); rval= -1; goto ready;	}
    ep.epStroff += stroffShift;

    tp= docMakeSpecialParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    DOCkindFIELDTAIL, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); rval= -1; goto ready;	}

    tp->tpObjectNumber= dfRef->dfFieldNumber;
    docSetFieldTail( dfRef, &ep );
    if  ( docAddChildToField( dfRef, dfParent ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ep.epStroff += tp->tpStrlen;

    if  ( te->teNumbered )
	{
	/* SEPARATOR */
	if  ( docParaStringReplace( &stroffShift, paraBi,
					ep.epStroff, ep.epStroff, " ", 1 ) )
	    { LDEB(1); rval= -1; goto ready; }
	tp= docInsertTextParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    stroffShift, DOCkindTAB, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}
	ep.epStroff += stroffShift;

	/* PAGEREF */
	dfPageref= docClaimField( dfl );
	if  ( ! dfPageref )
	    { XDEB(dfPageref); rval= -1; goto ready;	}

	tp= docMakeSpecialParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    DOCkindFIELDHEAD, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}

	tp->tpObjectNumber= dfPageref->dfFieldNumber;
	dfPageref->dfHeadPosition= ep;
	dfPageref->dfSelectionScope= *ss;

	dfPageref->dfKind= DOCfkPAGEREF;
	if  ( docSetPagerefField( dfPageref, &pf ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ep.epStroff += tp->tpStrlen;

	if  ( docParaStringReplace( &stroffShift, paraBi,
					ep.epStroff, ep.epStroff, "x", 1 ) )
	    { LDEB(1); rval= -1; goto ready; }
	tp= docInsertTextParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    stroffShift, DOCkindSPAN, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}
	ep.epStroff += stroffShift;

	tp= docMakeSpecialParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    DOCkindFIELDTAIL, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}

	tp->tpObjectNumber= dfPageref->dfFieldNumber;
	docSetFieldTail( dfPageref, &ep );
	if  ( docAddChildToField( dfPageref, dfParent ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ep.epStroff += tp->tpStrlen;
	}

    if  ( dfHyper )
	{
	tp= docMakeSpecialParticule( paraBi,
				    paraBi->biParaParticuleCount, ep.epStroff,
				    DOCkindFIELDTAIL, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}

	tp->tpObjectNumber= dfHyper->dfFieldNumber;
	docSetFieldTail( dfHyper, &ep );
	if  ( docAddChildToField( dfHyper, ct->ctDfTocTo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ep.epStroff += tp->tpStrlen;
	}

  ready:

    docCleanHyperlinkField( &hf );
    docCleanRefField( &rf );
    docCleanPagerefField( &pf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Create a paragraph to hold one TOC entry.				*/
/*									*/
/************************************************************************/

static BufferItem * docMakeTocParagraph(
				CalculateToc *			ct,
				BufferItem *			refBi,
				int				level,
				int				numbered )

    {
    const DocumentStyle *	ds= ct->ctLevelStyles[level];
    const DocumentProperties *	dp= &(ct->ctBdToc->bdProperties);
    int				textAttrNr= ct->ctLevelAttributeNumbers[level];

    BufferItem *		paraNodeToc= (BufferItem *)0;

    TabStopList			tsl;

    docInitTabStopList( &tsl );

    paraNodeToc= docInsertEmptyParagraph( ct->ctBdToc, refBi, textAttrNr );
    if  ( ! paraNodeToc )
	{ XDEB(paraNodeToc); goto ready;	}

    if  ( ds && ! utilPropMaskIsEmpty( &(ds->dsParaMask) ) )
	{
	const ParagraphProperties *	ppFrom= &(ds->dsParaProps);

	/*  No mapping needed: The style is in the target document */
	if  ( docUpdParaProperties( (PropertyMask *)0,
					&(paraNodeToc->biParaProperties),
					&(ds->dsParaMask), ppFrom,
					(const DocumentAttributeMap *)0 ) )
	    { LDEB(1);		}

	paraNodeToc->biParaStyle= ds->dsStyleNumber;
	}
    else{
	if  ( docCopyParagraphProperties( &(paraNodeToc->biParaProperties),
							    &(ct->ctRefPP) ) )
	    { LDEB(1);	}

	if  ( ct->ctTocField.tfType == TOCtypeTOC	&&
	      level >= 0				&&
	      level <= PPoutlineDEEPEST			)
	    {
	    paraNodeToc->biParaLeftIndentTwips= level* dp->dpTabIntervalTwips;
	    }
	}

    if  ( numbered							&&
	  paraNodeToc->biParaProperties.ppTabStopListNumber == 0	)
	{
	ParagraphFrame		pf;
	TabStop			ts;
	int			n;

	docParagraphFrameTwips( &pf, &(ct->ctBlockFrame), paraNodeToc );

	docInitTabStop( &ts );

	ts.tsAlignment= DOCtaRIGHT;
	ts.tsTwips= pf.pfCellContentRect.drX1- pf.pfCellContentRect.drX0;
	ts.tsLeader= DOCtlDOTS;

	if  ( docAddTabToListTwips( &tsl, &ts ) )
	    { LDEB(1);	}
	n= docTabStopListNumber( ct->ctBdToc, &tsl );
	if  ( n < 0 )
	    { LDEB(n);	}
	else{ paraNodeToc->biParaProperties.ppTabStopListNumber= n;	}
	}

  ready:

    docCleanTabStopList( &tsl );

    return paraNodeToc;
    }

/************************************************************************/
/*									*/
/*  Clone the source TOC field to hold all TOC entries in the		*/
/*  intermediary toc document.						*/
/*									*/
/************************************************************************/

static int docStartTocField(	CalculateToc *		ct,
				const DocumentField *	dfToc,
				BufferItem *		paraNodeToc )
    {
    EditPosition	epStart;
    const int		stroff= 0;
    const int		part= 0;
    TextParticule *	tp;
    int			textAttrNr= ct->ctDefaultTextAttributeNumber;

    DocumentPosition	dp;

    docSetDocumentPosition( &dp, paraNodeToc, stroff );
    docSetEditPosition( &epStart, &dp );

    ct->ctDfTocTo= docClaimFieldCopy( &(ct->ctBdToc->bdFieldList),
		    dfToc, &(ct->ctSectNode->biSectSelectionScope), &epStart );
    if  ( ! ct->ctDfTocTo )
	{ XDEB(ct->ctDfTocTo); return -1;	}

    docDeleteEmptySpan( paraNodeToc );

    tp= docMakeSpecialParticule( paraNodeToc, part, epStart.epStroff,
					    DOCkindFIELDHEAD, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= ct->ctDfTocTo->dfFieldNumber;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build the document that will receive the table of contents, before	*/
/*  it is copied over to the target document.				*/
/*									*/
/*  A couple of administrations are copied over to make sure that	*/
/*  properties have the same indexes in the administrations of both	*/
/*  documents.								*/
/*									*/
/************************************************************************/

static int docSetupTocDocument(		CalculateToc *		ct )
    {
    int			level;
    TextAttribute	ta;

    ct->ctBdToc= docIntermediaryDocument( &(ct->ctSectNode), ct->ctBdDoc );
    if  ( ! ct->ctBdToc )
	{ XDEB(ct->ctBdToc); return -1;	}

    ct->ctDefaultTextAttributeNumber= docTextAttributeNumber( ct->ctBdToc,
						    &(ct->ctTextAttribute) );

    for ( level= 0; level <= PPoutlineDEEPEST; level++ )
	{
	char			scratch[20+1];
	const DocumentStyle *	ds;

	sprintf( scratch, "toc %d", level+ 1 );
	ds= docGetStyleByName( &(ct->ctBdToc->bdStyleSheet), scratch );
	ct->ctLevelStyles[level]= ds;

	ct->ctLevelAttributeNumbers[level]= ct->ctDefaultTextAttributeNumber;
	if  ( ds && ! utilPropMaskIsEmpty( &(ds->dsTextMask) ) )
	    {
	    PropertyMask	doneMask;

	    docPlainTextAttribute( &ta, ct->ctBdToc );

	    utilUpdateTextAttribute( &doneMask, &ta,
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
	    ct->ctLevelAttributeNumbers[level]= docTextAttributeNumber(
							    ct->ctBdToc, &ta );
	    }
	}

    ct->ctLevelAttributeNumbers[PPoutlineBODYTEXT]=
					ct->ctDefaultTextAttributeNumber;

    return 0;
    }

/************************************************************************/

static int docFinishTocField(	CalculateToc *		ct,
				BufferItem *		paraNodeToc )
    {
    TextParticule *	tp;
    int			textAttrNr= ct->ctDefaultTextAttributeNumber;

    DocumentPosition	dpTail;
    EditPosition	epTail;

    docSetDocumentPosition( &dpTail,  paraNodeToc, docParaStrlen( paraNodeToc ) );
    docSetEditPosition( &epTail, &dpTail );

    tp= docMakeSpecialParticule( paraNodeToc, paraNodeToc->biParaParticuleCount,
			    epTail.epStroff, DOCkindFIELDTAIL, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}
    tp->tpObjectNumber= ct->ctDfTocTo->dfFieldNumber;

    ct->ctDfTocTo->dfTailPosition= epTail;

    if  ( docAddRootFieldToTree( &(ct->ctBdToc->bdBody), ct->ctDfTocTo ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int docGetTocParaProperties(
				CalculateToc *			ct,
				const DocumentSelection *	dsToc,
				int				part0,
				int				part1 )
    {
    int			rval= 0;

    docBlockFrameTwips( &(ct->ctBlockFrame),
				dsToc->dsHead.dpNode, ct->ctBdDoc,
				dsToc->dsHead.dpNode->biTopPosition.lpPage,
				dsToc->dsHead.dpNode->biTopPosition.lpColumn );


    if  ( docCopyParagraphProperties( &(ct->ctRefPP),
				&(dsToc->dsHead.dpNode->biParaProperties ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ct->ctRefPP.ppOutlineLevel= PPoutlineBODYTEXT;

    {
    int				part;
    const TextParticule *	tp= (const TextParticule *)0;

    tp= dsToc->dsHead.dpNode->biParaParticules+ part0;
    docGetTextAttributeByNumber( &(ct->ctTextAttribute),
					ct->ctBdDoc, tp->tpTextAttrNr );

    part= part0;
    while( part < dsToc->dsHead.dpNode->biParaParticuleCount )
	{
	if  ( tp->tpKind == DOCkindSPAN )
	    {
	    docGetTextAttributeByNumber( &(ct->ctTextAttribute),
					    ct->ctBdDoc, tp->tpTextAttrNr );

	    break;
	    }

	part++; tp++;
	}
    }

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Recalculate TOC field.						*/
/*									*/
/************************************************************************/

static int docCalculateTocField( CalculateToc *			ct,
				const DocumentSelection *	dsToc,
				int				part0,
				int				part1,
				const DocumentField *		dfToc )
    {
    BufferItem *		paraNodeToc= (BufferItem *)0;
    BufferItem *		refBi= (BufferItem *)0;

    int				entryNr= 0;
    TocEntry *			te;

    if  ( docGetTocParaProperties( ct, dsToc, part0, part1 ) )
	{ LDEB(1); return -1;	}

    if  ( docSetupTocDocument( ct ) )
	{ LDEB(1); return -1;	}

    refBi= ct->ctSectNode;
    te= ct->ctEntries;
    for ( entryNr= 0; entryNr < ct->ctEntryCount; te++, entryNr++ )
	{
	paraNodeToc= docMakeTocParagraph( ct, refBi,
					    te->teLevel, te->teNumbered );
	if  ( ! paraNodeToc )
	    { XDEB(paraNodeToc); return -1;	}

	if  ( ! ct->ctDfTocTo )
	    {
	    if  ( docStartTocField( ct, dfToc, paraNodeToc ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( docParaInsertTocEntry( ct, ct->ctTocField.tfHyperlinks, te,
				    &(dfToc->dfSelectionScope), paraNodeToc ) )
	    { LDEB(entryNr); return -1;	}

	refBi= paraNodeToc;
	}

    if  ( ! paraNodeToc )
	{
	int			stroff;
	int			stroffShift;
	const int		level= 0;
	const int		numbered= 0;

	const char *		text= "TOC";
	const int		len= 3;

	paraNodeToc= docMakeTocParagraph( ct, refBi, level, numbered );
	if  ( ! paraNodeToc )
	    { XDEB(paraNodeToc); return -1;	}

	if  ( docStartTocField( ct, dfToc, paraNodeToc ) )
	    { LDEB(1); return -1;	}

	stroff= docParaStrlen( paraNodeToc );
	if  ( docParaStringReplace( &stroffShift, paraNodeToc,
						stroff, stroff, text, len ) )
	    { LDEB(len); return -1;	}

	if  ( docRedivideStringInParticules( paraNodeToc, stroff, len,
				    paraNodeToc->biParaParticuleCount, 0, 
				    ct->ctDefaultTextAttributeNumber ) < 0 )
	    { LDEB(1); return -1;	}
	}

    if  ( docFinishTocField( ct, paraNodeToc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Recalculate one TOC field in a document.				*/
/*									*/
/************************************************************************/

int docRecalculateOneTocField(	BufferDocument *	bdDoc,
				const DocumentField *	dfToc )
    {
    int				rval= 0;

    const MemoryBuffer * const	refFileName= (const MemoryBuffer *)0;
    const int			forceAttributeTo= -1;

    DocumentSelection		dsInsideToc;
    DocumentSelection		dsAroundToc;
    int				part0;
    int				part1;

    EditOperation		eo;
    DocumentCopyJob		dcj;
    CalculateToc		ct;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );
    docInitCalculateToc( &ct );

    if  ( docDelimitFieldInDoc( &dsInsideToc, &dsAroundToc,
					    &part0, &part1, bdDoc, dfToc ) )
	{ LDEB(1); rval= -1; goto ready;	}

#   if 0
    /*  MS-Word does something like this: */
    docHeadPosition( &(dsAroundToc.dsHead), dsAroundToc.dsHead.dpNode );
    docTailPosition( &(dsAroundToc.dsTail), dsAroundToc.dsTail.dpNode );
#   endif

    eo.eoBodySectNode= docGetBodySectNodeOfScope(
					&(dfToc->dfSelectionScope), bdDoc );

    ct.ctBdDoc= bdDoc;
    docFieldGetToc( &(ct.ctTocField), dfToc );

    if  ( docCollectTocInput( &ct ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docCalculateTocField( &ct, &dsInsideToc, part0, part1, dfToc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docStartEditOperation( &eo, &dsAroundToc, bdDoc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ct.ctBdToc->bdProperties.dpHasOpenEnd= 1;

    if  ( docSet2DocumentCopyJob( &dcj, &eo, ct.ctBdToc, &(ct.ctBdToc->bdBody),
					    refFileName, forceAttributeTo) )
	{ LDEB(1); rval= -1; goto ready;	}

    dcj.dcjCopyHeadParaProperties= 1;
    dcj.dcjCopyTailParaProperties= 1;

    if  ( docIncludeDocument( &dcj ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );
    docCleanCalculateToc( &ct );

    return rval;
    }
