/************************************************************************/
/*									*/
/*  Image property tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_BITMAP_TOOL_H
#   define	TED_BITMAP_TOOL_H

#   include	<bitmap.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docPictureProperties.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Image' page.					*/
/*									*/
/************************************************************************/

typedef struct ImagePageResources
    {
    char *		iprObjectType;
    char *		iprImageType;
    char *		iprPixelSize;
    char *		iprTotalBytes;
    char *		iprImageWide;
    char *		iprImageHigh;
    char *		iprXScale;
    char *		iprYScale;
    } ImagePageResources;

/************************************************************************/
/*									*/
/*  A bitmap tool, i.e. the 'Image' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ImageTool
    {
    EditApplication *			itApplication;
    AppInspector *			itInspector;
    const ImagePageResources *		itPageResources;

    int					itPictureHasBitmap;
    int					itTotalBytes;
    BitmapDescription			itBitmapDescription;
    int					itObjectType;
    int					itImageType;

    PictureProperties			itPropertiesChosen;
    PictureProperties			itPropertiesSet;

    int					itImageHighAbsChanged;
    int					itImageWideAbsChanged;

    APP_WIDGET				itObjectTypeRow;
    APP_WIDGET				itObjectTypeLabel;
    APP_WIDGET				itObjectTypeText;

    APP_WIDGET				itImageTypeRow;
    APP_WIDGET				itImageTypeLabel;
    APP_WIDGET				itImageTypeText;

    APP_WIDGET				itPixelSizeRow;
    APP_WIDGET				itPixelsSizeLabel;
    APP_WIDGET				itPixelsSizeText;

    APP_WIDGET				itByteSizeRow;
    APP_WIDGET				itByteSizeLabel;
    APP_WIDGET				itByteSizeText;

    APP_WIDGET				itImageWideRow;
    APP_WIDGET				itImageWideLabel;
    APP_WIDGET				itImageWideText;

    APP_WIDGET				itImageHighRow;
    APP_WIDGET				itImageHighLabel;
    APP_WIDGET				itImageHighText;

    APP_WIDGET				itXScaleRow;
    APP_WIDGET				itXScaleLabel;
    APP_WIDGET				itXScaleText;

    APP_WIDGET				itYScaleRow;
    APP_WIDGET				itYScaleLabel;
    APP_WIDGET				itYScaleText;
    } ImageTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void tedImageToolGetImageResourceTable(
				EditApplication *		ea,
				ImagePageResources *		ipr,
				InspectorSubjectResources *	isr );

extern void tedImagePageFillChoosers(
				ImageTool *			it,
				const ImagePageResources *	ipr );

extern void tedImageFinishImagePage(
				ImageTool *			it,
				const ImagePageResources *	ipr );

extern void tedFormatFillImagePage(
				ImageTool *			it,
				const ImagePageResources *	ipr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedRefreshImageTool(
			    ImageTool *				it,
			    int *				pEnabled,
			    int *				pPref,
			    InspectorSubject *			is,
			    const DocumentSelection *		ds,
			    const SelectionDescription *	sd,
			    BufferDocument *			bd,
			    const unsigned char *		cmdEnabled );

extern void tedCleanImageTool(	ImageTool *	it );
extern void tedInitImageTool(	ImageTool *	it );

extern void tedImageToolGetResourceTable(
				EditApplication *		ea,
				ImagePageResources *		ipr,
				InspectorSubjectResources *	isr );

#   endif	/*  TED_BITMAP_TOOL_H */
