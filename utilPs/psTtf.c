#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	<sioMemory.h>
#   include	<sioEndian.h>
#   include	<utilEndian.h>
#   include	<utilMemoryBuffer.h>
#   include	<utilIndexMapping.h>
#   include	<appDebugon.h>

#   include	"psTtfIntern.h"

/************************************************************************/
/*									*/
/*  Elementary bookeeping.						*/
/*									*/
/************************************************************************/

static void utilInitTrueTypeOS_2Table(	TrueTypeOS_2Table *	ttot )
    {
    ttot->ttotVersion= 0;
    ttot->ttotAvgCharWidth= 0;
    ttot->ttotWeightClass= 0;
    ttot->ttotWidthClass= 0;
    ttot->ttotType= 0;
    ttot->ttotSubscriptXSize= 0;
    ttot->ttotSubscriptYSize= 0;
    ttot->ttotSubscriptXOffset= 0;
    ttot->ttotSubscriptYOffset= 0;
    ttot->ttotSuperscriptXSize= 0;
    ttot->ttotSuperscriptYSize= 0;
    ttot->ttotSuperscriptXOffset= 0;
    ttot->ttotSuperscriptYOffset= 0;
    ttot->ttotStrikeoutSize= 0;
    ttot->ttotStrikeoutPosition= 0;
    ttot->ttotFamilyClass= 0;
    memset( ttot->ttotPanose, '\0', 10 );
    ttot->ttotCharRange[0]= 0;
    ttot->ttotCharRange[1]= 0;
    ttot->ttotCharRange[2]= 0;
    ttot->ttotCharRange[3]= 0;
    memset( ttot->ttotVendID, '\0', 4+1 );
    ttot->ttotSelection= 0;
    ttot->ttotFirstCharIndex= 0;
    ttot->ttotLastCharIndex= 0;

    return;
    }

static void utilInitTrueTypePostTable(	TrueTypePostTable *	ttpt )
    {
    ttpt->ttptFormatUpper= 0;
    ttpt->ttptFormatLower= 0;
    ttpt->ttptItalicAngleUpper= 0;
    ttpt->ttptItalicAngleLower= 0;
    ttpt->ttptUnderlinePosition= 0;
    ttpt->ttptUnderlineThickness= 0;
    ttpt->ttptIsFixedPitch= 0;
    ttpt->ttptMinMemType42= 0;
    ttpt->ttptMaxMemType42= 0;
    ttpt->ttptMinMemType1= 0;
    ttpt->ttptMaxMemType1= 0;
    ttpt->ttptIndexCount= 0;
    ttpt->ttptIndices= (unsigned short *)0;
    ttpt->ttptStringCount= 0;
    ttpt->ttptStrings= (unsigned char **)0;
    }

static void utilCleanTrueTypePostTable(	TrueTypePostTable *	ttpt )
    {
    int		name;

    for ( name= 0; name < ttpt->ttptStringCount; name++ )
	{
	if  ( ttpt->ttptStrings[name] )
	    { free( ttpt->ttptStrings[name] );	}
	}

    if  ( ttpt->ttptIndices )
	{ free( ttpt->ttptIndices );	}
    if  ( ttpt->ttptStrings )
	{ free( ttpt->ttptStrings );	}
    }

static void utilInitTrueTypeKernSub(	TrueTypeKernSub *	ttks )
    {
    ttks->ttksVersion= 0;
    ttks->ttksLength= 0;
    ttks->ttksCoverage= 0;
    ttks->ttksPairCount= 0;
    ttks->ttksSearchRange= 0;
    ttks->ttksEntrySelector= 0;
    ttks->ttksRangeShift= 0;

    ttks->ttksPairs= (TrueTypeKernPair *)0;
    }

static void utilCleanTrueTypeKernSub(	TrueTypeKernSub *	ttks )
    {
    if  ( ttks->ttksPairs )
	{ free( ttks->ttksPairs );	}
    }

static void utilInitTrueTypeKernTable(	TrueTypeKernTable *	ttkt )
    {
    ttkt->ttktVersion= 0;
    ttkt->ttktKernSubCount= 0;
    ttkt->ttktKernSubs= (TrueTypeKernSub *)0;
    }

static void utilCleanTrueTypeKernTable(	TrueTypeKernTable *	ttkt )
    {
    int		i;

    for ( i= 0; i < ttkt->ttktKernSubCount; i++ )
	{ utilCleanTrueTypeKernSub( &(ttkt->ttktKernSubs[i]) ); }

    if  ( ttkt->ttktKernSubs )
	{ free( ttkt->ttktKernSubs );	}

    return;
    }

static void utilInitTrueTypeCmapRecord(	TrueTypeCmapRecord * ttcr )
    {
    ttcr->ttcr6_12GlyphCodes= (unsigned int *)0;
    }

static void utilCleanTrueTypeCmapRecord(	TrueTypeCmapRecord * ttcr )
    {
    if  ( ttcr->ttcr6_12GlyphCodes )
	{ free( ttcr->ttcr6_12GlyphCodes );	}
    }

static void utilInitTrueTypeCmapTable(	TrueTypeCmapTable *	ttct )
    {
    ttct->ttctVersion= 0;
    ttct->ttctEncodingRecordCount= 0;
    ttct->ttctEncodingRecords= (TrueTypeCmapRecord *)0;
    }

static void utilCleanTrueTypeCmapTable(	TrueTypeCmapTable *	ttct )
    {
    int		i;

    for ( i= 0; i < ttct->ttctEncodingRecordCount; i++ )
	{
	utilCleanTrueTypeCmapRecord( &(ttct->ttctEncodingRecords[i]) );
	}

    if  ( ttct->ttctEncodingRecords )
	{ free( ttct->ttctEncodingRecords );	}
    }

void psTtfInitTrueTypeFont(	TrueTypeFont *	ttf )
    {
    ttf->ttfVersion= 0;
    ttf->ttfTableCount= 0;
    ttf->ttfSearchRange= 0;
    ttf->ttfEntrySelector= 0;
    ttf->ttfRangeShift= 0;

    ttf->ttfTables= (TrueTypeTableEntry *)0;

    psTtfInitTrueTypeNameTable( &(ttf->ttfNameTable) );
    /* utilInitTrueTypePostTable( &(ttf->ttfPostTable) ); */
    /* utilInitTrueTypeHeadTable( &(ttf->ttfHeadTable) ); */
    /* utilInitTrueTypeHheaTable( &(ttf->ttfHheaTable) ); */
    /* utilInitTrueTypeVheaTable( &(ttf->ttfVheaTable) ); */
    utilInitTrueTypeCmapTable( &(ttf->ttfCmapTable) );
    utilInitTrueTypeKernTable( &(ttf->ttfKernTable) );
    utilInitTrueTypePostTable( &(ttf->ttfPostTable) );
    utilInitTrueTypeOS_2Table( &(ttf->ttfOS_2Table) );

    ttf->ttfLocations= (unsigned long *)0;
    ttf->ttfLocationCount= 0;

    ttf->ttfHorizontalMetrics= (HorizontalMetric *)0;
    ttf->ttfHorizontalMetricCount= 0;

    ttf->ttfGlyphs= (TrueTypeGlyph *)0;
    ttf->ttfGlyphCount= 0;

    utilInitIndexMapping( &(ttf->ttfToGlyphMapping) );

    return;
    }

void psTtfCleanTrueTypeFont(	TrueTypeFont *	ttf )
    {
    int		ent;

    for ( ent= 0; ent < ttf->ttfTableCount; ent++ )
	{ utilCleanTrueTypeTableEntry( &(ttf->ttfTables[ent]) );	}

    if  ( ttf->ttfTables )
	{ free( ttf->ttfTables );	}

    psTtfCleanTrueTypeNameTable( &(ttf->ttfNameTable) );
    /* utilCleanTrueTypePostTable( &(ttf->ttfPostTable) ); */
    /* utilCleanTrueTypeHeadTable( &(ttf->ttfHeadTable) ); */
    /* utilCleanTrueTypeHheaTable( &(ttf->ttfHheaTable) ); */
    /* utilCleanTrueTypeVheaTable( &(ttf->ttfVheaTable) ); */
    utilCleanTrueTypeCmapTable( &(ttf->ttfCmapTable) );
    utilCleanTrueTypeKernTable( &(ttf->ttfKernTable) );
    utilCleanTrueTypePostTable( &(ttf->ttfPostTable) );
    /* utilCleanTrueTypeOS_2Table( &(ttf->ttfOS_2Table) ); */

    if  ( ttf->ttfLocations )
	{ free( ttf->ttfLocations );	}

    if  ( ttf->ttfHorizontalMetrics )
	{ free( ttf->ttfHorizontalMetrics );	}

    if  ( ttf->ttfGlyphs )
	{ free( ttf->ttfGlyphs );	}

    utilCleanIndexMapping( &(ttf->ttfToGlyphMapping) );
    return;
    }

/************************************************************************/
/*									*/
/*  Open a stream to one of the tables.					*/
/*									*/
/************************************************************************/

static SimpleInputStream * utilTtfOpenTableStream(
					const TrueTypeTableEntry **	pTtte,
					const char *			tag,
					const TrueTypeFont *		ttf )
    {
    int				tab;
    const TrueTypeTableEntry *	ttte;
    SimpleInputStream *		sis;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ! strcmp( ttte->ttteTag, tag ) )
	    { break;	}
	}

    if  ( tab >= ttf->ttfTableCount )
	{ /*SLDEB(tag,ttf->ttfTableCount);*/ return (SimpleInputStream *)0; }

    sis= sioInMemoryOpen( &(ttte->ttteMemoryBuffer) );
    if  ( ! sis )
	{ XDEB(sis); return (SimpleInputStream *)0;	}

    *pTtte= ttte;
    return sis;
    }

/************************************************************************/
/*									*/
/*  Extract the name table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractNameTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisName= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeNameTable *		ttnt= &(ttf->ttfNameTable);

    sisName= utilTtfOpenTableStream( &ttte, "name", ttf );
    if  ( ! sisName )
	{ XDEB(sisName); rval= -1; goto ready;	}

    if  ( psTtfExtractNameRecords( sisName, ttte, ttnt ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sisName )
	{ sioInClose( sisName );	}

    return rval;
    }

static int utilTtfExtractPostTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisPost= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypePostTable *		ttpt= &(ttf->ttfPostTable);
    unsigned int		idxCount= 0;
    unsigned int		stringCount= 0;
    int				name;

    sisPost= utilTtfOpenTableStream( &ttte, "post", ttf );
    if  ( ! sisPost )
	{ XDEB(sisPost); rval= -1; goto ready;	}

    ttpt->ttptFormatUpper= sioEndianGetBeInt16( sisPost );
    ttpt->ttptFormatLower= sioEndianGetBeUint16( sisPost );
    ttpt->ttptItalicAngleUpper= sioEndianGetBeInt16( sisPost );
    ttpt->ttptItalicAngleLower= sioEndianGetBeUint16( sisPost );
    ttpt->ttptUnderlinePosition= sioEndianGetBeInt16( sisPost );
    ttpt->ttptUnderlineThickness= sioEndianGetBeInt16( sisPost );
    ttpt->ttptIsFixedPitch= sioEndianGetBeUint16( sisPost );
    /*reserved=*/ (void) sioEndianGetBeUint16( sisPost );
    ttpt->ttptMinMemType42= sioEndianGetBeUint32( sisPost );
    ttpt->ttptMaxMemType42= sioEndianGetBeUint32( sisPost );
    ttpt->ttptMinMemType1= sioEndianGetBeUint32( sisPost );
    ttpt->ttptMaxMemType1= sioEndianGetBeUint32( sisPost );

    switch( ttpt->ttptFormatUpper )
	{
	case 2:
	    if  ( ttpt->ttptFormatLower != 0 )
		{ LDEB(ttpt->ttptFormatLower); goto ready;	}

	    idxCount= sioEndianGetBeUint16( sisPost );
	    ttpt->ttptIndices= (unsigned short *)malloc( idxCount* sizeof(unsigned short) );
	    if  ( ! ttpt->ttptIndices )
		{ LXDEB( idxCount, ttpt->ttptIndices ); rval= -1; goto ready; }

	    for ( name= 0; name < idxCount; name++ )
		{ ttpt->ttptIndices[name]= 0; }
	    ttpt->ttptIndexCount= idxCount;

	    stringCount= 0;
	    for ( name= 0; name < idxCount; name++ )
		{
		ttpt->ttptIndices[name]= sioEndianGetBeUint16( sisPost );
		if  ( ttpt->ttptIndices[name] >= 258 )
		    { stringCount++;	}
		}

	    if  ( stringCount > 0 )
		{
		ttpt->ttptStrings= (unsigned char **)malloc( stringCount*
						    sizeof(unsigned char *) );
		if  ( ! ttpt->ttptStrings )
		    {
		    LXDEB(stringCount, ttpt->ttptStrings);
		    rval= -1; goto ready;
		    }

		for ( name= 0; name < stringCount; name++ )
		    { ttpt->ttptStrings[name]= (unsigned char *)0; }

		ttpt->ttptStringCount= stringCount;

		for ( name= 0; name < stringCount; name++ )
		    {
		    int	size= sioInGetByte( sisPost );

		    ttpt->ttptStrings[name]= (unsigned char *)malloc( size+ 1 );
		    if  ( ! ttpt->ttptStrings[name] )
			{
			LXDEB(size,ttpt->ttptStrings[name]);
			rval= -1; goto ready;
			}

		    if  ( sioInReadBytes( sisPost, ttpt->ttptStrings[name],
								size ) != size )
			{ LDEB(size); rval= -1; goto ready;	}
		    ttpt->ttptStrings[name][size]= '\0';
		    }
		}

	    break;

	case 3:
	    /* dummy post table: No glyph names */
	    break;

	default:
	    LLDEB(ttpt->ttptFormatUpper,ttpt->ttptFormatLower);
	    break;
	}

  ready:

    if  ( sisPost )
	{ sioInClose( sisPost );	}

    return rval;
    }

static int utilTtfExtractOS_2Table(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisOS_2= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeOS_2Table *		ttot= &(ttf->ttfOS_2Table);
    int				i;

    sisOS_2= utilTtfOpenTableStream( &ttte, "OS/2", ttf );
    if  ( ! sisOS_2 )
	{ XDEB(sisOS_2); goto ready;	}

    ttot->ttotVersion= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotAvgCharWidth= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotWeightClass= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotWidthClass= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotType= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptXSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptYSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptXOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSubscriptYOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptXSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptYSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptXOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotSuperscriptYOffset= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotStrikeoutSize= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotStrikeoutPosition= sioEndianGetBeInt16( sisOS_2 );
    ttot->ttotFamilyClass= sioEndianGetBeInt16( sisOS_2 );

    for ( i= 0; i < 10; i++ )
	{ ttot->ttotPanose[i]= sioInGetByte( sisOS_2 );	}

    ttot->ttotCharRange[0]= sioEndianGetBeUint32( sisOS_2 );
    ttot->ttotCharRange[1]= sioEndianGetBeUint32( sisOS_2 );
    ttot->ttotCharRange[2]= sioEndianGetBeUint32( sisOS_2 );
    ttot->ttotCharRange[3]= sioEndianGetBeUint32( sisOS_2 );

    for ( i= 0; i < 4; i++ )
	{ ttot->ttotVendID[i]= sioInGetByte( sisOS_2 );	}
    ttot->ttotVendID[i]= '\0';
    i= 3;
    while( i > 0 && ttot->ttotVendID[i] == ' ' )
	{ ttot->ttotVendID[i]= '\0'; i--; }

    ttot->ttotSelection= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotFirstCharIndex= sioEndianGetBeUint16( sisOS_2 );
    ttot->ttotLastCharIndex= sioEndianGetBeUint16( sisOS_2 );

  ready:

    if  ( sisOS_2 )
	{ sioInClose( sisOS_2 );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the Hhea table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractHheaTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisHhea= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    int				i;

    TrueTypeHheaTable *		hhea= &(ttf->ttfHheaTable);

    sisHhea= utilTtfOpenTableStream( &ttte, "hhea", ttf );
    if  ( ! sisHhea )
	{ XDEB(sisHhea); rval= -1; goto ready;	}

    for ( i= 0; i < 4; i++ )
	{ hhea->hheaVersion[i]= sioInGetByte( sisHhea ); }
    hhea->hheaVersion[4]= '\0';

    hhea->hheaAscender= sioEndianGetBeInt16( sisHhea );
    hhea->hheaDescender= sioEndianGetBeInt16( sisHhea );
    hhea->hheaLineGap= sioEndianGetBeInt16( sisHhea );
    hhea->hheaMaxAdvanceWidth= sioEndianGetBeUint16( sisHhea );
    hhea->hheaMinLsb= sioEndianGetBeInt16( sisHhea );
    hhea->hheaMinRsb= sioEndianGetBeInt16( sisHhea );
    hhea->hheaXMaxExtent= sioEndianGetBeInt16( sisHhea );
    hhea->hheaCaretSlopeRise= sioEndianGetBeInt16( sisHhea );
    hhea->hheaCaretSlopeRun= sioEndianGetBeInt16( sisHhea );

    for ( i= 0; i < 5; i++ )
	{ /*reserved=*/ (void)sioEndianGetBeInt16( sisHhea );	}

    hhea->hheaMetricDataFormat= sioEndianGetBeInt16( sisHhea );
    hhea->hheaMetricCount= sioEndianGetBeUint16( sisHhea );

  ready:

    if  ( sisHhea )
	{ sioInClose( sisHhea );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the Vhea table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractVheaTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisVhea= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    int				i;

    TrueTypeVheaTable *		vhea= &(ttf->ttfVheaTable);

    sisVhea= utilTtfOpenTableStream( &ttte, "vhea", ttf );
    if  ( ! sisVhea )
	{ /*XDEB(sisVhea); rval= -1; */ goto ready;	}

    for ( i= 0; i < 4; i++ )
	{ vhea->vheaVersion[i]= sioInGetByte( sisVhea ); }
    vhea->vheaVersion[4]= '\0';

    vhea->vheaAscender= sioEndianGetBeInt16( sisVhea );
    vhea->vheaDescender= sioEndianGetBeInt16( sisVhea );
    vhea->vheaLineGap= sioEndianGetBeInt16( sisVhea );
    vhea->vheaMaxAdvanceHeight= sioEndianGetBeUint16( sisVhea );
    vhea->vheaMinTsb= sioEndianGetBeInt16( sisVhea );
    vhea->vheaMinBsb= sioEndianGetBeInt16( sisVhea );
    vhea->vheaYMaxExtent= sioEndianGetBeInt16( sisVhea );
    vhea->vheaCaretSlopeRise= sioEndianGetBeInt16( sisVhea );
    vhea->vheaCaretSlopeRun= sioEndianGetBeInt16( sisVhea );
    vhea->vheaCaretOffset= sioEndianGetBeInt16( sisVhea );

    for ( i= 0; i < 4; i++ )
	{ /*reserved=*/ (void) sioEndianGetBeInt16( sisVhea );	}

    vhea->vheaMetricDataFormat= sioEndianGetBeInt16( sisVhea );
    vhea->vheaMetricCount= sioEndianGetBeUint16( sisVhea );

  ready:

    if  ( sisVhea )
	{ sioInClose( sisVhea );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the head table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractHeadTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisHead= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeHeadTable *		ttht= &(ttf->ttfHeadTable);

    int				i;

    sisHead= utilTtfOpenTableStream( &ttte, "head", ttf );
    if  ( ! sisHead )
	{ XDEB(sisHead); rval= -1; goto ready;	}

    ttht->tthtVersion= sioEndianGetBeUint32( sisHead );
    ttht->tthtFontRevision= sioEndianGetBeUint32( sisHead );
    ttht->tthtChecksumAdjust= sioEndianGetBeUint32( sisHead );
    ttht->tthtMagicNo= sioEndianGetBeUint32( sisHead );
    ttht->tthtFlags= sioEndianGetBeUint16( sisHead );
    ttht->tthtUnitsPerEm= sioEndianGetBeUint16( sisHead );

    for ( i= 0; i < 8; i++ )
	{ ttht->tthtCreated[i]= sioInGetByte( sisHead ); }
    ttht->tthtCreated[8]= '\0';

    for ( i= 0; i < 8; i++ )
	{ ttht->tthtModified[i]= sioInGetByte( sisHead ); }
    ttht->tthtModified[8]= '\0';

    ttht->tthtXMin= sioEndianGetBeInt16( sisHead );
    ttht->tthtYMin= sioEndianGetBeInt16( sisHead );
    ttht->tthtXMax= sioEndianGetBeInt16( sisHead );
    ttht->tthtYMax= sioEndianGetBeInt16( sisHead );

    ttht->tthtMacStyle= sioEndianGetBeUint16( sisHead );
    ttht->tthtLowestRecPPem= sioEndianGetBeUint16( sisHead );
    ttht->tthtFontDirection= sioEndianGetBeInt16( sisHead );
    ttht->tthtIndexToLocFormat= sioEndianGetBeInt16( sisHead );
    ttht->tthtGlyphDataFormat= sioEndianGetBeInt16( sisHead );

  ready:

    if  ( sisHead )
	{ sioInClose( sisHead );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the location table from a TrueType file.			*/
/*									*/
/************************************************************************/

static int utilTtfExtractLocaTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisLoca= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    TrueTypeHeadTable *		ttht= &(ttf->ttfHeadTable);

    int				i;

    sisLoca= utilTtfOpenTableStream( &ttte, "loca", ttf );
    if  ( ! sisLoca )
	{ XDEB(sisLoca); rval= -1; goto ready;	}

    switch( ttht->tthtIndexToLocFormat )
	{
	case 0:
	    if  ( ttte->ttteMemoryBuffer.mbSize % 2 )
		{ LLDEB(ttte->ttteMemoryBuffer.mbSize,2); rval= -1; goto ready; }
	    ttf->ttfLocationCount= ttte->ttteMemoryBuffer.mbSize/ 2;
	    break;

	case 1:
	    if  ( ttte->ttteMemoryBuffer.mbSize % 4 )
		{ LLDEB(ttte->ttteMemoryBuffer.mbSize,4); rval= -1; goto ready; }
	    ttf->ttfLocationCount= ttte->ttteMemoryBuffer.mbSize/ 4;
	    break;
	default:
	    LDEB(ttht->tthtIndexToLocFormat); rval= -1; goto ready;
	}

    ttf->ttfLocations= (unsigned long *)malloc( ttf->ttfLocationCount* sizeof(unsigned long) );
    if  ( ! ttf->ttfLocations )
	{
	LXDEB(ttf->ttfLocationCount,ttf->ttfLocations);
	rval= -1; goto ready;
	}

    switch( ttht->tthtIndexToLocFormat )
	{
	case 0:
	    for ( i= 0; i < ttf->ttfLocationCount; i++ )
		{ ttf->ttfLocations[i]= 2* sioEndianGetBeUint16( sisLoca ); }
	    break;

	case 1:
	    for ( i= 0; i < ttf->ttfLocationCount; i++ )
		{ ttf->ttfLocations[i]= sioEndianGetBeUint32( sisLoca ); }
	    break;

	default:
	    LDEB(ttht->tthtIndexToLocFormat); rval= -1; goto ready;
	}

  ready:

    if  ( sisLoca )
	{ sioInClose( sisLoca );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the horizontal metrics table from a TrueType file.		*/
/*									*/
/************************************************************************/

static int utilTtfExtractHmtxTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisHmtx= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;

    const TrueTypeHheaTable *	hhea= &(ttf->ttfHheaTable);

    int				i;
    int				partialCount;
    int				advanceWidth= 0;

    sisHmtx= utilTtfOpenTableStream( &ttte, "hmtx", ttf );
    if  ( ! sisHmtx )
	{ XDEB(sisHmtx); rval= -1; goto ready;	}

    if  ( ttte->ttteMemoryBuffer.mbSize % 2 )
	{
	LLDEB(ttte->ttteMemoryBuffer.mbSize,2);
	LLDEB(hhea->hheaMetricDataFormat,hhea->hheaMetricCount);
	/*rval= -1; goto ready;*/
	}
    partialCount= ( ttte->ttteMemoryBuffer.mbSize- 4 * hhea->hheaMetricCount )/ 2;

    ttf->ttfHorizontalMetricCount= hhea->hheaMetricCount+ partialCount;

    ttf->ttfHorizontalMetrics= (HorizontalMetric *)malloc( ttf->ttfHorizontalMetricCount*
						    sizeof(HorizontalMetric) );
    if  ( ! ttf->ttfHorizontalMetrics )
	{
	LXDEB(ttf->ttfHorizontalMetricCount,ttf->ttfHorizontalMetrics);
	rval= -1; goto ready;
	}

    for ( i= 0; i < hhea->hheaMetricCount; i++ )
	{
	ttf->ttfHorizontalMetrics[i].hmAdvanceWidth= advanceWidth=
					    sioEndianGetBeUint16( sisHmtx );
	ttf->ttfHorizontalMetrics[i].hmLsb= sioEndianGetBeInt16( sisHmtx );
	}

    for ( i= hhea->hheaMetricCount; i < ttf->ttfHorizontalMetricCount; i++ )
	{
	ttf->ttfHorizontalMetrics[i].hmAdvanceWidth= advanceWidth;
	ttf->ttfHorizontalMetrics[i].hmLsb= sioEndianGetBeInt16( sisHmtx );
	}

  ready:

    if  ( sisHmtx )
	{ sioInClose( sisHmtx );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the glyph table from a TrueType file.			*/
/*									*/
/************************************************************************/

# define GLYFSZ				2000

# define ONOROFF			0x01
# define XSHORT				0x02
# define YSHORT				0x04
# define REPEAT				0x08
# define XSAME				0x10
# define YSAME				0x20

# define ARG_1_AND_2_ARE_WORDS		0x0001
# define ARGS_ARE_XY_VALUES 		0x0002
# define XY_BOUND_TO_GRID		0x0004
# define WE_HAVE_A_SCALE		0x0008
# define MORE_COMPONENTS		0x0020
# define WE_HAVE_AN_X_AND_Y_SCALE	0x0040
# define WE_HAVE_A_TWO_BY_TWO		0x0080
# define WE_HAVE_INSTRUCTIONS		0x0100
# define USE_MY_METRICS			0x0200

# define fg_rmoveto(a,x,y) appDebug( "fg_rmoveto( %g, %g )\n", x, y )
# define fg_rlineto(a,x,y) appDebug( "fg_rlineto( %g, %g )\n", x, y )
# define g_closepath(a) appDebug( "g_closepath()\n" )
# define fg_rrcurveto(a,x1,y1,x2,y2,x3,y3) appDebug( "fg_rrcurveto(%g,%g,%g,%g,%g,%g)\n", x1,y1,x2,y2,x3,y3 )

static int utilTtfExtractGlyfTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    int				tab;
    const TrueTypeTableEntry *	ttte;
    TrueTypeGlyph *		ttg;

    int				glyphno;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ! strcmp( ttte->ttteTag, "glyf" ) )
	    { break;	}
	}

    if  ( tab >= ttf->ttfTableCount )
	{ LLDEB(tab,ttf->ttfTableCount); rval= -1; goto ready;	}

    ttf->ttfGlyphCount= ttf->ttfLocationCount- 1;

    ttf->ttfGlyphs= (TrueTypeGlyph *)malloc( ttf->ttfGlyphCount* sizeof(TrueTypeGlyph) );
    if  ( ! ttf->ttfGlyphs )
	{ LXDEB(ttf->ttfGlyphCount,ttf->ttfGlyphs); rval= -1; goto ready; }

    ttg= ttf->ttfGlyphs;
    for ( glyphno= 0; glyphno < ttf->ttfGlyphCount; ttg++, glyphno++ )
	{
	const unsigned char *	data;
	int			loc= ttf->ttfLocations[glyphno];

	if  ( loc+ 10 > ttte->ttteMemoryBuffer.mbSize )
	    {
	    /*LLDEB(loc,ttte->ttteMemoryBuffer.mbSize);*/

	    ttg->ttgContourCount= 0;
	    ttg->ttgXMin= 0;
	    ttg->ttgYMin= 0;
	    ttg->ttgXMax= 0;
	    ttg->ttgYMax= 0;

	    continue;
	    }

	data= ttte->ttteMemoryBuffer.mbBytes+ loc;

	/* signed ! */
	ttg->ttgContourCount= utilEndianExtractBeInt16( data ); data += 2;

	ttg->ttgXMin= utilEndianExtractBeInt16( data ); data += 2;
	ttg->ttgYMin= utilEndianExtractBeInt16( data ); data += 2;
	ttg->ttgXMax= utilEndianExtractBeInt16( data ); data += 2;
	ttg->ttgYMax= utilEndianExtractBeInt16( data ); data += 2;

	ttg->ttgData= data;
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the encoding table from a TrueType file.			*/
/*									*/
/*  Type 2 CMAP tables are almost useless for us: They refer to the	*/
/*  encoding of the font, rather than to the mapping of unicodes to	*/
/*  glyphs.								*/
/*									*/
/************************************************************************/

static int utilTtfExtractCmapTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisCmap= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;
    TrueTypeCmapTable *		ttct= &(ttf->ttfCmapTable);
    TrueTypeCmapRecord *	ttcr;

    int				i;

    sisCmap= utilTtfOpenTableStream( &ttte, "cmap", ttf );
    if  ( ! sisCmap )
	{ XDEB(sisCmap); rval= -1; goto ready;	}

    ttct->ttctVersion= sioEndianGetBeUint16( sisCmap );
    ttct->ttctEncodingRecordCount= sioEndianGetBeUint16( sisCmap );

    ttct->ttctEncodingRecords= (TrueTypeCmapRecord *)malloc( ttct->ttctEncodingRecordCount*
						sizeof(TrueTypeCmapRecord) );
    if  ( ! ttct->ttctEncodingRecords )
	{
	LXDEB(ttct->ttctEncodingRecordCount,ttct->ttctEncodingRecords);
	rval= -1; goto ready;
	}

    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{ utilInitTrueTypeCmapRecord( ttcr );	}

    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	const unsigned char *	p;
	const unsigned char *	pp;
	int			ent;
	int			count;

	ttcr->ttcrPlatformID= sioEndianGetBeUint16( sisCmap );
	ttcr->ttcrEncodingID= sioEndianGetBeUint16( sisCmap );
	ttcr->ttcrOffset= sioEndianGetBeUint32( sisCmap );

	p= ttte->ttteMemoryBuffer.mbBytes+ ttcr->ttcrOffset;
	ttcr->ttcrFormat= utilEndianExtractBeUint16( p ); p += 2;

	switch( ttcr->ttcrFormat )
	    {
	    int		k;

	    case 0:
		ttcr->ttcrLength= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcrLanguage= utilEndianExtractBeUint16( p ); p += 2;

		if  ( ttcr->ttcrLength < 6 )
		    {
		    LDEB(ttcr->ttcrLength);
		    LLDEB(ttcr->ttcrPlatformID,ttcr->ttcrEncodingID);
		    break;
		    }

		memcpy( ttcr->ttcr0GlyphIDArray, p, ttcr->ttcrLength- 6 );
		break;

	    case 2:
		ttcr->ttcrLength= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcrLanguage= utilEndianExtractBeUint16( p ); p += 2;

		ttcr->ttcr2SubHeaderKeys= p; p += 2* 256;
		ttcr->ttcr2SubHeaders= p;

		ttcr->ttcr2SubHeaderBytes= 0;
		for ( k= 0; k < 256; k++ )
		    {
		    int		byteOffset;

		    byteOffset= utilEndianExtractBeUint16(
					    ttcr->ttcr2SubHeaderKeys+ 2* k );
		    if  ( ttcr->ttcr2SubHeaderBytes < byteOffset )
			{ ttcr->ttcr2SubHeaderBytes= byteOffset;	}
		    }

		ttcr->ttcr2SubHeaderBytes += 8;

		p += ttcr->ttcr2SubHeaderBytes;
		ttcr->ttcr2GlyphIndexArray= p;
		break;

	    case 4:
		ttcr->ttcrLength= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcrLanguage= utilEndianExtractBeUint16( p ); p += 2;

		ttcr->ttcr4SegCountX2= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4SearchRange= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4EntrySelector= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4RangeShift= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr4Data= p;
		break;

	    case 6:
		ttcr->ttcrLength= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcrLanguage= utilEndianExtractBeUint16( p ); p += 2;

		ttcr->ttcr6FirstCode= utilEndianExtractBeUint16( p ); p += 2;
		ttcr->ttcr6_12CodeCount= utilEndianExtractBeUint16( p ); p += 2;

		ttcr->ttcr6_12GlyphCodes= (unsigned int *)malloc(
			    ttcr->ttcr6_12CodeCount* sizeof(unsigned int) );
		if  ( ! ttcr->ttcr6_12GlyphCodes )
		    {
		    LXDEB(ttcr->ttcr6_12CodeCount,ttcr->ttcr6_12GlyphCodes);
		    rval= -1; goto ready;
		    }
		for ( ent= 0; ent < ttcr->ttcr6_12CodeCount; ent++ )
		    {
		    ttcr->ttcr6_12GlyphCodes[ent]=
					utilEndianExtractBeUint16( p ); p += 2;
		    }
		break;

	    case 12:
		p += 2;
		ttcr->ttcrLength= utilEndianExtractBeUint32( p ); p += 4;
		ttcr->ttcrLanguage= utilEndianExtractBeUint32( p ); p += 4;

		ttcr->ttcr12GroupCount= utilEndianExtractBeUint32( p ); p += 4;
		pp= p;
		count= 0;
		for ( ent= 0; ent < ttcr->ttcr12GroupCount; ent++ )
		    {
		    int	sc, ec;

		    sc= utilEndianExtractBeUint32( pp ); pp += 4;
		    ec= utilEndianExtractBeUint32( pp ); pp += 4;
		    /*sg=*/ (void) utilEndianExtractBeUint32( pp ); pp += 4;
		    count += ec- sc+ 1;
		    }
		break;

	    default:
		LDEB(ttcr->ttcrFormat);
		break;
	    }
	}

  ready:

    if  ( sisCmap )
	{ sioInClose( sisCmap );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the kerning table from a TrueType file.			*/
/*									*/
/*  1)  The kerning table is optional, so it is not an error if it	*/
/*	cannot be opened.						*/
/*									*/
/************************************************************************/

static int ttfExtractFormat0KernPairs(	const TrueTypeKernSub *	ttks,
					const unsigned char *	p )
    {
    int				pair;
    TrueTypeKernPair *		ttkp;

    ttkp= ttks->ttksPairs;
    for ( pair= 0; pair < ttks->ttksPairCount; ttkp++, pair++ )
	{
	ttkp->ttkpLeft= utilEndianExtractBeUint16( p ); p += 2;
	ttkp->ttkpRight= utilEndianExtractBeUint16( p ); p += 2;
	ttkp->ttkpValue= utilEndianExtractBeInt16( p ); p += 2;
	}

    return 0;
    }

static int utilTtfExtractKernTable(	TrueTypeFont *	ttf )
    {
    int				rval= 0;

    SimpleInputStream *		sisKern= (SimpleInputStream *)0;
    const TrueTypeTableEntry *	ttte;
    TrueTypeKernTable *		ttkt= &(ttf->ttfKernTable);
    TrueTypeKernSub *		ttks;

    int				sub;
    int				subCount;
    const unsigned char *	data;

    /*  1  */
    sisKern= utilTtfOpenTableStream( &ttte, "kern", ttf );
    if  ( ! sisKern )
	{ rval= 0; goto ready;	}

    ttkt->ttktVersion= sioEndianGetBeUint16( sisKern );
    subCount= sioEndianGetBeUint16( sisKern );

    ttkt->ttktKernSubs= (TrueTypeKernSub *)malloc(
					subCount* sizeof(TrueTypeKernSub) );
    if  ( ! ttkt->ttktKernSubs )
	{
	LXDEB(subCount,ttkt->ttktKernSubs);
	rval= -1; goto ready;
	}

    ttks= ttkt->ttktKernSubs;
    for ( sub= 0; sub < subCount; ttks++, sub++ )
	{ utilInitTrueTypeKernSub( ttks );	}

    ttkt->ttktKernSubCount= subCount;

    data= ttte->ttteMemoryBuffer.mbBytes+ 4;

    ttks= ttkt->ttktKernSubs;
    for ( sub= 0; sub < subCount; ttks++, sub++ )
	{
	const unsigned char *	p= data;
	unsigned int		format;

	ttks->ttksVersion= utilEndianExtractBeUint16( p ); p += 2;
	ttks->ttksLength= utilEndianExtractBeUint16( p ); p += 2;
	ttks->ttksCoverage= utilEndianExtractBeUint16( p ); p += 2;
	ttks->ttksPairCount= utilEndianExtractBeUint16( p ); p += 2;

	if  ( ttks->ttksPairCount > 0 )
	    {
	    ttks->ttksPairs= (TrueTypeKernPair *)malloc( ttks->ttksPairCount*
						sizeof(TrueTypeKernPair) );
	    if  ( ! ttks->ttksPairs )
		{
		LXDEB(ttks->ttksPairCount,ttks->ttksPairs);
		ttks->ttksPairCount= 0;
		rval= -1; goto ready;
		}

	    format= ttks->ttksCoverage & 0x00ff;
	    switch( format )
		{
		case 0:
		case 1: /* against doc, as in FreeType library */
		    ttks->ttksSearchRange=
				utilEndianExtractBeUint16( p ); p += 2;
		    ttks->ttksEntrySelector=
				utilEndianExtractBeUint16( p ); p += 2;
		    ttks->ttksRangeShift=
				utilEndianExtractBeUint16( p ); p += 2;

		    if  ( ttfExtractFormat0KernPairs( ttks, p ) )
			{
			ttks->ttksPairCount= 0;
			LDEB(format); rval= -1; goto ready;
			}
		    break;

#		if 0
		case 1:
		LLDEB(sub,subCount);
		    {
		    const unsigned char *	stateTable= p;
		    const unsigned char *	classTable;

		    int stateClassCount;
		    int stateClassOffset;
		    int stateArrayOffset;
		    int entryTableOffset;
		    int valueOffset;

		    stateClassCount=
				utilEndianExtractBeUint16( p ); p += 2;
		    stateClassOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    stateArrayOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    entryTableOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    valueOffset=
				utilEndianExtractBeUint16( p ); p += 2;
		    }
		    break;
#		endif

		default:
		    XLDEB(ttks->ttksCoverage,ttks->ttksPairCount);
		    ttks->ttksPairCount= 0;
		    break;
		}
	    }

	data += ttks->ttksLength;
	}

  ready:

    if  ( sisKern )
	{ sioInClose( sisKern );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a true type font from file.					*/
/*									*/
/************************************************************************/

int psTtfLoadFont(	TrueTypeFont *		ttf,
			SimpleInputStream *	sisTtf,
			long			filePos )
    {
    int				tableCount;

    ttf->ttfVersion= sioEndianGetBeUint32( sisTtf ); filePos += 4;
    tableCount= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    ttf->ttfSearchRange= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    ttf->ttfEntrySelector= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    ttf->ttfRangeShift= sioEndianGetBeUint16( sisTtf ); filePos += 2;

    ttf->ttfTables= psTtfReadTableEntries( sisTtf, tableCount, filePos );
    if  ( ! ttf->ttfTables )
	{ LXDEB(tableCount,ttf->ttfTables); return -1;	}

    ttf->ttfTableCount= tableCount;

    if  ( utilTtfExtractHeadTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractNameTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractPostTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractHheaTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractVheaTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractLocaTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractHmtxTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractGlyfTable( ttf ) )
	{ LDEB(1); return -1;	}
    if  ( utilTtfExtractCmapTable( ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfExtractKernTable( ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfExtractOS_2Table( ttf ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int psTtcLoadFont(	TrueTypeFont *		ttf,
			SimpleInputStream *	sisTtf,
			int			fontIndex )
    {
    long		filePos= 0;

    char		tag[4+1];
    int			versionMA;
    long		fontCount;

    int			f;
    long		fontOffset;

    if  ( sioInReadBytes( sisTtf, (unsigned char *)tag, 4 ) != 4 )
	{ LDEB(4); return -1;	}
    filePos += 4;
    tag[4]= '\0';
    if  ( strcmp( tag, "ttcf" ) )
	{ SDEB(tag); return -1;	}

    versionMA= sioEndianGetBeUint16( sisTtf ); filePos += 2;
    /*versionMI=*/ (void) sioEndianGetBeUint16( sisTtf ); filePos += 2;
    if  ( versionMA < 0 || versionMA > 2 )
	{ LDEB(versionMA); return -1;	}

    fontCount= sioEndianGetBeUint32( sisTtf ); filePos += 4;
    if  ( fontCount <= fontIndex )
	{ LLDEB(fontCount,fontIndex); return -1;	}

    for ( f= 0; f < fontIndex; f++ )
	{ sioEndianGetBeUint32( sisTtf ); filePos += 4;	}
    fontOffset= sioEndianGetBeUint32( sisTtf ); filePos += 4;
    for ( f= fontIndex+ 1; f < fontCount; f++ )
	{ sioEndianGetBeUint32( sisTtf ); filePos += 4;	}

    if  ( versionMA == 2 )
	{
	sioEndianGetBeUint32( sisTtf ); filePos += 4;
	sioEndianGetBeUint32( sisTtf ); filePos += 4;
	sioEndianGetBeUint32( sisTtf ); filePos += 4;
	}

    while( filePos < fontOffset )
	{
	if  ( sioInGetByte( sisTtf ) < 0 )
	    { LDEB(1); return -1;	}

	filePos++;
	}

    return psTtfLoadFont( ttf, sisTtf, filePos );
    }

