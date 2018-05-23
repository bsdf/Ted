/************************************************************************/
/*									*/
/*  Text Buffer: Obects inserted in the text.				*/
/*  Objects are independent entities that are embedded in the text.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_H
#   define	DOC_OBJECT_H

#   include	<utilMemoryBuffer.h>

#   include	<bitmap.h>
#   include	"docLayoutPosition.h"
#   include	"docPictureProperties.h"

struct DrawingSurface;
struct BufferDocument;
struct BufferItem;
struct TextParticule;
struct DrawingShape;

/************************************************************************/
/*									*/
/*  Kinds of object.							*/
/*									*/
/************************************************************************/

typedef struct InsertedObject
    {
    int			ioKind;		/*  Kind of object.		*/
    int			ioResultKind;	/*  Kind of object.		*/
    int			ioTwipsWide;	/*  objw			*/
    int			ioTwipsHigh;	/*  objh			*/
					/*  NOTE: this is a property of	*/
					/*  the object. Not of the way	*/
					/*  in which it is included.	*/
					/*  That is controlled by the	*/
					/*  scale.			*/
    short int		ioScaleXSet;	/*  In %.			*/
    short int		ioScaleYSet;	/*  In %.			*/
    short int		ioScaleXUsed;	/*  In %.			*/
    short int		ioScaleYUsed;	/*  In %.			*/
    int			ioPixelsWide;	/*  Width of object on screen	*/
    int			ioPixelsHigh;	/*  Height of object on screen	*/

    PictureProperties	ioPictureProperties;

    LayoutPosition	ioY0Position;
    int			ioX0Twips;

    unsigned int	ioRtfResultKind:4; /*  From rslt* tags.		*/
    unsigned int	ioRtfEmbedKind:4;  /*  From objemb.. tags.	*/

    unsigned int	ioInline:8;  	/*  Part of the text flow?	*/

    int			ioTopCropTwips;
    int			ioBottomCropTwips;
    int			ioLeftCropTwips;
    int			ioRightCropTwips;

    MemoryBuffer	ioObjectData;
    MemoryBuffer	ioResultData;

    char *		ioObjectName;
    char *		ioObjectClass;

    struct DrawingShape *	ioDrawingShape;

    struct DrawingSurface *	ioDrawingSurface;
    RasterImage		ioRasterImage;
    } InsertedObject;

typedef void (*DOC_CLOSE_OBJECT)(
				const struct BufferDocument *	bd,
				const struct TextParticule *	tp );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docObjectSetData(	InsertedObject *		io,
				const char *			bytes,
				int				size );

extern int docSetResultData(	InsertedObject *		io,
				const char *			bytes,
				int				size );

extern int docSaveObjectTag(	InsertedObject *		io,
				const char *			tag,
				int				arg );

extern int docSaveResultTag(	InsertedObject *		io,
				const char *			tag,
				int				arg );

extern int docSetObjectName(	InsertedObject *		io,
				const char *			name,
				int				len );

extern int docSetObjectClass(	InsertedObject *		io,
				const char *			name,
				int				len );

extern void docInitInsertedObject(	InsertedObject *		io );
extern void docCleanInsertedObject(	InsertedObject *		io );

extern void docObjectAdaptToPictureGeometry(
				InsertedObject *		io,
				const PictureProperties *	pip );

extern int docReadWmfObject(		InsertedObject *	io,
					const MemoryBuffer *	filename );

extern int docReadEmfObject(		InsertedObject *	io,
					const MemoryBuffer *	filename );

extern int docReadBitmapObject(		InsertedObject *	io,
					const MemoryBuffer *	filename );

extern int docReadEpsObject(		const MemoryBuffer *	fullName,
					InsertedObject *	io );

extern int docReadFileObject(		const MemoryBuffer *	fullName,
					InsertedObject *	io );

extern void docObjectGetCropRect(	DocumentRectangle *		dr,
					const PictureProperties *	pip,
					const BitmapDescription *	bd );

extern void docObjectSetPixelSize(	InsertedObject *		io,
					double pixelsPerTwip		);

#   endif	/*  DOC_OBJECT_H	*/
