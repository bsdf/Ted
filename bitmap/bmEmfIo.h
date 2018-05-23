#   include	"bmEmf.h"
#   include	<utilColor.h>
#   include	<sioGeneral.h>

#   include	<geoAffineTransform.h>

extern int bmMetaReadEmfHeader(		EmfHeader *		eh,
					SimpleInputStream *	sis );

extern int bmMetaWriteEmfHeader(	const EmfHeader *	eh,
					SimpleOutputStream *	sos );

extern int bmEmfReadRectangle(		DocumentRectangle *		dr,
					SimpleInputStream *		sis );

extern int bmEmfReadXYWHRectangle(	DocumentRectangle *		dr,
					SimpleInputStream *		sis );

extern int bmEmfReadRgb8Color(	RGB8Color *			rgb8,
				SimpleInputStream *		sis );

extern int bmEmfReadAffineTransform2D(	AffineTransform2D *		at,
					SimpleInputStream *		sis );
