/************************************************************************/
/*									*/
/*  Information on sets of characters or glyphs in fonts.		*/
/*									*/
/************************************************************************/

#   ifndef	PS_GLYPHS_H
#   define	PS_GLYPHS_H

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern const char * const	psAppleTtfGlyphNames[258];	/*  258 !  */

extern const char * psUnicodeToGlyphName(	int	unicode );
extern int psGlyphNameToUnicode(	const char * glyphname );

#   endif
