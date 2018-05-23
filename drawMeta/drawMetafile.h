/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac						*/
/*									*/
/************************************************************************/

#   include	<bitmap.h>
#   include	<sioGeneral.h>
#   include	<utilMatchFont.h>

#   ifndef		DRAW_METAFILE_H
#   define		DRAW_METAFILE_H

typedef struct MetafilePlayer
    {
    SimpleInputStream *		mpInputStream;
    const PostScriptFontList *	mpPostScriptFontList;
    GetFontForAttribute		mpGetFontForAttribute;

    int				mpMapMode;
    int				mpXWinExt;
    int				mpYWinExt;
    int				mpPixelsWide;
    int				mpPixelsHigh;
    int				mpTwipsWide;
    int				mpTwipsHigh;
    } MetafilePlayer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfImg(	BitmapDescription *		pBd,
				unsigned char **		pBuffer,
				SimpleInputStream *		sis );

#   endif	/*	DRAW_METAFILE_H	*/
