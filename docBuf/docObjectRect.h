/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_RECT_H
#   define	DOC_OBJECT_RECT_H

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docObjectGetPageRect( DocumentRectangle *		drDest,
				const InsertedObject *		io,
				int				x0Twips,
				int				baselineTwips );

extern void docObjectGetSourceRect(	DocumentRectangle *		drSrc,
					const PictureProperties *	pip );

#   endif	/*  DOC_OBJECT_RECT_H  */
