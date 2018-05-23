/************************************************************************/
/*									*/
/*  The stylesheet of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STYLE_SHEET_H
#   define	DOC_STYLE_SHEET_H

#   include	"docStyle.h"

typedef struct DocumentStyleSheet
    {
    DocumentStyle **	dssStyles;
    int			dssStyleCount;
    void *		dssStylesByName;
    } DocumentStyleSheet;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern DocumentStyle * docInsertStyle(
				DocumentStyleSheet *		dss,
				int				n,
				const DocumentStyle *		dsFrom,
				const DocumentAttributeMap *	dam );

extern void docInitStyleSheet(	DocumentStyleSheet *	dss );
extern void docCleanStyleSheet(	DocumentStyleSheet *	dss );

extern DocumentStyle * docGetStyleByName(
				const DocumentStyleSheet *	dss,
				const char *			styleName );

extern int docCopyStyleSheet(	DocumentStyleSheet *		to,
				const DocumentStyleSheet *	from,
				const DocumentAttributeMap *	dam );

#   endif	/*  DOC_STYLE_SHEET_H  */
