/************************************************************************/
/*									*/
/*  Read/Write paragraph properties to/from RTF.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	<docParaRulerAdmin.h>
#   include	"docRtfTags.h"

/************************************************************************/

void docRtfSaveParaTableNesting(	RtfWriter *		rw,
					int			nest )
    {
    if  ( rw->rwDeepestTableNesting < nest )
	{ rw->rwDeepestTableNesting=  nest;	}

    if  ( rw->rwDeepestTableNesting > 1 )
	{
	switch( nest )
	    {
	    case 0:
		docRtfWriteArgTag( rw, "itap", nest );
		break;
	    case 1:
	    default:
		docRtfWriteArgTag( rw, "intbl\\itap", nest );
		break;
	    }
	}
    else{
	switch( nest )
	    {
	    case 0:
		docRtfWriteArgTag( rw, "intbl", nest );
		break;
	    case 1:
		docRtfWriteTag( rw, "intbl" );
		break;
	    default:
		LLDEB(rw->rwDeepestTableNesting,nest);
		docRtfWriteArgTag( rw, "intbl\\itap", nest );
		break;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save Paragraph Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveParagraphProperties(
				RtfWriter *			rw,
				const PropertyMask *		updMask,
				const ParagraphProperties *	pp )
    {
    const int			anyway= 1;

    if  ( PROPmaskISSET( updMask, PPpropSTYLE ) )
	{ docRtfWriteArgTag( rw, "s", pp->ppStyle ); }

    if  ( PROPmaskISSET( updMask, PPpropLISTOVERRIDE ) )
	{ docRtfWriteArgTag( rw, "ls", pp->ppListOverride ); }
    if  ( PROPmaskISSET( updMask, PPpropLISTLEVEL ) )
	{ docRtfWriteArgTag( rw, "ilvl", pp->ppListLevel ); }

    if  ( PROPmaskISSET( updMask, PPpropTABLE_NESTING ) )
	{ docRtfSaveParaTableNesting( rw, pp->ppTableNesting ); }

    if  ( PROPmaskISSET( updMask, PPpropHYPHPAR ) )
	{ docRtfWriteFlagTag( rw, "hyphpar", pp->ppHyphenateParagraph ); }

    if  ( PROPmaskISSET( updMask, PPpropWIDCTLPAR ) )
	{
	docRtfWriteAltTag( rw, "widctlpar", "nowidctlpar", pp->ppWidowControl );
	}

    if  ( PROPmaskISSET( updMask, PPpropRTOL ) )
	{
	docRtfWriteAltTag( rw, "rtlpar", "ltrpar", pp->ppRToL );
	}

    if  ( PROPmaskISSET( updMask, PPpropKEEP ) )
	{ docRtfWriteFlagTag( rw, "keep", pp->ppKeepOnPage );	}

    if  ( PROPmaskISSET( updMask, PPpropKEEPN ) )
	{ docRtfWriteFlagTag( rw, "keepn", pp->ppKeepWithNext );	}

    if  ( PROPmaskISSET( updMask, PPpropOUTLINELEVEL ) )
	{ docRtfWriteArgTag( rw, "outlinelevel", pp->ppOutlineLevel ); }

    if  ( PROPmaskISSET( updMask, PPpropFIRST_INDENT ) )
	{ docRtfWriteArgTag( rw, "fi", pp->ppFirstIndentTwips ); }

    if  ( PROPmaskISSET( updMask, PPpropLEFT_INDENT ) )
	{ docRtfWriteArgTag( rw, "li", pp->ppLeftIndentTwips ); }

    if  ( PROPmaskISSET( updMask, PPpropRIGHT_INDENT ) )
	{ docRtfWriteArgTag( rw, "ri", pp->ppRightIndentTwips ); }

    if  ( PROPmaskISSET( updMask, PPpropSPACE_BEFORE ) )
	{ docRtfWriteArgTag( rw, "sb", pp->ppSpaceBeforeTwips ); }

    if  ( PROPmaskISSET( updMask, PPpropSPACE_AFTER ) )
	{ docRtfWriteArgTag( rw, "sa", pp->ppSpaceAfterTwips ); }

    if  ( PROPmaskISSET( updMask, PPpropLINE_SPACING_DIST ) ||
	  PROPmaskISSET( updMask, PPpropLINE_SPACING_MULT ) )
	{
	docRtfWriteArgTag( rw, "sl", pp->ppLineSpacingTwips );
	docRtfWriteArgTag( rw, "slmult", pp->ppLineSpacingIsMultiple );
	}

    if  ( PROPmaskISSET( updMask, PPpropALIGNMENT ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_ParaAlignTags,
			    pp->ppAlignment,
			    DOCrtf_ParaAlignTagCount, DOCtha_COUNT );
	}

    if  ( PROPmaskISSET( updMask, PPpropTAB_STOPS )	&&
	  pp->ppTabStopListNumber >= 0			)
	{
	TabStopList	tsl;

	docGetTabStopListByNumber( &tsl, rw->rwDocument,
						    pp->ppTabStopListNumber );

	docRtfSaveTabStopList( rw, &tsl );
	}

    if  ( PROPmaskISSET( updMask, PPpropTOP_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "brdrt",
					pp->ppTopBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, PPpropBOTTOM_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "brdrb",
					pp->ppBottomBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, PPpropLEFT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "brdrl",
					pp->ppLeftBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, PPpropRIGHT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "brdrr",
					pp->ppRightBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, PPpropBETWEEN_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "brdrbtw",
					pp->ppBetweenBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, PPpropBAR_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "brdrbar",
					pp->ppBarNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, PPpropFRAME ) )
	{
	FrameProperties		fp;

	docGetFramePropertiesByNumber( &fp, rw->rwDocument,
				pp->ppFrameNumber );

	if  ( DOCisFRAME( &fp ) )
	    { docRtfSaveParaFrameProperties( rw, &fp );	}
	}

    if  ( PROPmaskISSET( updMask, PPpropSHADING ) )
	{
	docRtfSaveShadingByNumber( rw, pp->ppShadingNumber,
		DOCrtf_ParaShadingTags, DOCrtf_ParaShadingTagCount,
		"cfpat", "cbpat", "shading" );
	}

    /*  MUST be last, as DOCibkCOL results in a particule	*/
    if  ( PROPmaskISSET( updMask, PPpropBREAK_KIND ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_ParaBreakTags, pp->ppBreakKind,
				DOCrtf_ParaBreakTagCount, DOCibkpara_COUNT );
	}

    return;
    }

