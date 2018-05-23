/************************************************************************/
/*									*/
/*  Data that describes where a font belongs in the mess of font	*/
/*  encodings and character sets that we have to cope with.		*/
/*									*/
/************************************************************************/

#   ifndef	TEXT_OFFICE_CHARSET_H
#   define	TEXT_OFFICE_CHARSET_H

#  include	<utilIndexMapping.h>
#  include	<utilIndexSet.h>

/************************************************************************/
/*									*/
/*  Document character sets that are more or less actively supported	*/
/*									*/
/************************************************************************/

#   define	DOCcharsetANSI			0
#   define	DOCcharsetMAC			1
#   define	DOCcharsetPC			2
#   define	DOCcharsetPCA			3

/************************************************************************/
/*									*/
/*  Document font encodings that we more or less actively support.	*/
/*									*/
/************************************************************************/

typedef struct OfficeCharset
    {
    short int				ocOfficeCharset;
    short int				ocWindowsCodepage;
    short int				ocEmitSuffix;
    const char *			ocOfficeFontnameSuffix;

    const char *			ocEncodingName;
    const int *				ocUnicodes;

    IndexMapping			ocUniToCodeMapping;
    } OfficeCharset;

/************************************************************************/
/*									*/
/*  Font character sets as used by windows programs.			*/
/*									*/
/************************************************************************/

#   define CHARSETidx_COUNT		18

#   define FONTcharsetANSI		0	/*  cpg 1252		*/
#   define FONTcharsetDEFAULT		1	/*  i.e. font specific	*/
#   define FONTcharsetSYMBOL		2
#   define FONTcharsetINVALID		3

#   define FONTcharsetMAC_ROMAN		77
#   define FONTcharsetMAC_256		256

#   define FONTcharsetSHIFTJIS		128	/*  cpg 932		*/
#   define FONTcharsetHANGEUL		129	/*  cpg 949		*/
#   define FONTcharsetJOHAB		130	/*  cpg 1361		*/
#   define FONTcharsetGB2312		134	/*  cpg 936		*/
#   define FONTcharsetCHINESEBIG5	136	/*  cpg 950		*/
#   define FONTcharsetGREEK		161	/*  cpg 1253		*/
#   define FONTcharsetTURKISH		162	/*  cpg 1254		*/
#   define FONTcharsetVIETNAMESE	163	/*  cpg	1258		*/
#   define FONTcharsetHEBREW		177	/*  cpg 1255		*/
#   define FONTcharsetARABIC		178	/*  cpg 1256		*/
# if 0
#   define FONTcharsetARABIC_TRADITION	179
#   define FONTcharsetARABIC_USER	180
#   define FONTcharsetHEBREW_USER	181
# endif
#   define FONTcharsetBALTIC		186	/*  cpg 1257		*/
#   define FONTcharsetRUSSIAN		204	/*  cpg 1251		*/
#   define FONTcharsetTHAI		222	/*  cpg 874		*/
#   define FONTcharsetEE		238	/*  cpg 1250		*/

# if 0
#   define FONTcharsetPC_437		254	/*  cpg 437		*/
#   define FONTcharsetOEM		255
# endif

/************************************************************************/
/*									*/
/*  Font codepages as used by windows programs.				*/
/*									*/
/************************************************************************/

#   define FONTcodepagePC_437		437
#   define FONTcodepageTHAI		874
#   define FONTcodepageSHIFTJIS		932
#   define FONTcodepageGB2312		936
#   define FONTcodepageHANGEUL		949
#   define FONTcodepageCHINESEBIG5	950
#   define FONTcodepageEE		1250
#   define FONTcodepageRUSSIAN		1251
#   define FONTcodepageANSI		1252
#   define FONTcodepageGREEK		1253
#   define FONTcodepageTURKISH		1254
#   define FONTcodepageHEBREW		1255
#   define FONTcodepageARABIC		1256
#   define FONTcodepageBALTIC		1257
#   define FONTcodepageVIETNAMESE	1258
#   define FONTcodepageJOHAB		1361

#   define FONTcodepageMAC_ROMAN	10000

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern const OfficeCharset * utilGetOfficeCharsetByCharset(
						int *		pIdx,
						int		charset );

extern const OfficeCharset * utilGetOfficeCharsetByIndex(
						int		charsetIdx );

extern const OfficeCharset * utilGetOfficeCharsetBySuffix(
						int *		pIdx,
						const char *	fontname );

extern int utilOfficeCharsetCodeForUnicode(	int		charsetIdx,
						int		unicode );

extern const IndexSet * utilOfficeCharsetGetCodeSet(	int	charsetIdx );

#   endif
