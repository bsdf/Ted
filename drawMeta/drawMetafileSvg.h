/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac to SVG.					*/
/*									*/
/************************************************************************/

#   include	<svgWriter.h>
#   include	"drawMetafile.h"

#   ifndef		DRAW_METAFILE_SVG_H
#   define		DRAW_METAFILE_SVG_H

typedef int (*MetafileWriteSvg)( SvgWriter *			sw,
				const MetafilePlayer *		player );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfSvg(	SvgWriter *			sw,
				const MetafilePlayer *		player );

extern int appMacPictPlayFileSvg( SvgWriter *			sw,
				const MetafilePlayer *		player );

extern int appMetaPlayEmfSvg(	SvgWriter *			sw,
				const MetafilePlayer *		player );

#   endif	/*	DRAW_METAFILE_SVG_H	*/
