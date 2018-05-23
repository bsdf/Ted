/************************************************************************/
/*									*/
/*  Read/Write raster images from/to input/output streams.		*/
/*									*/
/************************************************************************/

#   ifndef	BM_IO_H
#   define	BM_IO_H

#   include	"bitmap.h"
#   include	<sioGeneral.h>

typedef int (*bmReadBitmap)(	BitmapDescription *		pBd,
				unsigned char **		pBuffer,
				SimpleInputStream *		sis );

typedef int (*bmWriteBitmap)(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int bmWmfWriteWmf(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmRtfWriteWmfRtf(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmRtfWriteRtf(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmPngWritePng(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmRtfWritePngRtf(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmJpegWriteJfif(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmRtfWriteJfifRtf(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmGifWriteGif(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmBmpReadDib(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmJpegReadJfif(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmPngReadPng(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmGifReadGif(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmEpsTestJpegEmbeddable(
			unsigned int *			pPixelsWide,
			unsigned int *			pPixelsHigh,
			unsigned int *			pComponentCount,
			unsigned int *			pBitsPerCompunent,
			SimpleInputStream *		sis );

extern int bmPsPrintJpegImage(
			SimpleOutputStream *		sos,
			double				xscale,
			double				yscale,
			int				componentCount,
			int				ox,
			int				oy,
			int				pixelsWide,
			int				pixelsHigh,
			int				bitsPerComponent,
			SimpleInputStream *		sis );

extern int bmMacPictGetImage(	RasterImage *		ri,
				DocumentRectangle *	drSource,
				DocumentRectangle *	drDest,
				SimpleInputStream *	sis,
				int			packed,
				int			direct,
				int			clipRegion );
#   endif
