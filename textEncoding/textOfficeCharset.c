#   include	"textEncodingConfig.h"

#   include	<string.h>

#   include	"textOfficeCharset.h"
#   include	"uniLegacyEncoding.h"

#   include	<appDebugon.h>

static OfficeCharset PS_OfficeCharsets[]= 
{
    { FONTcharsetANSI,	/* 0 */ FONTcodepageANSI, /* MUST BE FIRST!! */
			0, " Western",
			"CP1252",
			uniWinCp1252GlyphUnicodes,
			},
    { FONTcharsetEE,	/* 238 */ FONTcodepageEE,
			1, " CE",
			"CP1250",
			uniWinCp1250GlyphUnicodes,
			},
    { FONTcharsetSYMBOL, /* 2 */ -1,
			0, (const char *)0,
			"SYMBOL", /* Not a name supported by iconv */
			uniSymbolGlyphUnicodes,
			},
    { FONTcharsetRUSSIAN, /* 204 */ FONTcodepageRUSSIAN,
			1, " Cyr",
			"CP1251",
			uniWinCp1251GlyphUnicodes,
			},
    { FONTcharsetGREEK, /* 161 */ FONTcodepageGREEK,
			1, " Greek",
			"CP1253",
			uniWinCp1253GlyphUnicodes,
			},
    { FONTcharsetTURKISH, /* 162 */ FONTcodepageTURKISH,
			1, " Tur",
			"CP1254",
			uniWinCp1254GlyphUnicodes,
			},
    { FONTcharsetBALTIC, /* 186 */ FONTcodepageBALTIC,
			1, " Baltic",
			"CP1257",
			uniWinCp1257GlyphUnicodes,
			},
    { FONTcharsetHEBREW, /* 177 */ FONTcodepageHEBREW,
			1, " (Hebrew)",
			"CP1255",
			uniWinCp1255GlyphUnicodes,
			},
    { FONTcharsetARABIC, /* 178 */ FONTcodepageARABIC,
			1, " (Arabic)",
			"CP1256",
			uniWinCp1256GlyphUnicodes,
			},
    { FONTcharsetVIETNAMESE, /* 163 */ FONTcodepageVIETNAMESE,
			1, " (Vietnamese)",
			"CP1258",
			uniWinCp1258GlyphUnicodes,
			},
    { FONTcharsetTHAI, /* 222 */ FONTcodepageTHAI,
			1, " (Thai)",
			"CP874",
			uniWinCp874GlyphUnicodes,
			},

    { FONTcharsetSHIFTJIS, /* 0 */ FONTcodepageSHIFTJIS,
			0, (const char *)0,
			"SHIFT_JIS",
			(const int *)0,
			},
    { FONTcharsetHANGEUL, /* 0 */ FONTcodepageHANGEUL,
			0, (const char *)0,
			"CP949",
			(const int *)0,
			},
    { FONTcharsetCHINESEBIG5, /* 0 */ FONTcodepageCHINESEBIG5,
			0, (const char *)0,
			"BIG-5",
			(const int *)0,
			},
    /* default */
    { FONTcharsetDEFAULT, -1,
			0, (const char *)0,
			(const char *)0,
			(const int *)0,
			},

    /* conversion */
    { FONTcharsetMAC_ROMAN, FONTcodepageMAC_ROMAN,
			0, (const char *)0,
			"MACINTOSH"
			},

    { FONTcharsetMAC_256, FONTcodepageMAC_ROMAN,
			0, (const char *)0,
			"MACINTOSH"
			},

    { FONTcharsetGB2312, /* 0 */ FONTcodepageGB2312,
			0, (const char *)0,
			"GB2312",
			(const int *)0,
			},

    /*  FONTcharsetJOHAB	*/
};

const OfficeCharset * utilGetOfficeCharsetByCharset(
					int *		pIdx,
					int		charset )
    {
    int				i;
    const OfficeCharset *	oc;
    int				n;

    n= sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset);

    if  ( n > CHARSETidx_COUNT )
	{
	LLDEB(n,CHARSETidx_COUNT);
	*pIdx= -1; return (const OfficeCharset *)0;
	}

    oc= PS_OfficeCharsets;
    for ( i= 0; i < n; oc++, i++ )
	{
	if  ( oc->ocOfficeCharset == charset )
	    { *pIdx= i; return oc;	}
	}

    *pIdx= -1;
    return (const OfficeCharset *)0;
    }

const OfficeCharset * utilGetOfficeCharsetByIndex(
					int		charsetIdx )
    {
    int				n;

    n= sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset);

    if  ( n > CHARSETidx_COUNT )
	{ LLDEB(n,CHARSETidx_COUNT); return (const OfficeCharset *)0; }

    if  ( charsetIdx < 0 || charsetIdx >= n )
	{ LLDEB(charsetIdx,n); return (const OfficeCharset *)0; }

    return PS_OfficeCharsets+ charsetIdx;
    }

const OfficeCharset * utilGetOfficeCharsetBySuffix(
					int *		pIdx,
					const char *	fontname )
    {
    int				i;
    const OfficeCharset *	oc;
    int				n;

    int				lenN= strlen( fontname );

    n= sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset);

    if  ( n > CHARSETidx_COUNT )
	{
	LLDEB(n,CHARSETidx_COUNT);
	*pIdx= -1; return (const OfficeCharset *)0;
	}

    oc= PS_OfficeCharsets;
    for ( i= 0; i < sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset); oc++, i++ )
	{
	int		lenS;

	if  ( ! oc->ocOfficeFontnameSuffix )
	    { continue;	}

	lenS= strlen( oc->ocOfficeFontnameSuffix );

	if  ( lenN > lenS						&&
	      ! strcmp( fontname+ lenN- lenS,
					oc->ocOfficeFontnameSuffix )	)
	    { *pIdx= i; return oc;	}
	}

    *pIdx= -1;
    return (const OfficeCharset *)0;
    }

/************************************************************************/
/*									*/
/*  Translate a unicode value to this charset.				*/
/*									*/
/************************************************************************/

int utilOfficeCharsetCodeForUnicode(	int		charsetIdx,
					int		unicode )
    {
    OfficeCharset * oc= PS_OfficeCharsets+ charsetIdx;

    if  ( oc->ocUnicodes						&&
	  utilIndexMappingIsEmpty( &(oc->ocUniToCodeMapping) )		&&
	  utilIndexMappingBuildBackward( &(oc->ocUniToCodeMapping),
						oc->ocUnicodes, 256 )	)
	{ XDEB(oc->ocUnicodes);	}

    return utilIndexMappingGet( &(oc->ocUniToCodeMapping), unicode );
    }

/************************************************************************/
/*									*/
/*  Return the set of unicodes for a certain character set.		*/
/*									*/
/************************************************************************/

static IndexSet DF_LegacyCoverage[CHARSETidx_COUNT];
static int DF_LegacyCoverageCalculated= 0;

static int docFontCalculateLegacyCoverage( void )
    {
    int		charsetIdx;

    for ( charsetIdx= 0; charsetIdx < CHARSETidx_COUNT; charsetIdx++ )
	{
	const OfficeCharset *	oc;

	utilInitIndexSet( &(DF_LegacyCoverage[charsetIdx]) );

	oc= utilGetOfficeCharsetByIndex( charsetIdx );
	if  ( ! oc || ! oc->ocUnicodes )
	    { continue;	}

	if  ( utilIndexSetAddArray( &(DF_LegacyCoverage[charsetIdx]),
						    oc->ocUnicodes, 256 ) )
	    { LDEB(charsetIdx); return -1;	}
	}

    return 0;
    }

const IndexSet * utilOfficeCharsetGetCodeSet(	int	charsetIdx )
    {
    const OfficeCharset *	oc;

    if  ( ! DF_LegacyCoverageCalculated )
	{
	if  ( docFontCalculateLegacyCoverage() )
	    { return (const IndexSet *)0;	}

	DF_LegacyCoverageCalculated= 1;
	}

    oc= utilGetOfficeCharsetByIndex( charsetIdx );
    if  ( ! oc || ! oc->ocUnicodes )
	{ return (const IndexSet *)0;	}

    return &(DF_LegacyCoverage[charsetIdx]);
    }

