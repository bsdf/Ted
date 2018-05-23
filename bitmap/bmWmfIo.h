#   include	"bmWmf.h"
#   include	<sioGeneral.h>

extern int bmMetaReadWmfHeader(		WmfHeader *		wh,
					SimpleInputStream *	sis );

extern int bmWmfReadRectangle(		DocumentRectangle *		dr,
					SimpleInputStream *		sis );

extern int bmWmfReadHWYXRectangle(	DocumentRectangle *		dr,
					SimpleInputStream *		sis );

extern int bmMetaReadWmfFileHeader(	unsigned long *		pKey,
					WmfFileHeader *		wfh,
					SimpleInputStream *	sis );

extern int bmWmfGetColor(		SimpleInputStream *	sis,
					RGB8Color *		rgb8 );
