/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac To PostScript.				*/
/*									*/
/************************************************************************/

#   include	<psFace.h>
#   include	"drawMetafile.h"

#   ifndef		DRAW_METAFILE_PS_H
#   define		DRAW_METAFILE_PS_H

typedef int (*MetafileWriteListPs)(
				PostScriptTypeList *		pstl,
				const MetafilePlayer *		player,
				const char *			prefix );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appWmfListFontsPs(	PostScriptTypeList *		pstl,
				const MetafilePlayer *		player,
				const char *			prefix );

extern int appMacPictListFontsPs( PostScriptTypeList *		pstl,
				const MetafilePlayer *		player,
				const char *			prefix );

extern int appEmfListFontsPs(	PostScriptTypeList *		pstl,
				const MetafilePlayer *		player,
				const char *			prefix );

#   endif	/*	DRAW_METAFILE_PS_H	*/
