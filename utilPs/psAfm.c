#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	"psFontInfo.h"
#   include	"psReadWriteFontInfo.h"

#   include	<appDebugon.h>

static void psAfmEmitStringProperty(	SimpleOutputStream *	sos,
					const char *		keyword,
					const char *		val )
    {
    if  ( ! val )
	{ return;	}

    while( isspace( *val ) )
	{ val++;	}
    if  ( ! *val )
	{ return;	}
    
    /*  lines */
    while( *val )
	{
	int		line= 254- strlen( keyword );
	int		left= line;

	sioOutPrintf( sos, "%s ", keyword );

	/*  words */
	while( *val )
	    {
	    const char *	word= val;
	    const char *	past;
	    int			step;

	    while( *word && ! isspace( *word ) )
		{ word++;		}
	    past= word;
	    while( *past &&   isspace( *past ) )
		{ past++;		}

	    step= past- val;
	    /* fits with spaces */
	    if  ( step < left )
		{
		sioOutPrintf( sos, "%.*s", step, val ); left -= step;
		val= past;
		continue;
		}

	    step= word- val;
	    /* fits without spaces */
	    if  ( step < left )
		{
		sioOutPrintf( sos, "%.*s", step, val );
		val= past;
		}
	    else{
		/* does not fit. Can it fit? If not, emit what fits now */
		if  ( step > line )
		    {
		    sioOutPrintf( sos, "%.*s", left, val );
		    val += left;
		    }
		}

	    sioOutPrintf( sos, "\n" );
	    keyword= "Comment";
	    break;
	    }
	}

    sioOutPrintf( sos, "\n" );
    return;
    }

static void psWriteCharMetric(	SimpleOutputStream *	sos,
				int			c,
				const AfmCharMetric *	acm )
    {
    sioOutPrintf( sos, "C %d ; WX %d ; N %s ; B %d %d %d %d ;\n",
		    c,
		    acm->acmWX,
		    acm->acmN,
		    acm->acmBBox.drX0,
		    acm->acmBBox.drY0,
		    acm->acmBBox.drX1,
		    acm->acmBBox.drY1 );

#   if 0
    {
    int			i;
    const AfmCodeList *	acl= &(acm->acmUnicodeCodeList);

    for ( i= 0; i < acl->aclCodeCount; i++ )
	{ sioOutPrintf( sos, "Comment Unicode 0x%04X\n", acl->aclCodes[i] ); }
    }
#   endif

    return;
    }

static int psCountAfmMetrics(	const AfmFontInfo *	afi )
    {
    int		count= 0;
    int		glyph;

    for ( glyph= 0;glyph < afi->afiMetricCount; glyph++ )
	{
	const AfmCharMetric *	acm= afi->afiMetrics[glyph];
	const AfmCodeList *	acl;

	if  ( ! acm )
	    { continue;	}

	acl= &(acm->acmDefaultCodeList);

	if  ( acl->aclCodeCount == 0 )
	    { count++;				}	/*  -1		*/
	else{ count += acl->aclCodeCount;	}	/*  Codes	*/
	}

    return count;
    }

int psWriteAfmFile(	SimpleOutputStream *	sos,
			int			omitKernPairs,
			const AfmFontInfo *	afi )
    {
    int				i;
    int				nchars;
    int				kernPairCount;
    int				map[256];

    sioOutPrintf( sos, "StartFontMetrics 4.1\n" );

    if  ( afi->afiFontName )
	{ sioOutPrintf( sos, "FontName %s\n", afi->afiFontName );	}

    if  ( afi->afiFullName )
	{ sioOutPrintf( sos, "FullName %s\n", afi->afiFullName );	}

    if  ( afi->afiFamilyName )
	{ sioOutPrintf( sos, "FamilyName %s\n", afi->afiFamilyName );	}

    if  ( afi->afiWeightStr )
	{ sioOutPrintf( sos, "Weight %s\n", afi->afiWeightStr );	}

    if  ( afi->afiWidthStr )
	{ sioOutPrintf( sos, "Comment Width %s\n", afi->afiWidthStr );	}

    if  ( afi->afiItalicAngle )
	{ sioOutPrintf( sos, "ItalicAngle %g\n", afi->afiItalicAngle );	}

    sioOutPrintf( sos, "IsFixedPitch %s\n",
				    afi->afiIsFixedPitch?"true":"false" );

    sioOutPrintf( sos, "FontBBox %d %d %d %d\n",
			    afi->afiFontBBox.drX0,
			    afi->afiFontBBox.drY0,
			    afi->afiFontBBox.drX1,
			    afi->afiFontBBox.drY1 );

    sioOutPrintf( sos, "UnderlinePosition %d\n", afi->afiUnderlinePosition );
    sioOutPrintf( sos, "UnderlineThickness %d\n", afi->afiUnderlineThickness );

    if  ( afi->afiVersion )
	{ sioOutPrintf( sos, "Version %s\n", afi->afiVersion );	}

    psAfmEmitStringProperty( sos, "Notice", afi->afiNotice );

    if  ( afi->afiEncodingScheme )
	{
	sioOutPrintf( sos, "EncodingScheme %s\n", afi->afiEncodingScheme );
	}

    if  ( afi->afiCapHeight != 0 )
	{ sioOutPrintf( sos, "CapHeight %d\n", afi->afiCapHeight );	}

    if  ( afi->afiXHeight != 0 )
	{ sioOutPrintf( sos, "XHeight %d\n", afi->afiXHeight );	}

    if  ( afi->afiAscender != 0 )
	{ sioOutPrintf( sos, "Ascender %d\n", afi->afiAscender );	}

    if  ( afi->afiDescender != 0 )
	{ sioOutPrintf( sos, "Descender %d\n", afi->afiDescender );	}

    nchars= psCountAfmMetrics( afi );

    sioOutPrintf( sos, "StartCharMetrics %d\n", nchars );

    for ( i= 0; i < 256; i++ )
	{ map[i]= -1;	}

    for ( i= 0; i < afi->afiMetricCount; i++ )
	{
	AfmCharMetric *		acm=  afi->afiMetrics[i];
	const AfmCodeList *	acl;

	if  ( ! acm )
	    { continue;	}

	acl= &(acm->acmDefaultCodeList);
	if  ( acl->aclCodeCount > 0		&&
	      acl->aclCodes[0] < 256		)
	    { map[acl->aclCodes[0]]= i;	}
	}

    for ( i= 0; i < 256; i++ )
	{
	if  ( map[i] >= 0 )
	    {
	    const AfmCharMetric *	acm= afi->afiMetrics[map[i]];
	    const AfmCodeList *		acl;
	    int				code;

	    if  ( ! acm || ! acm->acmN || ! acm->acmN[0] )
		{ /*LXDEB(i,acm->acmN);*/ continue;	}

	    acl= &(acm->acmDefaultCodeList);
	    if  ( acl->aclCodeCount == 0 )
		{ continue;	}

	    for ( code= 0; code < acl->aclCodeCount; code++ )
		{ psWriteCharMetric( sos, acl->aclCodes[code], acm );	}
	    }
	}

    kernPairCount= 0;
    for ( i= 0; i < afi->afiMetricCount; i++ )
	{
	const AfmCharMetric *	acm=  afi->afiMetrics[i];
	const AfmCodeList *	acl;

	if  ( ! acm || ! acm->acmN || ! acm->acmN[0] )
	    { /*LXDEB(i,acm->acmN);*/ continue;	}

	kernPairCount += acm->acmKernPairCount;

	acl= &(acm->acmDefaultCodeList);
	/* Already emitted as part of the default encoding? */
	if  ( acl->aclCodeCount > 0 )
	    { continue; }

	psWriteCharMetric( sos, -1, acm );
	}

    sioOutPrintf( sos, "EndCharMetrics\n" );

    if  ( ! omitKernPairs && kernPairCount > 0 )
	{
	sioOutPrintf( sos, "StartKernData\n" );
	sioOutPrintf( sos, "StartKernPairs %d\n", kernPairCount );

	for ( i= 0; i < afi->afiMetricCount; i++ )
	    {
	    AfmCharMetric *	acm=  afi->afiMetrics[i];
	    int			k;

	    if  ( ! acm || ! acm->acmN || ! acm->acmN[0] )
		{ /*LXDEB(i,acm->acmN);*/ continue;	}

	    for ( k= 0; k < acm->acmKernPairCount; k++ )
		{
		const AfmCharMetric *	acmK;

		if  ( acm->acmKernPairs[k].akpPosition < 0	||
		      acm->acmKernPairs[k].akpPosition >=
					    afi->afiMetricCount	)
		    {
		    LLDEB(acm->acmKernPairs[k].akpPosition,afi->afiMetricCount);
		    continue;
		    }

		acmK= afi->afiMetrics[acm->acmKernPairs[k].akpPosition];
		if  ( ! acmK || ! acmK->acmN || ! acmK->acmN[0] )
		    { /*LXDEB(i,acmK->acmN);*/ continue;	}

		sioOutPrintf( sos, "KPX %s %s %d\n",
					    acm->acmN, acmK->acmN,
					    acm->acmKernPairs[k].akpXVec );
		}
	    }

	sioOutPrintf( sos, "EndKernPairs\n" );
	sioOutPrintf( sos, "EndKernData\n" );
	}

    if  ( afi->afiVendor )
	{ sioOutPrintf( sos, "Comment Vendor %s\n", afi->afiVendor );	}

    if  ( ! utilMemoryBufferIsEmpty( &(afi->afiFontFileName) ) )
	{
	const char *	f= utilMemoryBufferGetString( &(afi->afiFontFileName) );

	sioOutPrintf( sos, "%s/%s (%s) ;\n",
		"Comment Fontmap ", afi->afiFontName, f );

	sioOutPrintf( sos, "Comment FontFileName %s\n", f );
	if  ( afi->afiFontFileIndex >= 0 )
	    {
	    sioOutPrintf( sos, "Comment FontFileIndex %d\n",
						    afi->afiFontFileIndex );
	    }
	}

    for ( i= 0; i < afi->afiX11FontCount; i++ )
	{ sioOutPrintf( sos, "Comment X11Font %s\n", afi->afiX11Fonts[i] ); }

    sioOutPrintf( sos, "EndFontMetrics\n" );

    return 0;
    }

void psWriteFontInfoDict(	SimpleOutputStream *	sos,
				const AfmFontInfo *	afi )
    {
    sioOutPrintf( sos, "/FontInfo 10 dict dup begin\n" );

    if  ( afi->afiFamilyName )
	{
	sioOutPrintf( sos, "/FamilyName (%s) readonly def\n",
						    afi->afiFamilyName );
	}

    if  ( afi->afiFullName )
	{
	sioOutPrintf( sos, "/FullName (%s) readonly def\n",
						    afi->afiFullName );
	}

    if  ( afi->afiNotice )
	{
	sioOutPrintf( sos, "/Notice (%s) readonly def\n",
						    afi->afiNotice );
	}

    if  ( afi->afiWeightStr )
	{
	sioOutPrintf( sos, "/Weight (%s) readonly def\n",
						    afi->afiWeightStr );
	}

    if  ( afi->afiVersion )
	{
	sioOutPrintf( sos, "/Version (%s) readonly def\n",
						    afi->afiVersion );
	}

    sioOutPrintf( sos, "/ItalicAngle %g def\n", afi->afiItalicAngle );

    sioOutPrintf( sos, "/isFixedPitch %s def\n",
				    afi->afiIsFixedPitch?"true":"false" );

    sioOutPrintf( sos, "/UnderlinePosition %d def\n",
					afi->afiUnderlinePosition );
    sioOutPrintf( sos, "/UnderlineThickness %d def\n",
					afi->afiUnderlineThickness );

    sioOutPrintf( sos, "end readonly def\n" );

    return;
    }

