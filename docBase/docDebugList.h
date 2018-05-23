/************************************************************************/
/*									*/
/*  Lists: Diagnostic routines.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DEBUG_LIST_H
#   define	DOC_DEBUG_LIST_H

#   include	"docDocumentList.h"
#   include	"docListOverride.h"
#   include	"docListOverrideTable.h"
#   include	"docListTable.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docListDocumentList(	int				n,
					const DocumentList *		dl );

extern void docListListOverrideLevel(	int				n,
					const ListOverrideLevel *	lol );

extern void docListListOverrideTable(	int				n,
					const ListOverrideTable *	lot );

extern void docListListOverride(	int				n,
					const ListOverride *		lo );

extern void docListDocumentListTable( const DocumentListTable *		dlt );

#   endif	/*	DOC_DEBUG_LIST_H	*/
