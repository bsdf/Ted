/************************************************************************/
/*									*/
/*  Declarations for shapes.						*/
/*									*/
/*  Distances are in EMU's.						*/
/*  12700 EMU/Point.							*/
/*  635 EMU/Twip.							*/
/*  914400 EMU/Inch.							*/
/*  360000 EMU/cm.							*/
/*									*/
/*  With the exception of 'shplid', the ShapeProperties that come from	*/
/*  regular rtf only apply for the top level 'shp' or shpgrp'.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_H
#   define	DOC_SHAPE_H

struct DrawingSurface;

#   include	<utilColor.h>
#   include	<geo2DInteger.h>
#   include	<utilMemoryBuffer.h>
#   include	<geoAffineTransform.h>
#   include	<docShapeProperties.h>
#   include	<docPictureProperties.h>
#   include	"docDocumentTree.h"
#   include	<docShapeDrawing.h>
#   include	<bitmap.h>

typedef struct DrawingShape
    {
			/************************************************/
			/*  Properties as found in regular rtf tags.	*/
			/*  Are only set for the top level shape in a	*/
			/*  hierarchy.					*/
			/************************************************/
    ShapeProperties	dsShapeProperties;

			/************************************************/
			/*  Position.					*/
			/************************************************/
    unsigned char	dsIsChildShape;

			/************************************************/
			/*  Drawing.					*/
			/************************************************/
    ShapeDrawing	dsDrawing;

			/************************************************/
			/*  Embedding in the document.			*/
			/************************************************/
    int				dsShapeNumber;
    SelectionScope		dsSelectionScope;
    DocumentTree		dsDocumentTree;
    struct DrawingShape **	dsChildren;
    int				dsChildCount;

    PictureProperties		dsPictureProperties;
    MemoryBuffer		dsPictureData;
    struct DrawingSurface *	dsDrawingSurface;
    RasterImage			dsRasterImage;

    } DrawingShape;

# define DSflipHORIZONTAL( ds ) \
    ( ( (ds)->dsIsChildShape && (ds)->dsDrawing.sd_fRelFlipH ) || (ds)->dsDrawing.sd_fFlipH )

# define DSflipVERTICAL( ds ) \
    ( ( (ds)->dsIsChildShape && (ds)->dsDrawing.sd_fRelFlipV ) || (ds)->dsDrawing.sd_fFlipV )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void docInitShapeAllocated(		DrawingShape *		ds );
extern void docInitDrawingShape(	DrawingShape *		ds );
extern void docCleanDrawingShape(	DrawingShape *		ds );

extern void docDeleteDrawingShape(	struct BufferDocument *	bd,
					DrawingShape *		ds );

extern void docShapeStartShapeTransform(
				AffineTransform2D *		at,
				const DrawingShape *		ds,
				const DocumentRectangle *	dr,
				int				xSize,
				int				ySize );

extern void docShapeInternalTransform(	AffineTransform2D *	at,
					const DrawingShape *	ds );

extern int docShapeGetFill(	int *			pFill,
				RGB8Color *		rgb8,
				const DrawingShape *	ds );

extern int docShapeGetLine(	int *			pFill,
				RGB8Color *		rgb8,
				const DrawingShape *	ds );

extern void docShapeGetRects(	DocumentRectangle *		drHere,
				DocumentRectangle *		drNorm,
				const DocumentRectangle *	drTwips,
				const DrawingShape *		ds );

extern void docShapeGetChildRect(
				DocumentRectangle *		drChild,
				const DrawingShape *		dsChild,
				const DocumentRectangle *	dr,
				const DrawingShape *		ds );

#   endif	/*  DOC_SHAPE_H	*/
