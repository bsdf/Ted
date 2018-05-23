/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_CONFIG_H
#   define	PS_CONFIG_H

#   include	"psPostScriptFontList.h"
#   include	<sioGeneral.h>

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psFontmapForFiles(	SimpleOutputStream *	sosOut,
				int			fileCount,
				const char * const *	fileNames );

extern int psAfmToGSFontmap(	SimpleOutputStream *	sosFontDir,
				const char *		afmFileName );

extern int psGSLibAfmDirectory(	PostScriptFontList *	psfl,
				int			ignoreKerning,
				const MemoryBuffer *	afmDirectory,
				const MemoryBuffer *	psDirectory );

extern int psAfmForFontFiles(	PostScriptFontList *	psfl,
				int			ignoreKerning,
				int			fileCount,
				char **			fileNames,
				const MemoryBuffer *	afmDirectory,
				const MemoryBuffer *	psDirectory );

extern int psFontFileToAfm(	SimpleOutputStream *	sosAfm,
				int			omitKernPairs,
				const char *		fontFileName,
				const MemoryBuffer *	psDirectory );

#   endif
