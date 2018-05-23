/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<limits.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedLayout.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<docPageGrid.h>
#   include	<geoGrid.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<docParaParticules.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Drag routine for inserted objects.					*/
/*									*/
/************************************************************************/

typedef struct TedObjectDrag
    {
    EditDocument *	todEd;
    LayoutContext	todLayoutContext;

    InsertedObject *	todIo;
    DocumentPosition	todDp;
    PositionGeometry	todPg;
    int			todParticule;

    int			todMouseX0;
    int			todMouseX1;
    int			todMouseY0;
    int			todMouseY1;

    int			todMouseX;
    int			todMouseY;
    int			todCorner;
    } TedObjectDrag;

/************************************************************************/
/*									*/
/*  Handle mouse moves while an object or image is resized with the	*/
/*  mouse. The final mouse up event is also handeled here.		*/
/*									*/
/*  1)  Obtain the rectangle before the object was resized.		*/
/*  2)  Horizontal resize allowed.. Remember new X coordinate.		*/
/*  3)  Vertical resize allowed.. Remember new Y coordinate.		*/
/*  4)  If any coordinate changed.. Obtain new object rectangle and	*/
/*	cause the union ogf the old and new rectangles to be redrawn.	*/
/*  5)  Remember new mouse position.					*/
/*									*/
/************************************************************************/

static void tedObjectHandleMove(	TedObjectDrag *		tod,
					int			mouseX,
					int			mouseY )
    {
    EditDocument *		ed= tod->todEd;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				moved= 0;

    DocumentRectangle		drFrom;

    int				mouseXc= mouseX;
    int				mouseYc= mouseY;

    /*  1  */
    tedGetObjectRectangle( &drFrom, (Point2DI *)0,
					tod->todIo, &(tod->todPg),
					&(tod->todLayoutContext), ed );

    if  ( mouseXc < tod->todMouseX0- ox )
	{ mouseXc=  tod->todMouseX0- ox;	}
    if  ( mouseXc > tod->todMouseX1- ox )
	{ mouseXc=  tod->todMouseX1- ox;	}

    if  ( mouseYc < tod->todMouseY0- oy )
	{ mouseYc=  tod->todMouseY0- oy;	}
    if  ( mouseYc > tod->todMouseY1- oy )
	{ mouseYc=  tod->todMouseY1- oy;	}

    /*  2  */
    switch( tod->todCorner )
	{
	case RESIZE_BOTTOM_LEFT:
	case RESIZE_MIDDLE_LEFT:
	case RESIZE_TOP_LEFT:

	case RESIZE_BOTTOM_RIGHT:
	case RESIZE_MIDDLE_RIGHT:
	case RESIZE_TOP_RIGHT:
	    if  ( tod->todMouseX != mouseXc )
		{
		td->tdObjectCornerMovedX += mouseXc- tod->todMouseX;
		td->tdScaleChangedX= 1;
		moved= 1;
		}
	    break;
	}

    /*  3  */
    switch( tod->todCorner )
	{
	case RESIZE_TOP_LEFT:
	case RESIZE_TOP_MIDDLE:
	case RESIZE_TOP_RIGHT:

	case RESIZE_BOTTOM_LEFT:
	case RESIZE_BOTTOM_MIDDLE:
	case RESIZE_BOTTOM_RIGHT:
	    if  ( tod->todMouseY != mouseYc )
		{
		td->tdObjectCornerMovedY += mouseYc- tod->todMouseY;
		td->tdScaleChangedY= 1;
		moved= 1;
		}
	    break;
	}

    /*  4  */
    if  ( moved )
	{
	DocumentRectangle	drTo;
	DocumentRectangle	drExp;

	tedGetObjectRectangle( &drTo, (Point2DI *)0,
					tod->todIo, &(tod->todPg),
					&(tod->todLayoutContext), ed );

	geoUnionRectangle( &drExp, &drFrom, &drTo );
	geoShiftRectangle( &drExp, -ox, -oy );

	if  ( drExp.drX0 < 0 )
	    { drExp.drX0=  0;	}
	if  ( drExp.drY0 < 0 )
	    { drExp.drY0=  0;	}

	if  ( drExp.drX1 >= drExp.drX0 && drExp.drY1 >= drExp.drY0 )
	    {
	    guiExposeDrawingWidgetRectangle( ed->edDocumentWidget.dwWidget,
								    &drExp );
	    }
	}

    /*  5  */
    tod->todMouseX= mouseXc;
    tod->todMouseY= mouseYc;
    }

static APP_EVENT_HANDLER_H( tedObjectDragMouseMove, w, vtod, event )
    {
    TedObjectDrag *		tod= (TedObjectDrag *)vtod;

    int				x;
    int				y;

    if  ( guiGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    tedObjectHandleMove( tod, x, y );

    return;
    }

static APP_EVENT_HANDLER_H( tedObjectDragMouseUp, w, vtod, event )
    {
    TedObjectDrag *		tod= (TedObjectDrag *)vtod;

    int				x;
    int				y;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent( &x, &y,
			    &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    tedObjectHandleMove( tod, x, y );

    return;
    }

/************************************************************************/
/*									*/
/*  The user has possibly clicked one of the resize handles on an	*/
/*  inserted object or an image.					*/
/*									*/
/*  1)  Get the mouse coordinates of the click.				*/
/*  2)  Check that we have an object selection indeed.			*/
/*  3)  Do geometry calculations to check whether this was on the	*/
/*	object, and on one of the resize handles.			*/
/*  4)  A double click, a mouse up or a position outside an object	*/
/*	cannot start an object resize.					*/
/*  5)  Find the handle that was clicked.				*/
/*  6)  Not on a handle.. No resize drag.				*/
/*  7)  Was the mouse moved while it was down?				*/
/*  8)  To a different position? .. Recalculate scale.			*/
/*  9)  To the same position with a squeezed image? Use the squeezed	*/
/*	scale as the real scale.					*/
/* 10)  If the physical size has changed.. Recalculate layout and	*/
/*	redraw.								*/
/* 11)  Remember that the document was changed.				*/
/* 12)  Finish.								*/
/*									*/
/************************************************************************/

int tedObjectDrag(		APP_WIDGET	w,
				EditDocument *	ed,
				APP_EVENT *	downEvent )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    int				i;

    TedObjectDrag		tod;

    DocumentRectangle		drObj;
    Point2DI			xp[RESIZE_COUNT];

    tod.todEd= ed;
    layoutInitContext( &(tod.todLayoutContext) );
    tedSetScreenLayoutContext( &(tod.todLayoutContext), ed );
    tod.todCorner= -1;

    /*  1  */
    if  ( guiGetCoordinatesFromMouseButtonEvent(
				    &(tod.todMouseX), &(tod.todMouseY),
				    &button, &upDown, &seq, &keyState,
				    w, downEvent ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    docInitDocumentPosition( &(tod.todDp) );
    if  ( tedGetObjectSelection( ed,
			&(tod.todParticule), &(tod.todDp), &(tod.todIo) ) )
	{ return 2;	}

    /*  3  */
    tedPositionGeometry( &(tod.todPg), &(tod.todDp),
					PARAfindLAST, &(tod.todLayoutContext) );

    tedGetObjectRectangle( &drObj, xp, tod.todIo, &(tod.todPg),
					    &(tod.todLayoutContext), ed );

    /*  4  */
    if  ( seq > 1					||
	  upDown < 1					||
	  tod.todMouseX+ ox < drObj.drX0		||
	  tod.todMouseX+ ox > drObj.drX1		||
	  tod.todMouseY+ oy < drObj.drY0		||
	  tod.todMouseY+ oy > drObj.drY1		)
	{ return 2;	}

    /*  5  */
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	if  ( tod.todMouseX < xp[i].x- ox )
	    { continue;	}
	if  ( tod.todMouseY < xp[i].y- oy )
	    { continue;	}
	if  ( tod.todMouseX > xp[i].x- ox+ RESIZE_BLOCK )
	    { continue;	}
	if  ( tod.todMouseY > xp[i].y- oy+ RESIZE_BLOCK )
	    { continue;	}

	break;
	}

    /*  6  */
    if  ( i >= RESIZE_COUNT )
	{ return 1;	}

    tod.todCorner= i;
    td->tdObjectResizeCorner= i;
    td->tdObjectCornerMovedX= 0;
    td->tdObjectCornerMovedY= 0;
    td->tdScaleChangedX= 0;
    td->tdScaleChangedY= 0;

    {
    BlockFrame		bf;
    BufferItem *	paraBi= tod.todDp.dpNode;
    int			frameWide;
    int			frameHigh;

    docBlockFrameTwips( &bf, paraBi, bd, paraBi->biTopPosition.lpPage,
					    paraBi->biTopPosition.lpColumn );

    frameWide= bf.bfContentRect.drX1- bf.bfContentRect.drX0;
    frameHigh= bf.bfContentRect.drY1- bf.bfContentRect.drY0;

    frameWide= COORDtoGRID( td->tdPixelsPerTwip, frameWide );
    frameHigh= COORDtoGRID( td->tdPixelsPerTwip, frameHigh );

    switch( tod.todCorner )
	{
	case RESIZE_BOTTOM_LEFT:
	case RESIZE_MIDDLE_LEFT:
	case RESIZE_TOP_LEFT:
	    tod.todMouseX0= drObj.drX1- frameWide;
	    tod.todMouseX1= drObj.drX1;
	    break;

	case RESIZE_BOTTOM_RIGHT:
	case RESIZE_MIDDLE_RIGHT:
	case RESIZE_TOP_RIGHT:
	    tod.todMouseX0= drObj.drX0;
	    tod.todMouseX1= drObj.drX0+ frameWide;
	    break;

	default:
	    tod.todMouseX0= INT_MIN;
	    tod.todMouseX1= INT_MAX;
	    break;
	}

    switch( tod.todCorner )
	{
	case RESIZE_TOP_LEFT:
	case RESIZE_TOP_MIDDLE:
	case RESIZE_TOP_RIGHT:
	    tod.todMouseY0= drObj.drY1- frameHigh;
	    tod.todMouseY1= drObj.drY1;
	    break;

	case RESIZE_BOTTOM_LEFT:
	case RESIZE_BOTTOM_MIDDLE:
	case RESIZE_BOTTOM_RIGHT:
	    tod.todMouseY0= drObj.drY0;
	    tod.todMouseY1= drObj.drY0+ frameHigh;
	    break;

	default:
	    tod.todMouseY0= INT_MIN;
	    tod.todMouseY1= INT_MAX;
	    break;
	}
    }

    appRunDragLoop( w, ea, downEvent,
				tedObjectDragMouseUp,
				tedObjectDragMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&tod );

    /*  7  */
    if  ( td->tdScaleChangedX != 0	||
	  td->tdScaleChangedY != 0	)
	{
	double			xfac= td->tdPixelsPerTwip;
	DocumentRectangle	drResized;
	InsertedObject *	io= tod.todIo;

	PropertyMask		pipSetMask;
	PictureProperties	pipFrom= io->ioPictureProperties;

	utilPropMaskClear( &pipSetMask );

	tedGetObjectRectangle( &drResized, (Point2DI *)0, io, &(tod.todPg),
						&(tod.todLayoutContext), ed );

	/*  8  */
	if  ( td->tdObjectCornerMovedX != 0 )
	    {
	    io->ioScaleXSet= ( 100.0* ( drResized.drX1- drResized.drX0+ 1 ) )/
				    COORDtoGRID( xfac, io->ioTwipsWide );
	    if  ( io->ioScaleXSet < 1 )
		{ io->ioScaleXSet=  1;	}
	    io->ioScaleXUsed= io->ioScaleXSet;

	    pipFrom.pipScaleXSet= io->ioScaleXSet;
	    PROPmaskADD( &pipSetMask, PIPpropPICSCALE_X );
	    }

	/*  9  */
	if  ( td->tdScaleChangedX			&&
	      io->ioScaleXSet != io->ioScaleXUsed	)
	    {
	    io->ioScaleXSet= io->ioScaleXUsed;
	    pipFrom.pipScaleXSet= io->ioScaleXSet;
	    PROPmaskADD( &pipSetMask, PIPpropPICSCALE_X );
	    }

	/*  8  */
	if  ( td->tdObjectCornerMovedY != 0 )
	    {
	    io->ioScaleYSet= ( 100.0* ( drResized.drY1- drResized.drY0+ 1 ) )/
				    COORDtoGRID( xfac, io->ioTwipsHigh );
	    if  ( io->ioScaleYSet < 1 )
		{ io->ioScaleYSet=  1;	}
	    io->ioScaleYUsed= io->ioScaleYSet;

	    pipFrom.pipScaleYSet= io->ioScaleYSet;
	    PROPmaskADD( &pipSetMask, PIPpropPICSCALE_Y );
	    }

	/*  9  */
	if  ( td->tdScaleChangedY			&&
	      io->ioScaleYSet != io->ioScaleYUsed	)
	    {
	    io->ioScaleYSet= io->ioScaleYUsed;
	    pipFrom.pipScaleYSet= io->ioScaleYSet;
	    PROPmaskADD( &pipSetMask, PIPpropPICSCALE_Y );
	    }

	/*  10  */
	if  ( td->tdObjectCornerMovedX != 0	||
	      td->tdObjectCornerMovedY != 0	)
	    {
	    DocumentRectangle	drExp= drResized;

	    geoShiftRectangle( &drExp, -ox, -oy );
	    if  ( drExp.drX0 < 0 )
		{ drExp.drX0=  0;	}
	    if  ( drExp.drY0 < 0 )
		{ drExp.drY0=  0;	}

	    if  ( drExp.drX1 >= drExp.drX0 && drExp.drY1 >= drExp.drY0 )
		{
		guiExposeDrawingWidgetRectangle( ed->edDocumentWidget.dwWidget,
								    &drExp );
		}

	    tedObjectSetImageProperties( ed, io, &(tod.todDp), tod.todParticule,
						&pipSetMask, &pipFrom, ((TedDocument *)ed->edPrivateData)->tdTraced );

	    tedPositionGeometry( &(tod.todPg), &(tod.todDp),
					PARAfindLAST, &(tod.todLayoutContext) );
	    }
	}

    /*  12  */
    tod.todCorner= -1;
    td->tdObjectResizeCorner= -1;
    td->tdObjectCornerMovedX= 0;
    td->tdObjectCornerMovedY= 0;
    td->tdScaleChangedX= 0;
    td->tdScaleChangedY= 0;

    tedSetObjectWindows( ed, &(tod.todPg), tod.todIo, &(tod.todLayoutContext) );

    return 0;
    }

