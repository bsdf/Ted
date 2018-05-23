/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_READ_WRITE_FONT_INFO_H
#   define	PS_READ_WRITE_FONT_INFO_H

#   include	<sioGeneral.h>
#   include	"psFontInfo.h"

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psWriteAfmFile(		SimpleOutputStream *	sos,
					int			omitKernPairs,
					const AfmFontInfo *	afi );

extern void psWriteFontInfoDict(	SimpleOutputStream *	sos,
					const AfmFontInfo *	afi );

extern int psAfmReadAfm(		SimpleInputStream *	sisAfm,
					AfmFontInfo *		afi,
					int			deferMetrics );

#   endif
