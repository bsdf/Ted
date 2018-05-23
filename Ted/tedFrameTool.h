/************************************************************************/
/*									*/
/*  Frame tool:	(Positioned Objects and Frames)				*/
/*									*/
/************************************************************************/

#   ifndef	TED_FRAME_TOOL_H
#   define	TED_FRAME_TOOL_H

#   include	<docPropVal.h>
#   include	<docBuf.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docFrameProperties.h>
#   include	"tedHeightTool.h"
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Frame' page.					*/
/*									*/
/************************************************************************/

typedef struct FramePageResources
    {
    const char *	fprWidth;

    const char *	fprHorizontal;
    const char *	fprVertical;

    const char *	fprHeightFree;
    const char *	fprHeightAtLeast;
    const char *	fprHeightExactly;

    const char *	fprXReference;
    const char *	fprXRefItemTexts[FXref_COUNT];
    const char *	fprYReference;
    const char *	fprYRefItemTexts[FYref_COUNT];

    const char *	fprXPosItemTexts[FXpos_COUNT];
    const char *	fprYPosItemTexts[FYpos_COUNT];
    } FramePageResources;

/************************************************************************/
/*									*/
/*  A frame tool, i.e. the 'Frame' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct FrameTool
    {
    EditApplication *		ftApplication;
    AppInspector *		ftInspector;
    const FramePageResources *	ftPageResources;
    int				ftUnit;

    FrameProperties		ftPropertiesSet;
    FrameProperties		ftPropertiesChosen;

    unsigned char		ftCanChange;

    APP_WIDGET			ftHorizontalFrame;
    APP_WIDGET			ftHorizontalPaned;
    APP_WIDGET				ftWidthLabel;
    APP_WIDGET				ftWidthText;
    AppOptionmenu			ftXRefMenu;
    APP_WIDGET				ftXRefItems[FXref_COUNT];
    AppOptionmenu			ftXPosMenu;
    APP_WIDGET				ftXPosItems[FXpos_COUNT];
    APP_WIDGET				ftXText;

    APP_WIDGET			ftVerticalFrame;
    APP_WIDGET			ftVerticalPaned;
    HeightChooser			ftHeightChooser;
    AppOptionmenu			ftYRefMenu;
    APP_WIDGET				ftYRefItems[FYref_COUNT];
    AppOptionmenu			ftYPosMenu;
    APP_WIDGET				ftYPosItems[FYpos_COUNT];
    APP_WIDGET				ftYText;
    } FrameTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillFramePage( FrameTool *			ft,
			    const FramePageResources *		fpr,
			    AppInspector *			ai,
			    int					subjectPage,
			    InspectorSubject *			is,
			    APP_WIDGET				pageWidget,
			    const InspectorSubjectResources *	isr );

extern void tedFrameToolGetResourceTable(
				EditApplication *		ea,
				FramePageResources *		spr,
				InspectorSubjectResources *	isr );

extern void tedRefreshFrameTool( FrameTool *			ft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs,
				const SelectionDescription *	sd,
				BufferDocument *		bd );

extern void tedFrameToolFillChoosers(	FrameTool *			ft,
					const FramePageResources *	fpr );

extern void tedInitFrameTool(	FrameTool *	ft );
extern void tedCleanFrameTool(	FrameTool *	ft );

extern void tedFinishFrameTool( 	FrameTool *			ft,
					const FramePageResources *	fpr );

#   endif	/*  TED_FRAME_TOOL_H */
