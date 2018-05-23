/************************************************************************/
/*									*/
/*  Convert True Type Fonts.						*/
/*									*/
/************************************************************************/

#   ifndef	PS_TTF_H
#   define	PS_TTF_H

#   include	<sioGeneral.h>
#   include	<utilMemoryBuffer.h>
#   include	"psFontInfo.h"

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psTtfToPt42(		SimpleOutputStream *	sosPf42,
				const MemoryBuffer *	fontFileName,
				SimpleInputStream *	sisTtf );

extern int psTtcToPt42(		SimpleOutputStream *	sosPf42,
				const MemoryBuffer *	fontFileName,
				int			fontFileIndex,
				SimpleInputStream *	sisTtf );

extern int psTtfToAfi(		AfmFontInfo *		afi,
				const MemoryBuffer *	fontFileName,
				SimpleInputStream *	sisTtf );

#   endif
