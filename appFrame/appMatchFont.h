/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_MATCH_FONT_H
#   define	APP_MATCH_FONT_H

#   include	<utilDocFontList.h>
#   include	<textAttribute.h>
#   include	<psPostScriptFontList.h>

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appFcListFonts(	PostScriptFontList *		psfl );

AfmFontInfo * appFcGetFontInfoForAttribute(
				const char *			familyName,
				int				styleInt,
				const IndexSet *		unicodesUsed,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl );

extern AfmFontInfo * appGetFontInfo(
				const char *			familyName,
				int				styleInt,
				const IndexSet *		unicodesUsed,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl );

extern const AfmFontInfo * appGetFontInfoForAttribute(
				const IndexSet **		pUnicodesWanted,
				const TextAttribute *		ta,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl );

extern int appFcGetFontMetrics(	AfmFontInfo *			afi );

extern int appGetDeferredFontMetrics(	AfmFontInfo *		afi );
extern int appGetDeferredFontMetricsForList(
					PostScriptFontList *	psfl );

# ifdef __cplusplus
    }
# endif

#   endif
