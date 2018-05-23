#   include	"bmEmfIo.h"
#   include	<sioEndian.h>

#   include	<appDebugon.h>

int bmMetaReadEmfHeader(	EmfHeader *		eh,
				SimpleInputStream *	sis )
    {
    int			done= 0;
    unsigned long	recordType;

    recordType= sioEndianGetLeUint32( sis ); done += 4;
    eh->eh_headerSize= sioEndianGetLeUint32( sis ); done += 4;

    eh->ehBounds.drX0= sioEndianGetLeInt32( sis ); done += 4;
    eh->ehBounds.drY0= sioEndianGetLeInt32( sis ); done += 4;
    eh->ehBounds.drX1= sioEndianGetLeInt32( sis ); done += 4;
    eh->ehBounds.drY1= sioEndianGetLeInt32( sis ); done += 4;

    eh->ehFrame.drX0= sioEndianGetLeInt32( sis ); done += 4;
    eh->ehFrame.drY0= sioEndianGetLeInt32( sis ); done += 4;
    eh->ehFrame.drX1= sioEndianGetLeInt32( sis ); done += 4;
    eh->ehFrame.drY1= sioEndianGetLeInt32( sis ); done += 4;

    eh->eh_signature= sioEndianGetLeUint32( sis ); done += 4;
    eh->eh_version= sioEndianGetLeUint32( sis ); done += 4;
    eh->eh_size= sioEndianGetLeUint32( sis ); done += 4;
    eh->eh_numOfRecords= sioEndianGetLeUint32( sis ); done += 4;

    eh->eh_numOfHandles= sioEndianGetLeUint16( sis ); done += 2;
    eh->eh_reserved= sioEndianGetLeUint16( sis ); done += 2;

    eh->eh_sizeOfDescrip= sioEndianGetLeUint32( sis ); done += 4;
    eh->eh_offsOfDescrip= sioEndianGetLeUint32( sis ); done += 4;
    eh->eh_numPalEntries= sioEndianGetLeUint32( sis ); done += 4;

    eh->eh_widthDevPixels= sioEndianGetLeInt32( sis ); done += 4;
    eh->eh_heightDevPixels= sioEndianGetLeInt32( sis ); done += 4;
    eh->eh_widthDevMM= sioEndianGetLeInt32( sis ); done += 4;
    eh->eh_heightDevMM= sioEndianGetLeInt32( sis ); done += 4;

    /*********/

    if  ( recordType != 1 )
	{ LLDEB(recordType,1); return -1;	}
    if  ( eh->eh_signature != 0x464D4520 )
	{ XXDEB(eh->eh_signature,0x464D4520); return -1;	}
    if  ( eh->eh_version != 0x00010000 )
	{ XXDEB(eh->eh_version,0x00010000);	}

    return done;
    }

int bmEmfReadRectangle(	DocumentRectangle *		dr,
			SimpleInputStream *		sis )
    {
    int		done= 0;

    dr->drX0= sioEndianGetLeInt32( sis ); done += 4;
    dr->drY0= sioEndianGetLeInt32( sis ); done += 4;
    dr->drX1= sioEndianGetLeInt32( sis ); done += 4;
    dr->drY1= sioEndianGetLeInt32( sis ); done += 4;

    return done;
    }

int bmEmfReadXYWHRectangle(	DocumentRectangle *		dr,
				SimpleInputStream *		sis )
    {
    int		done= 0;
    int		w;
    int		h;

    dr->drX0= sioEndianGetLeInt32( sis ); done += 4;
    dr->drY0= sioEndianGetLeInt32( sis ); done += 4;
    w= sioEndianGetLeInt32( sis ); done += 4;
    h= sioEndianGetLeInt32( sis ); done += 4;

    dr->drX1= dr->drX0+ w- 1;
    dr->drY1= dr->drY0+ h- 1;

    return done;
    }

int bmEmfReadRgb8Color(	RGB8Color *			rgb8,
			SimpleInputStream *		sis )
    {
    int		done= 0;

    rgb8->rgb8Red= sioInGetByte( sis ); done += 1;
    rgb8->rgb8Green= sioInGetByte( sis ); done += 1;
    rgb8->rgb8Blue= sioInGetByte( sis ); done += 1;
    rgb8->rgb8Alpha= sioInGetByte( sis ); done += 1;

    return done;
    }

int bmEmfReadAffineTransform2D(	AffineTransform2D *		at,
				SimpleInputStream *		sis )
    {
    int		done= 0;

    at->at2Axx= sioEndianGetLeFloat( sis ); done += 4;
    at->at2Axy= sioEndianGetLeFloat( sis ); done += 4;
    at->at2Ayx= sioEndianGetLeFloat( sis ); done += 4;
    at->at2Ayy= sioEndianGetLeFloat( sis ); done += 4;

    at->at2Tx= sioEndianGetLeFloat( sis ); done += 4;
    at->at2Ty= sioEndianGetLeFloat( sis ); done += 4;

    return done;
    }

int bmMetaWriteEmfHeader(	const EmfHeader *	eh,
				SimpleOutputStream *	sos )
    {
    const int	recordType= 1;
    int		done= 0;

    sioEndianPutLeInt32( recordType, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_headerSize, sos ); done += 4;

    sioEndianPutLeInt32( eh->ehBounds.drX0, sos ); done += 4;
    sioEndianPutLeInt32( eh->ehBounds.drY0, sos ); done += 4;
    sioEndianPutLeInt32( eh->ehBounds.drX1, sos ); done += 4;
    sioEndianPutLeInt32( eh->ehBounds.drY1, sos ); done += 4;

    sioEndianPutLeInt32( eh->ehFrame.drX0, sos ); done += 4;
    sioEndianPutLeInt32( eh->ehFrame.drY0, sos ); done += 4;
    sioEndianPutLeInt32( eh->ehFrame.drX1, sos ); done += 4;
    sioEndianPutLeInt32( eh->ehFrame.drY1, sos ); done += 4;

    sioEndianPutLeInt32( eh->eh_signature, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_version, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_size, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_numOfRecords, sos ); done += 4;

    sioEndianPutLeInt16( eh->eh_numOfHandles, sos ); done += 2;
    sioEndianPutLeInt16( eh->eh_reserved, sos ); done += 2;

    sioEndianPutLeInt32( eh->eh_sizeOfDescrip, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_offsOfDescrip, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_numPalEntries, sos ); done += 4;

    sioEndianPutLeInt32( eh->eh_widthDevPixels, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_heightDevPixels, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_widthDevMM, sos ); done += 4;
    sioEndianPutLeInt32( eh->eh_heightDevMM, sos ); done += 4;

    return done;
    }
