/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"

/************************************************************************/
/*									*/
/*  Write a color table.						*/
/*									*/
/************************************************************************/

int docRtfWriteColorTable(	RtfWriter *			rwc,
				const DocumentProperties *	dp )
    {
    int				i;
    const RGB8Color *		rgb8= dp->dpColorPalette->cpColors;

    docRtfWriteDestinationBegin( rwc, "colortbl" );
    docRtfWriteNextLine( rwc );

    for ( i= 0; i < dp->dpColorPalette->cpColorCount; rgb8++, i++ )
	{
	if  ( i != dp->dpDefaultColor )
	    {
	    docRtfWriteArgTag( rwc, "red", rgb8->rgb8Red );
	    docRtfWriteArgTag( rwc, "green", rgb8->rgb8Green );
	    docRtfWriteArgTag( rwc, "blue", rgb8->rgb8Blue );
	    }

	if  ( sioOutPutByte( ';', rwc->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}

	rwc->rwCol += 1;
	docRtfWriteNextLine( rwc );
	}

    docRtfWriteDestinationEnd( rwc );
    docRtfWriteNextLine( rwc );

    return 0;
    }

