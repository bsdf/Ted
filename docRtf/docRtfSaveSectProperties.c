/************************************************************************/
/*									*/
/*  Exchange of section properties with RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"

/************************************************************************/
/*									*/
/*  Write Section Properties. For a node, or for the stylesheet.	*/
/*									*/
/************************************************************************/

void docRtfSaveSectionProperties( RtfWriter *			rw,
				const PropertyMask *		spMask,
				const SectionProperties *	sp )
    {
    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);

    /**************/

    if  ( PROPmaskISSET( spMask, DGpropPAGE_WIDTH ) )
	{ docRtfWriteArgTag( rw, "pgwsxn",	dg->dgPageWideTwips );	}
    if  ( PROPmaskISSET( spMask, DGpropPAGE_HEIGHT ) )
	{ docRtfWriteArgTag( rw, "pghsxn",	dg->dgPageHighTwips );	}
    if  ( PROPmaskISSET( spMask, DGpropLEFT_MARGIN ) )
	{ docRtfWriteArgTag( rw, "marglsxn",dg->dgLeftMarginTwips );	}
    if  ( PROPmaskISSET( spMask, DGpropTOP_MARGIN ) )
	{
	docRtfWriteArgTag( rw, "margtsxn",dg->dgTopMarginTwips );
	}
    if  ( PROPmaskISSET( spMask, DGpropRIGHT_MARGIN ) )
	{
	docRtfWriteArgTag( rw, "margrsxn",dg->dgRightMarginTwips );
	}
    if  ( PROPmaskISSET( spMask, DGpropBOTTOM_MARGIN ) )
	{
	docRtfWriteArgTag( rw, "margbsxn",dg->dgBottomMarginTwips );
	}
    if  ( PROPmaskISSET( spMask, DGpropGUTTER ) )
	{
	docRtfWriteArgTag( rw, "guttersxn",dg->dgGutterTwips );
	}
    if  ( PROPmaskISSET( spMask, DGpropMARGMIR ) && dg->dgMirrorMargins )
	{
	docRtfWriteTag( rw, "margmirsxn" );
	}
    if  ( PROPmaskISSET( spMask, DGpropHEADER_POSITION ) )
	{
	docRtfWriteArgTag( rw, "headery", dg->dgHeaderPositionTwips );
	}
    if  ( PROPmaskISSET( spMask, DGpropFOOTER_POSITION ) )
	{
	docRtfWriteArgTag( rw, "footery", dg->dgFooterPositionTwips );
	}

    /**************/

    if  ( PROPmaskISSET( spMask, SPpropRTOL ) )
	{
	docRtfWriteAltTag( rw, "rtlsect", "ltrsect", sp->spRToL );
	}

    if  ( PROPmaskISSET( spMask, SPpropSTYLE ) )
	{ docRtfWriteArgTag( rw, "ds",sp->spStyle );	}

    if  ( PROPmaskISSET( spMask, SPpropTITLEPG ) )
	{ docRtfWriteFlagTag( rw, "titlepg", sp->spHasTitlePage );	}

    if  ( PROPmaskISSET( spMask, SPpropBREAK_KIND ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_SectBreakTags, sp->spBreakKind,
				    DOCrtf_SectBreakTagCount, DOCibk_COUNT );
	}

    if  ( PROPmaskISSET( spMask, SPpropNUMBER_STYLE ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_PageNumberStyleTags,
			    sp->spPageNumberStyle,
			    DOCrtf_PageNumberStyleTagCount, DOCpgn_COUNT );
	}

    if  ( PROPmaskISSET( spMask, SPpropNUMBER_HYPHEN ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_PageNumberHyphenTags,
			    sp->spPageNumberHyphen,
			    DOCrtf_PageNumberHyphenTagCount, DOCpgnh_COUNT );
	}

    if  ( PROPmaskISSET( spMask, SPpropPAGE_RESTART ) )
	{
	docRtfWriteAltTag( rw, "pgnrestart", "pgncont",
						sp->spRestartPageNumbers );
	}

    if  ( PROPmaskISSET( spMask, SPpropSTART_PAGE ) )
	{
	docRtfWriteArgTag( rw, "pgnstarts", sp->spStartPageNumber+ 1 );
	}

    if  ( PROPmaskISSET( spMask, SPpropCOLUMN_COUNT ) )
	{ docRtfWriteArgTag( rw, "cols", sp->spColumnCount );	}
    if  ( PROPmaskISSET( spMask, SPpropCOLUMN_SPACING ) )
	{ docRtfWriteArgTag( rw, "colsx", sp->spColumnSpacingTwips ); }

    if  ( PROPmaskISSET( spMask, SPpropCOLUMNS )	||
	  sp->spColumnCount > 1				)
	{
	int			i;
	const SectionColumn *	sc= sp->spColumns;


	for ( i= 0; i < sp->spColumnCount; sc++, i++ )
	    {
	    if  ( sc->scColumnWidthTwips == 0	&&
		  sc->scSpaceToRightTwips == 0	)
		{ continue;	}

	    docRtfWriteArgTag( rw, "colno", i+ 1 );

	    if  ( sc->scColumnWidthTwips != 0 )
		{
		docRtfWriteArgTag( rw, "colw", sc->scColumnWidthTwips );
		}
	    if  ( sc->scSpaceToRightTwips != 0 )
		{
		docRtfWriteArgTag( rw, "colsr", sc->scSpaceToRightTwips );
		}
	    }
	}

    if  ( PROPmaskISSET( spMask, SPpropLINEBETCOL ) )
	{ docRtfWriteFlagTag( rw, "linebetcol", sp->spLineBetweenColumns ); }

    docRtfWriteNextLine( rw );

    /**************/

    docRtfSaveNotesProperties( rw, spMask,
			    &(sp->spNotesProperties.fepFootnotesProps),
			    DOCsectFOOTNOTE_PROP_MAP, "sftnstart",
			    DOCrtf_SectFootNotesJustificationTags,
			    DOCrtf_SectFootNotesJustificationTagCount,
			    (const char **)0, 0,
			    DOCrtf_SectFootNotesRestartTags,
			    DOCrtf_SectFootNotesRestartTagCount,
			    DOCrtf_SectFootNotesNumberStyleTags,
			    DOCrtf_SectFootNotesNumberStyleTagCount );

    docRtfSaveNotesProperties( rw, spMask,
			    &(sp->spNotesProperties.fepEndnotesProps),
			    DOCsectENDNOTE_PROP_MAP, "saftnstart",
			    DOCrtf_SectEndNotesJustificationTags,
			    DOCrtf_SectEndNotesJustificationTagCount,
			    (const char **)0, 0,
			    DOCrtf_SectEndNotesRestartTags,
			    DOCrtf_SectEndNotesRestartTagCount,
			    DOCrtf_SectEndNotesNumberStyleTags,
			    DOCrtf_SectEndNotesNumberStyleTagCount );

    return;
    }

/************************************************************************/
/*									*/
/*  Write Section Properties for a node					*/
/*									*/
/*  1)  Only save geometry if it differs from that of the document.	*/
/*  2)  Then always start from the default for this document.		*/
/*									*/
/************************************************************************/

int docRtfSaveSectionPropertiesOfNode(
				    RtfWriter *			rw,
				    const DocumentSelection *	ds,
				    const BufferItem *		sectNode )
    {
    const SectionProperties *	sp= &(sectNode->biSectProperties);
    const DocumentProperties *	dp= &(rw->rwDocument->bdProperties);

    SectionProperties		spDef;
    PropertyMask		dgSetMask;
    PropertyMask		spCmpMask;
    PropertyMask		spDifMask;

    /*  1  */
    docInitSectionProperties( &spDef );

    utilPropMaskClear( &dgSetMask );
    utilPropMaskFill( &dgSetMask, DGprop_COUNT );
    PROPmaskUNSET( &dgSetMask, DGpropHEADER_POSITION );
    PROPmaskUNSET( &dgSetMask, DGpropFOOTER_POSITION );

    utilUpdDocumentGeometry( (PropertyMask *)0, &(spDef.spDocumentGeometry),
						&dgSetMask, &(dp->dpGeometry) );

    spDef.spNotesProperties= dp->dpNotesProps;

    utilPropMaskClear( &spCmpMask );
    utilPropMaskFill( &spCmpMask, SPprop_COUNT );

    utilPropMaskClear( &spDifMask );
    docSectPropertyDifference( &spDifMask, &spDef, &spCmpMask, sp );

    if  ( ds )
	{ PROPmaskUNSET( &spDifMask, SPpropTITLEPG );	}

    if  ( rw->rwSpExtraMask )
	{ utilPropMaskOr( &spDifMask, &spDifMask, rw->rwSpExtraMask );	}

    docRtfWriteNextLine( rw );
    docRtfWriteTag( rw, "sectd" );

    docRtfSaveSectionProperties( rw, &spDifMask, sp );

    rw->rwSectionPropertiesSaved= 1;

  /*ready:*/

    docCleanSectionProperties( &spDef );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a header or a footer.. If it exists.				*/
/*									*/
/************************************************************************/

static int docRtfSaveSectHeaderFooter(	RtfWriter *		rw,
					const BufferItem *	sectNode,
					const BufferItem *	prevBi,
					const char *		tag,
					int			treeType )
    {
    const int			forcePar= 1;
    DocumentTree *		dt;
    int				evenIfAbsent= 0;
    unsigned char		applies= 1;

    dt= docSectionHeaderFooter( sectNode, &applies,
				&(rw->rwDocument->bdProperties), treeType );

    if  ( prevBi )
	{
	const DocumentTree *	prevDt;

	prevDt= docSectionHeaderFooter( prevBi, (unsigned char *)0,
				&(rw->rwDocument->bdProperties), treeType );
	if  ( prevDt && prevDt->dtRoot )
	    { evenIfAbsent= 1;	}
	}

    if  ( dt )
	{
	if  ( docRtfSaveDocumentTree( rw, tag, dt, evenIfAbsent, forcePar ) )
	    { LSDEB(treeType,tag); return -1;	}
	}

    return 0;
    }

/************************************************************************/

typedef struct HeaderFooterType
    {
    const char *	hftTag;
    int			htfTreeType;
    } HeaderFooterType;

static const HeaderFooterType	HeaderFooterTypes[]=
{
    { "headerf", DOCinFIRST_HEADER,	},
    { "headerl", DOCinLEFT_HEADER,	},
    { "headerr", DOCinRIGHT_HEADER,	},
    { "footerf", DOCinFIRST_FOOTER,	},
    { "footerl", DOCinLEFT_FOOTER,	},
    { "footerr", DOCinRIGHT_FOOTER,	},
};

static const int HeaderFooterTypeCount=
			    sizeof(HeaderFooterTypes)/sizeof(HeaderFooterType);

/************************************************************************/
/*									*/
/*  Save the headers and footers of a Section.				*/
/*									*/
/************************************************************************/

int docRtfSaveSectHeadersFooters(	RtfWriter *		rw,
					const BufferItem *	sectNode )
    {
    const BufferItem *		prevBi= (const BufferItem *)0;
    const DocumentProperties *	dp= &(rw->rwDocument->bdProperties);

    int				hdft;

    if  ( sectNode->biParent				&&
	  sectNode->biNumberInParent > 0			)
	{ prevBi= sectNode->biParent->biChildren[ sectNode->biNumberInParent- 1]; }

    /*  Word 11+ uses right header/footer anyway. For compatibility: */
    if  ( ! dp->dpHasFacingPages )
	{
	if  ( docRtfSaveSectHeaderFooter( rw, sectNode, prevBi,
					    "header", DOCinRIGHT_HEADER ) )
	    { LDEB(1); return -1;	}
	}

    /*  Word 11+ uses right header/footer anyway. For compatibility: */
    if  ( ! dp->dpHasFacingPages )
	{
	if  ( docRtfSaveSectHeaderFooter( rw, sectNode, prevBi,
					    "footer", DOCinRIGHT_FOOTER ) )
	    { LDEB(1); return -1;	}
	}

    for ( hdft= 0; hdft < HeaderFooterTypeCount; hdft++ )
	{
	const HeaderFooterType* hft= HeaderFooterTypes+ hdft;

	if  ( docRtfSaveSectHeaderFooter( rw, sectNode, prevBi,
					    hft->hftTag, hft->htfTreeType ) )
	    { SDEB(hft->hftTag); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save an individial header or footer.				*/
/*  Used to trace just this header or footer.				*/
/*									*/
/************************************************************************/

int docRtfSaveHeaderFooter(	RtfWriter *		rw,
				const DocumentTree *	dt )
    {
    const int			forcePar= 1;
    const int			evenIfAbsent= 0;

    int				hdft;

    if  ( ! dt->dtRoot )
	{ XDEB(dt->dtRoot); return -1;	}

    for ( hdft= 0; hdft < HeaderFooterTypeCount; hdft++ )
	{
	const HeaderFooterType* hft= HeaderFooterTypes+ hdft;

	if  ( hft->htfTreeType != dt->dtRoot->biTreeType )
	    { continue;	}

	if  ( docRtfSaveDocumentTree( rw, hft->hftTag, dt,
						    evenIfAbsent, forcePar ) )
	    { SDEB(hft->hftTag); return -1;	}

	return 0;
	}

    SDEB(docTreeTypeStr(dt->dtRoot->biTreeType));
    return -1;
    }
