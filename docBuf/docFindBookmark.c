/************************************************************************/
/*									*/
/*  Find a bookmark in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

int docFindBookmarkInDocument(	DocumentSelection *	dsInside,
				int *			pHeadPart,
				int *			pTailPart,
				const BufferDocument *	bd,
				const MemoryBuffer *	markName )
    {
    int			i;
    DocumentField *	df;

    i= docFindBookmarkField( &df, &(bd->bdFieldList), markName );
    if  ( i >= 0 )
	{
	DocumentSelection	dsAround;

	if  ( docDelimitFieldInDoc( dsInside, &dsAround,
					    pHeadPart, pTailPart, bd, df ) )
	    { LDEB(i); return -1;	}

	return 0;
	}

    return 1;
    }

