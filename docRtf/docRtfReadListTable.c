/************************************************************************/
/*									*/
/*  Read the list table of a document.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"

/************************************************************************/
/*									*/
/*  Remember the 'listlevel' related strings.				*/
/*									*/
/*  NOTE that some experimentation with MS-Word shows that the		*/
/*	level texts are in the document encoding. Not in the encoding	*/
/*	of the font of the bullet.					*/
/*									*/
/************************************************************************/

int docRtfCommitListLevelText(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    const int	removeSemicolon= 1;

    if  ( docRtfMemoryBufferSetText( &(rr->rrcListLevelText),
						    rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docRtfCommitListLevelNumbers(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    const int	removeSemicolon= 1;

    if  ( docRtfMemoryBufferSetText( &(rr->rrcListLevelNumbers),
						    rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember the name of a list.					*/
/*									*/
/************************************************************************/

int docRtfCommitListName(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    const int	removeSemicolon= 1;

    if  ( docRtfMemoryBufferSetText( &(rr->rrcDocumentList.dlListName),
						    rr, removeSemicolon ) )
	{ LDEB(1); return -1; }

    return 0;
    }

int docRtfCommitListStyleName(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    const int	removeSemicolon= 1;

    if  ( docRtfMemoryBufferSetText( &(rr->rrcDocumentList.dlListStyleName),
						    rr, removeSemicolon ) )
	{ LDEB(1); return -1; }

    return 0;
    }

int docRtfRememberListLevelProperty(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    if  ( docSetListLevelProperty( &(rr->rrcDocumentListLevel),
							rcw->rcwID, arg ) )
	{ SDEB(rcw->rcwWord);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember list properties.						*/
/*									*/
/*  1)  The same tag is used in the listoverrides, so set the value in	*/
/*	both structures. (Only one is used)				*/
/*									*/
/************************************************************************/

int docRtfRememberListProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	case DLpropLISTID:
	    /*  1  */
	    rr->rrcListOverride.loListID= arg;
	    break;

	case RTFid_NOT_SUPPORTED:
	    return 0;

	default:
	    break;
	}

    if  ( docSetListProperty( &(rr->rrcDocumentList), rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static int docRtfCommitListLevelProperties(	const RtfControlWord *	rcw,
						RtfReader *		rr )
    {
    BufferDocument *		bd= rr->rrDocument;
    RtfReadingState *		rrs= rr->rrcState;

    if  ( docListLevelFromRtfStrings( &(rr->rrcDocumentListLevel),
		    &(rr->rrcListLevelText), &(rr->rrcListLevelNumbers) ) )
	{ LDEB(1); return -1;	}

    docRtfRefreshParagraphProperties( bd, rrs );

    if  ( rrs->rrsTextShadingChanged )
	{
	docRtfRefreshTextShading( rr, rrs );
	PROPmaskADD( &(rr->rrcStyle.dsTextMask), TApropSHADING );
	}

    if  ( docListLevelSetStyle( &(rr->rrcDocumentListLevel),
					&(rr->rrcStyle.dsParaMask),
					&(rrs->rrsParagraphProperties),
					&(rr->rrcStyle.dsTextMask),
					&(rrs->rrsTextAttribute) ) )
	{ LDEB(1); return -1;	}

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, bd );

    utilPropMaskClear( &(rr->rrcStyle.dsParaMask) );
    utilPropMaskClear( &(rr->rrcStyle.dsTextMask) );

    return 0;
    }

int docRtfReadListLevelGroup(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrcState;

    docCleanDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rr->rrcDocumentListLevel) );
    utilEmptyMemoryBuffer( &(rr->rrcListLevelText) );
    utilEmptyMemoryBuffer( &(rr->rrcListLevelNumbers) );

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rr->rrDocument );

    utilPropMaskClear( &(rr->rrcStyle.dsParaMask) );
    utilPropMaskClear( &(rr->rrcStyle.dsTextMask) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
			    (RtfControlWord *)0, docRtfRefuseText,
			    docRtfCommitListLevelProperties ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

int docRtfRememberListLevel(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrcState;

    const int			copyIds= 1;
    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;
    const int * const		rulerMap= (const int *)0;

    docCleanDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rr->rrcDocumentListLevel) );

    utilEmptyMemoryBuffer( &(rr->rrcListLevelText) );
    utilEmptyMemoryBuffer( &(rr->rrcListLevelNumbers) );

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rr->rrDocument );

    utilPropMaskClear( &(rr->rrcStyle.dsParaMask) );
    utilPropMaskClear( &(rr->rrcStyle.dsTextMask) );

    if  ( docRtfReadListLevelGroup( rcw, rr ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskUNSET( &(rr->rrcDocumentListLevel.llTextAttributeMask),
							    TApropSHADING );
    PROPmaskUNSET( &(rr->rrcDocumentListLevel.llTextAttributeMask),
							    TApropBORDER );

    if  ( docDocumentListAddLevel( &(rr->rrcDocumentList),
				    &(rr->rrcDocumentListLevel),
				    copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rr->rrcDocumentListLevel) );

    return 0;
    }

int docRtfRememberList(	const RtfControlWord *		rcw,
			int				arg,
			RtfReader *			rr )
    {
    DocumentProperties *	dp= &(rr->rrDocument->bdProperties);

    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;
    const int * const		rulerMap= (const int *)0;

    const int			copyIds= 1;

    docCleanDocumentList( &(rr->rrcDocumentList) );
    docInitDocumentList( &(rr->rrcDocumentList) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
		    (RtfControlWord *)0, docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docDocumentListTableAddList( (DocumentList **)0,
				&(dp->dpListAdmin->laListTable),
				&(rr->rrcDocumentList),
				copyIds, fontMap, colorMap, rulerMap ) < 0 )
	{ LDEB(1); return -1;	}

    docCleanDocumentList( &(rr->rrcDocumentList) );
    docInitDocumentList( &(rr->rrcDocumentList) );

    return 0;
    }

int docRtfListTable(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    if  ( docRtfReadGroup( rcw, 0, 0, rr,
				(const RtfControlWord *)0,
				docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Word 95 type paragraph numbers: Just remember the properties.	*/
/*									*/
/************************************************************************/

int docRtfPnProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    ParagraphNumber *		pn= &(rr->rrcParagraphNumber);

    switch( rcw->rcwID )
	{
	case LLpropSTARTAT:
	    pn->pnStartAt= arg;
	    break;

	case LLpropSTYLE:
	    pn->pnNumberStyle= arg;
	    break;

	case LLpropJUSTIFY:
	    pn->pnJustification= arg;
	    break;

	case LLpropINDENT:
	    pn->pnIndent= arg;
	    break;

	case LLpropPREV:
	    pn->pnUsePrevText= arg != 0;
	    break;

	case LLpropSPACE:
	    pn->pnSpace= arg;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }


