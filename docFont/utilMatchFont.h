/************************************************************************/
/*									*/
/*  Map between PostScript type and Document type font administrations.	*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_MATCH_FONT_H
#   define	UTIL_MATCH_FONT_H

#   include	"utilDocFontList.h"
#   include	<psPostScriptFontList.h>
#   include	<textAttribute.h>

/************************************************************************/

typedef const AfmFontInfo * (*GetFontForAttribute)(
			const IndexSet **		pUnicodesWanted,
			const TextAttribute *		ta,
			const DocumentFontList *	dfl,
			const PostScriptFontList *	psfl );


/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilAddPsFontsToDocList( DocumentFontList *		dfl,
				const PostScriptFontList *	psfl );

extern int utilAddBase35FontsToDocList(	DocumentFontList *	dfl );

extern const char * utilGetEncodingName(	const char *	fontName,
						int		charset );

#   endif
