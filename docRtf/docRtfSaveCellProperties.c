/************************************************************************/
/*									*/
/*  Read/Write paragraph cell to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"

/************************************************************************/
/*									*/
/*  Save Cell Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveCellProperties(		RtfWriter *		rw,
					const PropertyMask *	cpSetMask,
					const CellProperties *	cpSet,
					int			shiftLeft )
    {
    const int			anyway= 0;

    if  ( PROPmaskISSET( cpSetMask, CLpropVALIGN ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_CellValignTags,
			    cpSet->cpValign,
			    DOCrtf_CellValignTagCount, DOCtva_COUNT );
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropHOR_MERGE ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_CellHorMergeTags,
			    cpSet->cpHorizontalMerge,
			    DOCrtf_CellHorMergeTagCount, CELLmerge_COUNT );
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropVERT_MERGE ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_CellVerMergeTags,
			    cpSet->cpVerticalMerge,
			    DOCrtf_CellVerMergeTagCount, CELLmerge_COUNT );
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropTOP_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "clbrdrt",
					cpSet->cpTopBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropLEFT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "clbrdrl",
					cpSet->cpLeftBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropRIGHT_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "clbrdrr",
					cpSet->cpRightBorderNumber, anyway );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropBOTTOM_BORDER ) )
	{
	docRtfSaveBorderByNumber( rw, "clbrdrb",
					cpSet->cpBottomBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropLEFT_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropLEFT_PADDING )	)
	{
	/*  NOTE: Word 2003 swaps left and top  */
	docRtfSaveAutoSpace( rw,
			    "clpadft", cpSet->cpLeftPaddingUnit,
			    "clpadt", cpSet->cpLeftPadding );  /* !!!  */
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropTOP_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropTOP_PADDING )		)
	{
	/*  NOTE: Word 2003 swaps left and top  */
	docRtfSaveAutoSpace( rw,
			    "clpadfl", cpSet->cpTopPaddingUnit,
			    "clpadl", cpSet->cpTopPadding );  /* !!!  */
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropRIGHT_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropRIGHT_PADDING )	)
	{
	docRtfSaveAutoSpace( rw,
			    "clpadfr", cpSet->cpRightPaddingUnit,
			    "clpadr", cpSet->cpRightPadding );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropBOTTOM_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropBOTTOM_PADDING )	)
	{
	docRtfSaveAutoSpace( rw,
			    "clpadfb", cpSet->cpBottomPaddingUnit,
			    "clpadb", cpSet->cpBottomPadding );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropCLFTS_WIDTH )	||
	  PROPmaskISSET( cpSetMask, CLpropCLW_WIDTH )	)
	{
	docRtfSaveAutoSpace( rw,
			    "clftsWidth", cpSet->cpPreferredWidthUnit,
			    "clwWidth", cpSet->cpPreferredWidth );
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropSHADING ) )
	{
	docRtfSaveShadingByNumber( rw, cpSet->cpShadingNumber,
		    DOCrtf_CellShadingTags, DOCrtf_CellShadingTagCount,
		    "clcfpat", "clcbpat", "clshdng" );
	}

    /**/
    if  ( PROPmaskISSET( cpSetMask, CLpropTEXT_FLOW ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_CellTextFlowTags,
			    cpSet->cpTextFlow,
			    DOCrtf_CellTextFlowTagCount, TXflow_COUNT );
	}

    /*  LAST! */
    if  ( PROPmaskISSET( cpSetMask, CLpropCELLX ) )
	{
	docRtfWriteArgTag( rw, "cellx",
				    cpSet->cpRightBoundaryTwips- shiftLeft );
	}

    docRtfWriteNextLine( rw );
    }

