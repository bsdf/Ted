/************************************************************************/
/*									*/
/*  Analysis and manipulation of font names.				*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_NAME_H
#   define	PS_FONT_NAME_H

#   include	"psFontInfo.h"

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilFontWeightFromString(	int *			pWeight,
					int *			pStart,
					int *			pLength,
					const char *		name );

extern int utilFontWidthFromString(	int *			pWidth,
					int *			pStart,
					int *			pLength,
					const char *		name );

extern int psRemoveWidthFromName(	char *			target,
					int			maxlen,
					int *			pWidth,
					const char *		name );

extern int psFontInfoMoveWidthToFamilyName(	AfmFontInfo *	afi );

extern const char * psWidthStr(		int			width );
extern const char * psWeightStr(	int			weight );

extern int psFontGetWeight(		unsigned char *		pWeight,
					const char *		weight );
extern int psFontGetWidth(		int *			pWidth,
					const char *		swdth );

#   endif
