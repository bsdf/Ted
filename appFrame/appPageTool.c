/************************************************************************/
/*									*/
/*  A Page Layout tool.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appDebugon.h>

#   include	"appFrame.h"
#   include	<appUnit.h>
#   include	"appPaperChooser.h"
#   include	"appMarginTool.h"
#   include	"appTool.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"
#   include	"guiDrawPage.h"

#   define	DRH_MM		32

/************************************************************************/
/*									*/
/*  Represents A Page Layout tool:					*/
/*									*/
/************************************************************************/
#   define	FILEL	400

typedef struct AppPageToolResources
    {
    char *		aptrUnitType;

    char *		aptrPaper;
    char *		aptrCustom;
    char *		aptrPortrait;
    char *		aptrLandscape;
    MarginToolResources	aptrMarginToolResources;
    char *		aptrRevert;
    char *		aptrSet;

#   if 0
    For use as a dialog.
    char *		aptrOk;
    char *		aptrCancel;
#   endif

    } AppPageToolResources;

typedef struct AppPageTool
    {
    EditApplication *		aptApplication;

    DrawingSurface		aptDrawingSurface;
    RGB8Color			aptBackgroundColor;

    APP_WIDGET			aptTopWidget;
    APP_WIDGET			aptMainWidget;

    APP_WIDGET			aptPageDrawing;
    APP_WIDGET			aptButtonRow;

    PaperChooser		aptPaperChooser;
    AppMarginTool		aptMarginTool;

    AppToolDestroy		aptDestroy;

    DocumentGeometry		aptGeometryChosen;
    DocumentGeometry		aptGeometrySet;

    int				aptUnitType;
    double			aptPixelsPerTwip;
    int				aptPageHighMm;

    int				aptCustomPaperSize;
    void *			aptTarget;
    } AppPageTool;


static AppConfigurableResource APP_PageToolresourceTable[]=
    {
	APP_RESOURCE( "unit",
		    offsetof(AppPageToolResources,aptrUnitType),
		    "inch" ),
	APP_RESOURCE( "pageToolPaper",
		    offsetof(AppPageToolResources,aptrPaper),
		    "Paper" ),
	APP_RESOURCE( "pageToolCustom",
		    offsetof(AppPageToolResources,aptrCustom),
		    "Custom" ),
	APP_RESOURCE( "pageToolPortrait",
		    offsetof(AppPageToolResources,aptrPortrait),
		    "Portrait" ),
	APP_RESOURCE( "pageToolLandscape",
		    offsetof(AppPageToolResources,aptrLandscape),
		    "Landscape" ),
	APP_RESOURCE( "pageToolMargins",
		    offsetof(AppPageToolResources,
		    aptrMarginToolResources.mtrTitleText),
		    "Margins" ),
	APP_RESOURCE( "pageToolLeftMargin",
		    offsetof(AppPageToolResources,
		    aptrMarginToolResources.mtrLeftMarginText),
		    "Left" ),
	APP_RESOURCE( "pageToolTopMargin",
		    offsetof(AppPageToolResources,
		    aptrMarginToolResources.mtrTopMarginText),
		    "Top" ),
	APP_RESOURCE( "pageToolRightMargin",
		    offsetof(AppPageToolResources,
		    aptrMarginToolResources.mtrRightMarginText),
		    "Right" ),
	APP_RESOURCE( "pageToolBottomMargin",
		    offsetof(AppPageToolResources,
		    aptrMarginToolResources.mtrBottomMarginText),
		    "Bottom" ),
	APP_RESOURCE( "pageToolRevert",
		    offsetof(AppPageToolResources,aptrRevert),
		    "Revert" ),
	APP_RESOURCE( "pageToolSet",
		    offsetof(AppPageToolResources,aptrSet),
		    "Set" ),
#	if 0
	For use as a Dialog.
	APP_RESOURCE( "pageToolOk",
		    offsetof(AppPageToolResources,aptrOk),
		    "Ok" ),
	APP_RESOURCE( "pageToolCancel",
		    offsetof(AppPageToolResources,aptrCancel),
		    "Cancel" ),
#	endif
    };

/************************************************************************/
/*									*/
/*  Draw a schematic view of the page.					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appPageToolDrawPage, w, voidapt, exposeEvent )
    {
    AppPageTool *		apt= (AppPageTool *)voidapt;
    DrawingSurface		ds= apt->aptDrawingSurface;

    appDrawPageDiagram( apt->aptPageDrawing, ds,
				    &(apt->aptBackgroundColor),
				    apt->aptPixelsPerTwip,
				    &(apt->aptGeometryChosen) );
    }

/************************************************************************/
/*									*/
/*  A paper size was chosen.						*/
/*									*/
/************************************************************************/

static void appPageToolPaperRectChanged(
				PaperChooser *			pc,
				void *				voidapt,
				const DocumentGeometry *	dg )
    {
    AppPageTool *		apt= (AppPageTool *)voidapt;

    apt->aptGeometryChosen.dgPageWideTwips= dg->dgPageWideTwips;
    apt->aptGeometryChosen.dgPageHighTwips= dg->dgPageHighTwips;

    guiExposeDrawingWidget( apt->aptPageDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  The buttons have been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appPageToolRevertPushed, w, voidapt )
    {
    AppPageTool *		apt= (AppPageTool *)voidapt;

    apt->aptGeometryChosen.dgPageWideTwips=
					apt->aptGeometrySet.dgPageWideTwips;
    apt->aptGeometryChosen.dgPageHighTwips=
					apt->aptGeometrySet.dgPageHighTwips;

    appPaperChooserAdaptToGeometry( &(apt->aptPaperChooser),
						&(apt->aptGeometryChosen) );

    apt->aptGeometryChosen.dgLeftMarginTwips=
					apt->aptGeometrySet.dgLeftMarginTwips;
    apt->aptGeometryChosen.dgTopMarginTwips=
					apt->aptGeometrySet.dgTopMarginTwips;
    apt->aptGeometryChosen.dgRightMarginTwips=
					apt->aptGeometrySet.dgRightMarginTwips;
    apt->aptGeometryChosen.dgBottomMarginTwips=
					apt->aptGeometrySet.dgBottomMarginTwips;

    appMarginToolShowMargins( &(apt->aptMarginTool),
				apt->aptUnitType, &(apt->aptGeometryChosen) );

    guiExposeDrawingWidget( apt->aptPageDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Set' button has been pushed:					*/
/*									*/
/*	Retrieve the margins and the paper size.			*/
/*	Check the values						*/
/*	If things are ok, report the values back to the application.	*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appPageToolSetPushed, w, voidapt )
    {
    AppPageTool *	apt= (AppPageTool *)voidapt;
    EditApplication *	ea= apt->aptApplication;
    EditDocument *	ed= ea->eaCurrentDocument;

    DocumentGeometry	dg;

    const int		wholeDocument= 1;

    PropertyMask	margUpdMask;
    PropertyMask	setMask;

    PropertyMask	margChgMask;
    PropertyMask	sizeChgMask;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    utilPropMaskClear( &margUpdMask );
    utilPropMaskFill( &margUpdMask, DGprop_COUNT );

    utilPropMaskClear( &setMask );
    utilPropMaskFill( &setMask, DGprop_COUNT );
    PROPmaskUNSET( &setMask, DGpropHEADER_POSITION );
    PROPmaskUNSET( &setMask, DGpropFOOTER_POSITION );

    utilPropMaskClear( &margChgMask );
    /*
    utilPropMaskClear( &sizeChgMask );
    */

    dg= apt->aptGeometrySet;

    if  ( appMarginToolGetMargins( &margChgMask, &margUpdMask,
			    apt->aptUnitType, &(apt->aptMarginTool), &dg ) )
	{ return;	}

    if  ( appPaperChooserGetSize( &sizeChgMask, &(apt->aptPaperChooser), &dg ) )
	{ LDEB(1); return;	}

    if  ( appMarginToolCheckMargins( &dg ) )
	{ return;	}

    appPageToolSetProperties( (void *)apt, &dg );

    (*ea->eaSetPageLayout)( ed, &setMask, &dg, wholeDocument );

    return;
    }

/************************************************************************/
/*									*/
/*  The page layout tool is closed by the user.				*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( appClosePageTool, w, voidapt )
    {
    AppPageTool *	apt= (AppPageTool *)voidapt;

    if  ( apt->aptDestroy )
	{ (*apt->aptDestroy)( apt->aptTarget );	}

    if  ( apt->aptDrawingSurface )
	{ drawFreeDrawingSurface( apt->aptDrawingSurface );	}

    appDestroyShellWidget( w );

    free( apt );

    return;
    }

/************************************************************************/
/*									*/
/*  One of the margins has been changed by the user.			*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( appPageToolMarginChanged, w, voidapt )
    {
    AppPageTool *		apt= (AppPageTool *)voidapt;

    PropertyMask		dgChgMask;
    PropertyMask		dgUpdMask;

    DocumentGeometry		dg;

    utilPropMaskClear( &dgChgMask );

    utilPropMaskClear( &dgUpdMask );
    utilPropMaskFill( &dgUpdMask, DGprop_COUNT );

    dg= apt->aptGeometryChosen;

    if  ( appMarginToolGetMargins( &dgChgMask, &dgUpdMask,
			    apt->aptUnitType, &(apt->aptMarginTool), &dg ) )
	{ return;	}

    if  ( utilPropMaskIsEmpty( &dgChgMask) )
	{ return;	}

    if  ( appMarginToolCheckMargins( &dg ) )
	{ return;	}

    apt->aptGeometryChosen.dgLeftMarginTwips= dg.dgLeftMarginTwips;
    apt->aptGeometryChosen.dgTopMarginTwips= dg.dgTopMarginTwips;
    apt->aptGeometryChosen.dgRightMarginTwips= dg.dgRightMarginTwips;
    apt->aptGeometryChosen.dgBottomMarginTwips= dg.dgBottomMarginTwips;

    guiExposeDrawingWidget( apt->aptPageDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame for paper size and orientation.			*/
/*									*/
/************************************************************************/

static void appPageToolMakePaperFrame(	APP_WIDGET		parent,
					AppPageToolResources *	aptr,
					AppPageTool *		apt )
    {
    appMakePaperChooserWidgets( parent, aptr->aptrPaper,
						apt->aptUnitType,
						&(apt->aptPaperChooser),
						appPageToolPaperRectChanged,
						(void *)apt );

    appPaperChooserAddOrientationToggles( &(apt->aptPaperChooser),
						aptr->aptrPortrait,
						aptr->aptrLandscape );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appPageToolMakeButtonRow( APP_WIDGET		parent,
					AppPageToolResources *	aptr,
					AppPageTool *		apt )
    {
    APP_WIDGET	row;
    APP_WIDGET	revertButton;
    APP_WIDGET	setButton;

    const int	heightResizable= 0;
    const int	buttonIsDefault= 0;
    const int	colspan= 1;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &revertButton, row, aptr->aptrRevert,
	    appPageToolRevertPushed, (void *)apt, 0, colspan, buttonIsDefault );

    appMakeButtonInRow( &setButton, row, aptr->aptrSet,
	    appPageToolSetPushed, (void *)apt, 1, colspan, buttonIsDefault );

    return row;
    }

/************************************************************************/
/*									*/
/*  The page tool has been closed, remember so.				*/
/*									*/
/************************************************************************/

static void appPageToolClosed(		void *		voidea )
    {
    EditApplication *	ea=	(EditApplication *)voidea;
    AppPageTool *	apt= (AppPageTool *)ea->eaPageTool;

    if  ( apt )
	{ appCleanPaperChooser( &(apt->aptPaperChooser) );	}

    ea->eaPageTool= (void *)0;
    }

/************************************************************************/
/*									*/
/*  make a page tool.							*/
/*									*/
/************************************************************************/

void * appMakePageTool(	EditApplication *	ea,
			APP_WIDGET		pageOption,
			const char *		pixmapName )
    {
    AppPageTool *	apt;
    
    APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	iconMask= (APP_BITMAP_MASK)0;

    const int		userResizable= 0;

    static int			gotResources= 0;
    static AppPageToolResources	aptr;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, &aptr,
				    APP_PageToolresourceTable,
				    sizeof(APP_PageToolresourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    if  ( appGetImagePixmap( ea, ea->eaMainIcon, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (void *)0;	}

    apt= (AppPageTool *)malloc( sizeof(AppPageTool) );
    if  ( ! apt )
	{ XDEB(apt); return (void *)0;	}

    apt->aptUnitType= appUnitTypeInt( aptr.aptrUnitType );
    if  ( apt->aptUnitType < 0 )
	{ SDEB(aptr.aptrUnitType); apt->aptUnitType= UNITtyINCH;	}

    apt->aptDestroy= appPageToolClosed;
    apt->aptApplication= ea;
    apt->aptCustomPaperSize= -1;

    appInitPaperChooser( &(apt->aptPaperChooser) );
    apt->aptDrawingSurface= (DrawingSurface)0;
    
    appMakeVerticalTool( &(apt->aptTopWidget), &(apt->aptMainWidget), ea,
		    iconPixmap, iconMask, 
		    userResizable, pageOption,
		    appClosePageTool, (void *)apt );

    apt->aptPageHighMm= DRH_MM;
    apt->aptPixelsPerTwip= ( apt->aptPageHighMm* ea->eaPixelsPerTwip )/
								A3_MM_HIGH;

    apt->aptPageDrawing= appMakePageDrawing( apt->aptMainWidget,
			ea->eaPixelsPerTwip, apt->aptPageHighMm,
			appPageToolDrawPage, (void *)apt );

    appPageToolMakePaperFrame( apt->aptMainWidget, &aptr, apt );

    appMakeMarginToolWidgets( apt->aptMainWidget,
					    &(aptr.aptrMarginToolResources),
					    &(apt->aptMarginTool),
					    appPageToolMarginChanged,
					    (void *)apt );

    apt->aptButtonRow=
		appPageToolMakeButtonRow( apt->aptMainWidget, &aptr, apt );

    appPaperChooserFillMenu( &(apt->aptPaperChooser), aptr.aptrCustom );

    appShowShellWidget( ea, apt->aptTopWidget );

    appPaperChooserRefreshMenuWidth( &(apt->aptPaperChooser) );

    apt->aptDrawingSurface= guiDrawingSurfaceForNativeWidget(
				apt->aptPageDrawing,
				ea->eaPostScriptFontList.psflAvoidFontconfig );

    guiGetForegroundColor( &(apt->aptBackgroundColor), apt->aptPageDrawing );

    return (void *)apt;
    }

/************************************************************************/
/*									*/
/*  Draw a page layout tool to front.					*/
/*									*/
/************************************************************************/

void appShowPageTool(		EditApplication *	ea )
    {
    AppPageTool *		apt= (AppPageTool *)ea->eaPageTool;

    appShowShellWidget( ea, apt->aptTopWidget );
    }

/************************************************************************/
/*									*/
/*  Set the properties in a page tool.					*/
/*									*/
/************************************************************************/

void appPageToolSetProperties(	void *				voidapt,
				const DocumentGeometry *	dg )
    {
    AppPageTool *	apt= (AppPageTool *)voidapt;

    apt->aptGeometryChosen= apt->aptGeometrySet= *dg;

    appPaperChooserAdaptToGeometry( &(apt->aptPaperChooser),
						&(apt->aptGeometryChosen) );

    appMarginToolShowMargins( &(apt->aptMarginTool),
				apt->aptUnitType, &(apt->aptGeometryChosen) );

    guiExposeDrawingWidget( apt->aptPageDrawing );

    return;
    }

void appEnablePageTool(		void *	voidapt,
				int	enabled )
    {
    AppPageTool *	apt= (AppPageTool *)voidapt;

    guiEnableWidget( apt->aptMainWidget, enabled != 0 );

    return;
    }
