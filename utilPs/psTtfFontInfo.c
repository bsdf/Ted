#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<utilEndian.h>
#   include	<utilMemoryBuffer.h>
#   include	"psFontName.h"
#   include	"psGlyphs.h"
#   include	"psTtf.h"
#   include	<utilIndexMapping.h>
#   include	<appDebugon.h>

#   include	"psTtfIntern.h"

/************************************************************************/
/*									*/
/*  Try to find names for a format0 unicode cmap.			*/
/*									*/
/************************************************************************/

static int utilTtfSetUnicodeCmapFormat0Names(
				const TrueTypeFont *		ttf,
				const TrueTypeCmapRecord *	ttcr,
				AfmFontInfo *			afi )
    {
    int		v;

    for ( v= 0; v < ttcr->ttcrLength- 6; v++ )
	{
	int		g= ttcr->ttcr0GlyphIDArray[v];
	const char *	name;

	if  ( g < 0 || g >= ttf->ttfGlyphCount )
	    { LLDEB(g,ttf->ttfGlyphCount); return -1; }

	name= psFontInfoGetGlyphName( afi, g );
	if  ( name && name[0] )
	    { continue;	}

	name= psUnicodeToGlyphName( v );
	if  ( ! name )
	    { XDEB(name); return -1;	}
	if  ( psFontInfoSetGlyphName( afi, g, name ) )
	    { LSDEB(g,name); return -1;	}
	}

    return 0;
    }

#   if 0
/************************************************************************/
/*									*/
/*  Try to find names for a format2 unicode cmap.			*/
/*									*/
/*  Type 2 CMAP tables are almost useless for us: They refer to the	*/
/*  encoding of the font, rather than to the mapping of unicodes to	*/
/*  glyphs.								*/
/*									*/
/************************************************************************/

static int utilTtfSetUnicodeCmapFormat2Names(
				const TrueTypeFont *		ttf,
				const TrueTypeCmapRecord *	ttcr,
				AfmFontInfo *			afi )
    {
    const unsigned char *	shk= ttcr->ttcr2SubHeaderKeys;
    int				hiByte;

    int				done0= 0;

    if  ( ! shk )
	{ XDEB(shk); return -1; }

    for ( hiByte= 0; hiByte < 256; hiByte++ )
	{
	int			byteOffset;
	const unsigned char *	p;

	int			firstCode;
	int			entryCount;
	int			idDelta;
	int			idRangeOffset;

	int			idx;

	int			e;

	byteOffset= utilEndianExtractBeUint16( shk ); shk += 2;
	if  ( byteOffset >= ttcr->ttcr2SubHeaderBytes )
	    { LLDEB(byteOffset,ttcr->ttcr2SubHeaderBytes); continue;	}
	highOffset= byteOffset/ 8;

	p= ttcr->ttcr2SubHeaders+ byteOffset;

	firstCode= utilEndianExtractBeUint16( p ); p += 2;
	entryCount= utilEndianExtractBeUint16( p ); p += 2;
	idDelta= utilEndianExtractBeInt16( p ); p += 2;
	idRangeOffset= utilEndianExtractBeUint16( p ); p += 2;

	appDebug( "%3d: %3d+%3d [@%-6d] idDelta=%6d idRangeOffset=%6d\n",
			hiByte, firstCode, entryCount, byteOffset,
			idDelta, idRangeOffset );

	for ( e= 0; e < entryCount; e++ )
	    {
	    idx= utilEndianExtractBeUint16( p+ idRangeOffset+ 2* e );

	    if  ( idx == 0 )
		{
		/*
		appDebug( "    0x%04x->%6d\n",
				256* hiByte+ firstCode+ e, idx );
		*/
		}
	    else{
		int	glyphIndex= 256* highOffset+ ( idx+ idDelta ) % 65536;

		appDebug( "    0x%04x->%6d%s\n",
			256* hiByte+ firstCode+ e, glyphIndex,
			((glyphIndex>=afi->afiMetricCount)?" ###":"")
			);
		}
	    }
	}

    return 0;
    }
#   endif

/************************************************************************/
/*									*/
/*  Try to find names for a format4 unicode cmap.			*/
/*									*/
/************************************************************************/

static int utilTtfSetUnicodeCmapFormat4Names(
				const TrueTypeFont *		ttf,
				const TrueTypeCmapRecord *	ttcr,
				AfmFontInfo *			afi )
    {
    const unsigned char *	p= ttcr->ttcr4Data;

    int				n= ttcr->ttcr4SegCountX2;
    const unsigned char *	e= p+ 0;
    const unsigned char *	s= p+ 1* n+ 2;
    const unsigned char *	d= p+ 2* n+ 2;
    const unsigned char *	r= p+ 3* n+ 2;

    int				seg;
    int				segCount= n/ 2- 1;

    if  ( ! p )
	{ XDEB(p); return -1; }

    for ( seg= 0; seg < segCount; seg++ )
	{
	int	startCode;
	int	endCode;
	int	idDelta;
	int	idRangeOffset;

	int	v;

	startCode= utilEndianExtractBeUint16( s ); s += 2;
	endCode= utilEndianExtractBeUint16( e ); e += 2;
	idDelta= utilEndianExtractBeUint16( d ); d += 2;
	idRangeOffset= utilEndianExtractBeUint16( r ); r += 2;

	for ( v= startCode; v <= endCode; v++ )
	    {
	    int			g;
	    const char *	name;

	    if  ( idRangeOffset > 0 )
		{
		int o= idRangeOffset/ 2+ ( v- startCode );
		g= utilEndianExtractBeUint16( r+ 2* o- 2 );
		}
	    else{
		g= ( v+ idDelta ) % 65536;
		}

	    if  ( g < 0 || g >= ttf->ttfGlyphCount )
		{ LLLDEB(g,ttf->ttfGlyphCount,idRangeOffset); return -1; }

	    name= psFontInfoGetGlyphName( afi, g );
	    if  ( name && name[0] )
		{ continue;	}

	    name= psUnicodeToGlyphName( v );
	    if  ( ! name )
		{ XDEB(name); return -1;	}
	    if  ( psFontInfoSetGlyphName( afi, g, name ) )
		{ LSDEB(g,name); return -1;	}
	    }
	}

    return 0;
    }

static int utilTtfSetWindowsCmapFormat4Names(
				const TrueTypeFont *		ttf,
				const TrueTypeCmapRecord *	ttcr,
				AfmFontInfo *			afi )
    {
    const unsigned char *	p= ttcr->ttcr4Data;

    int				n= ttcr->ttcr4SegCountX2;
    const unsigned char *	e= p+ 0;
    const unsigned char *	s= p+ 1* n+ 2;
    const unsigned char *	d= p+ 2* n+ 2;
    const unsigned char *	r= p+ 3* n+ 2;

    int				seg;
    int				segCount= n/ 2- 1;

    if  ( ! p )
	{ XDEB(p); return -1; }

    for ( seg= 0; seg < segCount; seg++ )
	{
	int	startCode;
	int	endCode;
	int	idDelta;
	int	idRangeOffset;

	int	v;

	startCode= utilEndianExtractBeUint16( s ); s += 2;
	endCode= utilEndianExtractBeUint16( e ); e += 2;
	idDelta= utilEndianExtractBeUint16( d ); d += 2;
	idRangeOffset= utilEndianExtractBeUint16( r ); r += 2;

	for ( v= startCode; v <= endCode; v++ )
	    {
	    int			c;
	    int			g;
	    const char *	name;
	    char		scratch[25];

	    if  ( idRangeOffset > 0 )
		{
		int o= idRangeOffset/ 2+ ( v- startCode );
		g= utilEndianExtractBeUint16( r+ 2* o- 2 );
		}
	    else{
		g= ( v+ idDelta ) % 65536;
		}

	    c= v % 256;

	    /*  HACK: MS Symbol */
	    if  ( ttcr->ttcrEncodingID == 0 /* && v < 256 */ )
		{
		if  ( psFontInfoSetGlyphCode( afi, g, c ) )
		    { LDEB(g); return -1;	}
		}

	    if  ( g < 0 || g >= ttf->ttfGlyphCount )
		{ SLLDEB(afi->afiFontName,g,ttf->ttfGlyphCount); continue; }

	    name= psFontInfoGetGlyphName( afi, g );
	    if  ( name && name[0] )
		{ continue;	}

	    sprintf( scratch, "win%04x", v );
	    if  ( psFontInfoSetGlyphName( afi, g, scratch ) )
		{ LSDEB(g,name); return -1;	}
	    if  ( psFontInfoSetGlyphCode( afi, g, c ) )
		{ LSDEB(g,name); return -1;	}
	    }
	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Try to set glyph names from the names found in the post table.	*/
/*									*/
/************************************************************************/

static int utilTtfSetPostGlyphNames(
				const TrueTypeFont *		ttf,
				AfmFontInfo *			afi )
    {
    const TrueTypePostTable *	ttpt= &(ttf->ttfPostTable);

    if  ( ttpt->ttptFormatUpper == 2 && ttpt->ttptFormatLower == 0 )
	{
	int			g;
	TrueTypeGlyph *		ttg;

	ttg= ttf->ttfGlyphs;
	for ( g= 0; g < ttf->ttfGlyphCount; ttg++, g++ )
	    {
	    int			idx;
	    const char *	name;

	    name= psFontInfoGetGlyphName( afi, g );
	    if  ( name && name[0] )
		{ continue;	}

	    if  ( g >= ttpt->ttptIndexCount )
		{ LLDEB(g,ttpt->ttptIndexCount); break; }

	    idx= ttpt->ttptIndices[g];
	    if  ( idx < 258 )
		{
		if  ( psFontInfoSetGlyphName( afi, g,
					    psAppleTtfGlyphNames[idx] ) )
		    { LSDEB(g,psAppleTtfGlyphNames[idx]); return -1;	}

		continue;
		}

	    idx -= 258;
	    if  ( idx >= ttpt->ttptStringCount )
		{
		/* SLLDEB(afi->afiFontName,idx,ttpt->ttptStringCount); */
		continue;
		}

	    if  ( psFontInfoSetGlyphName( afi, g,
					(char *)ttpt->ttptStrings[idx] ) )
		{ LSDEB(g,(char *)ttpt->ttptStrings[idx]); return -1;	}
	    }

	return 0;
	}

    /*  post table has no glyph names */
    if  ( ttpt->ttptFormatUpper == 3 && ttpt->ttptFormatLower == 0 )
	{ return 0;	}

    LLDEB(ttpt->ttptFormatUpper,ttpt->ttptFormatLower);
    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to find names for the glyphs in the font.			*/
/*									*/
/*  1)  Look for unicode encoding.					*/
/*  2)  Requirement per Appendix B of the ttf specification. (1)	*/
/*  3)  Requirement per Appendix B of the ttf specification. (2)	*/
/*  4)  Give post names priority.					*/
/*									*/
/************************************************************************/

static int utilTtfSetCharNames(	const TrueTypeFont *	ttf,
				AfmFontInfo *		afi )
    {
    const TrueTypeCmapTable *	ttct= &(ttf->ttfCmapTable);
    const TrueTypeCmapRecord *	ttcr;

    int				i;

    /*  2  */
    if  ( psFontInfoSetGlyphName( afi, 0, ".notdef" ) )
	{ LDEB(0); return -1;	}
    /*  3  */
    if  ( psFontInfoSetGlyphName( afi, 1, ".null" ) )
	{ LDEB(0); return -1;	}

    /*  4  */
    utilTtfSetPostGlyphNames( ttf, afi );

    /*  1  */
    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	if  ( ttcr->ttcrPlatformID == 0		&&
	      ttcr->ttcrFormat == 4		)
	    { break;	}
	if  ( ttcr->ttcrPlatformID == 3		&&
	      ttcr->ttcrEncodingID == 1		&&
	      ttcr->ttcrFormat == 4		)
	    { break;	}
	}

    if  ( i < ttct->ttctEncodingRecordCount )
	{
	if  ( utilTtfSetUnicodeCmapFormat4Names( ttf, ttcr, afi ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    /* * */
    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	if  ( ttcr->ttcrPlatformID == 0		&&
	      ttcr->ttcrFormat == 0		&&
	      ttcr->ttcrLength > 6		)
	    { break;	}
	}

    if  ( i < ttct->ttctEncodingRecordCount )
	{
	if  ( utilTtfSetUnicodeCmapFormat0Names( ttf, ttcr, afi ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    /* MS */
    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{
	if  ( ttcr->ttcrPlatformID == 3		&&
	      ttcr->ttcrFormat == 4		)
	    { break;	}
	}

    if  ( i < ttct->ttctEncodingRecordCount )
	{
	if  ( utilTtfSetWindowsCmapFormat4Names( ttf, ttcr, afi ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    /* * */
    ttcr= ttct->ttctEncodingRecords;
    for ( i= 0; i < ttct->ttctEncodingRecordCount; ttcr++, i++ )
	{ LLLDEB(ttcr->ttcrPlatformID,ttcr->ttcrEncodingID,ttcr->ttcrFormat); }

    LDEB(ttct->ttctEncodingRecordCount); return -1;
    }

/************************************************************************/
/*									*/
/*  Write an afm metric file.						*/
/*									*/
/************************************************************************/

static int utilTtfScaledInteger(	const TrueTypeFont *	ttf,
					int			val )
    {
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);
    double			fac= 1000.0/ ttht->tthtUnitsPerEm;

    if  ( val > 0 )
	{ return fac* val+ 0.5;	}
    else{ return fac* val- 0.5;	}
    }

static int utilTtfSetCharMetrics(	const TrueTypeFont *	ttf,
					AfmFontInfo *		afi )
    {
    const TrueTypeGlyph *	ttg;
    const HorizontalMetric *	hm;

    int				glyphno;

    afi->afiFallbackGlyph= 0;

    ttg= ttf->ttfGlyphs;
    hm= ttf->ttfHorizontalMetrics;
    for ( glyphno= 0; glyphno < ttf->ttfGlyphCount; ttg++, hm++, glyphno++ )
	{
	DocumentRectangle	bbox;
	int			WX;
	int			C= -1;
	const char *		N= (const char *)0;

	if  ( glyphno >= ttf->ttfHorizontalMetricCount )
	    {
	    LLDEB(glyphno,ttf->ttfHorizontalMetricCount);
	    WX= utilTtfScaledInteger( ttf, ttg->ttgXMax );
	    }
	else{
	    WX= utilTtfScaledInteger( ttf, hm->hmAdvanceWidth );
	    }

	bbox.drX0= utilTtfScaledInteger( ttf, ttg->ttgXMin );
	bbox.drY0= utilTtfScaledInteger( ttf, ttg->ttgYMin );
	bbox.drX1= utilTtfScaledInteger( ttf, ttg->ttgXMax );
	bbox.drY1= utilTtfScaledInteger( ttf, ttg->ttgYMax );

	if  ( psFontInfoAddMetric( afi, C, WX, N, &bbox ) )
	    { LDEB(afi->afiMetricCount); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract kerning pairs from the true type font.			*/
/*									*/
/************************************************************************/

static int utilTtfGetKernPairs(		AfmFontInfo *		afi,
					const TrueTypeFont *	ttf )
    {
    int				k0;
    int				glyphno;

    const TrueTypeKernTable *	ttkt= &(ttf->ttfKernTable);

    int				sub;
    const TrueTypeKernSub *	ttks;

    const TrueTypeKernPair *	ttkp;

    ttks= ttkt->ttktKernSubs;
    for ( sub= 0; sub < ttkt->ttktKernSubCount; ttks++, sub++ )
	{
	if  ( ttks->ttksPairCount > 0 )
	    { break;	}
	}

    if  ( sub >= ttkt->ttktKernSubCount )
	{ return 0;	}

    k0= 0; ttkp= ttks->ttksPairs;
    for ( glyphno= 0; glyphno < afi->afiMetricCount; glyphno++ )
	{
	int		n;
	int		i;

	AfmCharMetric *	acm= afi->afiMetrics[glyphno];

	if  ( ! acm )
	    { XDEB(acm); return -1;	}

	while( k0 < ttks->ttksPairCount		&&
	       ttkp->ttkpLeft < glyphno		)
	    { ttkp++; k0++;	}

	if  ( k0 >= ttks->ttksPairCount )
	    { break;	}
	if  ( ttkp->ttkpLeft > glyphno )
	    { continue;	}

	n= 1;
	while( k0+ n < ttks->ttksPairCount	&&
	       ttkp[n].ttkpLeft == glyphno	)
	    { n++;	}

	acm->acmKernPairs= (AfmKerningPair *)malloc( n* sizeof( AfmKerningPair ) );
	if  ( ! acm->acmKernPairs )
	    { LXDEB(n,acm->acmKernPairs); return -1;	}

	for ( i= 0; i < n; i++ )
	    {
	    acm->acmKernPairs[i].akpPosition= ttkp[i].ttkpRight;
	    acm->acmKernPairs[i].akpXVec=
			    utilTtfScaledInteger( ttf, ttkp[i].ttkpValue );
	    }

	acm->acmKernPairCount= n;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract character metrics from the true type font.			*/
/*									*/
/*  1)  Find glyph names.						*/
/*  2)  See whether we can derive known encodings from the glyph names.	*/
/*  3)  Is any of our encodings supported?				*/
/*  4)  Symply use the encoding as found in the file.			*/
/*  5)  If even that does not work, invent one.				*/
/*									*/
/************************************************************************/

static int utilTtfGetCharMetricsAndEncoding(
					AfmFontInfo *		afi,
					const TrueTypeFont *	ttf )
    {
    if  ( utilTtfSetCharMetrics( ttf, afi ) )
	{ LDEB(ttf->ttfGlyphCount); return -1;	}

    /*  1 */
    utilTtfSetCharNames( ttf, afi );

    /*  2 */

    return 0;
    }

/************************************************************************/

static int psTtfGetNames(	AfmFontInfo *		afi,
				char **			pSavedVariant,
				const TrueTypeFont *	ttf )
    {
    const TrueTypeNameTable *	ttnt= &(ttf->ttfNameTable);

    if  ( psTtfGetName( &(afi->afiFullName), ttnt, 4 ) )
	{ LDEB(4); return -1;	}

    if  ( psTtfGetName( &(afi->afiFontName), ttnt, 6 ) )
	{
	afi->afiFontName= strdup( afi->afiFullName );
	if  ( ! afi->afiFontName )
	    { XDEB(afi->afiFontName); return -1;	}
	}

    if  ( psTtfGetName( &(afi->afiNotice), ttnt, 0 ) )
	{
	afi->afiNotice= strdup( "No Copyright Notice" );
	if  ( ! afi->afiNotice )
	    { XDEB(afi->afiNotice); return -1;	}
	}

    if  ( psTtfGetName( &(afi->afiFamilyName), ttnt, 1 ) )
	{ LDEB(1); return -1;	}

    /* Silly Mukti: */
    if  ( strlen( afi->afiFullName ) < strlen( afi->afiFamilyName ) )
	{
	char *	swap= afi->afiFamilyName;
		      afi->afiFamilyName= afi->afiFullName;
					  afi->afiFullName= swap;
	}

    if  ( psTtfGetName( &(afi->afiVersion), ttnt, 5 ) )
	{
	afi->afiVersion= strdup( "1.0 Guessed" );
	if  ( ! afi->afiVersion )
	    { XDEB(afi->afiVersion); return -1;	}
	}

    if  ( psTtfGetName( pSavedVariant, ttnt, 2 ) )
	{ LDEB(2); return -1; }

    return 0;
    }

/************************************************************************/

static int psTtfGetWeight(	AfmFontInfo *		afi,
				const TrueTypeFont *	ttf,
				const char *		savedVariant )
    {
    const TrueTypeOS_2Table *	ttot= &(ttf->ttfOS_2Table);

    int				start;
    int				length;

    int				weight;

    if  ( ttot->ttotWeightClass > 0 )
	{
	afi->afiWeightInt= FONTweightTHIN;

	if  ( ttot->ttotWeightClass >= 200 )
	    { afi->afiWeightInt= FONTweightEXTRALIGHT;	}
	if  ( ttot->ttotWeightClass >= 300 )
	    { afi->afiWeightInt= FONTweightLIGHT;	}
	if  ( ttot->ttotWeightClass >= 400 )
	    { afi->afiWeightInt= FONTweightBOOK;	}
	/* no FONTweightREGULAR */
	if  ( ttot->ttotWeightClass >= 500 )
	    { afi->afiWeightInt= FONTweightMEDIUM;	}
	if  ( ttot->ttotWeightClass >= 600 )
	    { afi->afiWeightInt= FONTweightDEMIBOLD;	}
	if  ( ttot->ttotWeightClass >= 700 )
	    { afi->afiWeightInt= FONTweightBOLD;	}
	if  ( ttot->ttotWeightClass >= 800 )
	    { afi->afiWeightInt= FONTweightEXTRABOLD;	}
	if  ( ttot->ttotWeightClass >= 900 )
	    { afi->afiWeightInt= FONTweightBLACK;	}

	afi->afiWeightStr= strdup( psWeightStr( afi->afiWeightInt ) );
	}
    else{
	if  ( utilFontWeightFromString( &weight, &start, &length, savedVariant ) )
	    {
	    afi->afiWeightStr= (char *)malloc( length+ 1 );
	    if  ( ! afi->afiWeightStr )
		{ XDEB(afi->afiWeightStr); return -1;	}
	    strncpy( afi->afiWeightStr, savedVariant+ start, length )[length]= '\0';
	    afi->afiWeightInt= weight;
	    }
	}

    if  ( ! afi->afiWeightStr )
	{
	char *	str= afi->afiFullName;
	if  ( utilFontWeightFromString( &weight, &start, &length, str ) )
	    {
	    afi->afiWeightStr= (char *)malloc( length+ 1 );
	    if  ( ! afi->afiWeightStr )
		{ XDEB(afi->afiWeightStr); return -1;	}
	    strncpy( afi->afiWeightStr, str+ start, length )[length]= '\0';
	    afi->afiWeightInt= weight;
	    }
	}

    return 0;
    }

/************************************************************************/

static int psTtfGetWidth(	AfmFontInfo *		afi,
				const TrueTypeFont *	ttf,
				const char *		savedVariant )
    {
    const TrueTypeOS_2Table *	ttot= &(ttf->ttfOS_2Table);

    int				start;
    int				length;

    int				width;

    if  ( ttot->ttotWidthClass > 0 )
	{
	afi->afiWidthInt= FONTwidthULTRACONDENSED;

	if  ( ttot->ttotWidthClass >= 1 )
	    { afi->afiWidthInt= FONTwidthEXTRACONDENSED;	}
	if  ( ttot->ttotWidthClass >= 2 )
	    { afi->afiWidthInt= FONTwidthCONDENSED;		}
	if  ( ttot->ttotWidthClass >= 4 )
	    { afi->afiWidthInt= FONTwidthSEMICONDENSED;		}
	if  ( ttot->ttotWidthClass >= 5 )
	    { afi->afiWidthInt= FONTwidthNORMAL;		}
	if  ( ttot->ttotWidthClass >= 6 )
	    { afi->afiWidthInt= FONTwidthSEMIEXPANDED;		}
	if  ( ttot->ttotWidthClass >= 7 )
	    { afi->afiWidthInt= FONTwidthEXPANDED;		}
	if  ( ttot->ttotWidthClass >= 8 )
	    { afi->afiWidthInt= FONTwidthEXTRAEXPANDED;		}
	if  ( ttot->ttotWidthClass >= 9 )
	    { afi->afiWidthInt= FONTwidthULTRAEXPANDED;		}
	afi->afiWidthStr= strdup( psWidthStr( afi->afiWidthInt ) );
	}
    else{
	if  ( utilFontWidthFromString( &width, &start, &length, savedVariant ) )
	    {
	    afi->afiWidthStr= (char *)malloc( length+ 1 );
	    if  ( ! afi->afiWidthStr )
		{ XDEB(afi->afiWidthStr); return -1;	}
	    strncpy( afi->afiWidthStr, savedVariant+ start, length )[length]= '\0';
	    afi->afiWidthInt= width;
	    }
	}

    if  ( ! afi->afiWidthStr )
	{
	char *	str= afi->afiFullName;
	if  ( utilFontWidthFromString( &width, &start, &length, str ) )
	    {
	    afi->afiWidthStr= (char *)malloc( length+ 1 );
	    if  ( ! afi->afiWidthStr )
		{ XDEB(afi->afiWidthStr); return -1;	}
	    strncpy( afi->afiWidthStr, str+ start, length )[length]= '\0';
	    afi->afiWidthInt= width;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Try to derive complete font information from a true type font.	*/
/*									*/
/************************************************************************/

int psTtfFontInfo(	AfmFontInfo *		afi,
			const TrueTypeFont *	ttf )
    {
    const TrueTypePostTable *	ttpt= &(ttf->ttfPostTable);
    const TrueTypeHheaTable *	hhea= &(ttf->ttfHheaTable);
    const TrueTypeHeadTable *	ttht= &(ttf->ttfHeadTable);

    char *			savedVariant= (char *)0;

    if  ( psTtfGetNames( afi, &savedVariant, ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfGetCharMetricsAndEncoding( afi, ttf ) )
	{ LDEB(1); return -1;	}

    if  ( utilTtfGetKernPairs( afi, ttf ) )
	{ LDEB(1); return -1;	}

    if  ( psTtfGetWeight( afi, ttf, savedVariant ) )
	{ LDEB(1); return -1;	}

    if  ( psTtfGetWidth( afi, ttf, savedVariant ) )
	{ LDEB(1); return -1;	}

    if  ( savedVariant )
	{ free( savedVariant );	}

    if  ( ttpt->ttptItalicAngleUpper >= 0 )
	{
	afi->afiItalicAngle= 
		ttpt->ttptItalicAngleUpper+
		ttpt->ttptItalicAngleLower/ 65536.0;
	}
    else{
	afi->afiItalicAngle= 
		ttpt->ttptItalicAngleUpper-
		ttpt->ttptItalicAngleLower/ 65536.0;
	}

    if  ( afi->afiItalicAngle > -0.05 && afi->afiItalicAngle < 0.05 )
	{ afi->afiItalicAngle= 0.0;	}
    afi->afiTanItalicAngle= tan( afi->afiItalicAngle );

    afi->afiIsFixedPitch= ttpt->ttptIsFixedPitch != 0;

    afi->afiFontBBox.drX0= utilTtfScaledInteger( ttf, ttht->tthtXMin );
    afi->afiFontBBox.drY0= utilTtfScaledInteger( ttf, ttht->tthtYMin );
    afi->afiFontBBox.drX1= utilTtfScaledInteger( ttf, ttht->tthtXMax );
    afi->afiFontBBox.drY1= utilTtfScaledInteger( ttf, ttht->tthtYMax );

    afi->afiUnderlinePosition=
		    utilTtfScaledInteger( ttf, ttpt->ttptUnderlinePosition );
    afi->afiUnderlineThickness=
		    utilTtfScaledInteger( ttf, ttpt->ttptUnderlineThickness );
    afi->afiAscender=
		    utilTtfScaledInteger( ttf, hhea->hheaAscender );
    afi->afiDescender=
		    utilTtfScaledInteger( ttf, hhea->hheaDescender );

    if  ( ttf->ttfOS_2Table.ttotVendID[0] )
	{
	afi->afiVendor= strdup( (char *)ttf->ttfOS_2Table.ttotVendID );
	if  ( ! afi->afiVendor )
	    { XDEB(afi->afiVendor); return -1;	}
	}

    {
    char *	str= afi->afiFontName;

    while( *str )
	 {
	 if  ( *str == ' ' )
	     { *str=   '-';    }
	 str++;
	 }
    }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract afm from TrueType font.					*/
/*									*/
/************************************************************************/

int psTtfToAfi(		AfmFontInfo *		afi,
			const MemoryBuffer *	fontFileName,
			SimpleInputStream *	sisTtf )
    {
    int				rval= 0;

    TrueTypeFont		ttf;

    psTtfInitTrueTypeFont( &ttf );

    if  ( fontFileName )
	{
	if  ( utilCopyMemoryBuffer( &(afi->afiFontFileName), fontFileName ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( psTtfLoadFont( &ttf, sisTtf, 0L ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( psTtfFontInfo( afi, &ttf ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    psTtfCleanTrueTypeFont( &ttf );

    return rval;
    }

