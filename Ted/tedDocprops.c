/************************************************************************/
/*  Dialog with document properties.					*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appDebugon.h>

#   include	<guiTextUtil.h>
#   include	<docStatistics.h>
#   include	"tedApp.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"

#   define	DRH_CM	4.5

/************************************************************************/
/*  Represents a mail dialog.						*/
/************************************************************************/
#   define	FILEL	400

typedef struct DocumentPropertyPageResources
    {
    const char *		dpprOkText;
    const char *		dpprCancelText;

    const char *		dpprGeneratorText;
    const char *		dpprTitleText;
    const char *		dpprAuthorText;
    const char *		dpprCompanyText;
    const char *		dpprSubjectText;
    const char *		dpprKeywordsText;
    const char *		dpprDoccommText;
    const char *		dpprHlinkbaseText;

    const char *		dpprCreatimText;
    const char *		dpprRevtimText;
    const char *		dpprPrintimText;

    const char *		dpprPageCountText;
    const char *		dpprParaCountText;
    const char *		dpprLineCountText;
    const char *		dpprWordCountText;
    const char *		dpprCharCountText;
    } DocumentPropertyPageResources;

typedef struct PropertyDialog
    {
    AppDialog			dpdDialog;

    APP_WIDGET			dpdButtonRow;
    APP_WIDGET			dpdOkButton;

    APP_WIDGET			dpdGeneratorWidget;
    APP_WIDGET			dpdTitleWidget;
    APP_WIDGET			dpdAuthorWidget;
    APP_WIDGET			dpdCompanyWidget;
    APP_WIDGET			dpdSubjectWidget;
    APP_WIDGET			dpdKeywordsWidget;
    APP_WIDGET			dpdDoccommWidget;
    APP_WIDGET			dpdHlinkbaseWidget;

    APP_WIDGET			dpdCreatimWidget;
    APP_WIDGET			dpdRevtimWidget;
    APP_WIDGET			dpdPrintimWidget;

    APP_WIDGET			dpdPageCountWidget;
    APP_WIDGET			dpdParaCountWidget;
    APP_WIDGET			dpdLineCountWidget;
    APP_WIDGET			dpdWordCountWidget;
    APP_WIDGET			dpdCharCountWidget;
    } PropertyDialog;

static PropertyDialog *	TED_PropertyDialog;

static AppConfigurableResource TED_PropertyDialogResourceTable[]=
    {
	APP_RESOURCE( "propDialogGenerator",
		    offsetof(DocumentPropertyPageResources,dpprGeneratorText),
		    "Generator" ),
	APP_RESOURCE( "propDialogTitle",
		    offsetof(DocumentPropertyPageResources,dpprTitleText),
		    "Title" ),
	APP_RESOURCE( "propDialogAuthor",
		    offsetof(DocumentPropertyPageResources,dpprAuthorText),
		    "Author" ),
	APP_RESOURCE( "propDialogCompany",
		    offsetof(DocumentPropertyPageResources,dpprCompanyText),
		    "Company" ),
	APP_RESOURCE( "propDialogSubject",
		    offsetof(DocumentPropertyPageResources,dpprSubjectText),
		    "Subject" ),
	APP_RESOURCE( "propDialogKeywords",
		    offsetof(DocumentPropertyPageResources,dpprKeywordsText),
		    "Keywords" ),
	APP_RESOURCE( "propDialogDoccom",
		    offsetof(DocumentPropertyPageResources,dpprDoccommText),
		    "Comment" ),
	APP_RESOURCE( "propDialogHlinkbase",
		    offsetof(DocumentPropertyPageResources,dpprHlinkbaseText),
		    "Links Relative to" ),

	APP_RESOURCE( "propDialogCreatim",
		    offsetof(DocumentPropertyPageResources,dpprCreatimText),
		    "Created" ),
	APP_RESOURCE( "propDialogRevtim",
		    offsetof(DocumentPropertyPageResources,dpprRevtimText),
		    "Last Changed" ),
	APP_RESOURCE( "propDialogPrintim",
		    offsetof(DocumentPropertyPageResources,dpprPrintimText),
		    "Last Printed" ),

	APP_RESOURCE( "propDialogPages",
		    offsetof(DocumentPropertyPageResources,dpprPageCountText),
		    "Pages" ),
	APP_RESOURCE( "propDialogParagraphs",
		    offsetof(DocumentPropertyPageResources,dpprParaCountText),
		    "Paragraphs" ),
	APP_RESOURCE( "propDialogLines",
		    offsetof(DocumentPropertyPageResources,dpprLineCountText),
		    "Lines" ),
	APP_RESOURCE( "propDialogWords",
		    offsetof(DocumentPropertyPageResources,dpprWordCountText),
		    "Words" ),
	APP_RESOURCE( "propDialogCharacters",
		    offsetof(DocumentPropertyPageResources,dpprCharCountText),
		    "Characters" ),

	APP_RESOURCE( "propDialogCancel",
		    offsetof(DocumentPropertyPageResources,dpprCancelText),
		    "Cancel" ),
	APP_RESOURCE( "propDialogOk",
		    offsetof(DocumentPropertyPageResources,dpprOkText),
		    "Ok" ),
    };

/************************************************************************/
/*									*/
/*  One of the Text widgets changed.					*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedPropertyChanged, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    guiEnableWidget( dpd->dpdOkButton, 1 );

    appGuiSetDefaultButtonForDialog( &(dpd->dpdDialog), dpd->dpdOkButton );

    return;
    }

/************************************************************************/
/*									*/
/*  The buttons have been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedPropertiesCancelPushed, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiBreakDialog( &(dpd->dpdDialog), AQDrespCANCEL );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedPropertiesOkPushed, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiBreakDialog( &(dpd->dpdDialog), AQDrespOK );

    return;
    }

/************************************************************************/
/*									*/
/*  A Property Dialog must be destroyed.				*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( tedClosePropertyDialog, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiBreakDialog( &(dpd->dpdDialog), AQDrespCLOSED );

    return;
    }

static void tedPropsDialogMakePropertyRow(	APP_WIDGET*	pRow,
						APP_WIDGET *	pLabel,
						APP_WIDGET *	pText,
						APP_WIDGET	parent,
						PropertyDialog * dpd,
						const char *	labelText,
						int		textEnabled )
    {
    APP_WIDGET		row;
    APP_WIDGET		label;
    APP_WIDGET		text;

    const int		heightResizable= 0;
    const int		columnCount= 5;
    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		textColumn= 1;
    const int		textColspan= 4;
    const int		textColumns= 50;

    row= appMakeRowInColumn( parent, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeTextInRow( &text, row, textColumn, textColspan,
						textColumns, textEnabled );

    *pRow= row; *pLabel= label; *pText= text;

    if  ( textEnabled )
	{
	appGuiSetTypingCallbackForText( text,
					    tedPropertyChanged, (void *)dpd );
	}

    return;
    }

static void tedPropsDialogMakeTextForm(
				APP_WIDGET				parent,
				PropertyDialog *			dpd,
				const DocumentPropertyPageResources *	dppr )
    {
    APP_WIDGET		row;
    APP_WIDGET		label;

    const int		changeable= 1;
    const int		derived= 0;

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdTitleWidget),
			    parent, dpd, dppr->dpprTitleText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdAuthorWidget),
			    parent, dpd, dppr->dpprAuthorText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdCompanyWidget),
			    parent, dpd, dppr->dpprCompanyText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdSubjectWidget),
			    parent, dpd, dppr->dpprSubjectText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdKeywordsWidget),
			    parent, dpd, dppr->dpprKeywordsText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdDoccommWidget),
			    parent, dpd, dppr->dpprDoccommText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdHlinkbaseWidget),
			    parent, dpd, dppr->dpprHlinkbaseText, changeable );

    /**/
    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdGeneratorWidget),
			    parent, dpd, dppr->dpprGeneratorText, derived );

    /**/

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdCreatimWidget),
			    parent, dpd, dppr->dpprCreatimText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdRevtimWidget),
			    parent, dpd, dppr->dpprRevtimText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdPrintimWidget),
			    parent, dpd, dppr->dpprPrintimText, derived );

    /**/

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdPageCountWidget),
			    parent, dpd, dppr->dpprPageCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdParaCountWidget),
			    parent, dpd, dppr->dpprParaCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdLineCountWidget),
			    parent, dpd, dppr->dpprLineCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdWordCountWidget),
			    parent, dpd, dppr->dpprWordCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdCharCountWidget),
			    parent, dpd, dppr->dpprCharCountText, derived );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

static APP_WIDGET tedPropsDialogMakeButtonRow(
				APP_WIDGET				parent,
				PropertyDialog *			dpd,
				const DocumentPropertyPageResources *	dppr )
    {
    APP_WIDGET	row;
    APP_WIDGET	cancelButton;
    const int	heightResizable= 0;
    const int	colspan= 1;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(dpd->dpdOkButton), row, dppr->dpprOkText,
		    tedPropertiesOkPushed, (void *)dpd, 0, colspan, 1 );

    appMakeButtonInRow( &(cancelButton), row, dppr->dpprCancelText, 
		    tedPropertiesCancelPushed, (void *)dpd, 1, colspan, 0 );

    appGuiSetCancelButtonForDialog( &(dpd->dpdDialog), cancelButton );

    return row;
    }

/************************************************************************/
/*									*/
/*  Make a document properties dialog.					*/
/*									*/
/************************************************************************/

static PropertyDialog * tedMakePropertyDialog( EditDocument *	ed,
					APP_WIDGET		printOption,
					const char *		pixmapName )
    {
    EditApplication *	ea= ed->edApplication;
    PropertyDialog *	dpd;
    
    APP_WIDGET		paned;
    APP_WIDGET		separator;

    APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	iconMask= (APP_BITMAP_MASK)0;

    static DocumentPropertyPageResources	dppr;
    static int					gotResources;

    if  ( appGetImagePixmap( ea, ea->eaMainIcon, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (PropertyDialog *)0;	}

    dpd= (PropertyDialog *)malloc( sizeof(PropertyDialog) );
    if  ( ! dpd )
	{ XDEB(dpd); return (PropertyDialog *)0;	}

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&dppr,
				    TED_PropertyDialogResourceTable,
				    sizeof(TED_PropertyDialogResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    appMakeVerticalDialog( &(dpd->dpdDialog), &paned, ea,
						tedClosePropertyDialog,
						(APP_DESTROY_CALLBACK_T)0,
						(void *)dpd );

    appSetShellTitle( dpd->dpdDialog.adTopWidget,
				    printOption, ea->eaApplicationName );


    tedPropsDialogMakeTextForm( paned, dpd, &dppr );

    appGuiInsertSeparatorInColumn( &separator, paned );

    dpd->dpdButtonRow= tedPropsDialogMakeButtonRow( paned, dpd, &dppr );

    appGuiShowDialog( ea, &(dpd->dpdDialog), ed->edToplevel.atTopWidget );

    return dpd;
    }

/************************************************************************/
/*									*/
/*  Show the 'Document Properties...' dialog.				*/
/*									*/
/*  1)	Make or just show it.						*/
/*  2)	Set the default content.					*/
/*									*/
/************************************************************************/

static void tedDateToWidget(	APP_WIDGET			w,
				const struct tm *		tm )
    {
    if  ( tm->tm_mday == 0 )
	{ appStringToTextWidget( w, "" );	}
    else{
	char		scratch[40+1];
	const char *	frm;

	/*  GCC Shut Up! */
	frm= "%c";
	if  ( strftime( scratch, sizeof(scratch)- 1, frm, tm ) < 1 )
	    { LDEB(1); appStringToTextWidget( w, "?" );	}
	else{ appStringToTextWidget( w, scratch );	}
	}

    return;
    }

int tedRunPropertyDialog(	EditDocument *			ed,
				const DocumentStatistics *	ds,
				APP_WIDGET			option,
				const char *			pixmapName )
    {
    int				rval= 0;

    PropertyDialog *		dpd= TED_PropertyDialog;

    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dpSet= &(bd->bdProperties);

    DocumentProperties		dpChosen;

    PropertyMask		dpDifMask;
    PropertyMask		dpCmpMask;


    docInitDocumentProperties( &dpChosen );

    /*  1  */
    if  ( ! dpd )
	{
	dpd= TED_PropertyDialog= tedMakePropertyDialog( ed, option,
								pixmapName );
	if  ( ! dpd )
	    { XDEB(dpd); rval= -1; goto ready;	}

	}
    else{
	appSetShellTitle( dpd->dpdDialog.adTopWidget,
					    option, ea->eaApplicationName );

	appGuiShowDialog( ea, &(dpd->dpdDialog), ed->edToplevel.atTopWidget );
	}

    guiBufferToText( dpd->dpdGeneratorWidget, &(dpSet->dpGeneratorRead) );
    guiBufferToText( dpd->dpdTitleWidget, &(dpSet->dpTitle) );
    guiBufferToText( dpd->dpdAuthorWidget, &(dpSet->dpAuthor) );
    guiBufferToText( dpd->dpdCompanyWidget, &(dpSet->dpCompany) );
    guiBufferToText( dpd->dpdSubjectWidget, &(dpSet->dpSubject) );
    guiBufferToText( dpd->dpdKeywordsWidget, &(dpSet->dpKeywords) );
    guiBufferToText( dpd->dpdDoccommWidget, &(dpSet->dpDoccomm) );
    guiBufferToText( dpd->dpdHlinkbaseWidget, &(dpSet->dpHlinkbase) );

    guiEnableText( dpd->dpdTitleWidget, ! ed->edIsReadonly );
    guiEnableText( dpd->dpdAuthorWidget, ! ed->edIsReadonly );
    guiEnableText( dpd->dpdCompanyWidget, ! ed->edIsReadonly );
    guiEnableText( dpd->dpdSubjectWidget, ! ed->edIsReadonly );
    guiEnableText( dpd->dpdKeywordsWidget, ! ed->edIsReadonly );
    guiEnableText( dpd->dpdDoccommWidget, ! ed->edIsReadonly );
    guiEnableText( dpd->dpdHlinkbaseWidget, ! ed->edIsReadonly );

    tedDateToWidget( dpd->dpdCreatimWidget, &(dpSet->dpCreatim) );
    tedDateToWidget( dpd->dpdRevtimWidget, &(dpSet->dpRevtim) );
    tedDateToWidget( dpd->dpdPrintimWidget, &(dpSet->dpPrintim) );

    appIntegerToTextWidget( dpd->dpdPageCountWidget, ds->dsPageCount );
    appIntegerToTextWidget( dpd->dpdParaCountWidget, ds->dsParagraphCount );
    appIntegerToTextWidget( dpd->dpdLineCountWidget, ds->dsLineCount );
    appIntegerToTextWidget( dpd->dpdWordCountWidget, ds->dsWordCount );
    appIntegerToTextWidget( dpd->dpdCharCountWidget, ds->dsCharacterCount );

    guiEnableWidget( dpd->dpdOkButton, ed->edIsReadonly );

    appGuiRunDialog( &(dpd->dpdDialog), AQDrespNONE, ea );

    switch( dpd->dpdDialog.adResponse )
	{
	case AQDrespOK:
	    if  ( ed->edIsReadonly )
		{ appGuiHideDialog( &(dpd->dpdDialog) ); goto ready;	}
	    break;

	default:
	    LDEB(dpd->dpdDialog.adResponse);
	    /*FALLTHROUGH*/
	case AQDrespCANCEL:
	    appGuiHideDialog( &(dpd->dpdDialog) );
	    rval= -1; goto ready;

	case AQDrespCLOSED:
	    rval= -1; goto ready;
	}

    appGuiHideDialog( &(dpd->dpdDialog) );

    if  ( appBufferFromTextWidget( &(dpChosen.dpTitle),
						dpd->dpdTitleWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appBufferFromTextWidget( &(dpChosen.dpAuthor),
						dpd->dpdAuthorWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appBufferFromTextWidget( &(dpChosen.dpCompany),
						dpd->dpdCompanyWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appBufferFromTextWidget( &(dpChosen.dpSubject),
						dpd->dpdSubjectWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appBufferFromTextWidget( &(dpChosen.dpKeywords),
						dpd->dpdKeywordsWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appBufferFromTextWidget( &(dpChosen.dpDoccomm),
						dpd->dpdDoccommWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appBufferFromTextWidget( &(dpChosen.dpHlinkbase),
						dpd->dpdHlinkbaseWidget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilPropMaskClear( &dpCmpMask );
    utilPropMaskClear( &dpDifMask );

    PROPmaskADD( &dpCmpMask, DPpropTITLE );
    PROPmaskADD( &dpCmpMask, DPpropSUBJECT );
    PROPmaskADD( &dpCmpMask, DPpropKEYWORDS );
    PROPmaskADD( &dpCmpMask, DPpropDOCCOMM );
    PROPmaskADD( &dpCmpMask, DPpropAUTHOR );
    PROPmaskADD( &dpCmpMask, DPpropCOMPANY );
    PROPmaskADD( &dpCmpMask, DPpropHLINKBASE );

    docDocumentPropertyDifference( &dpDifMask, dpSet, &dpCmpMask, &dpChosen );

    if  ( tedDocSetDocumentProperties( ed, &dpDifMask, &dpChosen, td->tdTraced ) )
	{ LDEB(1);	}

  ready:

    docCleanDocumentProperties( &dpChosen );

    return rval;
    }
