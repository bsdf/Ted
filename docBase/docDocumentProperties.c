/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<utilPalette.h>
#   include	<utilPropMask.h>
#   include	<appUnit.h>
#   include	<appDebugon.h>

#   include	"docDocumentProperties.h"

/************************************************************************/
/*									*/
/*   Translate document property numbers to notes property numbers.	*/
/*									*/
/************************************************************************/

const int DOCdocNOTE_PROP_MAP[FEPprop_COUNT]=
{
    DPpropFOOTNOTE_STARTNR,
    DPpropFOOTNOTE_JUSTIFICATION,
    DPpropFOOTNOTE_PLACEMENT,
    DPpropFOOTNOTE_RESTART,
    DPpropFOOTNOTE_STYLE,

    DPpropENDNOTE_STARTNR,
    DPpropENDNOTE_JUSTIFICATION,
    DPpropENDNOTE_PLACEMENT,
    DPpropENDNOTE_RESTART,
    DPpropENDNOTE_STYLE,
};

const int * const DOCdocFOOTNOTE_PROP_MAP= DOCdocNOTE_PROP_MAP;
const int * const DOCdocENDNOTE_PROP_MAP= DOCdocNOTE_PROP_MAP+ NOTESprop_COUNT;

void docFillDocFootnoteMask(	PropertyMask *	dpMask )
    {
    PROPmaskADD( dpMask, DPpropFOOTNOTE_PLACEMENT );
    PROPmaskADD( dpMask, DPpropFOOTNOTE_JUSTIFICATION );
    PROPmaskADD( dpMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( dpMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( dpMask, DPpropFOOTNOTE_STARTNR );
    }

void docFillDocEndnoteMask(	PropertyMask *	dpMask )
    {
    PROPmaskADD( dpMask, DPpropENDNOTE_PLACEMENT );
    PROPmaskADD( dpMask, DPpropENDNOTE_JUSTIFICATION );
    PROPmaskADD( dpMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( dpMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( dpMask, DPpropENDNOTE_STARTNR );
    }

/************************************************************************/
/*									*/
/*  Fill a mask with notes properties that are relevant for the		*/
/*  document.								*/
/*  I.E: Set those document properties that are a note property.	*/
/*									*/
/************************************************************************/

void docFillDocNotesMask(	PropertyMask *	dpMask )
    {
    docFillDocFootnoteMask( dpMask );
    docFillDocEndnoteMask( dpMask );
    }

static const int DOC_IntProperties[] =
{
    DGpropPAGE_WIDTH,
    DGpropPAGE_HEIGHT,
    DGpropLEFT_MARGIN,
    DGpropRIGHT_MARGIN,
    DGpropTOP_MARGIN,
    DGpropBOTTOM_MARGIN,
    /* no DGpropHEADER_POSITION, */
    /* no DGpropFOOTER_POSITION, */
    DGpropGUTTER,
    DGpropMARGMIR,

    DPpropSTART_PAGE,

    DPpropFACING_PAGES,
    DPpropGUTTER_HORIZONTAL,
    DPpropWIDOWCTRL,
    DPpropTWO_ON_ONE,
    DPpropDOCTEMP,
    DPpropRTOL,

    DPpropNOTETYPES,	/*  \fetN	*/

    DPpropFOOTNOTE_STARTNR,
    DPpropFOOTNOTE_JUSTIFICATION,
    DPpropFOOTNOTE_PLACEMENT,
    DPpropFOOTNOTE_RESTART,
    DPpropFOOTNOTE_STYLE,

    DPpropENDNOTE_STARTNR,
    DPpropENDNOTE_JUSTIFICATION,
    DPpropENDNOTE_PLACEMENT,
    DPpropENDNOTE_RESTART,
    DPpropENDNOTE_STYLE,

    DPpropDEFTAB,
    DPpropDOC_CHARSET,
    DPpropANSICPG,
    DPpropDEFLANG,

    DPpropDEFF,
    DPpropSTSHFDBCH,
    DPpropSTSHFLOCH,
    DPpropSTSHFHICH,
    DPpropSTSHFBI,
};

static const int DOC_IntPropertyCount= sizeof(DOC_IntProperties)/sizeof(int);

static const int DOC_TextProperties[] =
{
    DPpropGENERATOR,
    DPpropTITLE,
    DPpropSUBJECT,
    DPpropKEYWORDS,
    DPpropDOCCOMM,
    DPpropAUTHOR,
    DPpropCOMPANY,
    DPpropHLINKBASE,
};

static const int DOC_TextPropertyCount= sizeof(DOC_TextProperties)/sizeof(int);

static const int DOC_TimeProperties[] =
{
    DPpropCREATIM,
    DPpropREVTIM,
    DPpropPRINTIM,
};

static const int DOC_TimePropertyCount= sizeof(DOC_TimeProperties)/sizeof(int);

static const int DOC_BorderProperties[] =
{
    DPpropTOP_BORDER,
    DPpropBOTTOM_BORDER,
    DPpropLEFT_BORDER,
    DPpropRIGHT_BORDER,
    DPpropHEAD_BORDER,
    DPpropFOOT_BORDER,
};

static const int DOC_BorderPropertyCount= sizeof(DOC_BorderProperties)/sizeof(int);

/************************************************************************/
/*									*/
/*  Initialise Document Properties.					*/
/*									*/
/************************************************************************/

void docInitDocumentProperties(	DocumentProperties *	dp )
    {
    dp->dpContainsTables= 0;
    dp->dpTabIntervalTwips= 720;
    dp->dpDefaultColor= 0;
    dp->dpDocumentCharset= DOCcharsetANSI;
    dp->dpAnsiCodepage= -1;
    dp->dpDefaultLanguage= -1;

    dp->dpDefaultFont= -1;
    dp->dpDefaultFontDbch= -1;
    dp->dpDefaultFontLoch= -1;
    dp->dpDefaultFontHich= -1;
    dp->dpDefaultFontBi= -1;

    dp->dpStartPageNumber= 1;
    dp->dpFootEndNoteType= DPfetFOOT_ONLY;

    docInitFootEndNotesProperties( &(dp->dpNotesProps) );

    dp->dpHasFacingPages= 0;
    dp->dpGutterHorizontal= 0;
    dp->dpWidowControl= 0;
    dp->dpTwoOnOne= 0;
    dp->dpIsDocumentTemplate= 0;
    dp->dpRToL= 0;

    utilInitDocumentGeometry( &(dp->dpGeometry) );

    dp->dpFontList= (struct DocumentFontList *)0;
    dp->dpListAdmin= (struct ListAdmin *)0;
    dp->dpColorPalette= (struct ColorPalette *)0;

    utilInitMemoryBuffer( &(dp->dpGeneratorRead) );
    utilInitMemoryBuffer( &(dp->dpGeneratorWrite) );
    utilInitMemoryBuffer( &(dp->dpTitle) );
    utilInitMemoryBuffer( &(dp->dpSubject) );
    utilInitMemoryBuffer( &(dp->dpKeywords) );
    utilInitMemoryBuffer( &(dp->dpDoccomm) );
    utilInitMemoryBuffer( &(dp->dpAuthor) );
    utilInitMemoryBuffer( &(dp->dpCompany) );
    utilInitMemoryBuffer( &(dp->dpHlinkbase) );
    utilInitMemoryBuffer( &(dp->dpFilename) );

    dp->dpTopBorderNumber= 0;
    dp->dpLeftBorderNumber= 0;
    dp->dpRightBorderNumber= 0;
    dp->dpBottomBorderNumber= 0;
    dp->dpHeadBorderNumber= 0;
    dp->dpFootBorderNumber= 0;

    utilInvalidateTime( &(dp->dpCreatim) );
    utilInvalidateTime( &(dp->dpRevtim) );
    utilInvalidateTime( &(dp->dpPrintim) );

    dp->dpHasOpenEnd= 0;
    }

void docCleanDocumentProperties(	DocumentProperties *	dp )
    {
    /* Owned by the document:
    dp->dpFontList
    dp->dpListAdmin
    dp->dpColorPalette
    */

    utilInvalidateTime( &(dp->dpCreatim) );
    utilInvalidateTime( &(dp->dpRevtim) );
    utilInvalidateTime( &(dp->dpPrintim) );

    utilCleanMemoryBuffer( &(dp->dpGeneratorRead ) );
    utilCleanMemoryBuffer( &(dp->dpGeneratorWrite ) );
    utilCleanMemoryBuffer( &(dp->dpTitle ) );
    utilCleanMemoryBuffer( &(dp->dpSubject ) );
    utilCleanMemoryBuffer( &(dp->dpKeywords ) );
    utilCleanMemoryBuffer( &(dp->dpDoccomm ) );
    utilCleanMemoryBuffer( &(dp->dpAuthor ) );
    utilCleanMemoryBuffer( &(dp->dpCompany ) );
    utilCleanMemoryBuffer( &(dp->dpHlinkbase ) );
    utilCleanMemoryBuffer( &(dp->dpFilename ) );

    return;
    }

/************************************************************************/
/*									*/
/*  Copy document properties.						*/
/*									*/
/************************************************************************/

int docCopyDocumentProperties(	DocumentProperties *		to,
				const DocumentProperties *	from )
    {
    PropertyMask	dpSetMask;

    int			rval =0;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    utilPropMaskClear( &dpSetMask );
    utilPropMaskFill( &dpSetMask, DPprop_COUNT );

    PROPmaskUNSET( &dpSetMask, DPpropFONTTABLE );
    PROPmaskUNSET( &dpSetMask, DPpropCOLORTABLE );
    PROPmaskUNSET( &dpSetMask, DPpropSTYLESHEET );
    PROPmaskUNSET( &dpSetMask, DPpropLISTTABLE );
    PROPmaskUNSET( &dpSetMask, DPpropLISTOVERRIDETABLE );

    PROPmaskUNSET( &dpSetMask, DPpropGENERATOR );
    PROPmaskUNSET( &dpSetMask, DPpropTITLE );
    PROPmaskUNSET( &dpSetMask, DPpropSUBJECT );
    PROPmaskUNSET( &dpSetMask, DPpropKEYWORDS );
    PROPmaskUNSET( &dpSetMask, DPpropDOCCOMM );
    PROPmaskUNSET( &dpSetMask, DPpropAUTHOR );
    PROPmaskUNSET( &dpSetMask, DPpropCOMPANY );
    PROPmaskUNSET( &dpSetMask, DPpropHLINKBASE );

    PROPmaskUNSET( &dpSetMask, DPpropCREATIM );
    PROPmaskUNSET( &dpSetMask, DPpropREVTIM );
    PROPmaskUNSET( &dpSetMask, DPpropPRINTIM );

    if  ( docUpdDocumentProperties( (PropertyMask *)0,
					    to, &dpSetMask, from, dam0 ) )
	{ LDEB(1); return -1;	}

    if  ( to->dpFontList != from->dpFontList			&&
	  docCopyFontList( to->dpFontList, from->dpFontList )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( to->dpListAdmin != from->dpListAdmin			&&
	  docCopyListAdmin( to->dpListAdmin, from->dpListAdmin ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( to->dpColorPalette != from->dpColorPalette			&&
	  utilCopyColorPalette( to->dpColorPalette, from->dpColorPalette ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpGeneratorRead),
						&(from->dpGeneratorRead) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpGeneratorWrite),
						&(from->dpGeneratorWrite) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpTitle), &(from->dpTitle) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpSubject), &(from->dpSubject) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpKeywords), &(from->dpKeywords) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpDoccomm), &(from->dpDoccomm) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpAuthor), &(from->dpAuthor) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpCompany), &(from->dpCompany) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpHlinkbase), &(from->dpHlinkbase) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(to->dpFilename), &(from->dpFilename) ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set the file name in the document properties.			*/
/*									*/
/************************************************************************/

int docPropertiesSetFilename(	DocumentProperties *	dp,
				const MemoryBuffer *	filename )
    {
    if  ( filename )
	{
	if  ( utilCopyMemoryBuffer( &(dp->dpFilename), filename ) )
	    { LDEB(1); return -1;	}
	}
    else{ utilEmptyMemoryBuffer( &(dp->dpFilename) );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change document properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdDocumentProperties(	PropertyMask *			pDoneMask,
				DocumentProperties *		dpTo,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpFrom,
				const DocumentAttributeMap *	dam )
    {
    int			i;

    PropertyMask	doneMask;

    utilPropMaskClear( &doneMask );

    for ( i= 0; i < DOC_IntPropertyCount; i++ )
	{
	int	prop= DOC_IntProperties[i];

	if  ( PROPmaskISSET( dpSetMask, prop ) )
	    {
	    int		from= docGetDocumentProperty( dpFrom, prop );
	    int		to= docGetDocumentProperty( dpTo, prop );

	    if  ( to != from )
		{
		if  ( docSetDocumentProperty( dpTo, prop, from ) )
		    { LLDEB(prop,from); return -1;	}

		PROPmaskADD( &doneMask, prop );
		}
	    }
	}

    for ( i= 0; i < DOC_TextPropertyCount; i++ )
	{
	int	prop= DOC_TextProperties[i];

	if  ( PROPmaskISSET( dpSetMask, prop ) )
	    {
	    const MemoryBuffer *	from;
	    MemoryBuffer *		to;

	    from= docGetDocumentPropertyBuffer(
					(DocumentProperties *)dpFrom, prop );
	    to= docGetDocumentPropertyBuffer( dpTo, prop );

	    if  ( ! utilEqualMemoryBuffer( to, from ) )
		{
		if  ( utilCopyMemoryBuffer( to, from ) )
		    { LDEB(prop); return -1;	}

		PROPmaskADD( &doneMask, prop );
		}
	    }
	}

    for ( i= 0; i < DOC_BorderPropertyCount; i++ )
	{
	int	prop= DOC_BorderProperties[i];

	if  ( PROPmaskISSET( dpSetMask, prop ) )
	    {
	    int		from= docGetDocumentProperty( dpFrom, prop );
	    int		to= docGetDocumentProperty( dpTo, prop );

	    if  ( from >= 0 && dam && dam->damBorderMap )
		{ from= dam->damBorderMap[from];	}

	    if  ( to != from )
		{
		if  ( docSetDocumentProperty( dpTo, prop, from ) )
		    { LLDEB(prop,from); return -1;	}

		PROPmaskADD( &doneMask, prop );
		}
	    }
	}

    for ( i= 0; i < DOC_TimePropertyCount; i++ )
	{
	int	prop= DOC_TimeProperties[i];

	if  ( PROPmaskISSET( dpSetMask, prop ) )
	    {
	    struct tm *	from= docGetDocumentPropertyTime(
					(DocumentProperties *)dpFrom, prop );
	    struct tm *	to= docGetDocumentPropertyTime( dpTo, prop );

	    /* tm_isdst makes this necessary: */

	    if  ( to->tm_sec != from->tm_sec )
		{ to->tm_sec = from->tm_sec; PROPmaskADD( &doneMask, prop ); }
	    if  ( to->tm_min != from->tm_min )
		{ to->tm_min = from->tm_min; PROPmaskADD( &doneMask, prop ); }
	    if  ( to->tm_hour != from->tm_hour )
		{ to->tm_hour = from->tm_hour; PROPmaskADD( &doneMask, prop ); }
	    if  ( to->tm_mday != from->tm_mday )
		{ to->tm_mday = from->tm_mday; PROPmaskADD( &doneMask, prop ); }
	    if  ( to->tm_mon != from->tm_mon )
		{ to->tm_mon = from->tm_mon; PROPmaskADD( &doneMask, prop ); }
	    if  ( to->tm_year != from->tm_year )
		{ to->tm_year = from->tm_year; PROPmaskADD( &doneMask, prop ); }
	    }
	}

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &doneMask );	}

    return 0;
    }

void docDocumentPropertyDifference(
				PropertyMask *			pDifMask,
				const DocumentProperties *	dp1,
				const PropertyMask *		cmpMask,
				const DocumentProperties *	dp2 )
    {
    int			i;

    PropertyMask	dpDifMask;

    utilPropMaskClear( &dpDifMask );

    for ( i= 0; i < DOC_IntPropertyCount; i++ )
	{
	int	prop= DOC_IntProperties[i];

	if  ( PROPmaskISSET( cmpMask, prop ) )
	    {
	    int		from= docGetDocumentProperty( dp1, prop );
	    int		to= docGetDocumentProperty( dp2, prop );

	    if  ( to != from )
		{ PROPmaskADD( &dpDifMask, prop );	}
	    }
	}

    for ( i= 0; i < DOC_TextPropertyCount; i++ )
	{
	int	prop= DOC_TextProperties[i];

	if  ( PROPmaskISSET( cmpMask, prop ) )
	    {
	    const MemoryBuffer *	from;
	    MemoryBuffer *		to;

	    from= docGetDocumentPropertyBuffer(
					(DocumentProperties *)dp1, prop );
	    to= docGetDocumentPropertyBuffer(
					(DocumentProperties *)dp2, prop );

	    if  ( ! utilEqualMemoryBuffer( to, from ) )
		{ PROPmaskADD( &dpDifMask, prop );	}
	    }
	}

    for ( i= 0; i < DOC_BorderPropertyCount; i++ )
	{
	int	prop= DOC_BorderProperties[i];

	if  ( PROPmaskISSET( cmpMask, prop ) )
	    {
	    int		from= docGetDocumentProperty( dp1, prop );
	    int		to= docGetDocumentProperty( dp2, prop );

	    if  ( to != from )
		{ PROPmaskADD( &dpDifMask, prop );	}
	    }
	}

    for ( i= 0; i < DOC_TimePropertyCount; i++ )
	{
	int	prop= DOC_TimeProperties[i];

	if  ( PROPmaskISSET( cmpMask, prop ) )
	    {
	    struct tm *	from= docGetDocumentPropertyTime(
					(DocumentProperties *)dp1, prop );
	    struct tm *	to= docGetDocumentPropertyTime(
					(DocumentProperties *)dp2, prop );

	    if  ( mktime( to ) != mktime( from ) )
		{ PROPmaskADD( &dpDifMask, prop );	}
	    }
	}

    *pDifMask= dpDifMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Allocate a color.							*/
/*									*/
/************************************************************************/

int docAllocateDocumentColor(		DocumentProperties *	dp,
					const RGB8Color *	rgb8 )
    {
    const int			avoidZero= 1;
    const int			maxColors= 256;
    int				color;

    color= utilPaletteInsertColor( dp->dpColorPalette,
						avoidZero, maxColors, rgb8 );
    if  ( color < 0 )
	{ LDEB(color); return -1;	}

    return color;
    }

int docSetDocumentProperty(	DocumentProperties *	dp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case DGpropLEFT_MARGIN:
	    dp->dpGeometry.dgLeftMarginTwips= arg;
	    return 0;
	case DGpropTOP_MARGIN:
	    dp->dpGeometry.dgTopMarginTwips= arg;
	    return 0;
	case DGpropRIGHT_MARGIN:
	    dp->dpGeometry.dgRightMarginTwips= arg;
	    return 0;
	case DGpropBOTTOM_MARGIN:
	    dp->dpGeometry.dgBottomMarginTwips= arg;
	    return 0;
	case DGpropPAGE_WIDTH:
	    dp->dpGeometry.dgPageWideTwips= arg;
	    return 0;
	case DGpropPAGE_HEIGHT:
	    dp->dpGeometry.dgPageHighTwips= arg;
	    return 0;
	case DGpropGUTTER:
	    dp->dpGeometry.dgGutterTwips= arg;
	    return 0;
	case DGpropMARGMIR:
	    dp->dpGeometry.dgMirrorMargins= arg != 0;
	    return 0;

	case DPpropFACING_PAGES:
	    dp->dpHasFacingPages= arg != 0;
	    return 0;
	case DPpropGUTTER_HORIZONTAL:
	    dp->dpGutterHorizontal= arg != 0;
	    return 0;
	case DPpropWIDOWCTRL:
	    dp->dpWidowControl= arg != 0;
	    return 0;
	case DPpropTWO_ON_ONE:
	    dp->dpTwoOnOne= arg != 0;
	    return 0;
	case DPpropDOCTEMP:
	    dp->dpIsDocumentTemplate= arg != 0;
	    return 0;
	case DPpropRTOL:
	    dp->dpRToL= arg != 0;
	    return 0;

	case DPpropSTART_PAGE:
	    dp->dpStartPageNumber= arg;
	    return 0;
	case DPpropDEFTAB:
	    dp->dpTabIntervalTwips= arg;
	    return 0;

	case DPpropDEFF:
	    dp->dpDefaultFont= arg;
	    return 0;
	case DPpropSTSHFDBCH:
	    dp->dpDefaultFontDbch= arg;
	    return 0;
	case DPpropSTSHFLOCH:
	    dp->dpDefaultFontLoch= arg;
	    return 0;
	case DPpropSTSHFHICH:
	    dp->dpDefaultFontHich= arg;
	    return 0;
	case DPpropSTSHFBI:
	    dp->dpDefaultFontBi= arg;
	    return 0;

	case DPpropNOTETYPES:
	    if  ( arg < 0 || arg > 2 )
		{ LDEB(arg); return -1;	}
	    dp->dpFootEndNoteType= arg;
	    return 0;

	/* FOOTNOTE */
	case DPpropFOOTNOTE_STARTNR:
	    docSetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropSTARTNR, arg );
	    return 0;
	case DPpropFOOTNOTE_JUSTIFICATION:
	    docSetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropJUSTIFICATION, arg );
	    return 0;
	case DPpropFOOTNOTE_PLACEMENT:
	    docSetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropPLACEMENT, arg );
	    return 0;
	case DPpropFOOTNOTE_RESTART:
	    docSetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropRESTART, arg );
	    return 0;
	case DPpropFOOTNOTE_STYLE:
	    docSetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropSTYLE, arg );
	    return 0;

	/* ENDNOTE */
	case DPpropENDNOTE_STARTNR:
	    docSetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropSTARTNR, arg );
	    return 0;
	case DPpropENDNOTE_JUSTIFICATION:
	    docSetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropJUSTIFICATION, arg );
	    return 0;
	case DPpropENDNOTE_PLACEMENT:
	    docSetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropPLACEMENT, arg );
	    return 0;
	case DPpropENDNOTE_RESTART:
	    docSetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropRESTART, arg );
	    return 0;
	case DPpropENDNOTE_STYLE:
	    docSetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropSTYLE, arg );
	    return 0;

	case DPpropDOC_CHARSET:
	    dp->dpDocumentCharset= arg;
	    return 0;

	case DPpropANSICPG:
	    dp->dpAnsiCodepage= arg;
	    return 0;

	case DPpropDEFLANG:
	    dp->dpDefaultLanguage= arg;
	    return 0;

	case DPpropTOP_BORDER:
	    dp->dpTopBorderNumber= arg;
	    return 0;
	case DPpropBOTTOM_BORDER:
	    dp->dpBottomBorderNumber= arg;
	    return 0;
	case DPpropLEFT_BORDER:
	    dp->dpLeftBorderNumber= arg;
	    return 0;
	case DPpropRIGHT_BORDER:
	    dp->dpRightBorderNumber= arg;
	    return 0;
	case DPpropHEAD_BORDER:
	    dp->dpHeadBorderNumber= arg;
	    return 0;
	case DPpropFOOT_BORDER:
	    dp->dpFootBorderNumber= arg;
	    return 0;
	default:
	    LDEB(prop); return -1;
	}
    }

int docGetDocumentProperty(	const DocumentProperties *	dp,
				int				prop )
    {
    switch( prop )
	{
	case DGpropPAGE_WIDTH:
	    return dp->dpGeometry.dgPageWideTwips;
	case DGpropPAGE_HEIGHT:
	    return dp->dpGeometry.dgPageHighTwips;
	case DGpropLEFT_MARGIN:
	    return dp->dpGeometry.dgLeftMarginTwips;
	case DGpropRIGHT_MARGIN:
	    return dp->dpGeometry.dgRightMarginTwips;
	case DGpropTOP_MARGIN:
	    return dp->dpGeometry.dgTopMarginTwips;
	case DGpropBOTTOM_MARGIN:
	    return dp->dpGeometry.dgBottomMarginTwips;
	/* NO DGpropHEADER_POSITION: */
	/* NO DGpropFOOTER_POSITION: */
	case DGpropGUTTER:
	    return dp->dpGeometry.dgGutterTwips;
	case DGpropMARGMIR:
	    return dp->dpGeometry.dgMirrorMargins;

	case DPpropSTART_PAGE:
	    return dp->dpStartPageNumber;

	case DPpropFACING_PAGES:
	    return dp->dpHasFacingPages;
	case DPpropGUTTER_HORIZONTAL:
	    return dp->dpGutterHorizontal;
	case DPpropWIDOWCTRL:
	    return dp->dpWidowControl;
	case DPpropTWO_ON_ONE:
	    return dp->dpTwoOnOne;
	case DPpropDOCTEMP:
	    return dp->dpIsDocumentTemplate;
	case DPpropRTOL:
	    return dp->dpRToL;

	case DPpropNOTETYPES:
	    return dp->dpFootEndNoteType;

	/* FOOTNOTE */
	case DPpropFOOTNOTE_STARTNR:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropSTARTNR );
	case DPpropFOOTNOTE_JUSTIFICATION:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropJUSTIFICATION );
	case DPpropFOOTNOTE_PLACEMENT:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropPLACEMENT );
	case DPpropFOOTNOTE_RESTART:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropRESTART );
	case DPpropFOOTNOTE_STYLE:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepFootnotesProps),
						NOTESpropSTYLE );

	/* ENDNOTE */
	case DPpropENDNOTE_STARTNR:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropSTARTNR );
	case DPpropENDNOTE_JUSTIFICATION:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropJUSTIFICATION );
	case DPpropENDNOTE_PLACEMENT:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropPLACEMENT );
	case DPpropENDNOTE_RESTART:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropRESTART );
	case DPpropENDNOTE_STYLE:
	    return docGetNotesProperty( &(dp->dpNotesProps.fepEndnotesProps),
						NOTESpropSTYLE );

	case DPpropDEFTAB:
	    return dp->dpTabIntervalTwips;
	case DPpropDOC_CHARSET:
	    return dp->dpDocumentCharset;
	case DPpropANSICPG:
	    return dp->dpAnsiCodepage;
	case DPpropDEFLANG:
	    return dp->dpDefaultLanguage;

	case DPpropDEFF:
	    return dp->dpDefaultFont;
	case DPpropSTSHFDBCH:
	    return dp->dpDefaultFontDbch;
	case DPpropSTSHFLOCH:
	    return dp->dpDefaultFontLoch;
	case DPpropSTSHFHICH:
	    return dp->dpDefaultFontHich;
	case DPpropSTSHFBI:
	    return dp->dpDefaultFontBi;

	case DPpropTOP_BORDER:
	    return dp->dpTopBorderNumber;
	case DPpropBOTTOM_BORDER:
	    return dp->dpBottomBorderNumber;
	case DPpropLEFT_BORDER:
	    return dp->dpLeftBorderNumber;
	case DPpropRIGHT_BORDER:
	    return dp->dpRightBorderNumber;
	case DPpropHEAD_BORDER:
	    return dp->dpHeadBorderNumber;
	case DPpropFOOT_BORDER:
	    return dp->dpFootBorderNumber;
	default:
	    LDEB(prop); return -1;
	}
    }

int docSetDocumentPropertyString(	DocumentProperties *	dp,
					int			prop,
					const char *		val,
					int			vallen )
    {
    switch( prop )
	{
	case DPpropGENERATOR:
	    return utilMemoryBufferSetBytes( &(dp->dpGeneratorRead),
					(const unsigned char *)val, vallen );
	case DPpropTITLE:
	    return utilMemoryBufferSetBytes( &(dp->dpTitle),
					(const unsigned char *)val, vallen );
	case DPpropSUBJECT:
	    return utilMemoryBufferSetBytes( &(dp->dpSubject),
					(const unsigned char *)val, vallen );
	case DPpropKEYWORDS:
	    return utilMemoryBufferSetBytes( &(dp->dpKeywords),
					(const unsigned char *)val, vallen );
	case DPpropDOCCOMM:
	    return utilMemoryBufferSetBytes( &(dp->dpDoccomm),
					(const unsigned char *)val, vallen );
	case DPpropAUTHOR:
	    return utilMemoryBufferSetBytes( &(dp->dpAuthor),
					(const unsigned char *)val, vallen );
	case DPpropCOMPANY:
	    return utilMemoryBufferSetBytes( &(dp->dpCompany),
					(const unsigned char *)val, vallen );
	case DPpropHLINKBASE:
	    return utilMemoryBufferSetBytes( &(dp->dpHlinkbase),
					(const unsigned char *)val, vallen );

	default:
	    LDEB(prop); return -1;
	}
    }

MemoryBuffer * docGetDocumentPropertyBuffer(
					DocumentProperties *	dp,
					int			prop )
    {
    switch( prop )
	{
	case DPpropGENERATOR:
	    return &(dp->dpGeneratorRead);
	case DPpropTITLE:
	    return &(dp->dpTitle);
	case DPpropSUBJECT:
	    return &(dp->dpSubject);
	case DPpropKEYWORDS:
	    return &(dp->dpKeywords);
	case DPpropDOCCOMM:
	    return &(dp->dpDoccomm);
	case DPpropAUTHOR:
	    return &(dp->dpAuthor);
	case DPpropCOMPANY:
	    return &(dp->dpCompany);
	case DPpropHLINKBASE:
	    return &(dp->dpHlinkbase);

	default:
	    LDEB(prop); return (MemoryBuffer *)0;
	}
    }

struct tm * docGetDocumentPropertyTime(	DocumentProperties *	dp,
					int			prop )
    {
    switch( prop )
	{
	case DPpropCREATIM:
	    return &(dp->dpCreatim);

	case DPpropREVTIM:
	    return &(dp->dpRevtim);

	case DPpropPRINTIM:
	    return &(dp->dpPrintim);

	default:
	    LDEB(prop); return (struct tm *)0;
	}
    }

int docSetDocumentPropertyTime(		DocumentProperties *	dp,
					int			prop,
					const struct tm *	val )
    {
    switch( prop )
	{
	case DPpropCREATIM:
	    dp->dpCreatim= *val;
	    return 0;

	case DPpropREVTIM:
	    dp->dpRevtim= *val;
	    return 0;

	case DPpropPRINTIM:
	    dp->dpPrintim= *val;
	    return 0;

	default:
	    LDEB(prop); return -1;
	}

    }
