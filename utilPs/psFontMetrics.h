/************************************************************************/
/*									*/
/*  Derive text metrics from font metrics.				*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_METRICS_H
#   define	PS_FONT_METRICS_H

#   include	"psFontInfo.h"

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psCalculateStringExtents(	DocumentRectangle *	dr,
					const char *		ss,
					int			len,
					int			twipsSize,
					int			withKerning,
					int			vswap,
					const AfmFontInfo *	afi );

extern void psUnderlineGeometry(	int *			pPos,
					int *			pThick,
					int			baseline,
					int			twipsSize,
					const AfmFontInfo *	afi );

extern void psStrikethroughGeometry(	int *			pPos,
					int *			pThick,
					int			baseline,
					int			twipsSize,
					const AfmFontInfo *	afi );

extern int psFontBBox(			DocumentRectangle *	drBBox,
					DocumentRectangle *	drAscDesc,
					int			twipsSize,
					int			vswap,
					const AfmFontInfo *	afi );

extern int psGetSuperBaseline(		int *			pSuperBaseline,
					int			baseline,
					int			fontSizeTwips,
					const AfmFontInfo *	afi );

extern int psGetSubBaseline(		int *			pSubBaseline,
					int			baseline,
					int			fontSizeTwips,
					const AfmFontInfo *	afi );

extern int psGetSmallcapsSize(		int *			pSmallcapsSize,
					int			fontSizeTwips,
					const AfmFontInfo *	afi );

#   endif
