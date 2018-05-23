/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*  Individual list levels.						*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docListOverrideLevel.h"

void docInitListOverrideLevel(	ListOverrideLevel *	lol )
    {
    lol->lolOverrideStartAt= 0;
    lol->lolOverrideFormat= 0;

    docInitDocumentListLevel( &(lol->lolListLevel) );

    return;
    }

void docCleanListOverrideLevel(	ListOverrideLevel *	lol )
    {
    docCleanDocumentListLevel( &(lol->lolListLevel) );
    }

int docCopyListOverrideLevel(	ListOverrideLevel *		to,
				const ListOverrideLevel *	from,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap )
    {
    if  ( from->lolOverrideFormat )
	{
	const int	copyIds= 0;

	if  ( docCopyDocumentListLevel( &(to->lolListLevel),
				    &(from->lolListLevel),
				    copyIds, fontMap, colorMap, rulerMap ) )
	    { LDEB(1); return -1;	}
	}
    else{ to->lolListLevel.llStartAt= from->lolListLevel.llStartAt;	}

    to->lolOverrideStartAt= from->lolOverrideStartAt;
    to->lolOverrideFormat= from->lolOverrideFormat;

    return 0;
    }

