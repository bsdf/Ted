/************************************************************************/
/*									*/
/*  Read/Write MS-Word 95 style drawing objects from/to rtf.		*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<bitmap.h>

#   include	<docDrawingObject.h>
#   include	<docShape.h>
#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"

static int docRtfDrawingObjectAllocatePoints(	DrawingShape *	ds,
						int		n )
    {
    ShapeDrawing *	sd= &(ds->dsDrawing);
    Point2DI *		fresh;

    fresh= (Point2DI *)realloc( sd->sdVertices, n* sizeof(Point2DI) );
    if  ( ! fresh )
	{ LXDEB(n,fresh); return -1;	}

    sd->sdVertices= fresh;
    fresh += sd->sdVertexCount;

    while( sd->sdVertexCount < n )
	{
	fresh->x= fresh->y= 0;
	fresh++; sd->sdVertexCount++;
	}

    return 0;
    }

int docRtfDrawingObjectProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrDrawingShape;
    ShapeDrawing *	sd;
    ShapeProperties *	sp;

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rrc->rrCurrentLine,ds); return 0;	}

    sd= &(ds->dsDrawing);
    sp= &(ds->dsShapeProperties);

    switch( rcw->rcwID )
	{
	case DOpropANCHOR_LOCKED:
	    sp->spLockAnchor= arg != 0;
	    break;

	case DOpropX_ATTACH:
	    sd->sdXReference= arg;
	    sp->spXReference= arg;
	    break;

	case DOpropY_ATTACH:
	    sd->sdYReference= arg;
	    sp->spYReference= arg;
	    break;

	case DOpropKIND:
	    sd->sdShapeType= arg;
	    sd->sd_fFilled= 0; /* Have a different default from shapes ? */

	    if  ( sd->sdShapeType == SHPtyLINE )
		{
		rrc->rrcNextObjectVertex= sd->sdVertexCount;
		if  ( docRtfDrawingObjectAllocatePoints( ds, 2 ) )
		    { LDEB(arg); return -1;	}
		}
	    break;

	case DOpropLINE_STYLE:
	    if  ( arg == DSdashHOLLOW )
		{
		sd->sdLineDashing= DSdashSOLID;
		sd->sd_fLine= 0;
		}
	    else{
		sd->sdLineDashing= arg;
		sd->sd_fLine= 1;
		}
	    break;

	case DOpropFILL_PATTERN:
	    switch( arg )
		{
		static int op[]= { 100,5,10,20,30,40,50,60,70,75,80,90 };

		case 0:
		    sd->sd_fFilled= 0;
		    sd->sdFillType= DSfillSOLID;
		    break;
		case  1: case  2: case  3: case  4: case  5: case  6: case  7:
		case  8: case  9: case 10: case 11: case 12: case 13:
		    sd->sd_fFilled= 1;
		    sd->sdFillType= DSfillSOLID;
		    sd->sdFillOpacity= ( op[arg-1]* 65536 )/ 100;
		    break;

		default:
		    LDEB(arg); break;
		}
	    break;

	case DOpropARC_FLIP_X:
	    sd->sd_fFlipH= arg != 0;
	    sd->sd_fRelFlipH= arg != 0;
	    break;
	case DOpropARC_FLIP_Y:
	    sd->sd_fFlipV= arg != 0;
	    sd->sd_fRelFlipV= arg != 0;
	    break;

	case DOpropX:
	    sp->spRect.drX0= arg;
	    sd->sdGeoRect.drX0= arg;
	    break;
	case DOpropY:
	    sp->spRect.drY0= arg;
	    sd->sdGeoRect.drY0= arg;
	    break;
	case DOpropZ:
	    sp->spZ= arg;
	    break;
	case DOpropWIDE:
	    sp->spRect.drX1= sp->spRect.drX0+ arg;
	    sd->sdGeoRect.drX1= sd->sdGeoRect.drX0+ arg;
	    break;
	case DOpropHIGH:
	    sp->spRect.drY1= sp->spRect.drY0+ arg;
	    sd->sdGeoRect.drY1= sd->sdGeoRect.drY0+ arg;
	    break;

	case DOpropTEXT_BOX_MARGIN:
	    sd->sd_dxTextLeft= TWIPStoEMU( arg );
	    sd->sd_dyTextTop= TWIPStoEMU( arg );
	    sd->sd_dxTextRight= TWIPStoEMU( arg );
	    sd->sd_dyTextBottom= TWIPStoEMU( arg );
	    break;

	case DOpropLINE_WIDTH:
	    sd->sdLineWidthEmu= TWIPStoEMU( arg );
	    break;

	case DOpropPOINT_COUNT:
	    rrc->rrcNextObjectVertex= sd->sdVertexCount;
	    if  ( docRtfDrawingObjectAllocatePoints( ds, arg ) )
		{ LDEB(arg); return -1;	}
	    break;

	case DOpropSTART_ARROW_HEAD:
	    sd->sdLineHeadArrow.saArrowHead= arg;
	    break;
	case DOpropEND_ARROW_HEAD:
	    sd->sdLineTailArrow.saArrowHead= arg;
	    break;
	case DOpropSTART_ARROW_WIDTH:
	    sd->sdLineHeadArrow.saArrowWidth= arg;
	    break;
	case DOpropEND_ARROW_WIDTH:
	    sd->sdLineTailArrow.saArrowWidth= arg;
	    break;
	case DOpropSTART_ARROW_LENGTH:
	    sd->sdLineHeadArrow.saArrowLength= arg;
	    break;
	case DOpropEND_ARROW_LENGTH:
	    sd->sdLineTailArrow.saArrowLength= arg;
	    break;

	case DOpropLINE_RED:
	    sd->sdLineColor.rgb8Red= arg;
	    break;
	case DOpropLINE_GREEN:
	    sd->sdLineColor.rgb8Green= arg;
	    break;
	case DOpropLINE_BLUE:
	    sd->sdLineColor.rgb8Blue= arg;
	    break;
	case DOpropLINE_GRAY:
	    sd->sdLineColor.rgb8Red= arg;
	    sd->sdLineColor.rgb8Green= arg;
	    sd->sdLineColor.rgb8Blue= arg;
	    break;

			/*  Note swap of fore and back.		*/
	case DOpropFILL_FORE_RED:
	    sd->sdFillBackColor.rgb8Red= arg;
	    break;
	case DOpropFILL_FORE_GREEN:
	    sd->sdFillBackColor.rgb8Green= arg;
	    break;
	case DOpropFILL_FORE_BLUE:
	    sd->sdFillBackColor.rgb8Blue= arg;
	    break;
	case DOpropFILL_FORE_GRAY:
	    sd->sdFillBackColor.rgb8Red= arg;
	    sd->sdFillBackColor.rgb8Green= arg;
	    sd->sdFillBackColor.rgb8Blue= arg;
	    break;

			/*  Note swap of fore and back.		*/
	case DOpropFILL_BACK_RED:
	    sd->sdFillColor.rgb8Red= arg;
	    break;
	case DOpropFILL_BACK_GREEN:
	    sd->sdFillColor.rgb8Green= arg;
	    break;
	case DOpropFILL_BACK_BLUE:
	    sd->sdFillColor.rgb8Blue= arg;
	    break;
	case DOpropFILL_BACK_GRAY:
	    sd->sdFillColor.rgb8Red= arg;
	    sd->sdFillColor.rgb8Green= arg;
	    sd->sdFillColor.rgb8Blue= arg;
	    break;

	case DOpropTEXT_FLOW:
	    /* SLDEB(rcw->rcwWord,rcw->rcwID); */
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

int docRtfDrawingObjectCoordinate(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    DrawingShape *	ds= rrc->rrDrawingShape;
    ShapeDrawing *	sd;

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rrc->rrCurrentLine,ds); return 0;	}

    sd= &(ds->dsDrawing);

    if  ( rrc->rrcNextObjectVertex >= sd->sdVertexCount	||
	  rrc->rrcNextObjectVertex < 0			)
	{
	LLLDEB(sd->sdShapeType,rrc->rrcNextObjectVertex,sd->sdVertexCount);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case DOpropX:
	    sd->sdVertices[rrc->rrcNextObjectVertex  ].x= arg;
	    break;
	case DOpropY:
	    sd->sdVertices[rrc->rrcNextObjectVertex++].y= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

