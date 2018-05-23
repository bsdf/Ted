#    include "bmWmfIo.h"
#    include <sioEndian.h>

int bmMetaReadWmfHeader(	WmfHeader *		wh,
				SimpleInputStream *	sis )
    {
    wh->wh_fileType= sioEndianGetLeInt16( sis );
    wh->wh_headerSize= sioEndianGetLeInt16( sis );
    wh->wh_windowsVersion= sioEndianGetLeInt16( sis );
    wh->wh_fileSize= sioEndianGetLeInt32( sis );
    wh->wh_objectCount= sioEndianGetLeInt16( sis );
    wh->wh_maxRecordSize= sioEndianGetLeInt32( sis );
    wh->wh_parameterCount= sioEndianGetLeInt16( sis );

    return 0;
    }

int bmMetaReadWmfFileHeader(	unsigned long *		pKey,
				WmfFileHeader *		wfh,
				SimpleInputStream *	sis )
    {
    unsigned long	key= sioEndianGetLeUint32( sis );

    if  ( key == 0x9ac6cdd7 )
	{
	wfh->wfh_key= key;
	wfh->wfh_handle= sioEndianGetLeUint16( sis );
	wfh->wfhBounds.drX0= sioEndianGetLeInt16( sis );
	wfh->wfhBounds.drY0= sioEndianGetLeInt16( sis );
	wfh->wfhBounds.drX1= sioEndianGetLeInt16( sis );
	wfh->wfhBounds.drY1= sioEndianGetLeInt16( sis );
	wfh->wfhUnitsPerInch= sioEndianGetLeUint16( sis );

	wfh->wfh_reserved= sioEndianGetLeUint32( sis );
	wfh->wfh_checksum= sioEndianGetLeUint16( sis );

	*pKey= key;
	return 0;
	}

    *pKey= key;
    return 1;
    }

int bmWmfReadRectangle(	DocumentRectangle *		dr,
			SimpleInputStream *		sis )
    {
    int		done= 0;

    dr->drY1= sioEndianGetLeInt16( sis ); done += 2;
    dr->drX1= sioEndianGetLeInt16( sis ); done += 2;
    dr->drY0= sioEndianGetLeInt16( sis ); done += 2;
    dr->drX0= sioEndianGetLeInt16( sis ); done += 2;

    return done;
    }

int bmWmfReadHWYXRectangle(	DocumentRectangle *		dr,
				SimpleInputStream *		sis )
    {
    int		done= 0;
    int		h;
    int		w;

    h= sioEndianGetLeInt16( sis ); done += 2;
    w= sioEndianGetLeInt16( sis ); done += 2;
    dr->drY0= sioEndianGetLeInt16( sis ); done += 2;
    dr->drX0= sioEndianGetLeInt16( sis ); done += 2;

    dr->drY1= dr->drY0+ h- 1;
    dr->drX1= dr->drX0+ w- 1;
   
    return done;
    }

int bmWmfGetColor(		SimpleInputStream *	sis,
				RGB8Color *		rgb8 )
    {
    int		done= 0;

    rgb8->rgb8Red= sioInGetByte( sis ); done++;
    rgb8->rgb8Green= sioInGetByte( sis ); done++;
    rgb8->rgb8Blue= sioInGetByte( sis ); done++;

    (void) sioInGetByte( sis ); done++;

    return done;
    }

