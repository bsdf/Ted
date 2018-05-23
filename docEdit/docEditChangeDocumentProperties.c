/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docField.h>
#   include	"docEditSetProperties.h"

#   include	<appDebugon.h>

/************************************************************************/

int docChangeDocumentProperties(
			EditOperation *			eo,
			PropertyMask *			docDpDoneMask,
			const PropertyMask *		dpSetMask,
			const DocumentProperties *	dpSet,
			const DocumentAttributeMap *	dam )
    {
    PropertyMask	dpDoneMask;

    utilPropMaskClear( &dpDoneMask );

    if  ( docUpdDocumentProperties( &dpDoneMask,
					    &(eo->eoDocument->bdProperties),
					    dpSetMask, dpSet, dam ) )
	{ LDEB(1); return -1;	}

    if  ( ! utilPropMaskIsEmpty( &dpDoneMask ) )
	{
	if  ( PROPmaskISSET( &dpDoneMask, DGpropPAGE_WIDTH )		||
	      PROPmaskISSET( &dpDoneMask, DGpropPAGE_HEIGHT )		||
	      PROPmaskISSET( &dpDoneMask, DGpropLEFT_MARGIN )		||
	      PROPmaskISSET( &dpDoneMask, DGpropRIGHT_MARGIN )		||
	      PROPmaskISSET( &dpDoneMask, DGpropTOP_MARGIN )		||
	      PROPmaskISSET( &dpDoneMask, DGpropBOTTOM_MARGIN )		||
	      PROPmaskISSET( &dpDoneMask, DGpropHEADER_POSITION )	||
	      PROPmaskISSET( &dpDoneMask, DGpropFOOTER_POSITION )	||
	      PROPmaskISSET( &dpDoneMask, DGpropGUTTER )		||
	      PROPmaskISSET( &dpDoneMask, DGpropMARGMIR )		)
	    {
	    eo->eoReformatNeeded= REFORMAT_DOCUMENT;
	    eo->eoParaAdjustParagraphNumber= -1;
	    }

	if  ( PROPmaskISSET( &dpDoneMask, DPpropFOOTNOTE_RESTART )	||
	      PROPmaskISSET( &dpDoneMask, DPpropENDNOTE_RESTART )	||
	      PROPmaskISSET( &dpDoneMask, DPpropFOOTNOTE_STYLE )	||
	      PROPmaskISSET( &dpDoneMask, DPpropFOOTNOTE_STARTNR )	||
	      PROPmaskISSET( &dpDoneMask, DPpropENDNOTE_STYLE )		||
	      PROPmaskISSET( &dpDoneMask, DPpropENDNOTE_STARTNR )	)
	    { eo->eoFieldUpdate |= FIELDdoCHFTN;	}

	if  ( PROPmaskISSET( &dpDoneMask, DPpropTITLE )			||
	      PROPmaskISSET( &dpDoneMask, DPpropSUBJECT )		||
	      PROPmaskISSET( &dpDoneMask, DPpropKEYWORDS )		||
	      PROPmaskISSET( &dpDoneMask, DPpropDOCCOMM )		||
	      PROPmaskISSET( &dpDoneMask, DPpropAUTHOR )		||
	      PROPmaskISSET( &dpDoneMask, DPpropCOMPANY )		||
	      PROPmaskISSET( &dpDoneMask, DPpropCREATIM )		||
	      PROPmaskISSET( &dpDoneMask, DPpropREVTIM )		||
	      PROPmaskISSET( &dpDoneMask, DPpropPRINTIM )		)
	    { eo->eoFieldUpdate |= FIELDdoDOC_INFO;	}
	}

    if  ( docDpDoneMask )
	{ utilPropMaskOr( docDpDoneMask, docDpDoneMask, &dpDoneMask );	}

    return 0;
    }

