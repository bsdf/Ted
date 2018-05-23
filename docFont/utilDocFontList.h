/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_LIST_H
#   define	DOC_FONT_LIST_H

#   include	"utilDocFont.h"

typedef struct DocFontSort
    {
    int		arrayIndex;
    int		sortIndex;
    } DocFontSort;

typedef struct DocumentFontList
    {
    int				dflFontCount;
    DocumentFont **		dflFontPages;
    int				dflFontPageCount;
    DocFontSort *		dflFontSortIndex;
    } DocumentFontList;

# define DFL_PGSZ	256

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern DocumentFont * docFontListGetFontByNumber(
					const DocumentFontList *	dfl,
					int				n );

extern void docInitFontList(		DocumentFontList *	dfl );

extern void docCleanFontList(		DocumentFontList *	dfl );

extern int docCopyFontList(		DocumentFontList *		to,
					const DocumentFontList *	from );

extern DocumentFont * utilDocFontListInsertFont(
					DocumentFontList *		dfl,
					const DocumentFont *		df );

extern int docGetFontByName(		DocumentFontList *	dfl,
					const char *		fontName );

extern int docMergeFontIntoFontlist(	DocumentFontList *	dflTo,
					const DocumentFont *	dfFrom );

extern const DocumentFont * utilDocumentFontListGetFontBySortIndex(
						DocumentFontList *	dfl,
						int			idx );

extern int utilDocumentFontListGetSortIndex(	DocumentFontList *	dfl,
						int			aidx );

extern int utilDocumentFontListGetArrayIndex(	DocumentFontList *	dfl,
						int			sidx );

extern DocumentFont * utilAddFontToDocList( DocumentFontList *	dfl,
					const char *		name,
					int			styleInt,
					int			pitch );

extern void utilDocFontListClearCharsUsed(	DocumentFontList *	dfl );

#   endif	/*  DOC_FONT_LIST_H	*/
