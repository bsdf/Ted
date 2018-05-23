/************************************************************************/
/*									*/
/*  Save a BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	<docBuf.h>
#   include	<docField.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docWriteCss.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<psShading.h>
#   include	<docTextParticule.h>

#   include	<appDebugon.h>

static int docHtmlSaveParticules( const BufferItem *		paraNode,
				int *				pNoteRefCount,
				int				part,
				int				partUpto,
				HtmlWritingContext *		hwc )
    {
    XmlWriter *				xw= &(hwc->hwcXmlWriter);
    int					done= 0;
    int					noteRefCount= 0;

    int					afterSpace= 0;

    TextParticule *			tp= paraNode->biParaParticules+ part;
    int					stroff= tp->tpStroff;
    unsigned char *			s= docParaString( paraNode, stroff );

    TextAttribute			ta;

    const DocumentField *		df;
    const FieldKindInformation *	fki;

    int					len;

    BufferDocument *			bd= hwc->hwcDocument;

    while( part < partUpto )
	{
	int	textAttrNr;

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	switch( tp->tpKind )
	    {
	    case DOCkindTAB:
		docHtmlPutString( " ", hwc );
		afterSpace= 1;
		s++; stroff++;
		break;

	    case DOCkindSPAN:
		docHtmlChangeAttributes( hwc, textAttrNr );

		if  ( afterSpace && xw->xwColumn+ tp->tpStrlen > 76 )
		    { docHtmlNewLine( hwc );	}

		len= tp->tpStroff+ tp->tpStrlen- stroff;

		if  ( ! hwc->hwcInHyperlink	||
		      ! hwc->hwcInPageref	||
		      hwc->hwcBytesInLink == 0	)
		    {
		    docHtmlEscapeCharacters( hwc, (char *)s, len );

		    if  ( hwc->hwcInHyperlink )
			{ hwc->hwcBytesInLink += len;	}
		    }

		s += len;
		stroff += len;

		afterSpace= 0;
		if  ( tp->tpStrlen > 0 && s[-1] == ' ' )
		    { afterSpace= 1;	}
		break;

	    case DOCkindOBJECT:
		{
		int			pictureDone= 0;
		InsertedObject *	io;

		io= docGetObject( bd, tp->tpObjectNumber );
		if  ( ! io )
		    { LXDEB(tp->tpObjectNumber,io); return -1;	}

		if  ( docHtmlSaveImgElement( &pictureDone, hwc,
					tp->tpObjectNumber, paraNode, io ) < 0 )
		    { XDEB(io);	}

		if  ( ! pictureDone )
		    { docHtmlPutString( " ", hwc );	}
		}

		afterSpace= 0; s++; stroff++;
		break;

	    case DOCkindFIELDHEAD:
		df= docGetFieldByNumber( &(bd->bdFieldList),
						    tp->tpObjectNumber );
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevSPAN	)
		    {
		    docHtmlStartField( df, hwc, paraNode, textAttrNr );
		    }

		if  ( df->dfKind == DOCfkBOOKMARK )
		    {
		    docHtmlStartField( df, hwc, paraNode, textAttrNr );
		    }

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    int		count;
		    int		closed;
		    char	scratch[20+1];

		    count= docCountParticulesInField( paraNode, &closed,
							    part, partUpto );

		    docHtmlStartField( df, hwc, paraNode, textAttrNr );

		    if  ( paraNode->biTreeType == DOCinBODY )
			{
			sprintf( scratch, "%d", hwc->hwcNoteRefCount+ 1 );
			hwc->hwcNoteRefCount++;
			noteRefCount++;
			}
		    else{
			sprintf( scratch, "%d", hwc->hwcNoteDefCount+ 1 );
			hwc->hwcNoteDefCount++;
			}

		    docHtmlPutString( scratch, hwc );

		    if  ( hwc->hwcInHyperlink )
			{ hwc->hwcBytesInLink += strlen( scratch );	}

		    done= count+ 1;
		    stroff= tp[done].tpStroff; s= docParaString( paraNode, stroff );

		    if  ( paraNode->biTreeType == DOCinBODY	&&
			  hwc->hwcInlineNotes			)
			{
			if  ( utilIndexMappingAppend( &(hwc->hwcDeferredNotes),
					    (int *)0, tp->tpObjectNumber ) )
			    { LDEB(tp->tpObjectNumber);	}
			}

		    break;
		    }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		done= 1;
		break;

	    case DOCkindFIELDTAIL:
		df= docGetFieldByNumber( &(bd->bdFieldList),
						    tp->tpObjectNumber );
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    docHtmlFinishField( df, hwc );
		    }

		if  ( df->dfKind == DOCfkBOOKMARK )
		    { docHtmlFinishField( df, hwc ); }

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevSPAN	)
		    { docHtmlFinishField( df, hwc ); }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		done= 1;
		break;

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		docHtmlPutString( "<br/>", hwc );
		docHtmlNewLine( hwc );
		afterSpace= 0;
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		break;

	    case DOCkindOPT_HYPH:
	    case DOCkindLTR_MARK:
	    case DOCkindRTL_MARK:
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		break;

	    default:
		LSDEB(tp->tpKind,docKindStr(tp->tpKind));
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		break;
	    }

	done++; part++; tp++;
	}

    docHtmlChangeAttributes( hwc, -1 );

    *pNoteRefCount += noteRefCount;
    return done;
    }

static void docHtmlStartParagraphBody(	const BufferItem *	paraNode,
					int *			pBulletTab,
					const char *		tag,
					int			fontHeight,
					HtmlWritingContext *	hwc )
    {
    const BufferDocument *		bd= hwc->hwcDocument;

    XmlWriter *				xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *		sos= xw->xwSos;

    ItemShading				is;

    int					hasBullet= 0;
    int					part= 0;

    docGetItemShadingByNumber( &is, bd, paraNode->biParaShadingNumber );

    if  ( hwc->hwcSupportsBullets				&&
	  paraNode->biParaLeftIndentTwips >=  100		&&
	  paraNode->biParaFirstIndentTwips <= 100		)
	{
	int			x;
	const TextParticule *	tp;

	x= 0;
	part= 0;
	tp= paraNode->biParaParticules;
	for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	    {
	    x += tp->tpTwipsWide;

	    if  ( tp->tpKind == DOCkindTAB )
		{
		if  ( x <= paraNode->biParaLeftIndentTwips )
		    { hasBullet= 1;	}
		break;
		}
	    }
	}

    docHtmlPutString( "<", hwc );
    docHtmlPutString( tag, hwc );
    docHtmlPutString( " style=\"", hwc );

    if  ( paraNode->biParaLeftIndentTwips <= -100	||
	  paraNode->biParaLeftIndentTwips >=  100	)
	{
	docHtmlPutString( "padding-left:", hwc );
	sioOutPrintf( sos, "%dpt;",
		    ( paraNode->biParaLeftIndentTwips+ 10 )/ 20 );
	xw->xwColumn += 6;
	}

    if  ( ( paraNode->biParaFirstIndentTwips <= -100	||
	    paraNode->biParaFirstIndentTwips >=  100	)	)
	{
	docHtmlPutString( "text-indent:", hwc );
	sioOutPrintf( sos, "%dpt;", paraNode->biParaFirstIndentTwips/ 20 );
	xw->xwColumn += 6;
	}

    if  ( is.isPattern == DOCspSOLID )
	{ docCssEmitBackgroundStyle( &(xw->xwColumn), sos, bd, &is ); }

    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-top", paraNode->biParaTopBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-left", paraNode->biParaLeftBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-right", paraNode->biParaRightBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-bottom", paraNode->biParaBottomBorderNumber );

    if  ( paraNode->biParaSpaceBeforeTwips > 0 )
	{
	docHtmlPutString( "margin-top:", hwc );
	sioOutPrintf( sos, "%dpt;",
			( paraNode->biParaSpaceBeforeTwips+ 10 )/ 20 );
	xw->xwColumn += 6;
	}
    if  ( paraNode->biParaSpaceAfterTwips > 0 )
	{
	docHtmlPutString( "margin-bottom:", hwc );
	sioOutPrintf( sos, "%dpt;",
			( paraNode->biParaSpaceAfterTwips+ 10 )/ 20 );
	xw->xwColumn += 6;
	}

    switch( paraNode->biParaAlignment )
	{
	case DOCthaLEFT:
	    break;
	case DOCthaRIGHT:
	    docHtmlPutString( " text-align:right;", hwc );
	    break;

	case DOCthaCENTERED:
	    docHtmlPutString( " text-align:center;", hwc );
	    break;

	case DOCthaJUSTIFIED:
	    docHtmlPutString( " text-align:justify;", hwc );
	    break;

	default:
	    LDEB(paraNode->biParaAlignment);
	    break;
	}

    docHtmlPutString( "\"", hwc );
    docHtmlPutString( ">", hwc );

    if  ( paraNode->biParaSpaceBeforeTwips > fontHeight/ 2 )
	{
	docHtmlPutString( "&#160;<br/>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( hasBullet )
	{ *pBulletTab= part;		}
    else{ *pBulletTab= -1;		}

    return;
    }

/************************************************************************/
/*									*/
/*  Save a 'Paragraph'							*/
/*									*/
/*  But not as a <P>...</P>.						*/
/*									*/
/************************************************************************/

int docHtmlSaveParaNode(	HtmlWritingContext *		hwc,
				const BufferItem *		paraNode,
				const BufferItem *		bodySectBi,
				const DocumentSelection *	ds )
    {
    int				part= 0;
    int				noteRefCount= 0;

    TextAttribute		ta;
    int				fontHeight;

    PropertyMask		ppSetMask;
    int				bulletTab= 0;

    BufferDocument *		bd= hwc->hwcDocument;

    if  ( paraNode->biParaParticuleCount == 0		||
	  docParaStrlen( paraNode ) == 0		)
	{
	docHtmlPutString( "<div>&#160;</div>", hwc );
	docHtmlNewLine( hwc );
	return 0;
	}

    part= 0;
    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    fontHeight= 10* ta.taFontSizeHalfPoints;

    docHtmlStartParagraphBody( paraNode, &bulletTab, "div", fontHeight, hwc );

    if  ( bulletTab >= 0 )
	{
	docHtmlPutString( "<div", hwc );
	docHtmlWriteStringAttribute( hwc, "style", "float: left;" );
	docHtmlPutString( ">", hwc );

	if  ( docHtmlSaveParticules( paraNode, &noteRefCount, 0,
						    bulletTab, hwc ) < 0 )
	    { LDEB(bulletTab); return -1;	}

	docHtmlPutString( "</div>", hwc );		
	docHtmlPutString( "<div", hwc );
	docHtmlWriteStringAttribute( hwc, "style", "text-indent: 0;" );
	docHtmlPutString( ">", hwc );

	part= bulletTab+ 1;
	}

    if  ( docHtmlSaveParticules( paraNode, &noteRefCount, part,
				paraNode->biParaParticuleCount, hwc ) < 0 )
	{ LDEB(part); return -1;	}

    if  ( hwc->hwcInlineNotes		&&
	  noteRefCount > 0		&&
	  hwc->hwcTableNesting == 0	)
	{
	if  ( docHtmlSaveDeferredNotes( hwc ) )
	    { LDEB(noteRefCount); return -1;	}
	}

    if  ( bulletTab >= 0 )
	{ docHtmlPutString( "</div>", hwc );		}

    if  ( paraNode->biParaSpaceAfterTwips > fontHeight/ 2 )
	{ docHtmlPutString( "<br/>&#160;</div>", hwc );	}
    else{
	docHtmlPutString( "</div>", hwc );		
	}
    docHtmlNewLine( hwc );

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
				    &(hwc->hwcParagraphProperties),
				    &ppSetMask, &(paraNode->biParaProperties),
				    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1);	}

    return 0;
    }


