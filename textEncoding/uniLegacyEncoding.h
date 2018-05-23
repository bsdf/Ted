/************************************************************************/
/*									*/
/*  Translate the mess of text encodings and character sets that we	*/
/*  to cope with to unicode.						*/
/*									*/
/************************************************************************/

#   ifndef	UNI_LEGACY_ENCODING_H
#   define	UNI_LEGACY_ENCODING_H

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern const int	uniIso8859_1GlyphUnicodes[256];
extern const int	uniIso8859_2GlyphUnicodes[256];
extern const int	uniIso8859_5GlyphUnicodes[256];
extern const int	uniIso8859_7GlyphUnicodes[256];
extern const int	uniIso8859_9GlyphUnicodes[256];
extern const int	uniIso8859_13GlyphUnicodes[256];
extern const int	uniIso8859_15GlyphUnicodes[256];
extern const int	uniKoi8RGlyphUnicodes[256];
extern const int	uniDingbatsGlyphUnicodes[256];
extern const int	uniSymbolGlyphUnicodes[256];
extern const int	uniWinCp874GlyphUnicodes[256];
extern const int	uniWinCp1250GlyphUnicodes[256];
extern const int	uniWinCp1251GlyphUnicodes[256];
extern const int	uniWinCp1252GlyphUnicodes[256];
extern const int	uniWinCp1253GlyphUnicodes[256];
extern const int	uniWinCp1254GlyphUnicodes[256];
extern const int	uniWinCp1255GlyphUnicodes[256];
extern const int	uniWinCp1256GlyphUnicodes[256];
extern const int	uniWinCp1257GlyphUnicodes[256];
extern const int	uniWinCp1258GlyphUnicodes[256];
extern const int	uniMacRomanGlyphUnicodes[256];
extern const int	uniPDFDocEncodingGlyphUnicodes[256];

extern const char *	const utilHtmlCharEntities[256];

#   endif
