/************************************************************************/
/*									*/
/*  Map between PostScript type and Document type font administrations.	*/
/*									*/
/************************************************************************/

#   ifndef	PS_MATCH_FONT_H
#   define	PS_MATCH_FONT_H

#   include	"psPostScriptFontList.h"
#   include	"textAttribute.h"

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern AfmFontInfo * psGetPsFontInfoForAttribute(
				const char *			familyName,
				int				styleInt,
				const IndexSet *		unicodesUsed,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl );

#   endif
