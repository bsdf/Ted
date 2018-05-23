/************************************************************************/
/*									*/
/*  The Frame page of the Format Tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appUnit.h>

#   include	"tedFrameTool.h"
#   include	"tedAppFront.h"
#   include	"tedToolUtil.h"
#   include	"tedDocFront.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the frame page.						*/
/*									*/
/************************************************************************/

static void tedFormatRefreshXPosText(	FrameTool *	ft )
    {
    const FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( fp->fpXPosition == FXposXGIVEN )
	{
	guiEnableText( ft->ftXText, 1 );
	appLengthToTextWidget( ft->ftXText,  fp->fpXTwips, ft->ftUnit );
	}
    else{
	guiEnableText( ft->ftXText, 0 );
	appStringToTextWidget( ft->ftXText, "" );
	}

    return;
    }

static void tedFormatRefreshYPosText(	FrameTool *	ft )
    {
    const FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( fp->fpYPosition == FYposYGIVEN )
	{
	guiEnableText( ft->ftYText, 1 );
	appLengthToTextWidget( ft->ftYText,  fp->fpYTwips, ft->ftUnit );
	}
    else{
	guiEnableText( ft->ftYText, 0 );
	appStringToTextWidget( ft->ftYText, "" );
	}

    return;
    }

static void tedFormatToolRefreshFramePage( FrameTool *	ft )
    {
    const FrameProperties *	fp= &(ft->ftPropertiesChosen);

    /*  X  */
    appLengthToTextWidget( ft->ftWidthText, fp->fpWideTwips, ft->ftUnit );

    tedFormatRefreshXPosText( ft );

    appSetOptionmenu( &(ft->ftXRefMenu), fp->fpXReference );
    appSetOptionmenu( &(ft->ftXPosMenu), fp->fpXPosition );

    /*  Y  */
    tedFormatEnableHeightChooser( &(ft->ftHeightChooser),
						    ft->ftCanChange );
    tedFormatRefreshHeightChooser( &(ft->ftHeightChooser),
						    fp->fpHighTwips );

    tedFormatRefreshYPosText( ft );

    appSetOptionmenu( &(ft->ftYRefMenu), fp->fpYReference );
    appSetOptionmenu( &(ft->ftYPosMenu), fp->fpYPosition );
    }

void tedRefreshFrameTool(	FrameTool *			ft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd )
    {
    const BufferItem *		paraBi;

    paraBi= ds->dsHead.dpNode;

if(1)
{ *pEnabled= 0; return;	}

    if  ( ! paraBi || ! sd->sdInContiguousParagraphs )
	{ *pEnabled= 0; return;	}

    docGetFramePropertiesByNumber( &(ft->ftPropertiesSet),
					    bd, paraBi->biParaFrameNumber );

    if  ( DOCisFRAME( &(ft->ftPropertiesSet) ) )
	{ *pPref += 2;	}

    docCopyFrameProperties( &(ft->ftPropertiesChosen),
						&(ft->ftPropertiesSet) );

    tedFormatToolRefreshFramePage( ft );

    guiEnableWidget( is->isSelectButton, sd->sdIsSingleFrame );
    guiEnableWidget( is->isDeleteButton, sd->sdIsSingleFrame );

    *pEnabled= ! sd->sdDocumentReadonly;
    return;
    }

/************************************************************************/

static int tedFrameToolGetChosen(	FrameTool *		ft )
    {
    FrameProperties *		fpChosen= &(ft->ftPropertiesChosen);

    int				changed;

    const int			minPos= INT_MIN;
    const int			minWidth= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( tedFormatToolGetHeight( &(fpChosen->fpHighTwips),
						&(ft->ftHeightChooser) ) )
	{ return -1;	}

    if  ( appGetLengthFromTextWidget( ft->ftWidthText,
					    &(fpChosen->fpWideTwips), &changed,
					    UNITtyPOINTS,
					    minWidth, adaptToMin,
					    maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( fpChosen->fpXPosition == FXposXGIVEN )
	{
	if  ( appGetLengthFromTextWidget( ft->ftXText,
					    &(fpChosen->fpXTwips), &changed,
					    UNITtyPOINTS,
					    minPos, adaptToMin,
					    maxValue, adaptToMax ) )
	    { return -1;	}
	}

    if  ( fpChosen->fpYPosition == FYposYGIVEN )
	{
	if  ( appGetLengthFromTextWidget( ft->ftYText,
					    &(fpChosen->fpYTwips), &changed,
					    UNITtyPOINTS,
					    minPos, adaptToMin,
					    maxValue, adaptToMax ) )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the frame tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeFramePushed, w, voidft )
    {
    FrameTool *			ft= (FrameTool *)voidft;
    const FrameProperties *	fpSet= &(ft->ftPropertiesSet);
    const FrameProperties *	fpChosen= &(ft->ftPropertiesChosen);
    int				frameNumberSet;

    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, ft->ftApplication );

    ParagraphProperties		ppChosen;
    PropertyMask		ppSetMask;

    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    docInitParagraphProperties( &ppChosen );

    if  ( tedFrameToolGetChosen( ft ) )
	{ goto ready;	}

    ppChosen.ppFrameNumber= docFramePropertiesNumber( bd, fpChosen );
    if  ( ppChosen.ppFrameNumber < 0 )
	{ LDEB(ppChosen.ppFrameNumber); goto ready;	}
    frameNumberSet= docFramePropertiesNumber( bd, fpSet );
    if  ( frameNumberSet < 0 )
	{ LDEB(frameNumberSet); goto ready;	}

    utilPropMaskClear( &ppSetMask );
    if  ( ppChosen.ppFrameNumber != frameNumberSet )
	{ PROPmaskADD( &ppSetMask, PPpropFRAME );	}

    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, &ppChosen, traced ) )
	{ LDEB(1); goto ready;	}

  ready:

    docCleanParagraphProperties( &ppChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  The frame height/width changed, check it.				*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedFrameHeightChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    int			value;

    if  ( ! tedFormatToolGetHeight( &value, &(ft->ftHeightChooser) ) )
	{
	tedFormatRefreshHeightChooser( &(ft->ftHeightChooser), value );
	}
    }

static APP_TXACTIVATE_CALLBACK_H( tedFrameWidthChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
				    ft->ftWidthText, fp->fpWideTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedFrameXChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
				    ft->ftXText, fp->fpXTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedFrameYChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
				    ft->ftYText, fp->fpYTwips );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the frame tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertFramePushed, w, voidft )
    {
    FrameTool *	ft= (FrameTool *)voidft;

    docCopyFrameProperties( &(ft->ftPropertiesChosen), &(ft->ftPropertiesSet) );

    tedFormatToolRefreshFramePage( ft );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectFrame, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    EditApplication *	ea= ft->ftApplication;

    tedAppSelectCurrentFrame( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Delete' button of the frame tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatDeleteFrame, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    EditApplication *	ea= ft->ftApplication;

    tedAppDeleteCurrentFrame( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Row buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatSetFrame, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    EditApplication *	ea= ft->ftApplication;

    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    LDEB(1);
    }

static APP_BUTTON_CALLBACK_H( tedFormatUnsetFrame, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    EditApplication *	ea= ft->ftApplication;

    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    LDEB(1);
    }

static void tedFrameHeightChosen(	int		how,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    HeightChooser *	hc= &(ft->ftHeightChooser);

    int			defaultValue;

    defaultValue= ft->ftPropertiesChosen.fpHighTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= ft->ftPropertiesSet.fpHighTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

static void tedFrameXRefChosen(		int		ref,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( ref < 0 || ref >= FXref__FRAME_COUNT )
	{ LLDEB(ref,FXref__FRAME_COUNT); return;	}

    fp->fpXReference= ref;

    return;
    }

static void tedFrameXPosChosen(		int		pos,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( pos < 0 || pos >= FXpos_COUNT )
	{ LLDEB(pos,FXpos_COUNT); return;	}

    fp->fpXPosition= pos;
    tedFormatRefreshXPosText( ft );

    return;
    }

/************************************************************************/
/*									*/
/*  Combinations of YRef/Ypos:						*/
/*									*/
/************************************************************************/

static void tedFrameYRefChosen(		int		ref,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( ref < 0 || ref >= FYref__FRAME_COUNT )
	{ LLDEB(ref,FYref__FRAME_COUNT); return;	}

    fp->fpYReference= ref;

    if  ( ! docFrameYAllowed[fp->fpYPosition][fp->fpYReference] )
	{
	int	pos;

	for ( pos= 0; pos < FYpos_COUNT; pos++ )
	    {
	    if  ( docFrameYAllowed[pos][ref] )
		{
		fp->fpYPosition= pos;
		appSetOptionmenu( &(ft->ftYPosMenu), fp->fpYPosition );
		tedFormatRefreshYPosText( ft );
		}
	    }
	}

    return;
    }

static void tedFrameYPosChosen(		int		pos,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( pos < 0 || pos >= FYpos_COUNT )
	{ LLDEB(pos,FYpos_COUNT); return;	}

    fp->fpYPosition= pos;
    tedFormatRefreshYPosText( ft );

    if  ( ! docFrameYAllowed[fp->fpYPosition][fp->fpYReference] )
	{
	int		ref;

	for ( ref= 0; ref < FYref__FRAME_COUNT; ref++ )
	    {
	    if  ( docFrameYAllowed[pos][ref] )
		{
		fp->fpYReference= ref;
		appSetOptionmenu( &(ft->ftYRefMenu), fp->fpYReference );
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the controls to select a frame position.			*/
/*									*/
/************************************************************************/

static void tedFillFramePositionTool(
				APP_WIDGET *			pRow,
				APP_WIDGET *			pText,
				void *				through,
				APP_WIDGET			parent,
				AppOptionmenu *			aom,
				APP_TXACTIVATE_CALLBACK_T	textCallback,
				OptionmenuCallback		menuCallback )
    {
    APP_WIDGET	row;
    APP_WIDGET	text;

    const int	menuColumn= 0;
    const int	menuColspan= 1;
    const int	textColumn= menuColumn+ menuColspan;
    const int	textColspan= 1;
    const int	heightResizable= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeOptionmenuInRow( aom, row, menuColumn, menuColspan,
						    menuCallback, through );

    appMakeTextInRow( &text, row, textColumn, textColspan, 10, 0 );

    if  ( textCallback )
	{ appGuiSetGotValueCallbackForText( text, textCallback, through ); }

    *pRow= row;
    *pText= text;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame that controls the horizontal position and size.	*/
/*									*/
/************************************************************************/

static void tedFrameToolMakeHorizontalGroup(
			    FrameTool *				ft,
			    const FramePageResources *		fpr,
			    APP_WIDGET				pageWidget )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    const int	textColumns= 9;

    appMakeColumnFrameInColumn( &(ft->ftHorizontalFrame),
				&(ft->ftHorizontalPaned),
				pageWidget, fpr->fprHorizontal );

    guiToolMakeLabelAndTextRow( &row, &(ft->ftWidthLabel), &(ft->ftWidthText),
				ft->ftHorizontalPaned,
				fpr->fprWidth, textColumns, 1 );
    appGuiSetGotValueCallbackForText( ft->ftWidthText,
				tedFrameWidthChanged, (void *)ft );


    tedFillFramePositionTool( &row, &(ft->ftXText), (void *)ft,
				ft->ftHorizontalPaned,
				&(ft->ftXPosMenu),
				tedFrameXChanged,
				tedFrameXPosChosen );

    guiToolMakeLabelAndMenuRow( &row, &(ft->ftXRefMenu),
				&label, ft->ftHorizontalPaned,
				fpr->fprXReference,
				tedFrameXRefChosen, (void *)ft );
    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame that controls the fertical position and size.	*/
/*									*/
/************************************************************************/

static void tedFrameToolMakeVerticalGroup(
			    FrameTool *				ft,
			    const FramePageResources *		fpr,
			    APP_WIDGET				pageWidget )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    appMakeColumnFrameInColumn( &(ft->ftVerticalFrame),
				&(ft->ftVerticalPaned),
				pageWidget, fpr->fprVertical );

    tedFormatMakeHeightRow( ft, ft->ftVerticalPaned,
				&(ft->ftHeightChooser),
				tedFrameHeightChanged,
				tedFrameHeightChosen );

    tedFillFramePositionTool( &row, &(ft->ftYText), (void *)ft,
				ft->ftVerticalPaned,
				&(ft->ftYPosMenu),
				tedFrameYChanged,
				tedFrameYPosChosen );

    guiToolMakeLabelAndMenuRow( &row, &(ft->ftYRefMenu),
				&label, ft->ftVerticalPaned,
				fpr->fprYReference,
				tedFrameYRefChosen, (void *)ft );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a frame tool. I.E. the 'Frame' page of the format tool.	*/
/*									*/
/************************************************************************/

void tedFormatFillFramePage( FrameTool *			ft,
			    const FramePageResources *		fpr,
			    AppInspector *			ai,
			    int					subjectPage,
			    InspectorSubject *			is,
			    APP_WIDGET				pageWidget,
			    const InspectorSubjectResources *	isr )
    {
    APP_WIDGET	row;

    /**/
    is->isPrivate= ft;

    ft->ftPageResources= fpr;

    /**************/
    tedFrameToolMakeHorizontalGroup( ft, fpr, pageWidget );
    tedFrameToolMakeVerticalGroup( ft, fpr, pageWidget );

    /**************/
    guiToolMake2BottonRow( &row, pageWidget,
	    &(is->isSelectButton), &(is->isDeleteButton),
	    isr->isrSelectButtonText, isr->isrDeleteButtonText,
	    tedFormatSelectFrame, tedFormatDeleteFrame, (void *)ft );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(is->isInsertButton), &(is->isAppendButton),
	    isr->isrInsertButtonText, isr->isrAppendButtonText,
	    tedFormatSetFrame, tedFormatUnsetFrame, (void *)ft );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedFormatRevertFramePushed,
	    tedFormatChangeFramePushed, (void *)ft );

    return;
    }

void tedFrameToolFillChoosers(		FrameTool *			ft,
					const FramePageResources *	fpr )
    {
    tedFormatFillHeightChooser( &(ft->ftHeightChooser),
					    fpr->fprHeightFree,
					    fpr->fprHeightAtLeast,
					    fpr->fprHeightExactly );

    appFillInspectorMenu( FXref__FRAME_COUNT, FXrefCOLUMN,
					    ft->ftXRefItems,
					    fpr->fprXRefItemTexts,
					    &(ft->ftXRefMenu) );

    appFillInspectorMenu( FXpos_COUNT, FXposXL,
					    ft->ftXPosItems,
					    fpr->fprXPosItemTexts,
					    &(ft->ftXPosMenu) );

    appFillInspectorMenu( FYref__FRAME_COUNT, FYrefMARGIN,
					    ft->ftYRefItems,
					    fpr->fprYRefItemTexts,
					    &(ft->ftYRefMenu) );

    appFillInspectorMenu( FYpos_COUNT, FYposYT,
					    ft->ftYPosItems,
					    fpr->fprYPosItemTexts,
					    &(ft->ftYPosMenu) );
    }

void tedFinishFrameTool( 	FrameTool *			ft,
				const FramePageResources *	fpr )
    {
    appOptionmenuRefreshWidth( &(ft->ftHeightChooser.hcOptionmenu) );

    appOptionmenuRefreshWidth( &(ft->ftXRefMenu) );
    appOptionmenuRefreshWidth( &(ft->ftXPosMenu) );

    appOptionmenuRefreshWidth( &(ft->ftYRefMenu) );
    appOptionmenuRefreshWidth( &(ft->ftYPosMenu) );
    }

/************************************************************************/
/*									*/
/*  Clean a frame tool.							*/
/*									*/
/************************************************************************/

void tedInitFrameTool(	FrameTool *	ft )
    {
    ft->ftApplication= (EditApplication *)0;
    ft->ftInspector= (AppInspector *)0;
    ft->ftPageResources= (const FramePageResources *)0;

    ft->ftUnit= UNITtyPOINTS;

    docInitFrameProperties( &(ft->ftPropertiesSet) );
    docInitFrameProperties( &(ft->ftPropertiesChosen) );

    ft->ftWidthLabel= (APP_WIDGET)0;
    ft->ftWidthText= (APP_WIDGET)0;

    tedInitHeightChooser( &(ft->ftHeightChooser) );

    appInitOptionmenu( &(ft->ftXRefMenu) );
    appInitOptionmenu( &(ft->ftYRefMenu) );

    ft->ftXText= (APP_WIDGET)0;
    ft->ftYText= (APP_WIDGET)0;

    return;
    }

void tedCleanFrameTool(	FrameTool *	ft )
    {
    docCleanFrameProperties( &(ft->ftPropertiesSet) );
    docCleanFrameProperties( &(ft->ftPropertiesChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get frame tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedFrameSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolFrame",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Frame" ),
    APP_RESOURCE( "frameToolChangeRow",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply" ),
    APP_RESOURCE( "frameToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "frameToolSelectFrame",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Frame" ),
    APP_RESOURCE( "frameToolDeleteFrame",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Frame" ),
    APP_RESOURCE( "frameToolInsertRow",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Row" ),
    APP_RESOURCE( "frameToolAppendRow",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Row" ),
    };

static AppConfigurableResource TED_TedFrameToolResourceTable[]=
    {
    APP_RESOURCE( "frameToolHorizontal",
	    offsetof(FramePageResources,fprHorizontal),
	    "Horizontal" ),
    APP_RESOURCE( "frameToolVertical",
	    offsetof(FramePageResources,fprVertical),
	    "Vertical" ),

    APP_RESOURCE( "frameToolWidth",
	    offsetof(FramePageResources,fprWidth),
	    "Width" ),

    /**/
    APP_RESOURCE( "frameToolHeightFree",
	    offsetof(FramePageResources,fprHeightFree),
	    "Height Free" ),
    APP_RESOURCE( "frameToolHeightExactly",
	    offsetof(FramePageResources,fprHeightExactly),
	    "Height Exactly" ),
    APP_RESOURCE( "frameToolHeightAtLeast",
	    offsetof(FramePageResources,fprHeightAtLeast),
	    "Height at Least" ),

    /**/
    APP_RESOURCE( "frameToolXPosGiven",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXGIVEN]),
		"X at" ),
    APP_RESOURCE( "frameToolXPosLeft",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXL]),
		"X Left" ),
    APP_RESOURCE( "frameToolXPosRight",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXR]),
		"X Right" ),
    APP_RESOURCE( "frameToolXPosCenter",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXC]),
		"X Centered" ),
    APP_RESOURCE( "frameToolXPosInside",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXI]),
		"X Inside" ),
    APP_RESOURCE( "frameToolXPosOutside",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXO]),
		"X Outside" ),
    APP_RESOURCE( "frameToolXReference",
	    offsetof(FramePageResources,fprXReference),
	    "Relative To" ),
    APP_RESOURCE( "frameToolXRefColumn",
		offsetof(FramePageResources,fprXRefItemTexts[FXrefCOLUMN]),
		"Column" ),
    APP_RESOURCE( "frameToolXRefPage",
		offsetof(FramePageResources,fprXRefItemTexts[FXrefPAGE]),
		"Page" ),
    APP_RESOURCE( "frameToolXRefMargin",
		offsetof(FramePageResources,fprXRefItemTexts[FXrefMARGIN]),
		"Margin" ),
    APP_RESOURCE( "frameToolYPosGiven",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYGIVEN]),
		"Y at" ),
    APP_RESOURCE( "frameToolYPosInside",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYIN]),
		"Y Inside" ),
    APP_RESOURCE( "frameToolYPosOutside",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYOUT]),
		"Y Outside" ),
    APP_RESOURCE( "frameToolYPosTop",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYT]),
		"Y at Top" ),
    APP_RESOURCE( "frameToolYPosCenter",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYC]),
		"Y at Center" ),
    APP_RESOURCE( "frameToolYPosBottom",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYB]),
		"Y at Bottom" ),
    APP_RESOURCE( "frameToolYReference",
	    offsetof(FramePageResources,fprYReference),
	    "Relative To" ),
    APP_RESOURCE( "frameToolYRefMargin",
		offsetof(FramePageResources,fprYRefItemTexts[FYrefMARGIN]),
		"Margin" ),
    APP_RESOURCE( "frameToolYRefPage",
		offsetof(FramePageResources,fprYRefItemTexts[FYrefPAGE]),
		"Page" ),
    APP_RESOURCE( "frameToolYRefParagraph",
		offsetof(FramePageResources,fprYRefItemTexts[FYrefPARA]),
		"Paragraph" ),
    };

void tedFrameToolGetResourceTable(	EditApplication *		ea,
					FramePageResources *		fpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)fpr,
				TED_TedFrameToolResourceTable,
				sizeof(TED_TedFrameToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedFrameSubjectResourceTable,
				sizeof(TED_TedFrameSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
