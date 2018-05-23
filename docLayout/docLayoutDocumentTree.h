/************************************************************************/
/*									*/
/*  Forward declaration for calculating the layout of external items.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LAYOUT_EXTERNAL_ITEM_H
#   define	DOC_LAYOUT_EXTERNAL_ITEM_H

struct LayoutJob;
struct DocumentTree;

typedef int (*INIT_LAYOUT_EXTERNAL)(
				struct LayoutJob *		lj,
				struct DocumentTree *		dt,
				int				page,
				int				column );

#   endif	/*  DOC_LAYOUT_EXTERNAL_ITEM_H  */
