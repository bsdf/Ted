/************************************************************************/
/*									*/
/*  Border tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_BORDER_TOOL_H
#   define	TED_BORDER_TOOL_H

#   include	<docBorderProperties.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<appColorChooser.h>

struct BorderTool;

typedef struct BorderTool
    {
    AppInspector *		btInspector;
    int				btSubjectPage;
    int				btWhich;
    const char *		btLabel;

    unsigned char		btEnabled;

    APP_WIDGET			btFrame;
    APP_WIDGET			btPaned;

    APP_WIDGET			btOnOffRow;
    APP_WIDGET			btOnOffToggle;
    APP_WIDGET			btWideText;

    APP_WIDGET			btStyleRow;
    APP_WIDGET			btStyleLabel;
    AppDrawnPulldown		btStylePulldown;

    APP_WIDGET			btColorRow;
    APP_WIDGET			btColorLabel;
    ColorChooser		btColorChooser;

    ExpandedBorderProperties	btPropertiesChosen;
    PropertyMask		btPropertiesChanged;
    } BorderTool;

typedef struct BorderToolResources
    {
    char *			btrWidth;
    char *			btrColor;
    char *			btrStyle;
    ColorChooserResources	btrColorChooserResources;
    } BorderToolResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedEnableBorderTool(	BorderTool *		bt,
					int			enabled );

extern void tedInitBorderTool(		BorderTool *		bt );
extern void tedCleanBorderTool(		BorderTool *		bt );

extern void tedBorderToolSetPropertiesByNumber( BorderTool *		bt,
					const BufferDocument *		bd,
					int				num );

extern int tedBorderToolGetNumber(	int *			pNum,
					int *			pChanged,
					const BorderTool *	bt,
					BufferDocument *	bd );

extern void tedMakeBorderTool(	BorderTool *			bt,
				AppInspector *			ai,
				APP_WIDGET			page,
				const char *			label,
				const BorderToolResources *	btr,
				int				subjectPage,
				int				which );

extern void tedFinishBorderTool(BorderTool *			bt,
				const PostScriptFontList *	psfl );

extern void tedBorderSetExplicitColorChoice(
					BorderTool *		bt,
					const RGB8Color *	rgb8Set );

#   endif	/*  TED_BORDER_TOOL_H */
