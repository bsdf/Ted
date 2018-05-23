/************************************************************************/
/*									*/
/*  Height tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_HEIGHT_TOOL_H
#   define	TED_HEIGHT_TOOL_H

#   include	<appFrame.h>
#   include	<guiOptionmenu.h>

/************************************************************************/
/*									*/
/*  A combination of a pulldown menu and a text widget to manage a	*/
/*  height property.							*/
/*									*/
/************************************************************************/

typedef enum HeightSetting
    {
    HC_FREE= 0,
    HC_AT_LEAST,
    HC_EXACTLY,

    HC__COUNT
    } HeightSetting;

typedef struct HeightChooser
    {
    APP_WIDGET		hcRow;
    AppOptionmenu	hcOptionmenu;
    APP_WIDGET		hcText;

    APP_WIDGET		hcMenuItems[HC__COUNT];
    unsigned char	hcHow;
    unsigned char	hcEnabled;
    } HeightChooser;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedInitHeightChooser(	HeightChooser *		hc );

extern void tedFormatMakeHeightRow(
				void *				through,
				APP_WIDGET			parent,
				HeightChooser *			hc,
				APP_TXACTIVATE_CALLBACK_T	textCallback,
				OptionmenuCallback		menuCallback );

extern void tedFormatRefreshHeightChooser(	HeightChooser *	hc,
						int		height );

extern void tedFormatEnableHeightChooser(	HeightChooser *	hc,
						int		enabled );

extern void tedFormatHeightChosen(	int			how,
					HeightChooser *		hc,
					int			defaultValue );

extern void tedFormatFillHeightChooser(	HeightChooser *		hc,
					const char *		freeText,
					const char *		atLeastText,
					const char *		exactlyText );

extern int tedFormatToolGetHeight(	int *			pHeight,
					HeightChooser *		hc );

extern int tedHeightToolValidateDimension(
					int *			pValue,
					int *			pChanged,
					const HeightChooser *	hc,
					int			orig );

#   endif	/*  TED_HEIGHT_TOOL_H */
