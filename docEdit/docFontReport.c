/************************************************************************/
/*									*/
/*  Substitute values into a document based on a font.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<sioFileio.h>
#   include	<appSystem.h>
#   include	<utilMatchFont.h>
#   include	<ucdBlock.h>
#   include	<psGlyphs.h>
#   include	<uniUtf8.h>
#   include	<ucdGeneralCategory.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docField.h>
#   include	"docCopyNode.h"
#   include	"docDocumentCopyJob.h"
#   include	"docFontsDocuments.h"
#   include	<docRecalculateFields.h>
#   include	<docRtfReadWrite.h>
#   include	<docRtfFlags.h>

/************************************************************************/

typedef struct FontReport
    {
    const AfmFontInfo *	frAfi;
    BufferDocument *	frBd;

    int			frBlockIndex;
    unsigned int	frPageFirst;
    unsigned int	frPageLast;
    int			frCurrentSection;
    } FontReport;

/************************************************************************/
/*									*/
/*  Evaluate a single font attribute.					*/
/*									*/
/************************************************************************/

static int fontReportProvideData(	int *		pCalculated,
					MemoryBuffer *	mbResult,
					const char *	value )
    {
    utilMemoryBufferAppendBytes( mbResult, (unsigned char *)value, strlen( value ) );
    *pCalculated= 1;
    return 0;
    }

static int fontReportProvideOptional(	int *		pCalculated,
					MemoryBuffer *	mbResult,
					const char *	value )
    {
    if  ( value )
	{
	return fontReportProvideData( pCalculated, mbResult, value );
	}
    else{
	return fontReportProvideData( pCalculated, mbResult, "(None)" );
	}

    return 0;
    }

static int fontReportProvideMb(	int *			pCalculated,
				MemoryBuffer *		mbResult,
				const MemoryBuffer *	value )
    {
    if  ( value && ! utilMemoryBufferIsEmpty( value ) )
	{
	utilMemoryAppendBuffer( mbResult, value );
	*pCalculated= 1;
	return 0;
	}
    else{
	return fontReportProvideData( pCalculated, mbResult, "(None)" );
	}

    return 0;
    }

static int docFontReportGetUnicode(	const FontReport *	fr,
					const char *		fieldName,
					int			off )
    {
    char *		past= (char *)fieldName+ off;
    long		l= strtol( fieldName+ off, &past, 10 );

    if  ( past == fieldName+ off || *past )
	{ SDEB(fieldName); return -1;	}

    return fr->frPageFirst+ l;
    }

static int fontReportFieldDataProvider(	int *		pCalculated,
					MemoryBuffer *	mbResult,
					const char *	fieldName,
					void *		through )
    {
    FontReport *	fr= (FontReport *)through;
    const AfmFontInfo *	afi= fr->frAfi;

    static char		scratch[20];

    int			past= 65536;
    const UcdBlock *	ub= UCD_Blocks+ fr->frBlockIndex;

    if  ( past > ub->ubLast+ 1 )
	{ past=  ub->ubLast+ 1;	}

    if  ( ! strcmp( fieldName, "FullName" ) )
	{
	return fontReportProvideOptional( pCalculated, mbResult,
						    afi->afiFullName );
	}

    if  ( ! strcmp( fieldName, "FontName" ) )
	{
	return fontReportProvideOptional( pCalculated, mbResult,
						    afi->afiFontName );
	}

    if  ( ! strcmp( fieldName, "FamilyName" ) )
	{
	return fontReportProvideOptional( pCalculated, mbResult,
						    afi->afiFamilyName );
	}

    if  ( ! strcmp( fieldName, "Width" ) )
	{
	return fontReportProvideOptional( pCalculated, mbResult,
						    afi->afiWidthStr );
	}

    if  ( ! strcmp( fieldName, "Weight" ) )
	{
	return fontReportProvideOptional( pCalculated, mbResult,
						    afi->afiWeightStr );
	}

    if  ( ! strcmp( fieldName, "Slant" ) )
	{
	return fontReportProvideData( pCalculated, mbResult,
						    afi->afiItalicAngle < -1?
						    "Italic":"Roman" );
	}

    if  ( ! strcmp( fieldName, "IsFixed" ) )
	{
	return fontReportProvideData( pCalculated, mbResult,
						    afi->afiIsFixedPitch?
						    "Fixed":"Proportional" );
	}

    if  ( ! strcmp( fieldName, "PageFirst" ) )
	{
	sprintf( scratch, "uni%04X", fr->frPageFirst );

	return fontReportProvideOptional( pCalculated, mbResult,
						    scratch );
	}

    if  ( ! strcmp( fieldName, "PageLast" ) )
	{
	sprintf( scratch, "uni%04X", fr->frPageLast );

	return fontReportProvideOptional( pCalculated, mbResult,
						    scratch );
	}

    if  ( ! strcmp( fieldName, "BlockName" ) )
	{
	return fontReportProvideOptional( pCalculated, mbResult,
				UCD_Blocks[fr->frBlockIndex].ubDescription );
	}

    if  ( ! strcmp( fieldName, "AfmFile" ) )
	{
	return fontReportProvideMb( pCalculated, mbResult,
						    &(afi->afiAfmFileName) );
	}

    if  ( ! strcmp( fieldName, "FontFile" ) )
	{
	return fontReportProvideMb( pCalculated, mbResult,
						    &(afi->afiFontFileName) );
	}

    if  ( ! strncmp( fieldName, "Nr", 2 ) && isdigit( fieldName[2] ) )
	{
	int		uni= docFontReportGetUnicode( fr, fieldName, 2 );

	if  ( uni < past && ! ucdIsCn( uni ) )
	    {
	    if  ( uni <= 0xff )
		{ sprintf( scratch, "0x%02x", (unsigned)uni );	}
	    else{ sprintf( scratch, "0x%04x", (unsigned)uni );	}

	    return fontReportProvideData( pCalculated, mbResult, scratch );
	    }

	return fontReportProvideOptional( pCalculated, mbResult, " " );
	}

    if  ( ! strncmp( fieldName, "N", 1 ) && isdigit( fieldName[1] ) )
	{
	int		uni= docFontReportGetUnicode( fr, fieldName, 1 );

	if  ( uni > 0							    &&
	      uni < past						    &&
	      utilIndexMappingGet( &(afi->afiUnicodeToGlyphMapping), uni ) >= 0 )
	    {
	    const char *	N= psUnicodeToGlyphName( uni );

	    return fontReportProvideOptional( pCalculated, mbResult, N );
	    }

	return fontReportProvideOptional( pCalculated, mbResult, " " );
	}

    if  ( ! strncmp( fieldName, "G", 1 ) && isdigit( fieldName[1] ) )
	{
	int		uni= docFontReportGetUnicode( fr, fieldName, 1 );

	if  ( uni > 0							    &&
	      uni < past						    &&
	      utilIndexMappingGet( &(afi->afiUnicodeToGlyphMapping), uni ) >= 0 )
	    {
	    int		step= uniPutUtf8( scratch, uni );

	    if  ( step < 1 )
		{ LDEB(step);	}
	    else{
		scratch[step]= '\0';

		return fontReportProvideData( pCalculated, mbResult, scratch );
		}
	    }

	return fontReportProvideData( pCalculated, mbResult, " " );
	}

    SDEB(fieldName);
    *pCalculated= 0;
    return 0;
    }

/************************************************************************/

static int docSubstituteFontPage(	int *			pUsed,
					RecalculateFields *	rf,
					const FontReport *	fr )
    {
    int			uni;
    int			rval= 0;
    int			used= 0;

    DocumentCopyJob	dcj;
    EditOperation	eo;

    const AfmFontInfo *	afi= fr->frAfi;
    BufferDocument *	bd= fr->frBd;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );

    for ( uni= fr->frPageFirst; uni <= fr->frPageLast; uni++ )
	{
	if  ( utilIndexMappingGet( &(afi->afiUnicodeToGlyphMapping), uni ) >= 0 )
	    { used= 1; break;	}
	}

    if  ( used )
	{
	BufferItem *	bodySectBi= bd->bdBody.dtRoot->biChildren[0];

	if  ( fr->frCurrentSection < bd->bdBody.dtRoot->biChildCount )
	    {
	    const int	page= 0;

	    bodySectBi= bd->bdBody.dtRoot->biChildren[fr->frCurrentSection];

	    if  ( docRecalculateTextLevelFieldsInDocumentTree(
				    rf, &(bd->bdBody), bodySectBi, page ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    DocumentPosition		dp;
	    DocumentSelection		ds;
	    const int			copyFields= 1;

	    if  ( docTailPosition( &dp, bd->bdBody.dtRoot ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    docSetRangeSelection( &ds, &dp, &dp, 1 );

	    if  ( docStartEditOperation( &eo, &ds, bd ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( docSet1DocumentCopyJob( &dcj, &eo, copyFields ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    bodySectBi= docCopyNode( &dcj, bd->bdBody.dtRoot,
					fr->frCurrentSection, bodySectBi );
	    if  ( ! bodySectBi )
		{ XDEB(bodySectBi); rval= -1; goto ready;	}

	    if  ( docRecalculateTextLevelFields( rf, bodySectBi ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}

    *pUsed= used;

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Substitute values into a document based on a font.			*/
/*									*/
/************************************************************************/

static int docSubstituteFontProperties(	const AfmFontInfo *	afi,
					BufferDocument *	bd )
    {
    FontReport		fr;
    RecalculateFields	rf;
    const UcdBlock *	ub;

    fr.frAfi= afi;
    fr.frBd= bd;
    fr.frBlockIndex= 0;
    fr.frPageFirst= 0;
    fr.frPageLast= 0xff;
    fr.frCurrentSection= 0;

    docInitRecalculateFields( &rf );
    rf.rfDocument= bd;
    rf.rfUpdateFlags= FIELDdoDOC_INFO;
    rf.rfFieldDataProvider= fontReportFieldDataProvider;
    rf.rfMergeThrough= &fr;

    ub= UCD_Blocks;
    while( fr.frBlockIndex < UCD_BlockCount )
	{
	int		used= 0;

	/* Stop at the specials */
	if  ( ub->ubFirst >= 0xfff0 )
	    { break;	}

	fr.frPageFirst= ub->ubFirst;

	while( fr.frPageFirst <= ub->ubLast )
	    {
	    fr.frPageLast= fr.frPageFirst+ 255;

	    if  ( fr.frPageLast > ub->ubLast )
		{ fr.frPageLast=  ub->ubLast;	}

	    if  ( docSubstituteFontPage( &used, &rf, &fr ) )
		{ LDEB(1); return -1;	}
	    if  ( used )
		{ fr.frCurrentSection++;	}

	    fr.frPageFirst= fr.frPageLast+ 1;
	    }

	fr.frBlockIndex++;
	ub++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Substitute values into a document based on a font.			*/
/*									*/
/************************************************************************/

static int docMakeFontExample(	const AfmFontInfo *		afi,
				const PostScriptFontList *	psfl,
				DocumentFontList *		dfl,
				const MemoryBuffer *		templateName,
				const MemoryBuffer *		outputDir )
    {
    int				rval= 0;

    MemoryBuffer		relative;
    MemoryBuffer		absolute;

    SimpleInputStream *		sis= (SimpleInputStream *)0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    BufferDocument *		bd= (BufferDocument *)0;

    const int			relativeIsFile= 0;
    unsigned char *		s;

    const unsigned int		rtfFlags= 0;

    utilInitMemoryBuffer( &relative );
    utilInitMemoryBuffer( &absolute );

    if  ( utilMemoryBufferSetString( &relative, afi->afiFontName ) )
	{ SDEB(afi->afiFontName); rval= -1; goto ready;	}
    if  ( utilMemoryBufferAppendBytes( &relative,
				    (const unsigned char *)".rtf", 4 ) )
	{ LDEB(4); rval= -1; goto ready;	}

    s= relative.mbBytes;
    while( *s )
	{
	if  ( isspace( *s ) )
	    { *s= '-';	}
	s++;
	}

    if  ( appAbsoluteName( &absolute, &relative,
					    relativeIsFile, outputDir ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    sis= sioInFileioOpen( templateName );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    bd= docRtfReadFile( sis, rtfFlags );
    if  ( ! bd )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSubstituteFontProperties( afi, bd ) )
	{ SDEB(afi->afiFontName); rval= -1; goto ready;	}

    /*  HACK internals of document font list. */
    {
    int				n;
    DocumentFont *		dfTo;

    n= docGetFontByName( bd->bdProperties.dpFontList, "Times" );
    if  ( n < 0 )
	{ SLDEB("Times",n); rval= -1; goto ready;	}

    dfTo= docFontListGetFontByNumber( bd->bdProperties.dpFontList, n );
    if  ( ! dfTo )
	{ LXDEB(n,dfTo); goto ready;	}

    if  ( docFontSetFamilyName( dfTo, afi->afiFamilyName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    }

    sos= sioOutFileioOpen( &absolute );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    if  ( docRtfSaveDocument( sos, bd,
			(const DocumentSelection *)0, RTFflagNO_MERGEFIELDS ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &relative );
    utilCleanMemoryBuffer( &absolute );

    if  ( sos )
	{ sioOutClose( sos );		}
    if  ( bd )
	{ docFreeDocument( bd );	}
    if  ( sis )
	{ sioInClose( sis );		}

    return rval;
    }

int docFontsDocuments(		const PostScriptFontList *	psfl,
				const MemoryBuffer *		templDir,
				const MemoryBuffer *		outputDir )
    {
    int				rval= 0;
    int				fam;

    DocumentFontList		dfl;
    MemoryBuffer		absolute;
    MemoryBuffer		relative;

    utilInitMemoryBuffer( &absolute );
    utilInitMemoryBuffer( &relative );
    docInitFontList( &dfl );

    if  ( utilAddPsFontsToDocList( &dfl, psfl ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( fam= 0; fam < psfl->psflFamilyCount; fam++ )
	{
	PsFontFamily *	aff= psfl->psflFamilies[fam];
	int		face;

	for ( face= 0; face < aff->psfFaceCount; face++ )
	    {
	    const char *	relativeName;
	    AfmFontInfo *	afi= aff->psfFaces[face];
	    const int		relativeIsFile= 0;

	    if  ( afi->afiMetricsDeferred )
		{ continue;	}

	    if  ( afi->afiWeightInt >= FONTweightDEMIBOLD )
		{
		if  ( afi->afiItalicAngle < -1.0 )
		    { relativeName= "font-bi.rtf";	}
		else{ relativeName= "font-b.rtf";	}
		}
	    else{
		if  ( afi->afiItalicAngle < -1.0 )
		    { relativeName= "font-i.rtf";	}
		else{ relativeName= "font.rtf";		}
		}

	    if  ( utilMemoryBufferSetString( &relative, relativeName ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( appAbsoluteName( &absolute,
				    &relative, relativeIsFile, templDir ) < 0 )
		{ SDEB(relativeName); rval= -1; goto ready;	}

	    if  ( afi->afiMetricsDeferred		&&
		  psGetDeferredMetrics( afi )		)
		{ SDEB(afi->afiFontName); rval= -1; goto ready;	}

	    if  ( docMakeFontExample( afi, psfl, &dfl, &absolute, outputDir ) )
		{ SDEB(afi->afiFontName); rval= -1; goto ready;	}
	    }
	}

  ready:

    docCleanFontList( &dfl );
    utilCleanMemoryBuffer( &absolute );
    utilCleanMemoryBuffer( &relative );

    return rval;
    }

