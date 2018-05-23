/************************************************************************/
/*									*/
/*  Row margin tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_ROW_MARGIN_TOOL_H
#   define	TED_ROW_MARGIN_TOOL_H

#   include	<appFrame.h>
#   include	<docRowProperties.h>

struct BufferItem;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Resources for the 'Row Margins' controls.				*/
/*									*/
/************************************************************************/

typedef struct RowMarginsToolResources
    {
    char *		rmtrCellMargin;
    char *		rmtrLeftIndent;
    } RowMarginsToolResources;

/************************************************************************/
/*									*/
/*  The margins controls on a row tool, i.e. the 'Row' or 'Table' page	*/
/*  of the format tool.							*/
/*									*/
/************************************************************************/

typedef struct RowMarginsTool
    {
				/**
				 *  The margin inside the parent. Usually,
				 *  this is the page left margin, but for 
				 *  nested rows this is the left margin 
				 *  of the parent cell.
				 */
    int				rmtParentLeftMargin;
				/**
				 *  The right most position inside the parent.
				 *  Usually, this is the page width, but for
				 *  nested rows this is the width of the 
				 *  parent cell.
				 */
    int				rmtParentWidth;

    APP_WIDGET			rmtCellMarginRow;
    APP_WIDGET			rmtCellMarginLabel;
    APP_WIDGET			rmtCellMarginText;

    APP_WIDGET			rmtLeftIndentRow;
    APP_WIDGET			rmtLeftIndentLabel;
    APP_WIDGET			rmtLeftIndentText;
    } RowMarginsTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillRowMarginsTool(
			RowMarginsTool *		rmt,
			const RowMarginsToolResources *	rmtr,
			APP_TXACTIVATE_CALLBACK_T	leftIndentChanged,
			APP_TXACTIVATE_CALLBACK_T	cellMarginChanged,
			void *				through,
			APP_WIDGET			pageWidget );

extern void tedRefreshRowMarginsTool(
				RowMarginsTool *		rmt,
				const RowProperties *		rp,
				int				canChange );

extern int tedRowMarginToolGetValues(	RowProperties *		rp,
					RowMarginsTool *	rmt );

extern void tedInitRowMarginsTool(	RowMarginsTool *	rmt );
/*
extern void tedCleanRowMarginsTool(	RowMarginsTool *	rmt );
*/

extern int tedMarginToolCheckCellMargin(	RowMarginsTool *	rmt,
						RowProperties *		rp );

extern int tedMarginToolCheckLeftIndent(	RowMarginsTool *	rmt,
						RowProperties *		rp );

extern int tedRowMarginToolGetMargins(	RowMarginsTool *	rmt,
					struct BufferItem *	rowNode,
					struct BufferDocument *	bd );

#   endif	/*  TED_ROW_MARGIN_TOOL_H */
