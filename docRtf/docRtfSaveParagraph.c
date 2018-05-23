/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>

#   include	<appDebugon.h>

#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	<docTreeType.h>
#   include	<docObjectProperties.h>
#   include	<docParaParticules.h>
#   include	"docRtfWriterImpl.h"

/************************************************************************/
/*									*/
/*  Save a paragraph in RTF format.					*/
/*									*/
/*  Column counting is approximate as it is only for cosmetic reasons.	*/
/*									*/
/************************************************************************/

static void docRtfParaSaveProperties(
				RtfWriter *			rw,
				int				flattenCell,
				int				firstInRow,
				int *				pPropertyChange,
				int *				pTabsSaved,
				const ParagraphProperties *	newPP,
				const ParagraphProperties *	prevPP )
    {
    ParagraphProperties		defPP;
    PropertyMask		ppDifMask;
    PropertyMask		ppCmpMask;

    utilPropMaskClear( &ppDifMask );
    utilPropMaskClear( &ppCmpMask );
    utilPropMaskFill( &ppCmpMask, PPprop_FULL_COUNT );
    if  ( flattenCell )
	{ PROPmaskUNSET( &ppCmpMask, PPpropTABLE_NESTING );	}

    docParaPropertyDifference( &ppDifMask, newPP, &ppCmpMask, prevPP );

    if  ( rw->rwPpExtraMask )
	{ utilPropMaskOr( &ppDifMask, &ppDifMask, rw->rwPpExtraMask );	}

    if  ( utilPropMaskIsEmpty( &ppDifMask ) )
	{
	if  ( ! flattenCell		&&
	      firstInRow		&&
	      newPP->ppTableNesting > 0	)
	    { docRtfSaveParaTableNesting( rw, newPP->ppTableNesting );	}

	return;
	}

    docInitParagraphProperties( &defPP );

    /*  1  */
    docRtfWriteTag( rw, RTFtag_pard );
    *pPropertyChange= 1;

    utilPropMaskClear( &ppDifMask );
    docParaPropertyDifference( &ppDifMask, newPP, &ppCmpMask, &defPP );

    if  ( rw->rwPpExtraMask )
	{ utilPropMaskOr( &ppDifMask, &ppDifMask, rw->rwPpExtraMask );	}

    docCleanParagraphProperties( &defPP );

    docRtfSaveParagraphProperties( rw, &ppDifMask, newPP );

    if  ( PROPmaskISSET( &ppDifMask, PPpropTAB_STOPS )	&&
	  newPP->ppTabStopListNumber != 0		)
	{ *pTabsSaved= 1;	}

    return;
    }

static int docRtfSaveObject(	RtfWriter *		rwc,
				const BufferItem *	paraNode,
				const TextParticule *	tp )
    {
    InsertedObject *	io;
    PictureProperties *	pip;

    PropertyMask	pipSetMask;

    io= docGetObject( rwc->rwDocument, tp->tpObjectNumber );
    if  ( ! io )
	{ LXDEB(tp->tpObjectNumber,io); return -1;	}

    pip= &(io->ioPictureProperties);

    docRtfWriteNextLine( rwc );

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokPICTEMFBLIP:

	    docRtfWriteDestinationBegin( rwc, RTFtag_pict );

	    docRtfPicturePropertyMask( &pipSetMask,
						&(io->ioPictureProperties) );
	    docRtfSavePictureTags( rwc, &pipSetMask,
						&(io->ioPictureProperties) );

	    docRtfWriteMemoryBuffer( rwc, &(io->ioObjectData) );
	    docRtfWriteNextLine( rwc );
	    docRtfWriteDestinationEnd( rwc );
	    return 0;

	case DOCokOLEOBJECT:
	    docRtfWriteDestinationBegin( rwc, "object" );

	    switch( io->ioRtfEmbedKind )
		{
		case EMBEDkindOBJEMB:
		    docRtfWriteTag( rwc, "objemb" );
		    break;
		case EMBEDkindOBJLINK:
		    docRtfWriteTag( rwc, "objlink" );
		    break;
		case EMBEDkindOBJAUTLINK:
		    docRtfWriteTag( rwc, "objautlink" );
		    break;
		case EMBEDkindOBJSUB:
		    docRtfWriteTag( rwc, "objsub" );
		    break;
		case EMBEDkindOBJPUB:
		    docRtfWriteTag( rwc, "objpub" );
		    break;
		case EMBEDkindOBJICEMB:
		    docRtfWriteTag( rwc, "objicemb" );
		    break;
		case EMBEDkindOBJHTML:
		    docRtfWriteTag( rwc, "objhtml" );
		    break;
		case EMBEDkindOBJOCX:
		    docRtfWriteTag( rwc, "objocx" );
		    break;
		default:
		    LDEB(io->ioRtfEmbedKind);
		    break;
		}

	    switch( io->ioRtfResultKind )
		{
		case RESULTkindUNKNOWN:
		    break;
		case RESULTkindRTF:
		    docRtfWriteTag( rwc, "rsltrtf" );
		    break;
		case RESULTkindTXT:
		    docRtfWriteTag( rwc, "rslttxt" );
		    break;
		case RESULTkindPICT:
		    docRtfWriteTag( rwc, "rsltpict" );
		    break;
		case RESULTkindBMP:
		    docRtfWriteTag( rwc, "rsltbmp" );
		    break;
		case RESULTkindHTML:
		    docRtfWriteTag( rwc, "rslthtml" );
		    break;
		default:
		    LDEB(io->ioRtfResultKind);
		    break;
		}

	    if  ( io->ioObjectClass )
		{
		const int	addSemicolon= 0;

		docRtfWriteDocEncodedStringDestination( rwc, "*\\objclass",
				io->ioObjectClass, strlen( io->ioObjectClass ),
				addSemicolon );
		}

	    if  ( io->ioObjectName )
		{
		const int	addSemicolon= 0;

		docRtfWriteDocEncodedStringDestination( rwc, "*\\objname",
				io->ioObjectName, strlen( io->ioObjectName ),
				addSemicolon );
		}

	    docRtfWriteArgTag( rwc, "objw", io->ioTwipsWide );
	    docRtfWriteArgTag( rwc, "objh", io->ioTwipsHigh );
	    if  ( io->ioScaleXSet != 100 )
		{ docRtfWriteArgTag( rwc, "objscalex", io->ioScaleXSet ); }
	    if  ( io->ioScaleYSet != 100 )
		{ docRtfWriteArgTag( rwc, "objscaley", io->ioScaleYSet ); }

	    docRtfWriteDestinationBegin( rwc, "*\\objdata" );
	    docRtfWriteMemoryBuffer( rwc, &io->ioObjectData );
	    docRtfWriteNextLine( rwc );
	    docRtfWriteDestinationEnd( rwc );

	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		docRtfWriteDestinationBegin( rwc, RTFtag_result );
		docRtfWriteDestinationBegin( rwc, RTFtag_pict );
		docRtfWriteArgTag( rwc, "wmetafile", pip->pipMapMode );

		docRtfPicturePropertyMask( &pipSetMask,
						&(io->ioPictureProperties) );
		docRtfSavePictureTags( rwc, &pipSetMask,
						&(io->ioPictureProperties) );

		docRtfWriteMemoryBuffer( rwc, &(io->ioResultData) );
		docRtfWriteNextLine( rwc );
		docRtfWriteDestinationEnd( rwc );

		docRtfWriteDestinationEnd( rwc );
		}

	    docRtfWriteDestinationEnd( rwc );
	    return 0;

	case DOCokEPS_FILE:
	    return 0;

	case DOCokDRAWING_SHAPE:
	    SDEB(docObjectKindStr(io->ioKind));
	    return 0;

	default:
	    SDEB(docObjectKindStr(io->ioKind));
	    return -1;
	}

    /*  Not reached ..
    return 0;
    */
    }

/************************************************************************/

static int docRtfSaveTagParticule(	RtfWriter *		rwc,
					int			kind )
    {
    switch( kind )
	{
	case DOCkindCHFTNSEP:
	    docRtfWriteTag( rwc, "chftnsep" );
	    break;

	case DOCkindCHFTNSEPC:
	    docRtfWriteTag( rwc, "chftnsepc" );
	    break;

	case DOCkindOPT_HYPH:
	    docRtfWriteTag( rwc, "-" );
	    rwc->rwcAfter= RTFafterTEXT;
	    break;

	case DOCkindTAB:
	    docRtfWriteTag( rwc, "tab" );
	    break;

	case DOCkindLINEBREAK:
	    docRtfWriteTag( rwc, "line" );
	    docRtfWriteNextLine( rwc );
	    break;

	case DOCkindPAGEBREAK:
	    docRtfWriteTag( rwc, "page" );
	    docRtfWriteNextLine( rwc );
	    break;

	case DOCkindCOLUMNBREAK:
	    docRtfWriteTag( rwc, "column" );
	    docRtfWriteNextLine( rwc );
	    break;

	case DOCkindLTR_MARK:
	    docRtfWriteTag( rwc, "ltrmark" );
	    break;

	case DOCkindRTL_MARK:
	    docRtfWriteTag( rwc, "rtlmark" );
	    break;

	default:
	    LDEB(kind); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a paragraph to rtf.						*/
/*									*/
/*  8)  MS-Word has a tendency to only pick up certain paragraph	*/
/*	properties, such as the tabs, at the paragraph mark. On the	*/
/*	other do superfluous paragraph marks result in empty paragraphs.*/
/*	Try to find a compromise: The \cell or \row count as a paragraph*/
/*	mark, so no need to save a paragraph mark in a table. This	*/
/*	construct was necessary to activate tabs in headers and footers.*/
/*									*/
/************************************************************************/

int docRtfSaveParaNode(		RtfWriter *			rwc,
				const BufferItem *		paraNode,
				const DocumentSelection *	ds,
				const int			flattenCell,
				const int			firstInRow )
    {
    TextParticule *		tp;

    const int			paralen= docParaStrlen( paraNode );
    int				stroffFrom= -1;
    int				stroffUpto= paralen;
    int				part= 0;
    int				partUpto= paraNode->biParaParticuleCount;
    int				headFlags= 0;
    int				tailFlags= 0;
    int				restartFromDefault= 0;
    int				paraPropChange= 0;
    int				tabsSaved= 0;

    PropertyMask		ppUpdMask;

    DocumentPosition		dpLast;

    docRtfParaSaveProperties( rwc, flattenCell, firstInRow,
					&paraPropChange, &tabsSaved,
					&(paraNode->biParaProperties),
					&(rwc->rwcParagraphProperties) );

    if  ( restartFromDefault || paraPropChange )
	{ docRtfWriteSwitchToPlain( rwc );	}

    if  ( paraNode->biParaParticuleCount == 0 )
	{ LDEB(paraNode->biParaParticuleCount);	}

    {
    DocumentSelection	dsPara;

    if  ( docIntersectSelectionWithParagraph( &dsPara,
					    &part, &partUpto,
					    &headFlags, &tailFlags,
					    paraNode, ds ) )
	{ LDEB(1); return -1;	}

    stroffFrom= dsPara.dsHead.dpStroff;
    stroffUpto= dsPara.dsTail.dpStroff;
    }

    tp= paraNode->biParaParticules+ part;
    for ( ; part < paraNode->biParaParticuleCount; part++, tp++ )
	{
	int	pastSel= ( tailFlags & POSflagPARA_FOUND ) && tp->tpStroff >= stroffUpto;

	if  ( rwc->rwSaveFlags & RTFflagOMIT_TEXT )
	    { continue;	}

	if  ( ! pastSel || ( tp->tpKind == DOCkindSPAN && tp->tpStrlen == 0 ) )
	    {
	    const char *	first= (const char *)0;

	    if  ( tp->tpStroff < paralen )
		{
		first= (const char *)docParaString( paraNode, tp->tpStroff );
		}

	    if  ( tp->tpKind != DOCkindFIELDTAIL )
		{
		docRtfWriteSwitchTextAttributes( rwc, tp->tpTextAttrNr, first );
		}
	    }

	switch( tp->tpKind )
	    {
	    int		extra;

	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
	    case DOCkindOPT_HYPH:
	    case DOCkindTAB:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
	    case DOCkindLTR_MARK:
	    case DOCkindRTL_MARK:
		if  ( pastSel )
		    { break;	}

		if  ( docRtfSaveTagParticule( rwc, tp->tpKind ) )
		    { LSDEB(tp->tpKind,docKindStr(tp->tpKind)); return -1; }

		continue;

	    case DOCkindSPAN:
		if  ( pastSel )
		    { break;	}

		{
		int		n;
		int		stroff= tp->tpStroff;

		if  ( stroff < stroffFrom )
		    { stroff=  stroffFrom;	}

		if  ( ( tailFlags & POSflagPARA_FOUND )		&&
		      tp->tpStroff+ tp->tpStrlen > stroffUpto	)
		    { n= stroffUpto- stroff;			}
		else{ n= tp->tpStroff+ tp->tpStrlen- stroff;	}

		if  ( rwc->rwCol > 0				&&
		      rwc->rwCol+ n > 72			&&
		      n < 72					)
		    { docRtfWriteNextLine( rwc ); }

		docRtfWriteFontEncodedString( rwc,
				(char *)docParaString( paraNode, stroff ), n );
		}

		continue;

	    case DOCkindOBJECT:
		if  ( pastSel )
		    { break;	}

		if  ( docRtfSaveObject( rwc, paraNode, tp ) )
		    { LDEB(tp->tpKind); }
		continue;

	    case DOCkindFIELDHEAD:
		if  ( pastSel )
		    { break;	}

		extra= docRtfSaveFieldHead( rwc, paraNode, part );
		if  ( extra < 0 )
		    { LDEB(extra); return -1;	}
		tp += extra; part += extra;
		continue;

	    case DOCkindFIELDTAIL:
		extra= docRtfSaveFieldTail( rwc, paraNode, part );
		if  ( extra != 0 )
		    { LDEB(extra); return -1;	}
		continue;

	    default:
		LDEB(tp->tpKind);
		continue;
	    }

	break;
	}

    {
    int		saveSpace= 0;

    if  ( ( rwc->rwSaveFlags & RTFflagSAVE_SOMETHING )	&&
	  ds						&&
	  ds->dsHead.dpNode == paraNode			&&
	  docIsIBarSelection( ds )			)
	{ saveSpace= 1;	}

    if  ( rwc->rwSaveFlags & RTFflagOMIT_TEXT )
	{ saveSpace= 1;	}

    if  ( saveSpace )
	{ docRtfWriteFontEncodedString( rwc, " ", 1 );	}
    }

    docInitDocumentPosition( &dpLast );
    docTailPosition( &dpLast, (BufferItem *)paraNode );

    /*  8  */
    if  ( ! ( tailFlags & POSflagPARA_FOUND )	||
	  stroffUpto == dpLast.dpStroff 	||
	  ds					)
	{
	if  ( ( paraNode->biParaTableNesting == 0		&&
		paraNode->biTreeType == DOCinBODY		)	||
	      ( tabsSaved && paraNode->biParaTableNesting == 0	)	||
	      paraNode->biNumberInParent < paraNode->biParent->biChildCount- 1 )
	    {
	    docRtfWriteTag( rwc, RTFtag_par  );
	    docRtfWriteNextLine( rwc );
	    }
	}

    utilPropMaskClear( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
				    &(rwc->rwcParagraphProperties),
				    &ppUpdMask, &(paraNode->biParaProperties),
				    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

