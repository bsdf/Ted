#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	"bmintern.h"
#   include	<sioGeneral.h>
#   include	<utilMemoryBuffer.h>

/************************************************************************/
/*									*/
/*  Data structures.							*/
/*									*/
/************************************************************************/

/************************************************************************/
/*  Describes a BMP file.						*/
/************************************************************************/

#define COMPRESS_NONE        0L      /* No compression               */
#define COMPRESS_RLE8       1L      /* 8 bits per pixel compression */
#define COMPRESS_RLE4       2L      /* 4 bits per pixel compression */
#define BMP_ID              0x4d42  /* BMP "magic" number           */

/************************************************************************/
/*  Common information for all kinds of BMP files.			*/
/************************************************************************/

typedef struct BmpImageHeader
    {
			/************************************************/
			/*  Common to all variants.			*/
			/************************************************/
    short	bihMagic;
    long	bihFileSize;
    short	bihReserved1;
    short	bihReserved2;
    long	bihBitmapDataOffset;
    long	bihSizeOfRestOfHeader;	/*  It is the Type of file!	*/
			/************************************************/
			/*  Common to all variants.			*/
			/************************************************/
    long	bihPixelsWide;
    long	bihPixelsHigh;
    short	bihPlanes;
    short	bihBitsPerPixel;
			/************************************************/
			/*  Common to Windows 3.* and OS/2 2.*		*/
			/************************************************/
    long	bihCompression;
    long	bihBufferLength;
    long	bihXResolution;
    long	bihYResolution;
    long	bihColorCount;
    long	bihImportantColors;
    } BmpImageHeader;

/************************************************************************/
/*  Structures desribing ICO files.					*/
/************************************************************************/

typedef struct IcoFileHeader
    {
    short	ifhReserved;
    short	ifhType;
    short	ifhCount;
    } IcoFileHeader;

typedef struct IcoDirectoryEntry
    {
    unsigned char	ideWidth;
    unsigned char	ideHeight;
    unsigned char	ideColorCount;
    unsigned char	ideReserved;
    short		idePlanes;
    short		ideBitCount;
    long		ideBytesInRes;
    long		ideImageOffset;
    } IcoDirectoryEntry;

/************************************************************************/
/*  Routine declarations.						*/
/************************************************************************/

extern int bmCollectDib(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				BmpImageHeader *		bih,
				MemoryBuffer *			bmpBytes,
				RGB8Color **			pPalette,
				int				privateFormat,
				int				fileHeader );

extern int bmpWriteImageHeader(	const BmpImageHeader *		bih,
				SimpleOutputStream *		sos,
				const RGB8Color *		palette );

extern int bmpReadImageHeader(	BmpImageHeader *	bih,
				SimpleInputStream *	sis,
				RGB8Color **		pPalette );

extern int bmpHeaderToDescription(	BitmapDescription *	bd,
					int *			pBytesPerRow,
					const BmpImageHeader *	bih,
					const RGB8Color *	palette,
					int			assumeAlpha );

extern int bmBmpReadImageBytes(	unsigned char **		pBuffer,
				SimpleInputStream *		sis,
				int				compression,
				int				bytesPerRowF,
				int				bitsPerPixelFile,
				const BitmapDescription *	bd );

extern int bmBmpReadDibX(	BitmapDescription *		pBd,
				unsigned char **		pBuffer,
				BmpImageHeader *		bih,
				const RGB8Color *		palette,
				SimpleInputStream *		sis );

extern int bmbmpDescriptionToHeader(	BmpImageHeader *	bih,
					RGB8Color **		pPalette,
					int			privateFormat,
					int			hasAlpha,
					const BitmapDescription * bd );

extern int bmpSizeImageHeader(	const BmpImageHeader *	bih,
				RGB8Color *		palette );

extern int bmBmpSaveImageBytes(	MemoryBuffer *			bmpBytes,
				const BitmapDescription *	bd,
				const unsigned char *		buffer,
				int				compression );
