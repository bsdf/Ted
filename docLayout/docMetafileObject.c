/************************************************************************/
/*									*/
/*  Set the metafile player for an object.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docBuf.h>
#   include	"docMetafileObject.h"

#   include	<appDebugon.h>

int docSetMetafilePlayer(	MetafilePlayer *		mp,
				SimpleInputStream *		sisMeta,
				const LayoutContext *		lc,
				const PictureProperties *	pip,
				int				pixelsWide,
				int				pixelsHigh )
    {
    mp->mpInputStream= sisMeta;
    mp->mpPostScriptFontList= lc->lcPostScriptFontList;
    mp->mpGetFontForAttribute= lc->lcGetFontForAttribute;
    mp->mpMapMode= pip->pipMapMode;
    mp->mpXWinExt= pip->pip_xWinExt;
    mp->mpYWinExt=  pip->pip_yWinExt;
    mp->mpPixelsWide= pixelsWide;
    mp->mpPixelsHigh= pixelsHigh;
    mp->mpTwipsWide= pip->pipTwipsWide;
    mp->mpTwipsHigh= pip->pipTwipsHigh;

    return 0;
    }

