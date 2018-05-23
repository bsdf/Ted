/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   include	"psFontFamily.h"

#   ifndef	PS_FONT_LIST_H
#   define	PS_FONT_LIST_H

typedef struct PostScriptFontList
    {
    PsFontFamily **	psflFamilies;
    int			psflFamilyCount;
    AfmFontInfo **	psflInfos;
    int			psflInfoCount;
    int			psflAvoidFontconfig;
				/*  Do not use fontconfig. This is a	*/
				/*  strange place to store this piece	*/
				/*  of information. However: everywhere	*/
				/*  where we need it, we have a font	*/
				/*  list available.			*/

    void *		psflFontNameToInfo;
    void *		psflFontFileToInfo;
    } PostScriptFontList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void psInitPostScriptFontList(	PostScriptFontList *	psfl );
extern void psCleanPostScriptFontList(	PostScriptFontList *	psfl );

extern int psPostScriptFontListAddInfo(	PostScriptFontList *	psfl,
					AfmFontInfo *		afi );

extern int psPostScriptFontListAddFamily( PostScriptFontList *	psfl,
					PsFontFamily *		aff );

extern int psPostScriptFontListInfosToFamilies(
					PostScriptFontList *	psfl );

extern int psPostScriptFontListGetFamilyByName(
					const PostScriptFontList *	psfl,
					const char *			name );

extern AfmFontInfo * psPostScriptFontListGetFontInfoByFaceFile(
					const PostScriptFontList *	psfl,
					const char *			file );

extern int psPostScriptFontListSetFontInfoForFaceFile(
					PostScriptFontList *		psfl,
					AfmFontInfo *			afi,
					const char *			file );

extern AfmFontInfo * psPostScriptFontListGetFontInfoByFontName(
					const PostScriptFontList *	psfl,
					const char *			name );

extern int psSaveAfms(		const PostScriptFontList *	psfl,
				int				omitKernPairs,
				const MemoryBuffer *		afmDirectory );

extern int psFontCatalog(		PostScriptFontList *	psfl,
					int			ignoreKerning,
					const MemoryBuffer *	afmDirectory );

#   endif
