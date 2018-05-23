/************************************************************************/
/*									*/
/*  Description of a bitmap.						*/
/*									*/
/************************************************************************/

#   ifndef	BITMAP_H
#   define	BITMAP_H

#   include	<utilColor.h>
#   include	<utilPalette.h>
#   include	<utilMemoryBuffer.h>
#   include	<geo2DInteger.h>

/************************************************************************/
/*									*/
/*  Color encodings.							*/
/*									*/
/************************************************************************/

typedef enum BitmapColorEncoding
    {
    BMcoBLACKWHITE= 0,
    BMcoWHITEBLACK,
    BMcoRGB,
    BMcoRGB8PALETTE,

    BMco_COUNT
    } BitmapColorEncoding;

#   define	BMcoILLEGALVALUE	BMco_COUNT

extern const char *	bmcoIntToString( int colorEncodingInt );
extern int		bmcoStringToInt( const char * colorEncodingString );
extern const char *	bmcoStrings[];

/************************************************************************/
/*									*/
/*  Units of measurement.						*/
/*									*/
/************************************************************************/

typedef enum BitmapResolution
    {
    BMunM= 0,
    BMunINCH,
    BMunPOINT,
    BMunPIXEL,

    BMun_COUNT
    } BitmapResolution;

#   define	BMunILLEGALVALUE	BMun_COUNT

extern const char *	bmunIntToString( int unitInt );
extern int		bmunStringToInt( const char * unitString );
extern const char *	bmunStrings[];

/************************************************************************/
/*									*/
/*  Description of a bitmap.						*/
/*									*/
/************************************************************************/

typedef struct	BitmapDescription
    {
					/********************************/
					/*  Number of bytes in an image	*/
					/*  buffer; Idem for one row.	*/
					/*  Rows are never divided over	*/
					/*  more than one byte.		*/
					/********************************/
    unsigned int	bdBufferLength;
    unsigned int	bdBytesPerRow;
					/********************************/
					/*  Height/width of an image	*/
					/********************************/
    unsigned int	bdPixelsWide;
    unsigned int	bdPixelsHigh;
					/********************************/
					/*  Description of how pixels	*/
					/*  are encoded.		*/
					/*  For direct images, all three*/
					/*  refer to the buffer.	*/
					/*  For palette type images,	*/
					/*  BitsPerSample,		*/
					/*  SamplesPerPixel refer to the*/
					/*  image; BitsPerPixel refers	*/
					/*  to the buffer.		*/
					/********************************/
    int			bdBitsPerSample;
    int			bdSamplesPerPixel;
    int			bdBitsPerPixel;
					/********************************/
					/*  Pixels per metric unit;	*/
					/*  The unit.			*/
					/********************************/
    int			bdXResolution;
    int			bdYResolution;
    unsigned char	bdUnit;
					/********************************/
					/*  How are colors encoded.	*/
					/********************************/
    unsigned char	bdColorEncoding;
					/********************************/
					/*  Does it have a transparency	*/
					/*  mask? If so, 0 in the data	*/
					/*  means transparent, the	*/
					/*  maximum value opaque. Values*/
					/*  between are not supported.	*/
					/********************************/
    unsigned char	bdHasAlpha;
					/********************************/
					/*  Palette is only relevant	*/
					/*  with BMcoRGB8PALETTE.	*/
					/********************************/
    ColorPalette	bdPalette;
    } BitmapDescription;

/************************************************************************/
/*									*/
/*  Used to store and manage a bitmap image.				*/
/*									*/
/************************************************************************/

typedef struct RasterImage
    {
    BitmapDescription	riDescription;
    unsigned char *	riBytes;
    int			riFormat;
    } RasterImage;

/************************************************************************/
/*									*/
/*  Description of a bitmap file format.				*/
/*									*/
/************************************************************************/

typedef struct	BitmapFileType
    {
    int (*bftWrite)(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat );

    int (*bftCanWrite)( const BitmapDescription *	bd,
			int				privateFormat );

    int (*bftRead)(	const MemoryBuffer *		filename,
			unsigned char **		pBuffer,
			BitmapDescription *		bd,
			int *				pPrivateFormat );

    const char *	bftFileExtension;
    const char *	bftFileFilter;
    const char *	bftTypeId;
    const char *	bftTypeDescription;
    } BitmapFileType;

typedef struct	BitmapFileFormat
    {
    const char *	bffDescription;
    const char *	bffId;
    int			bffPrivate;
    BitmapFileType *	bffFileType;
    } BitmapFileFormat;

/************************************************************************/
/*									*/
/*  Catalog of available of a bitmap file formats.			*/
/*									*/
/************************************************************************/

extern BitmapFileFormat	bmFileFormats[];
extern const int	bmNumberOfFileFormats;

extern BitmapFileType *	bmFileTypes[];
extern const int	bmNumberOfFileTypes;

/************************************************************************/
/*									*/
/*  For Reading/Writing from/to streams.				*/
/*									*/
/************************************************************************/

extern int bmWriteGifFile(	const MemoryBuffer *		filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat );

extern int bmCanWriteGifFile(	const BitmapDescription *	bd,
				int				privateFormat );

extern int bmWriteJpegFile(	const MemoryBuffer *		filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat );

extern int bmCanWriteJpegFile(	const BitmapDescription *	bd,
				int				privateFormat );

/************************************************************************/

extern int bmWriteRtfFile(	const MemoryBuffer *		filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat );

extern int bmCanWriteRtfFile(	const BitmapDescription *	bd,
				int				privateFormat );

extern int bmCanWritePngFile(	const BitmapDescription *	bd,
				int				privateFormat );

/************************************************************************/
/*  Routines.								*/
/************************************************************************/

extern void bmInitDescription(	BitmapDescription *	bd	);

extern void bmCleanDescription(	BitmapDescription *	bd	);

extern int bmCopyDescription(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn	);

extern int bmWrite(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				fileFormat );

extern int bmCanWrite(	const BitmapDescription *	bd,
			int				fileFormat );

extern int bmRead(	const MemoryBuffer *	filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pFileFormat );

extern int bmSelect(	RasterImage *			riOut,
			const RasterImage *		riIn,
			const DocumentRectangle *	drSrc );

extern int bmComponents( void ***		pComponents,
			int *			pCount,
			const unsigned char *	buffer,
			const BitmapDescription * bd );

extern int bmComponentBitmap( unsigned char **		buffer,
			BitmapDescription *		bdout,
			DocumentRectangle *		dr,
			const BitmapDescription *	bdin,
			const void *			component );

extern int bmGroupBitmap( unsigned char **		buffer,
			BitmapDescription *		bdout,
			DocumentRectangle *		dr,
			const BitmapDescription *	bdin,
			const void *			vbc	);

int bmgGroupBitmap(	unsigned char **		pBuffer,
			BitmapDescription *		bdout,
			DocumentRectangle *		dr,
			const BitmapDescription *	bdin,
			const void *			vcg );

extern int bmGroups(	void ***			pGroups,
			void **				comps,
			int				ncomp,
			const BitmapDescription *	bd		);

extern void bmFreeGroups( void **			groups,
			int				numberOfGroups	);

/************************************************************************/
/*									*/
/*  Palette expansion for drawing.					*/
/*									*/
/************************************************************************/

int bmExpandRGB8Palette(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			bitsPerColor,
				const ColorPalette *	palette,
				int			hasAlpha );

/************************************************************************/
/*									*/
/*  Bitmap transformations.						*/
/*									*/
/************************************************************************/

typedef int (*BM_PIX_FUN)(	void *				through,
				int				row,
				int				col );

typedef void (*BM_SEG_FUN)(	void *				through,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

typedef int (*RasterTransformation)(RasterImage *		riOut,
				const RasterImage *		riIn,
				int				option );

extern int bmWhiteToTransparent(RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmVerticalFlip(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmHorizontalFlip(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmRotate180(		RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmRotate90(		RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmColorReduce(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				colorCount );

extern int bmRotate(	RasterImage *			riOut,
			const RasterImage *		riIn,
			double				angle	);

extern int bmFilterSobel(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmFilterLaplace(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmFilterSmoothe(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmFilterBlur(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmFilterRelative(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmFlipBytes(		unsigned char *	buffer,
				int		byteCount,
				int		bitsPerPixel );

int bmMapImageColors(		const BitmapDescription *	bd,
				const int *			map,
				unsigned char *			buffer );

extern int bmAverage(		RasterImage *			riOut,
				const RasterImage *		riIn,
				int				xpixels,
				int				ypixels );

extern int bmExpandPaletteImage( RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern int bmTransparentImage(	BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				wide,
				int				high );

extern int bmRGBImage(		BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				r,
				int				g,
				int				b,
				int				wide,
				int				high );

extern int bmGammaCorrect(	RasterImage *			riOut,
				const RasterImage *		riIn,
				double				gammaValue );

extern int bmInvertImage(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt );

extern void bmImageSizeTwips(
			int *				pImageWideTwips,
			int *				pImageHighTwips,
			const BitmapDescription *	bd );

extern void bmRectangleSizeTwips(
			int *				pRectangleWideTwips,
			int *				pRectangleHighTwips,
			const BitmapDescription *	bd,
			int				pixelsWide,
			int				pixelsHigh );

extern int bmCalculateSizes(	BitmapDescription *	bd );
extern int bmAllocateBuffer(	RasterImage *		ri );

extern int bmToGrayscale(
			RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmToWebSafe(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmThreshold(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmMorphoDilateSimple(
			RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmMorphoErodeSimple(
			RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmMorphoDilate(	RasterImage *			riOut,
				const RasterImage *		riIn,
				const RasterImage *		riSe,
				int				rowOrig,
				int				colOrig );

extern int bmMorphoErode(	RasterImage *			riOut,
				const RasterImage *		riIn,
				const RasterImage *		riSe,
				int				rowOrig,
				int				colOrig );

extern int bmMorphoSetSimpleSe(	RasterImage *			riOut );

extern int bmMorphoLineElement(	RasterImage *			riOut,
				int				wide,
				int				high,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

extern void bmDraw1BitImage(	const BitmapDescription *	bdTo,
				unsigned char *			bufTo,
				const RasterImage *		riIn,
				unsigned char			invertMaskIn,
				int				rowTo0,
				int				colTo0 );

extern int bmForAll1Pixels(	const RasterImage *		riIn,
				unsigned char			invertMaskIn,
				void *				through,
				BM_PIX_FUN			pixFun );

extern int bmFindLineSegments(	const RasterImage *		riIn,
				int				size,
				void *				through,
				BM_SEG_FUN			segFun );

extern int bmCountLinePixels(	const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

extern int bmDrawLine(		unsigned char *			buffer,
				const BitmapDescription *	bd,
				int				x0,
				int				y0,
				int				x1,
				int				y1,
				int				wide );

extern int bmDrawBox(		unsigned char *			buffer,
				const BitmapDescription *	bd,
				int				x0,
				int				y0,
				int				x1,
				int				y1,
				int				wide );

extern int bmSuggestFormat(	const MemoryBuffer *		filename,
				int				suggestedFormat,
				const BitmapDescription *	bd );

extern int bmSetSolidWhite(	RasterImage *			ri );
extern int bmSetSolidBlack(	RasterImage *			ri );

extern int bmCopyArea(		int				up,
				int				vp,
				RasterImage *			riTo,
				const RasterImage *		riFrom );

extern int bmPaintArea(		int				up,
				int				vp,
				RasterImage *			riTo,
				const RasterImage *		riFrom );

extern int bmGetAlphaMask(
			RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmRemoveAlpha( RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt );

extern int bmSetAlphaMask(
			BitmapDescription *		bdOut,
			const BitmapDescription *	bdImage,
			const BitmapDescription *	bdAlpha,
			unsigned char **		pBufOut,
			const unsigned char *		bufImage,
			const unsigned char *		bufAlpha );

extern void bmInitRasterImage(	RasterImage *		ri );
extern void bmCleanRasterImage(	RasterImage *		ri );
extern void bmFreeRasterImage(	RasterImage *		ri );

extern int bmCopyRasterImage(	RasterImage *		to,
				const RasterImage *	from );

#   endif
