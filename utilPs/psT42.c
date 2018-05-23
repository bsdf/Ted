/************************************************************************/
/*									*/
/*  Type 42 font utility functions. See:				*/
/*  http://partners.adobe.com/public/developer/en/font/5012.Type42_Spec.pdf */
/*									*/
/************************************************************************/

#   include	<stdio.h>
#   include	<string.h>

#   include	<sioHex.h>
#   include	<sioEndian.h>
#   include	<utilMemoryBuffer.h>
#   include	"psTtf.h"
#   include	"psReadWriteFontInfo.h"
#   include	<utilIndexMapping.h>
#   include	<appDebugon.h>

#   include	"psTtfIntern.h"

/************************************************************************/
/*									*/
/*  Write the CharStrings dictonary.					*/
/*									*/
/*  It attaches names to the glyphs enumerated in the 'loca' table.	*/
/*									*/
/************************************************************************/

static void utilTtfWriteCharStrings(	SimpleOutputStream *	sos,
					const AfmFontInfo *	afi )
    {
    int				g;

    sioOutPrintf( sos, "/CharStrings %d dict dup begin\n",
						    afi->afiMetricCount );

    for ( g= 0; g < afi->afiMetricCount; g++ )
	{
	const char *	name;

	name= psFontInfoGetGlyphName( afi, g );
	if  ( ! name || ! name[0] )
	    { continue;	}

	sioOutPrintf( sos, "  /%s %d def\n", name, g );
	}

    sioOutPrintf( sos, "end readonly def\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a selection of the true type file to the sfnts dictionary	*/
/*									*/
/*  1)  List of tables that might be used by the PostScript engine.	*/
/*	See paragraph 4.7 on p 12 of the T42 spec. [Techmote #5012].	*/
/*  2)  Determine new file offsets for the tables.			*/
/*									*/
/************************************************************************/

static long utilTtfWriteTableByteArray(	SimpleOutputStream *	sos,
					long			filePos,
					const char *		tag,
					const unsigned char *	bytes,
					unsigned long		count,
					int			pad )
    {
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;

    const int			wide= 72;
    const int			lastNL= 0;

    sioOutPrintf( sos, "%% %s %ld bytes @%lu\n", tag, count, filePos );

    sioOutPrintf( sos, "<\n" );
    sosHex= sioOutHexOpenFolded( sos, wide, lastNL );
    if  ( ! sosHex )
	{ XDEB(sosHex); filePos= -1; goto ready;	}

    if  ( sioOutWriteBytes( sosHex, bytes, count ) != count )
	{ LDEB(count); filePos= -1; goto ready;	}

    if  ( pad > 1 )
	{
	while( count % pad )
	    {
	    if  ( sioOutPutByte( '\0', sosHex ) < 0 )
		{ LDEB(count); filePos= -1; goto ready;	}

	    count++;
	    }
	}

    filePos += count;

    if  ( sioOutPutByte( '\0', sosHex ) < 0 )
	{ filePos= -1; goto ready;	}
    sioOutClose( sosHex ); sosHex= (SimpleOutputStream *)0;
    sioOutPrintf( sos, ">\n" );

  ready:

    if  ( sosHex )
	{ sioOutClose( sosHex );	}

    return filePos;
    }

static long utilTtfStepTableByteArray(	long			filePos,
					unsigned long		count,
					int			pad )
    {
    if  ( pad > 1 )
	{
	while( count % pad )
	    { count++; }
	}

    filePos += count;
    return filePos;
    }

static long psTtfLocaChunk(	int *				pPad,
				int *				pGlyphIdx,
				const TrueTypeFont *		ttf,
				const TrueTypeTableEntry *	ttte,
				int				upto,
				const int			glyphIdx )
    {
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);
    int				glyphCount= ttf->ttfLocationCount;
    int				glyphs;
    int				pad;

    switch( ttht->tthtIndexToLocFormat )
	{
	case 0:
	    pad= 2;
	    break;
	case 1:
	    pad= 4;
	    break;
	default:
	    LDEB(ttht->tthtIndexToLocFormat); return -1;
	}

    glyphs= glyphCount- glyphIdx;
    if  ( upto > pad* glyphCount )
	{ upto=  pad* glyphCount;	}

    upto= pad* ( upto / pad );
    glyphs= upto/ pad;
    if  ( glyphs == 0 )
	{ LDEB(glyphs); *pPad= 4; return upto+ 4; }

    if  ( upto == ttte->ttteMemoryBuffer.mbSize )
	{ pad= 4;	}

    *pPad= pad;
    *pGlyphIdx= glyphIdx+ glyphs;
    return upto;
    }

static long psTtfHmtxChunk(	int *				pPad,
				int *				pGlyphIdx,
				const TrueTypeFont *		ttf,
				const TrueTypeTableEntry *	ttte,
				int				upto,
				const int			glyphIdx )
    {
    const TrueTypeHheaTable *	hhea= &(ttf->ttfHheaTable);
    int				partialCount;
    int				fullCount;
    int				glyphs;
    int				pad;

    fullCount= hhea->hheaMetricCount;
    partialCount= ( ttte->ttteMemoryBuffer.mbSize- 4 * hhea->hheaMetricCount )/ 2;

    if  ( glyphIdx < fullCount )
	{
	pad= 4;
	glyphs= fullCount- glyphIdx;

	/*  Issue full metrics separately */
	if  ( upto > pad* fullCount )
	    { upto=  pad* fullCount;	}

	/*  Issue full metrics separately */
	upto= pad* ( upto / pad );
	glyphs= upto/ pad;

	*pPad= pad;
	*pGlyphIdx= glyphIdx+ glyphs;
	return upto;
	}

    pad= 2;
    glyphs= fullCount+ partialCount- glyphIdx;
    if  ( glyphs == 0 )
	{ LDEB(glyphs); *pPad= 4; return upto+ 4; }

    if  ( upto > 4* fullCount+ 2* partialCount )
	{ upto=  4* fullCount+ 2* partialCount;	}

    upto= pad* ( upto / pad );
    glyphs= upto/ pad;

    if  ( upto == ttte->ttteMemoryBuffer.mbSize )
	{ pad= 4;	}

    *pPad= pad;
    *pGlyphIdx= glyphIdx+ glyphs;
    return upto;
    }

static long psTtfVmtxChunk(	int *				pPad,
				int *				pGlyphIdx,
				const TrueTypeFont *		ttf,
				const TrueTypeTableEntry *	ttte,
				int				upto,
				const int			glyphIdx )
    {
    const TrueTypeVheaTable *	vhea= &(ttf->ttfVheaTable);
    int				partialCount;
    int				fullCount;
    int				glyphs;
    int				pad;

    fullCount= vhea->vheaMetricCount;
    partialCount= ( ttte->ttteMemoryBuffer.mbSize- 4 * vhea->vheaMetricCount )/ 2;

    if  ( glyphIdx < fullCount )
	{
	pad= 4;
	glyphs= fullCount- glyphIdx;

	/*  Issue full metrics separately */
	if  ( upto > pad* fullCount )
	    { upto=  pad* fullCount;	}

	/*  Issue full metrics separately */
	upto= pad* ( upto / pad );
	glyphs= upto/ pad;

	*pPad= pad;
	*pGlyphIdx= glyphIdx+ glyphs;
	return upto;
	}

    pad= 2;
    glyphs= fullCount+ partialCount- glyphIdx;
    if  ( glyphs == 0 )
	{ LDEB(glyphs); *pPad= 4; return upto+ 4; }

    if  ( upto > 4* fullCount+ 2* partialCount )
	{ upto=  4* fullCount+ 2* partialCount;	}

    upto= pad* ( upto / pad );
    glyphs= upto/ pad;

    if  ( upto == ttte->ttteMemoryBuffer.mbSize )
	{ pad= 4;	}

    *pPad= pad;
    *pGlyphIdx= glyphIdx+ glyphs;
    return upto;
    }

static long psTtfGlyphChunk(	int *				pPad,
				int *				pGlyphIdx,
				const TrueTypeFont *		ttf,
				const TrueTypeTableEntry *	ttte,
				int				upto,
				const int			glyphIdx )
    {
    int		l, r, m;
    int		pad= 2;

    l= glyphIdx; r= ttf->ttfLocationCount; m= ( l+ r )/ 2;

    while ( l < m )
	{
	if  ( ttf->ttfLocations[m] > upto )
	    { r= m;	}
	else{ l= m; }

	m= ( l+ r )/ 2;
	}

    if  ( m == ttf->ttfLocationCount- 1 )
	{ upto= ttte->ttteMemoryBuffer.mbSize; pad= 4;	}
    else{ upto= ttf->ttfLocations[m]; pad= 2;	}

    if  ( upto == ttte->ttteMemoryBuffer.mbSize )
	{ pad= 4;	}

    *pPad= pad; *pGlyphIdx= m;
    return upto;
    }

static int utilTtfWriteT42Sfnts(	SimpleOutputStream *	sos,
					const char *		debFontName,
					const TrueTypeFont *	ttf )
    {
    int				rval= 0;
    int				tab;
    TrueTypeTableEntry *	ttte= ttf->ttfTables;
    long			filePos;
    int				tableCount;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;

    const int			wide= 72;
    const int			lastNL= 0;

    /*  1  */
    static const char * const	sfntfTags[]=
	{
	"head", "hhea", "hmtx", "loca",
	"prep", "fpgm", "vhea", "glyf",
	"cvt ", "maxp", "vmtx",
	};

    /*  2  */
    tableCount= 0;
    filePos= 12;
    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	int	i;

	for ( i= 0; i < sizeof(sfntfTags)/sizeof(char *); i++ )
	    {
	    if  ( ! strcmp( ttte->ttteTag, sfntfTags[i] ) )
		{ break;	}
	    }

	if  ( i >= sizeof(sfntfTags)/sizeof(char *) )
	    { ttte->ttteSaveOffset= 0;	}
	else{
	    ttte->ttteSaveOffset= filePos;
	    tableCount++;

	    if  ( ttte->ttteMemoryBuffer.mbSize > 65528 )
		{
		long	pos= 0;
		int	glyphIdx= 0;
		long	upto= 65534;

		if  ( ! strcmp( ttte->ttteTag, "glyf" ) )
		    {
		    while( pos < ttte->ttteMemoryBuffer.mbSize )
			{
			int		pad= 2;

			upto= psTtfGlyphChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

			filePos= utilTtfStepTableByteArray( filePos,
							    upto- pos, pad );
			pos= upto;
			upto += 65528;
			}

		    continue;
		    }

		if  ( ! strcmp( ttte->ttteTag, "hmtx" ) )
		    {
		    while( pos < ttte->ttteMemoryBuffer.mbSize )
			{
			int		pad= 2;

			upto= psTtfHmtxChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

			filePos= utilTtfStepTableByteArray( filePos,
							    upto- pos, pad );
			pos= upto;
			upto += 65528;
			}

		    continue;
		    }

		if  ( ! strcmp( ttte->ttteTag, "vmtx" ) )
		    {
		    while( pos < ttte->ttteMemoryBuffer.mbSize )
			{
			int		pad= 2;

			upto= psTtfVmtxChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

			filePos= utilTtfStepTableByteArray( filePos,
							    upto- pos, pad );
			pos= upto;
			upto += 65528;
			}

		    continue;
		    }

		if  ( ! strcmp( ttte->ttteTag, "loca" ) )
		    {
		    while( pos < ttte->ttteMemoryBuffer.mbSize )
			{
			int		pad= 2;

			upto= psTtfLocaChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

			filePos= utilTtfStepTableByteArray( filePos,
							    upto- pos, pad );
			pos= upto;
			upto += 65528;
			}

		    continue;
		    }

		SSLDEB(debFontName,ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize);
		rval= -1; goto ready;
		}
	    else{
		filePos= utilTtfStepTableByteArray( filePos,
							ttte->ttteMemoryBuffer.mbSize, 4 );
		}
	    }
	}

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ttte->ttteSaveOffset > 0 )
	    { ttte->ttteSaveOffset += 16* tableCount;	}
	}

    filePos= 0;

    sioOutPrintf( sos, "/sfnts[\n" );

    sioOutPrintf( sos, "<\n" );
    sosHex= sioOutHexOpenFolded( sos, wide, lastNL );
    if  ( ! sosHex )
	{ XDEB(sosHex); rval= -1; goto ready;	}

    sioEndianPutBeUint32( ttf->ttfVersion, sosHex ); filePos += 4;
    sioEndianPutBeUint16( tableCount, sosHex ); filePos += 2;
    sioEndianPutBeUint16( ttf->ttfSearchRange, sosHex ); filePos += 2;
    sioEndianPutBeUint16( ttf->ttfEntrySelector, sosHex ); filePos += 2;
    sioEndianPutBeUint16( ttf->ttfRangeShift, sosHex ); filePos += 2;

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	int		b;

	if  ( ttte->ttteSaveOffset == 0 )
	    { continue;	}

	for ( b= 0; b < 4; b++ )
	    {
	    if  ( sioOutPutByte( ttte->ttteTag[b], sosHex ) < 0 )
		{ rval= -1; goto ready;	}
	    }
	filePos += 4;

	sioEndianPutBeUint32( ttte->ttteChecksum, sosHex ); filePos += 4;
	sioEndianPutBeUint32( ttte->ttteSaveOffset, sosHex ); filePos += 4;
	sioEndianPutBeUint32( ttte->ttteMemoryBuffer.mbSize, sosHex ); filePos += 4;
	}

    if  ( sioOutPutByte( '\0', sosHex ) < 0 )
	{ rval= -1; goto ready;	}
    sioOutClose( sosHex ); sosHex= (SimpleOutputStream *)0;
    sioOutPrintf( sos, ">\n" );

    ttte= ttf->ttfTables;
    for ( tab= 0; tab < ttf->ttfTableCount; ttte++, tab++ )
	{
	if  ( ttte->ttteSaveOffset == 0 )
	    { continue;	}

	if  ( filePos != ttte->ttteSaveOffset )
	    {
	    SLLDEB(ttte->ttteTag,filePos,ttte->ttteSaveOffset);
	    rval= -1; goto ready;
	    }

	if  ( ttte->ttteMemoryBuffer.mbSize > 65528 )
	    {
	    long	pos= 0;
	    int		glyphIdx= 0;
	    long	upto= 65534;

	    if  ( ! strcmp( ttte->ttteTag, "glyf" ) )
		{
		while( pos < ttte->ttteMemoryBuffer.mbSize )
		    {
		    int		pad= 2;

		    upto= psTtfGlyphChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

		    filePos= utilTtfWriteTableByteArray( sos, filePos,
					ttte->ttteTag, ttte->ttteMemoryBuffer.mbBytes+ pos,
					upto- pos, pad );
		    if  ( filePos < 0 )
			{
			SLLDEB(ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize,filePos);
			rval= -1; goto ready;
			}

		    pos= upto;
		    upto += 65528;
		    }

		continue;
		}

	    if  ( ! strcmp( ttte->ttteTag, "hmtx" ) )
		{
		while( pos < ttte->ttteMemoryBuffer.mbSize )
		    {
		    int		pad= 2;

		    upto= psTtfHmtxChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

		    filePos= utilTtfWriteTableByteArray( sos, filePos,
					ttte->ttteTag, ttte->ttteMemoryBuffer.mbBytes+ pos,
					upto- pos, pad );
		    if  ( filePos < 0 )
			{
			SLLDEB(ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize,filePos);
			rval= -1; goto ready;
			}

		    pos= upto;
		    upto += 65528;
		    }

		continue;
		}

	    if  ( ! strcmp( ttte->ttteTag, "vmtx" ) )
		{
		while( pos < ttte->ttteMemoryBuffer.mbSize )
		    {
		    int		pad= 2;

		    upto= psTtfVmtxChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

		    filePos= utilTtfWriteTableByteArray( sos, filePos,
					ttte->ttteTag, ttte->ttteMemoryBuffer.mbBytes+ pos,
					upto- pos, pad );
		    if  ( filePos < 0 )
			{
			SLLDEB(ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize,filePos);
			rval= -1; goto ready;
			}

		    pos= upto;
		    upto += 65528;
		    }

		continue;
		}

	    if  ( ! strcmp( ttte->ttteTag, "loca" ) )
		{
		while( pos < ttte->ttteMemoryBuffer.mbSize )
		    {
		    int		pad= 2;

		    upto= psTtfLocaChunk( &pad, &glyphIdx,
						ttf, ttte, upto, glyphIdx );

		    filePos= utilTtfWriteTableByteArray( sos, filePos,
					ttte->ttteTag, ttte->ttteMemoryBuffer.mbBytes+ pos,
					upto- pos, pad );
		    if  ( filePos < 0 )
			{
			SLLDEB(ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize,filePos);
			rval= -1; goto ready;
			}

		    pos= upto;
		    upto += 65528;
		    }

		continue;
		}

	    SSLDEB(debFontName,ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize);
	    rval= -1; goto ready;
	    }
	else{
	    filePos= utilTtfWriteTableByteArray( sos, filePos, ttte->ttteTag,
				    ttte->ttteMemoryBuffer.mbBytes, ttte->ttteMemoryBuffer.mbSize, 4 );
	    if  ( filePos < 0 )
		{
		SLLDEB(ttte->ttteTag,ttte->ttteMemoryBuffer.mbSize,filePos);
		rval= -1; goto ready;
		}
	    }
	}

    sioOutPrintf( sos, "]def\n" );

  ready:

    if  ( sosHex )
	{ sioOutClose( sosHex );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit the contents of a PostScript font encoding array.		*/
/*									*/
/************************************************************************/

static void psWriteFontEncoding(	SimpleOutputStream *	sos,
					const AfmFontInfo *	afi )
    {
    int		glyph;

    for ( glyph= 0; glyph < afi->afiMetricCount; glyph++ )
	{
	const AfmCharMetric *	acm= afi->afiMetrics[glyph];
	const AfmCodeList *	acl= &(acm->acmDefaultCodeList);
	int			code;

	for ( code= 0; code < acl->aclCodeCount; code++ )
	    {
	    sioOutPrintf( sos, "   dup %d /%s put\n",
					    acl->aclCodes[code], acm->acmN );
	    }
	}
    }

static int utilTtfWriteT42(	SimpleOutputStream *	sos,
				const AfmFontInfo *	afi,
				const TrueTypeFont *	ttf )
    {
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);

    sioOutPrintf( sos, "%%!PS-TrueTypeFont-%lu-%lu\n",
				    ttht->tthtVersion,
				    ttht->tthtFontRevision );
    sioOutPrintf( sos, "11 dict begin\n" );
    sioOutPrintf( sos, "  /FontName /%s def\n", afi->afiFontName );
    sioOutPrintf( sos, "  /Encoding 256 array\n" );
    sioOutPrintf( sos, "   0 1 255 { 1 index exch /.notdef put } for\n" );

    psWriteFontEncoding( sos, afi );

    sioOutPrintf( sos, "  readonly def\n" );
    sioOutPrintf( sos, "  /PaintType 0 def\n" );
    sioOutPrintf( sos, "  /FontMatrix [1 0 0 1 0 0] def\n" );
    sioOutPrintf( sos, "  /FontBBox [%d %d %d %d] def\n",
					afi->afiFontBBox.drX0,
					afi->afiFontBBox.drY0,
					afi->afiFontBBox.drX1,
					afi->afiFontBBox.drY1 );
    sioOutPrintf( sos, "  /FontType 42 def\n" );

    psWriteFontInfoDict( sos, afi );

    utilTtfWriteT42Sfnts( sos, afi->afiFontName, ttf );

    utilTtfWriteCharStrings( sos, afi );

    sioOutPrintf( sos, "FontName currentdict end definefont pop\n" );

    return 0;
    }

static int psTtfWriteT42File(	SimpleOutputStream *	sosPf42,
				const TrueTypeFont *	ttf )
    {
    int				rval= 0;

    AfmFontInfo			afi;

    psInitAfmFontInfo( &afi );

    if  ( psTtfFontInfo( &afi, ttf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilTtfWriteT42( sosPf42, &afi, ttf );

  ready:

    psCleanAfmFontInfo( &afi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Translate Truetype font to PostScript type 42.			*/
/*									*/
/************************************************************************/

int psTtfToPt42(		SimpleOutputStream *	sosPf42,
				const MemoryBuffer *	fontFileName,
				SimpleInputStream *	sisTtf )
    {
    int				rval= 0;
    TrueTypeFont		ttf;

    psTtfInitTrueTypeFont( &ttf );

    if  ( psTtfLoadFont( &ttf, sisTtf, 0L ) )
	{ LDEB(1); rval= -1; goto ready;	}

    psTtfWriteT42File( sosPf42, &ttf );

  ready:

    psTtfCleanTrueTypeFont( &ttf );

    return rval;
    }

int psTtcToPt42(		SimpleOutputStream *	sosPf42,
				const MemoryBuffer *	fontFileName,
				int			fontFileIndex,
				SimpleInputStream *	sisTtf )
    {
    int				rval= 0;
    TrueTypeFont		ttf;

    psTtfInitTrueTypeFont( &ttf );

    if  ( psTtcLoadFont( &ttf, sisTtf, fontFileIndex ) )
	{ LDEB(1); rval= -1; goto ready;	}

    psTtfWriteT42File( sosPf42, &ttf );

  ready:

    psTtfCleanTrueTypeFont( &ttf );

    return rval;
    }

