/************************************************************************/
/*									*/
/*  Geometry and extra descriptive information on selections.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<appDebugon.h>

#   include	"docSelectionGeometry.h"

/************************************************************************/
/*									*/
/*  Initialise Position/Selection.					*/
/*									*/
/************************************************************************/

void docInitPositionGeometry(	PositionGeometry *		pg )
    {
    pg->pgLine= 0;
    pg->pgPositionFlags= 0;

    pg->pgXTwips= -1;
    pg->pgXPixels= -1;
    docInitLayoutPosition( &pg->pgTopPosition );
    docInitLayoutPosition( &pg->pgBasePosition );
    docInitLayoutPosition( &pg->pgBottomPosition );

    pg->pgParaFrameX0= 0;
    pg->pgParaFrameX1= 0;

    pg->pgBlockFrameX0= 0;
    pg->pgBlockFrameX1= 0;

    return;
    }

void docInitSelectionGeometry(	SelectionGeometry *		sg )
    {
    geoInitRectangle( &(sg->sgRectangle) );

    docInitPositionGeometry( &(sg->sgHead) );
    docInitPositionGeometry( &(sg->sgTail) );
    docInitPositionGeometry( &(sg->sgAnchor) );
    }

