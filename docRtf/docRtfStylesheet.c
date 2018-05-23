/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"

/************************************************************************/
/*									*/
/*  Consume the style name in the stylesheet of a document. It comes	*/
/*  last in the style, so we can save the style.			*/
/*									*/
/************************************************************************/

static int docRtfStyleName(	RtfReader *	rrc,
				const char *		name,
				int			len )
    {
    RtfReadingState *		rrs= rrc->rrcState;

    DocumentStyle *		ds;

    int				size;
    const int			removeSemicolon= 1;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( docRtfSaveDocEncodedText( rrc, name, len ) )
	{ LDEB(len); return -1;	}

    ds= &(rrc->rrcStyle);
    switch( ds->dsLevel )
	{
	case DOClevSPAN:
	    break;

	case 0: case -1:
	    LDEB(rrc->rrcStyle.dsLevel);
	    ds->dsStyleNumber= 0;
	    ds->dsLevel= DOClevPARA;
	    break;

	case DOClevPARA:
	    ds->dsStyleNumber= rrs->rrsParagraphProperties.ppStyle;
	    break;

	case DOClevROW:
	    /* NO! They have trowd and the sheet owns the style
	    ds->dsStyleNumber= rrc->rrcRowProperties.rpRowStyle;
	    */
	    break;

	case DOClevSECT:
	    ds->dsStyleNumber= rrc->rrcSectionProperties.spStyle;
	    break;

	default:
	    LDEB(rrc->rrcStyle.dsLevel); return -1;
	}

    if  ( docRtfStoreStyleProperties( rrc ) )
	{ LDEB(1);	}

    if  ( docRtfStoreSavedText( &(rrc->rrcStyle.dsName),
					    &size, rrc, removeSemicolon ) )
	{ LDEB(len); return -1;	}

    ds= docInsertStyle( &(rrc->rrDocument->bdStyleSheet),
					rrc->rrcStyle.dsStyleNumber,
					&(rrc->rrcStyle), dam0 );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docCleanDocumentStyle( &(rrc->rrcStyle) );
    docInitDocumentStyle( &(rrc->rrcStyle) );

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rrc->rrDocument );

    return 0;
    }

static int docRtfStyleGroup(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    switch( rcw->rcwID )
	{
	case DOClevSPAN:
	    rrc->rrcStyle.dsStyleNumber= arg;
	    rrc->rrcStyle.dsLevel= DOClevSPAN;
	    break;

	case DOClevSECT:
	    rrc->rrcSectionProperties.spStyle= arg;
	    rrc->rrcStyle.dsStyleNumber= arg;
	    rrc->rrcStyle.dsLevel= DOClevSECT;
	    break;

	case DOClevROW:
	    rrc->rrcRowProperties.rpRowStyle= arg;
	    rrc->rrcStyle.dsStyleNumber= arg;
	    rrc->rrcStyle.dsLevel= DOClevROW;
	    break;

	case DOClevPARA:
	    rrs->rrsParagraphProperties.ppStyle= arg;
	    rrc->rrcStyle.dsStyleNumber= arg;
	    rrc->rrcStyle.dsLevel= DOClevPARA;
	    break;

	default:
	    LDEB(rcw->rcwID);
	}

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
		    (RtfControlWord *)0, docRtfStyleName, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    rrc->rrcStyle.dsLevel= DOClevANY;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Inside a stylesheet.. These tags are handled differently.		*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfStylesheetGroups[]=
    {
	RTF_DEST_XX( "cs",	DOClevSPAN,	docRtfStyleGroup ),
	RTF_DEST_XX( "ds",	DOClevSECT,	docRtfStyleGroup ),
	RTF_DEST_XX( "ts",	DOClevROW,	docRtfStyleGroup ),
	RTF_DEST_XX( "tsrowd",	DOClevROW,	docRtfStyleGroup ),
	RTF_DEST_XX( "s",	DOClevPARA,	docRtfStyleGroup ),

	{ (char *)0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Consume a group introduced by a control word that is not a		*/
/*  destination.							*/
/*									*/
/************************************************************************/

static int docRtfReadWordGroup(	RtfReader *	rrc,
				int			gotArg,
				int			arg,
				const char *		controlWord,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule )
    {
    const RtfControlWord *	rcw;

    rcw= docRtfFindPropertyWord( controlWord );
    if  ( rcw )
	{
	if  ( docRtfReadGroupX( rcw, rcw, gotArg, arg, rrc, groupWords,
					addParticule, (RtfCommitGroup)0 ) )
	    { SDEB(rcw->rcwWord); return -1;	}
	}
    else{
	if  ( rrc->rrcComplainUnknown )
	    { LSDEB(rrc->rrCurrentLine,controlWord);	}

	if  ( docRtfReadUnknownGroup( rrc ) )
	    { LSDEB(rrc->rrCurrentLine,controlWord); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume the stylesheet in a document.				*/
/*									*/
/*  Because <styledef> is optional in the stylesheet. (For the first	*/
/*  style at least.) The normal mechanism of the parser does not work	*/
/*  for the stylesheet. Do things by hand.				*/
/*									*/
/************************************************************************/

int docRtfStylesheet(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    int				res;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				c;

    docCleanDocumentStyle( &(rrc->rrcStyle) );
    docInitDocumentStyle( &(rrc->rrcStyle) );

    res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    rrc->rrcStyle.dsLevel= DOClevPARA;
    rrs->rrsParagraphProperties.ppStyle= 0;

    for (;;)
	{
	rrs= rrc->rrcState;

	switch( res )
	    {
	    case RTFfiCLOSE:
		break;

	    case RTFfiCTRLGROUP:
		rcw= docRtfFindWord( controlWord, docRtfStylesheetGroups );
		if  ( ! rcw )
		    {
		    if  ( docRtfReadWordGroup( rrc, gotArg, arg, controlWord,
					(RtfControlWord *)0, docRtfStyleName ) )
			{ SDEB(controlWord); return -1;	}
		    }
		else{
		  groupFound:
		    res= docRtfApplyControlWord( rcw, gotArg, arg, rrc );
		    if  ( res < 0 )
			{ LDEB(res); SDEB(controlWord); return -1;	}
		    }

		res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		continue;

	    case RTFfiSTARGROUP:
		rcw= docRtfFindWord( controlWord, docRtfStylesheetGroups );
		if  ( rcw )
		    { goto groupFound; }

		rrc->rrcInIgnoredGroup++;

		if  ( docRtfReadUnknownGroup( rrc ) )
		    { LDEB(1); rrc->rrcInIgnoredGroup--; return -1;	}

		rrc->rrcInIgnoredGroup--;

		res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		continue;

	    case RTFfiTEXTGROUP:
		{
		const RtfControlWord *	rcwApplyFirst= (RtfControlWord *)0;
		int			argApplyFirst= -1;
		int			gotArgApplyFirst= 0;

		if  ( docRtfReadGroupX( (const RtfControlWord *)0,
			rcwApplyFirst, gotArgApplyFirst, argApplyFirst,
			rrc, docRtfStylesheetGroups,
			docRtfStyleName, (RtfCommitGroup)0 ) )
		    { LDEB(1); return -1;	}

		res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		}
		continue;

	    default:
		LDEB(res); return -1;
	    }

	break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a style property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberStyleProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    DocumentStyle *	ds= &(rrc->rrcStyle);

    /* \s \cs \ds tags but \s and \ds handeled with the para/sect style */
    if  ( rcw->rcwID == DSpropSTYLE_NUMBER )
	{
	ds->dsStyleNumber= arg;
	/* Exceptional trick */
	ds->dsLevel= rcw->rcwEnumValue;
	return 0;
	}

    if  ( docSetStyleProperty( ds, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember accumulated properties of the reader on the current style.	*/
/*									*/
/************************************************************************/

int docRtfStoreStyleProperties(		RtfReader *		rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    BufferDocument *		bd= rrc->rrDocument;
    int				mindTable= 1;

    DocumentStyle *		ds= &(rrc->rrcStyle);

    /*  SECT	*/
    docCopySectionProperties( &(ds->dsSectProps),
					    &(rrc->rrcSectionProperties) );

    /*  ROW	*/

    /*  CELL	*/
    ds->dsCellProps.cpShadingNumber= docItemShadingNumber(
				    rrc->rrDocument, &(rrc->rrcCellShading) );
    if  ( ds->dsCellProps.cpShadingNumber < 0 )
	{ LDEB(ds->dsCellProps.cpShadingNumber);	}

    /*  PARA	*/
    if  ( docRtfSetParaProperties( &(ds->dsParaProps), bd,
						    mindTable, rrs, -1 ) )
	{ LDEB(1);	}

    /*  SPAN	*/
    if  ( rrs->rrsTextShadingChanged )
	{
	docRtfRefreshTextShading( rrc, rrs );
	PROPmaskADD( &(rrc->rrcStyle.dsTextMask), TApropSHADING );
	}

    ds->dsTextAttribute= rrs->rrsTextAttribute;

    return 0;
    }

