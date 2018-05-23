#   ifndef	BM_INTERN_H
#   define	BM_INTERN_H

#   include	"bitmap.h"

/************************************************************************/
/*									*/
/*  Byte Masks								*/
/*									*/
/************************************************************************/

extern unsigned char	Bmc1Masks[8];
extern unsigned char	Bmc7Masks[8];

/************************************************************************/
/*									*/
/*  Routines.								*/
/*									*/
/************************************************************************/

extern int bmReadTiffFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmWriteTiffFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWriteTiffFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmReadBmpFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmReadWbmpFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmReadIcoFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteBmpFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWriteWbmpFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWriteIcoFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteBmpFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

extern int bmWriteWbmpFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

extern int bmWriteIcoFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

extern int bmReadXwdFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmWriteXwdFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat );

extern int bmReadGifFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteEpsFile(	const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWritePdfFile(	const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteEpsFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWritePdfFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmReadJpegFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmReadXbmFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteXbmFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteXbmFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmCanWriteWmfFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteWmfFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmReadPngFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmWritePngFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmReadXpmFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWriteXpmFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWriteXpmFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmReadPbmFile(	const MemoryBuffer *	filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat );

extern int bmCanWritePbmFile( const BitmapDescription *	bd,
				int			privateFormat );

extern int bmWritePbmFile(	const MemoryBuffer *	filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat );

extern int bmFillBlock(	unsigned char *		buffer,
			int			x0,
			int			y0,
			int			x1,
			int			y1,
			int			bytesPerRow );

extern unsigned char *	bmBackgroundBuffer(	int	bufferLength,
						int	colorEncoding );
extern unsigned char *	bmForegroundBuffer(	int	bufferLength,
						int	colorEncoding );

extern int bmMakeGrayPalette(
			const BitmapDescription *	bd,
			int *				pColorCount,
			int *				pTransparentColor,
			RGB8Color *			palette,
			int				maxColors );

extern int bmInflateTo8bit(		unsigned char *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd,
					int				trnsC,
					int				remA );

extern int bmInflateToInt(		unsigned int *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd,
					int				trnsC,
					int				remA );

extern int bmInflateTo8bitGray(	
			unsigned char *			to,
			const unsigned char *		from,
			int				bitsPerPixel,
			int				pixelsWide );

extern int bmMakeMonochrome(		BitmapDescription *	bd,
					unsigned char *		buffer );

extern int bmPlanarToChunky(		unsigned char *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd );

extern int bmPaletteColor(	BitmapDescription *		bd,
				int				r,
				int				g,
				int				b,
				int				a );

#   endif /*  BM_INTERN_H	*/
