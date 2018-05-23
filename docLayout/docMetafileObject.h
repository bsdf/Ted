/************************************************************************/
/*									*/
/*  Set the player for an object.					*/
/*									*/
/************************************************************************/

#   ifndef		DOC_METAFILE_OBJECT_H
#   define		DOC_METAFILE_OBJECT_H

#   include		<drawMetafile.h>
#   include		<docBuf.h>
#   include		"layoutContext.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSetMetafilePlayer( MetafilePlayer *		mp,
				SimpleInputStream *		sisMeta,
				const LayoutContext *		lc,
				const PictureProperties *	pip,
				int				pixelsWide,
				int				pixelsHigh );

#   endif	/*	DOC_METAFILE_OBJECT_H	*/

