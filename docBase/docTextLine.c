/************************************************************************/
/*									*/
/*  Manage lines in a paragraph.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docTextLine.h"

/************************************************************************/
/*									*/
/*  Set a text line to sensible defaults.				*/
/*									*/
/************************************************************************/

void docInitTextLine(		TextLine *	tl )
    {
    tl->tlStroff= 0;
    tl->tlFirstParticule= 0;
    tl->tlStrlen= 0;
    tl->tlParticuleCount= 0;
    tl->tlWordCount= 0;

    docInitLayoutPosition( &(tl->tlTopPosition) );

    tl->tlAscY0= 0;
    tl->tlDescY1= 0;
    tl->tlLineStride= 0;

    tl->tlFlags= 0;

    tl->tlFlowWidthTwips= 0;
    tl->tlLineIndent= 0;

    return;
    }

