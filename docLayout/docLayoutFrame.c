/************************************************************************/
/*									*/
/*  Layout functionality relating to positioned frames etc.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	<docPageGrid.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resolve X position of frame						*/
/*									*/
/************************************************************************/

# if 0

Word GUI:

    ALIGNMENT/BOOK LAYOUT:
    (Left/Centerted/Right) x (Margin/Page/Column/Character)

    Frame: \phmrg -> Margin
    Frame: \phpg -> Page
    Frame: \phcol -> Column

    Frame: \posx99 -> Absolute
    Frame: \posxl -> Left
    Frame: \posxc -> Center
    Frame: \posxr -> Right
    Frame: \posxi -> Inside
    Frame: \posxo -> Outside

    Table: \tphmrg -> Margin
    Table: \tphpg -> Page
    Table: \tphcol -> Column

    Table: \tposx99 -> Absolute
    Table: \tposxl -> Left
    Table: \tposxc -> Center
    Table: \tposxr -> Right
    Table: \tposxi -> Inside
    Table: \tposxo -> Outside

    Shape: \shpbxmargin -> Margin or \shpbxignore and posrelh=0
    Shape: \shpbxpage -> Page or \shpbxignore and posrelh=1
    Shape: \shpbxcolumn -> Column or \shpbxignore and posrelh=2
    Shape: \shpbxignore and posrelh=3 -> Character

    Shape: posh absent -> Absolute
    Shape: posh= 1 -> Left
    Shape: posh= 2 -> Center
    Shape: posh= 3 -> Right
    Shape: posh= 4 -> Inside
    Shape: posh= 5 -> Outside

    Drawing Object: \dobxpage -> Page
    Drawing Object: \dobxcolumn -> Column
    Drawing Object: \dobxmargin -> Margin

    Margin:
    -------
    Left, Margin:	Left of text box at left margin.
    Centered, Margin:	Center of text box at center of page.
    Right, Margin:	Right of text box at right margin.

    Inside, Margin:	Left of text box at left margin. (=Left *)
    Outside, Margin:	Right of text box at right margin. (=Right *)
			*) On odd pages. On Even pages, Inside and 
			   outside are swapped. Even if the document 
			   does not have the facing pages property set.

    Page:
    -------
    Left, Page:		Right of text box at left margin.
    Centered, Page:	Center of text box at center of page.
    Right, Page:	Left of text box at right margin.

    Inside, Margin:	Right of text box at left margin. (=Left *)
    Outside, Margin:	Left of text box at right margin. (=Right *)
			*) On odd pages. On Even pages, Inside and 
			   outside are swapped. Even if the document 
			   does not have the facing pages property set.

    Character:
    ----------
    The information below applies if the frame is below or above the 
    regular text. If text wraps around the frame, MS-Word makes some 
    attempts to do something similar, but the result is rather inpredictable. 
    This is not surprising: this is almost impossible to implement.
    Left, Character:	The left of the frame is at the position where
			the frame is anchored.
    Center, Character:	The center of the frame is at the position where
			the frame is anchored.
    Right, Character:	The right of the frame is at the position where
			the frame is anchored.

    ABS POS: (Margin/Page/Column/Character)
    The left of the frame is positioned relative to the positions.

# endif

void docLayoutFrameX(		BlockFrame *			bfTextFrame,
				int				xRefProp,
				int				xPosProp,
				int				xPosVal,
				int				layoutInCell,
				const ParagraphFrame *		pfRef,
				const BlockFrame *		bfRef,
				int				xChar,
				int				frameWide )
    {
    int		xRef= 0;
    int		xRel= 0;
    int		xRight= 0;

    if  ( xPosProp == FXposXI )
	{
	if  ( bfRef->bfPage % 2 )
	    { xPosProp= FXposXR;	}
	else{ xPosProp= FXposXL;	}
	}

    if  ( xPosProp == FXposXO )
	{
	if  ( bfRef->bfPage % 2 )
	    { xPosProp= FXposXL;	}
	else{ xPosProp= FXposXR;	}
	}

    switch( xRefProp )
	{
	case FXrefMARGIN:
	    xRef= bfRef->bfPageGeometry.dgLeftMarginTwips;
	    xRight= bfRef->bfPageGeometry.dgPageWideTwips-
				    bfRef->bfPageGeometry.dgRightMarginTwips;

	    switch( xPosProp )
		{
		case FXposXGIVEN:
		    xRel= xPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( xRel < -xRef )
			{ xRel=  -xRef;	}
		    break;
		case FXposXL:
		    xRel= 0;
		    break;
		case FXposXC:
		    xRef= ( xRef+ xRight )/ 2;
		    xRel= -frameWide/ 2;
		    break;
		case FXposXR:
		    xRef= xRight;
		    xRel= -frameWide;
		    break;
		case FXposXI: case FXposXO: default:
		    LDEB(xPosProp);
		    break;
		}
	    break;

	case FXrefCOLUMN:
	    if  ( layoutInCell )
		{
		xRef= pfRef->pfCellContentRect.drX0;
		xRight= pfRef->pfCellContentRect.drX1;
		}
	    else{
		xRef= bfRef->bfContentRect.drX0;
		xRight= bfRef->bfContentRect.drX1;
		}

	    switch( xPosProp )
		{
		case FXposXGIVEN:
		    xRel= xPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( xRel < -xRef )
			{ xRel=  -xRef;	}
		    break;
		case FXposXL:
		    xRel= 0;
		    break;
		case FXposXC:
		    xRef= ( xRef+ xRight )/ 2;
		    xRel= -frameWide/ 2;
		    break;
		case FXposXR:
		    xRef= xRight;
		    xRel= -frameWide;
		    break;
		case FXposXI: case FXposXO: default:
		    LDEB(xPosProp);
		    break;
		}
	    break;

	case FXrefPAGE:
	    xRef= 0;
	    xRight= bfRef->bfPageGeometry.dgPageWideTwips;

	    switch( xPosProp )
		{
		case FXposXGIVEN:
		    xRel= xPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( xRel < -xRef )
			{ xRel=  -xRef;	}
		    break;
		case FXposXL:
		    xRef= bfRef->bfPageGeometry.dgLeftMarginTwips;
		    xRel= -frameWide;
		    break;
		case FXposXC:
		    xRef= ( xRef+ xRight )/ 2;
		    xRel= -frameWide/ 2;
		    break;
		case FXposXR:
		    xRef= bfRef->bfPageGeometry.dgPageWideTwips-
				    bfRef->bfPageGeometry.dgRightMarginTwips;
		    xRel= 0;
		    break;
		case FXposXI: case FXposXO: default:
		    LDEB(xPosProp);
		    break;
		}
	    break;

	case FXrefCHARACTER:
	    xRef= xChar;
	    xRel= 0;

	    switch( xPosProp )
		{
		case FXposXGIVEN:
		    xRel= xPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( xRel < -xRef )
			{ xRel=  -xRef;	}
		    break;
		case FXposXL:
		    xRel= 0;
		    break;
		case FXposXC:
		    xRel= -frameWide/ 2;
		    break;
		case FXposXR:
		    xRel= -frameWide;
		    break;
		case FXposXI: case FXposXO: default:
		    LDEB(xPosProp);
		    break;
		}
	    break;

	default:
	    LDEB(xRefProp); return;
	}

    bfTextFrame->bfPageGeometry= bfRef->bfPageGeometry;

    bfTextFrame->bfContentRect.drX0= xRef+ xRel;
    bfTextFrame->bfContentRect.drX1= bfTextFrame->bfContentRect.drX0+ frameWide;

    bfTextFrame->bfFlowRect.drX0= bfTextFrame->bfContentRect.drX0;
    bfTextFrame->bfFlowRect.drX1= bfTextFrame->bfContentRect.drX1;
    return;
    }

# if 0

Word GUI:

    ALIGNMENT: (Top/Centerted/Bottom/Inside/Outside) x (Margin/Page/Line)

    Frame: \pvmrg -> Margin
    Frame: \pvpg -> Page
    Frame: \pvpara -> Paragraph

    Frame: \posy99 -> Absolute
    Frame: \posyt -> Top
    Frame: \posyc -> Center
    Frame: \posyb -> Bottom
    Frame: \posyin -> Inside
    Frame: \posyout -> Outside

    It looks like posyil: "Positions the paragraph vertically to be inline"
    means the same as pvpara,posyt. But it might be that the tag is simply 
    ignored. 

    Table: \tpvmrg -> Margin
    Table: \tpvpg -> Page
    Table: \tpvpara -> Paragraph

    Table: \tposy99 -> Absolute
    Table: \tposyt -> Top
    Table: \tposyc -> Center
    Table: \tposyb -> Bottom
    Table: \tposyin -> Inside
    Table: \tposyout -> Outside

    Shape: \shpbymargin -> Margin or \shpbyignore and posrelv=0
    Shape: \shpbypage -> Page or \shpbyignore and posrelv=1
    Shape: \shpbypara -> Column or \shpbyignore and posrelv=2
    Shape: \shpbyignore and posrelv=3 -> Line

    Shape: posv absent -> Absolute
    Shape: posv= 1 -> Top
    Shape: posv= 2 -> Center
    Shape: posv= 3 -> Bottom
    Shape: posv= 4 -> Inside
    Shape: posv= 5 -> Outside

    Drawing Object: \dobypage -> Page
    Drawing Object: \dobypara -> Paragraph
    Drawing Object: \dobymargin -> Margin

    Margin:
    -------
    Top, Margin:	Top of text box at top margin.
    Centerted, Margin:	Vertically centered on the page.
    Bottom, Margin:	Bottom of text box at bottom margin of the page.
    Inside, Margin:	Top of text box at top margin. (=Top)
    Outside, Margin:	Bottom of text box at bottom margin. (=Bottom)

    Page:
    -------
    Top, Page:		Top of text box at top of page.
    Centerted, Page:	Vertically centered on the page.
    Bottom, Page:	Bottom of text box at bottom of the page.
    Inside, Page:	Center of text box at header (top) position.
    Outside, Page:	Center of text box at footer (bottom) position.

    Line:
    -----
    Top, Line:		Top of text box at top of line.
    Centerted, Line:	Line at the side of the text box
			Looks like the lines in/outside box are aligned, 
			rather than that the box and the line are precisely 
			centered.
    Bottom, Line:	Bottom of text box at top of line.
    Inside, Line:	Top of text box at top of line.
    Outside, Line:	Bottom of text box at top of line.

    Paragraph:
    ----------
    NOTE that there is no option to verically align to the paragraph in 
    the MS-Word UI. Experiments were made with a manually modified 
    rtf file.

    For Old-Fashined "Positioned Objects and Frames", the paragraph is 
    the next paragraph. For 'Drawing Shapes' it is the paragraph that the 
    shape is anchored in.

    Top, Paragraph:	All vertical alignments to a paragraph place the top 
			of the text box at the top of the paragraph.
			Most probably, MS-Word switches to an absolute 
			position, relative to the paragraph top.
    Centerted, Paragraph: All vertical alignments to a paragraph etc.
    Bottom, Paragraph:	All vertical alignments to a paragraph etc.
    Inside, Paragraph:	All vertical alignments to a paragraph etc.
    Outside, Paragraph:	All vertical alignments to a paragraph etc.

    ABS POS: (Margin/Page/Paragraph/Line)
    Positions are relative to the top of the objects.

# endif

void docLayoutFrameY(		BlockFrame *			bfTextFrame,
				int				yRefProp,
				int				yPosProp,
				int				yPosVal,
				const LayoutPosition *		lpLineTop,
				const LayoutPosition *		lpParaTop,
				const BlockFrame *		bfRef,
				int				frameHighProp,
				int				frameHighVal )
    {
    /*
    const DocumentGeometry *	dg= &(bfRef->bfPageGeometry);
    */

    int				yRef= 0;
    int				yRel= 0;
    int				yBot= 0;

    switch( yRefProp )
	{
	case FYrefMARGIN:
	    yRef= bfRef->bfContentRect.drY0;
	    yBot= bfRef->bfPageGeometry.dgPageHighTwips-
				    bfRef->bfPageGeometry.dgBottomMarginTwips;
	    switch( yPosProp )
		{
		case FYposYGIVEN:
		    yRel= yPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( yRel < -yRef )
			{ yRel=  -yRef;	}
		    break;
		case FYposYT:
		    yRel= 0;
		    break;
		case FYposYC:
		    yRef= ( yRef+ yBot )/ 2;
		    yRel= -frameHighVal/ 2;
		    break;
		case FYposYB:
		    yRef= yBot;
		    yRel= -frameHighVal;
		    break;
		case FYposYIN:
		    yRel= 0;
		    break;
		case FYposYOUT:
		    yRef= yBot;
		    yRel= -frameHighVal;
		    break;
		default:
		    LDEB(yPosProp);
		    break;
		}
	    break;

	case FYrefPAGE:
	    yRef= 0;
	    yBot= bfRef->bfPageGeometry.dgPageHighTwips;

	    switch( yPosProp )
		{
		case FYposYGIVEN:
		    yRel= yPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( yRel < -yRef )
			{ yRel=  -yRef;	}
		    break;
		case FYposYT:
		    yRel= 0;
		    break;
		case FYposYC:
		    yRef= ( yRef+ yBot )/ 2;
		    yRel= -frameHighVal/ 2;
		    break;
		case FYposYB:
		    yRef= yBot;
		    yRel= -frameHighVal;
		    break;
		case FYposYIN:
		    yRef= bfRef->bfPageGeometry.dgHeaderPositionTwips;
		    yRel= -frameHighVal/ 2;
		    break;
		case FYposYOUT:
		    yRef= bfRef->bfPageGeometry.dgFooterPositionTwips;
		    yRel= -frameHighVal/ 2;
		    break;
		default:
		    LDEB(yPosProp);
		    break;
		}
	    break;

	case FYrefPARA:
	    yRef= lpParaTop->lpPageYTwips;

	    switch( yPosProp )
		{
		case FYposYGIVEN:
		    yRel= yPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( yRel < -yRef )
			{ yRel=  -yRef;	}
		    break;
		case FYposYT:
		case FYposYC:
		case FYposYB:
		case FYposYIN:
		case FYposYOUT:
		    yRel= 0;
		    break;
		default:
		    LDEB(yPosProp);
		    break;
		}
	    break;

	case FYrefLINE:
	    yRef= lpLineTop->lpPageYTwips;

	    switch( yPosProp )
		{
		case FYposYGIVEN:
		    yRel= yPosVal;
		    /* Copy MS-Word behaviour	*/
		    if  ( yRel < -yRef )
			{ yRel=  -yRef;	}
		    break;
		case FYposYT:
		    yRel= 0;
		    break;
		case FYposYC:
		    /* See above: Align baselines! */
		    yRel= -frameHighVal/ 2;
		    break;
		case FYposYB:
		    yRel= -frameHighVal;
		    break;
		case FYposYIN:
		    yRel= 0;
		    break;
		case FYposYOUT:
		    yRel= -frameHighVal;
		    break;
		default:
		    LDEB(yPosProp);
		    break;
		}
	    break;

	default:
	    LDEB(yRefProp);
	    break;
	}

    bfTextFrame->bfContentRect.drY0= yRef+ yRel;
    if  ( frameHighProp < 0 )
	{
	bfTextFrame->bfContentRect.drY1=
			    bfTextFrame->bfContentRect.drY0- frameHighProp;
	}
    else{
	/* will see later on */
	bfTextFrame->bfContentRect.drY1= bfTextFrame->bfContentRect.drY0+
					bfRef->bfPageGeometry.dgPageHighTwips;
	}

    bfTextFrame->bfFlowRect.drY0= bfTextFrame->bfContentRect.drY0;
    bfTextFrame->bfFlowRect.drY1= bfTextFrame->bfContentRect.drY1;

    return;
    }

/************************************************************************/
/*									*/
/*  Translate FrameProperties to a block frame for the text box.	*/
/*									*/
/*  1)  Should be irrelevant as text frames cannot be recursive. It	*/
/*	does no harm to set the correct value however.			*/
/*  2)  Should be irrelevant as text frames cannot be attached to a	*/
/*	character.							*/
/*  3)  Should be irrelevant as text frames cannot be bound to a table	*/
/*	column. (Shapes can!)						*/
/*									*/
/************************************************************************/

void docLayoutSetTextFrame(	BlockFrame *			bfTextFrame,
				const LayoutPosition *		lpRef,
				const BlockFrame *		bfRef,
				const FrameProperties *		fp,
				int				frameHighVal )
    {
    /*  2  */
    const int				xChar= 0;
    /*  3  */
    const int				layoutInCell= 0;
    const ParagraphFrame *		pfRef= (const ParagraphFrame *)0;

    /*  1  */
    bfTextFrame->bfPage= bfRef->bfPage;
    bfTextFrame->bfColumn= bfRef->bfColumn; /* ignored */

    /*  X  */
    docLayoutFrameX( bfTextFrame,
			fp->fpXReference, fp->fpXPosition, fp->fpXTwips,
			layoutInCell, pfRef, bfRef, xChar, fp->fpWideTwips );

    /*  Y  */
    docLayoutFrameY( bfTextFrame,
			    fp->fpYReference, fp->fpYPosition, fp->fpYTwips,
			    lpRef, lpRef, bfRef,
			    fp->fpHighTwips, frameHighVal );

    return;
    }

