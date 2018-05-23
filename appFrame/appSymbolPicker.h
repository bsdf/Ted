/************************************************************************/
/*									*/
/*  A Symbol picker.							*/
/*									*/
/************************************************************************/

#   ifndef		APP_SYMBL_PICKER_H
#   define		APP_SYMBL_PICKER_H

#   include	"appFrame.h"
#   include	"appInspector.h"
#   include	<docExpandedTextAttribute.h>
#   include	<utilMemoryBuffer.h>

/************************************************************************/
/*									*/
/*  Resources for the Symbol Picker.					*/
/*									*/
/************************************************************************/

typedef struct SymbolPickerResources
    {
    char *	sprFont;
    char *	sprBlock;
    char *	sprNone;
    } SymbolPickerResources;

/************************************************************************/
/*									*/
/*  Tell the target to insert a string.					*/
/*									*/
/************************************************************************/

typedef void (*SymbolPickerInsert)(
				void *				target,
				const char *			bytes,
				int				size,
				const TextAttribute *		taSet,
				const PropertyMask *		taSetMask );

/************************************************************************/
/*									*/
/*  Represents a symbol picker.						*/
/*									*/
/************************************************************************/

typedef struct SymbolPicker
    {
    EditApplication *			spApplication;
    AppInspector *			spInspector;

    unsigned int			spCurrentDocumentId;
    unsigned char			spEnabled;

    const SymbolPickerResources *	spResources;

    APP_WIDGET				spFontFrame;
    APP_WIDGET				spFontPaned;

    APP_WIDGET				spNextPageButton;
    APP_WIDGET				spPrevPageButton;
    APP_WIDGET				spClearButton;
    APP_WIDGET				spInsertButton;

    AppOptionmenu			spFontOptionmenu;
    AppOptionmenu			spBlockOptionmenu;

    APP_WIDGET				spSymbolDrawing;

    SymbolPickerInsert			spInsert;
    void *				spTarget;

    int					spSymbolSelected;
    int					spSymbolOffset;

    DocumentFontList			spDocumentFontList;
    const PostScriptFontList *		spPostScriptFontList;
    TextAttribute			spTextAttribute;
    int					spFontFamilyChosen;
    const AfmFontInfo *			spFontInfo;

    double				spPixelsPerTwip;
    DrawingSurface			spDrawingSurface;
    RGB8Color				spBackgroundColor;
    int					spScreenFont;

    int					spCellSizePixels;
    int					spCellsWide;
    int					spCellsHigh;

    APP_WIDGET *			spFontFamilyOpts;
    int					spFontFamilyOptCount;
    APP_WIDGET *			spUnicodeBlockOpts;
    int					spUnicodeBlockOptCount;

    MemoryBuffer			spCollectedString;
    					/*
					 *  Cope with silly double clicks 
					 *  From GTK.
					 */
    int					spPreviousSize;
    } SymbolPicker;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appFillSymbolPicker( SymbolPicker *			sp,
				const SymbolPickerResources *	spr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern int appAdaptSymbolPickerToFontFamily(
				void *				voidasp,
				unsigned int			documentId,
				const DocumentFontList *	dfl,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet );

extern void appSymbolPickerGetResourceTable(
					EditApplication *		ea,
					SymbolPickerResources *		spr,
					InspectorSubjectResources *	isr );

extern void appInitSymbolPicker(	SymbolPicker *	sp );
extern void appCleanSymbolPicker(	SymbolPicker *	sp );

extern void appSymbolPickerFillChoosers( SymbolPicker *			sp,
					const SymbolPickerResources *	spr );

extern void appFinishSymbolPicker(	SymbolPicker *			sp,
					const SymbolPickerResources *	spr );

extern void appEnableSymbolPicker(	SymbolPicker *		sp,
					int			enabled );

#   endif	/*	APP_SYMBL_PICKER_H	*/
