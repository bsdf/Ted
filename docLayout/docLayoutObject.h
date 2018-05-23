/************************************************************************/
/*									*/
/*  Set the player for an object.					*/
/*									*/
/************************************************************************/

#   ifndef		DOC_LAYOUT_OBJECT_H
#   define		DOC_LAYOUT_OBJECT_H

#   include		<docBuf.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docGetBitmapForObject(	InsertedObject *	io );

extern int docCheckObjectLayout(	int *			pFixed,
					InsertedObject *	io );

extern int docGetBitmapForObjectData(
				int				kind,
				RasterImage *			ri,
				const MemoryBuffer *		mb );

extern void docLayoutScaleObjectToFitParagraphFrame(
				int *				pChanged,
				InsertedObject *		io,
				int				pageHigh,
				const DocumentRectangle *	drParaFrame );

extern void docScaleObjectToParagraph(
				BufferDocument *		bd,
				struct BufferItem *		paraBi,
				double				xfac,
				InsertedObject *		io );

#   endif	/*	DOC_LAYOUT_OBJECT_H	*/

