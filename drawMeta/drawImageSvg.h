/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac to SVG.					*/
/*									*/
/************************************************************************/

#   include	<svgWriter.h>
#   include	<bitmap.h>
#   include	<utilMemoryBuffer.h>

#   ifndef		DRAW_IMAGE_SVG_H
#   define		DRAW_IMAGE_SVG_H

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int drawRasterImageSvg(	SvgWriter *			sw,
				const RasterImage *		abiSrc,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest );

extern int drawRasterImageSvgFromData(
				SvgWriter *			sw,
				const char *			contentType,
				const MemoryBuffer *		mb,
				const DocumentRectangle *	drDest );

#   endif	/*	DRAW_IMAGE_SVG_H	*/
