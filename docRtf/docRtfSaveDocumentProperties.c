/************************************************************************/
/*									*/
/*  Read/Write document properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<textOfficeCharset.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"

/************************************************************************/
/*									*/
/*  Save Document Properties to RTF.					*/
/*									*/
/************************************************************************/

static int docRtfSaveInfo(	RtfWriter *		rw,
				const char *		tag,
				const MemoryBuffer *	info )
    {
    const int		addSemicolon= 0;

    docRtfWriteDocEncodedStringDestination( rw, tag,
				    (const char *)info->mbBytes,
				    info->mbSize, addSemicolon );

    docRtfWriteNextLine( rw );

    return 0;
    }

int docRtfSaveDate(	RtfWriter *		rw,
			const char *		tag,
			const struct tm *	tm )
    {
    docRtfWriteDestinationBegin( rw, tag );

    if  ( tm->tm_mday != 0 )
	{
	docRtfWriteArgTag( rw, "yr",	tm->tm_year+ 1900 );
	docRtfWriteArgTag( rw, "mo",	tm->tm_mon+ 1 );
	docRtfWriteArgTag( rw, "dy",	tm->tm_mday );
	docRtfWriteArgTag( rw, "hr",	tm->tm_hour );
	docRtfWriteArgTag( rw, "min",	tm->tm_min );
	docRtfWriteArgTag( rw, "sec",	tm->tm_sec );
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

int docRtfSaveDocumentProperties(	RtfWriter *			rw,
					int				fet,
					const PropertyMask *		dpMask,
					const DocumentProperties *	dp )
    {
    const int			anyway= 1;
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    /* NO! We always write ansi 1252 documents
    int				ansicpg= dp->dpAnsiCodepage;

    if  ( ansicpg >= 0 )
	{
	if  ( docRtfSetAnsicpg( rw->rwRtfTextConverter, ansicpg ) )
	    { LDEB(ansicpg);	}
	}
    */

    if  ( PROPmaskISSET( dpMask, DPpropDEFF ) )
	{
	int	deff= docRtfWriteGetDefaultFont( rw, dp->dpDefaultFont );

	if  ( deff >= 0 )
	    { docRtfWriteArgTag( rw, "deff", deff );	}
	}

    if  ( PROPmaskISSET( dpMask, DPpropDEFLANG ) )
	{ docRtfWriteArgTag( rw, "deflang", dp->dpDefaultLanguage );	}

    if  ( PROPmaskISSET( dpMask, DPpropRTOL ) )
	{
	docRtfWriteAltTag( rw, "rtldoc", "ltrdoc", dp->dpRToL );
	}

    /* Strictly spoken, this is not a document property */
    docRtfWriteArgTag( rw, "uc", 1 );

    if  ( PROPmaskISSET( dpMask, DPpropFONTTABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteFontTable( rw );
	}

    if  ( PROPmaskISSET( dpMask, DPpropCOLORTABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteColorTable( rw, dp );
	}

    if  ( PROPmaskISSET( dpMask, DPpropSTYLESHEET ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteStyleSheet( rw, &(rw->rwDocument->bdStyleSheet) );
	}

    if  ( PROPmaskISSET( dpMask, DPpropLISTTABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteListTable( rw, &(dp->dpListAdmin->laListTable) );
	}

    if  ( PROPmaskISSET( dpMask, DPpropLISTOVERRIDETABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteListOverrideTable( rw,
				    &(dp->dpListAdmin->laListOverrideTable) );
	}

    if  ( PROPmaskISSET( dpMask, DPpropGENERATOR ) )
	{ docRtfSaveInfo( rw, RTFtag__generator, &(dp->dpGeneratorWrite) ); }

    if  ( PROPmaskISSET( dpMask, DPpropTITLE )		||
	  PROPmaskISSET( dpMask, DPpropAUTHOR )		||
	  PROPmaskISSET( dpMask, DPpropCOMPANY )	||
	  PROPmaskISSET( dpMask, DPpropSUBJECT )	||
	  PROPmaskISSET( dpMask, DPpropKEYWORDS )	||
	  PROPmaskISSET( dpMask, DPpropDOCCOMM )	||
	  PROPmaskISSET( dpMask, DPpropHLINKBASE )	||
	  PROPmaskISSET( dpMask, DPpropCREATIM )	||
	  PROPmaskISSET( dpMask, DPpropREVTIM )		||
	  PROPmaskISSET( dpMask, DPpropPRINTIM )	)
	{
	docRtfWriteNextLine( rw );
	docRtfWriteDestinationBegin( rw, "info" );
	docRtfWriteNextLine( rw );

	if  ( PROPmaskISSET( dpMask, DPpropTITLE ) )
	    { docRtfSaveInfo( rw, "title",	&(dp->dpTitle) );	}
	if  ( PROPmaskISSET( dpMask, DPpropAUTHOR ) )
	    { docRtfSaveInfo( rw, "author",	&(dp->dpAuthor) );	}
	if  ( PROPmaskISSET( dpMask, DPpropCOMPANY ) )
	    { docRtfSaveInfo( rw, "company",	&(dp->dpCompany) );	}
	if  ( PROPmaskISSET( dpMask, DPpropSUBJECT ) )
	    { docRtfSaveInfo( rw, "subject",	&(dp->dpSubject) );	}
	if  ( PROPmaskISSET( dpMask, DPpropKEYWORDS ) )
	    { docRtfSaveInfo( rw, "keywords",	&(dp->dpKeywords) );	}
	if  ( PROPmaskISSET( dpMask, DPpropDOCCOMM ) )
	    { docRtfSaveInfo( rw, "doccomm",	&(dp->dpDoccomm) );	}
	if  ( PROPmaskISSET( dpMask, DPpropHLINKBASE ) )
	    { docRtfSaveInfo( rw, RTFtag_hlinkbase, &(dp->dpHlinkbase) );	}

	if  ( PROPmaskISSET( dpMask, DPpropCREATIM ) )
	    { docRtfSaveDate( rw, "creatim",	&(dp->dpCreatim) );	}
	if  ( PROPmaskISSET( dpMask, DPpropREVTIM ) )
	    { docRtfSaveDate( rw, RTFtag_revtim, &(dp->dpRevtim) );	}
	if  ( PROPmaskISSET( dpMask, DPpropPRINTIM ) )
	    { docRtfSaveDate( rw, "printim",	&(dp->dpPrintim) );	}

	docRtfWriteDestinationEnd( rw );
	}

    docRtfWriteNextLine( rw );

    if  ( PROPmaskISSET( dpMask, DGpropPAGE_WIDTH ) )
    	{ docRtfWriteArgTag( rw, "paperw", dg->dgPageWideTwips );	}
    if  ( PROPmaskISSET( dpMask, DGpropPAGE_HEIGHT ) )
    	{ docRtfWriteArgTag( rw, "paperh", dg->dgPageHighTwips );	}

    if  ( PROPmaskISSET( dpMask, DGpropLEFT_MARGIN ) )
    	{ docRtfWriteArgTag( rw, "margl", dg->dgLeftMarginTwips );	}
    if  ( PROPmaskISSET( dpMask, DGpropRIGHT_MARGIN ) )
    	{ docRtfWriteArgTag( rw, "margr", dg->dgRightMarginTwips );	}
    if  ( PROPmaskISSET( dpMask, DGpropTOP_MARGIN ) )
    	{ docRtfWriteArgTag( rw, "margt", dg->dgTopMarginTwips );	}
    if  ( PROPmaskISSET( dpMask, DGpropBOTTOM_MARGIN ) )
    	{ docRtfWriteArgTag( rw, "margb", dg->dgBottomMarginTwips );	}

    if  ( PROPmaskISSET( dpMask, DGpropGUTTER ) )
	{ docRtfWriteArgTag( rw, "gutter", dg->dgGutterTwips ); }
    if  ( PROPmaskISSET( dpMask, DGpropMARGMIR ) )
	{ docRtfWriteTag( rw, "margmirror" ); }

    if  ( PROPmaskISSET( dpMask, DPpropFACING_PAGES ) )
	{ docRtfWriteFlagTag( rw, "facingp", dp->dpHasFacingPages );	}
    if  ( PROPmaskISSET( dpMask, DPpropGUTTER_HORIZONTAL ) )
	{ docRtfWriteFlagTag( rw, "gutterprl", dp->dpGutterHorizontal ); }

    if  ( PROPmaskISSET( dpMask, DPpropWIDOWCTRL ) )
	{ docRtfWriteFlagTag( rw, "widowctrl", dp->dpWidowControl );	}

    if  ( PROPmaskISSET( dpMask, DPpropTWO_ON_ONE ) )
	{ docRtfWriteFlagTag( rw, "twoonone", dp->dpTwoOnOne );	}
    if  ( PROPmaskISSET( dpMask, DPpropDOCTEMP ) )
	{ docRtfWriteFlagTag( rw, "doctemp", dp->dpIsDocumentTemplate ); }

    if  ( dp->dpTabIntervalTwips > 0 && dp->dpTabIntervalTwips != 720 )
	{ docRtfWriteArgTag( rw, "deftab", dp->dpTabIntervalTwips ); }

    docRtfWriteNextLine( rw );

    if  ( fet >= 0 )
	{ docRtfWriteArgTag( rw, RTFtag_fet, fet );	}

    docRtfSaveNotesProperties( rw, dpMask,
			    &(dp->dpNotesProps.fepFootnotesProps),
			    DOCdocFOOTNOTE_PROP_MAP, "ftnstart",
			    DOCrtf_DocFootNotesJustificationTags,
			    DOCrtf_DocFootNotesJustificationTagCount,
			    DOCrtf_DocFootNotesPlacementTags,
			    DOCrtf_DocFootNotesPlacementTagCount,
			    DOCrtf_DocFootNotesRestartTags,
			    DOCrtf_DocFootNotesRestartTagCount,
			    DOCrtf_DocFootNotesNumberStyleTags,
			    DOCrtf_DocFootNotesNumberStyleTagCount );

    docRtfSaveNotesProperties( rw, dpMask,
			    &(dp->dpNotesProps.fepEndnotesProps),
			    DOCdocENDNOTE_PROP_MAP, "aftnstart",
			    DOCrtf_DocEndNotesJustificationTags,
			    DOCrtf_DocEndNotesJustificationTagCount,
			    DOCrtf_DocEndNotesPlacementTags,
			    DOCrtf_DocEndNotesPlacementTagCount,
			    DOCrtf_DocEndNotesRestartTags,
			    DOCrtf_DocEndNotesRestartTagCount,
			    DOCrtf_DocEndNotesNumberStyleTags,
			    DOCrtf_DocEndNotesNumberStyleTagCount );

    if  ( PROPmaskISSET( dpMask, DPpropTOP_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "pgbrdrt",
					dp->dpTopBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( dpMask, DPpropBOTTOM_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "pgbrdrb",
					dp->dpBottomBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( dpMask, DPpropLEFT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "pgbrdrl",
					dp->dpLeftBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( dpMask, DPpropRIGHT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "pgbrdrr",
					dp->dpRightBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( dpMask, DPpropHEAD_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "pgbrdrhead",
					dp->dpHeadBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( dpMask, DPpropFOOT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "pgbrdrfoot",
					dp->dpFootBorderNumber, anyway );
	}

    docRtfWriteNextLine( rw );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the various separators relating to notes.			*/
/*									*/
/************************************************************************/

int docRtfSaveDocNotesSeparators(	RtfWriter *		rw,
					const BufferDocument *	bd )
    {
    const int			evenIfAbsent= 0;
    const int			forcePar= 0;

    if  ( docRtfSaveDocumentTree( rw, "ftnsep", &(bd->bdEiFtnsep),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "ftnsepc", &(bd->bdEiFtnsepc),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "ftncn", &(bd->bdEiFtncn),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    /******/

    if  ( docRtfSaveDocumentTree( rw, "aftnsep", &(bd->bdEiAftnsep),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "aftnsepc", &(bd->bdEiAftnsepc),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "aftncn", &(bd->bdEiAftncn),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the mask of scalar properties that differ from the	*/
/*  defaults.								*/
/*									*/
/************************************************************************/

int docRtfDocPropMask(	PropertyMask *			dpSaveMask,
			const DocumentProperties *	dpDoc )
    {
    int				rval= 0;

    PropertyMask		dpSetMask;
    DocumentProperties		dpDef;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    docInitDocumentProperties( &dpDef );

    utilPropMaskClear( &dpSetMask );
    utilPropMaskFill( &dpSetMask, DPprop_COUNT );
    utilPropMaskClear( dpSaveMask );

    if  ( docUpdDocumentProperties( dpSaveMask, &dpDef,
						    &dpSetMask, dpDoc, dam0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentProperties( &dpDef );

    return rval;
    }
