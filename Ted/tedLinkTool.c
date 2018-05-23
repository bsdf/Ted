/************************************************************************/
/*									*/
/*  Ted: Link page on format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedAppFront.h"
#   include	"tedLinkTool.h"
#   include	"tedToolUtil.h"
#   include	<appInspector.h>
#   include	<docField.h>
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  A)  A Bookmark was chosen in the list.				*/
/*  B)  Something was typed in the 'Destination' text widgets.		*/
/*									*/
/************************************************************************/

static void tedLinkAdaptToText(	LinkTool *		lt )
    {
    int				gotSomething;
    int				changed;
    int				canSetLink;

    BookmarkList *		bl= &(lt->ltBookmarkList);

    const HyperlinkField *	hfChosen= &(lt->ltLinkChosen);
    const HyperlinkField *	hfSet= &(lt->ltLinkSet);

    lt->ltInProgrammaticChange++;

    bl->blIsLocal= ! docHyperlinkFieldIsExtern( hfChosen );
    gotSomething= docHyperlinkFieldIsLink( hfChosen );

    changed= ! docEqualHyperlinkFields( hfChosen, hfSet );
    canSetLink= lt->ltCanChange && changed && gotSomething;

    guiEnableWidget( lt->ltSetLinkButton, canSetLink );
    guiEnableWidget( lt->ltRevertButton, changed );

    guiEnableWidget( lt->ltRemoveLinkButton,
				lt->ltCanChange && gotSomething && ! changed );

    guiEnableWidget( lt->ltFollowLinkButton, gotSomething );

    guiEnableWidget( bl->blMarkListWidget,
				    lt->ltCanChange && bl->blIsLocal );

    guiEnableText( lt->ltFileTextWidget, lt->ltCanChange );
    guiEnableText( bl->blMarkTextWidget, lt->ltCanChange );

    lt->ltInProgrammaticChange--;

    return;
    }

static void tedLinkAdaptToValues(	LinkTool *		lt,
					int			adaptFile,
					int			adaptMark,
					int			search,
					int			select )
    {
    BookmarkList *		bl= &(lt->ltBookmarkList);
    const HyperlinkField *	hfChosen= &(lt->ltLinkChosen);

    bl->blIsLocal= ! docHyperlinkFieldIsExtern( hfChosen );

    if  ( search )
	{
	tedBookmarkFindChosen( bl, lt->ltApplication, &(hfChosen->hfBookmark) );
	}

    lt->ltInProgrammaticChange++;

    if  ( adaptFile)
	{
	guiBufferToText( lt->ltFileTextWidget, &(hfChosen->hfFile) );
	}

    if  ( adaptMark )
	{
	guiBufferToText( bl->blMarkTextWidget, &(hfChosen->hfBookmark) );
	}

    if  ( select )
	{
	tedBookmarkUpdateSelectionInList( bl, &(hfChosen->hfBookmark) );
	}

    tedLinkAdaptToText( lt );

    lt->ltInProgrammaticChange--;

    return;
    }

/*  A  */
static APP_LIST_CALLBACK_H( tedLinkBookmarkChosen, w, voidlt, voidlcs )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);
    HyperlinkField *		hfChosen= &(lt->ltLinkChosen);

    lt->ltInProgrammaticChange++;

    tedBookmarkListToText( bl, &(hfChosen->hfBookmark), voidlcs, w );
    tedLinkAdaptToText( lt );

    lt->ltInProgrammaticChange--;

    return;
    }

/*  B  */
static APP_TXTYPING_CALLBACK_H( tedLinkFileChanged, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;

    if  ( ! lt->ltInProgrammaticChange )
	{
	HyperlinkField *	hfChosen= &(lt->ltLinkChosen);

	const int		adaptToFile= 0;
	const int		adaptToMark= 0;
	const int		search= 1;
	const int		select= 1;

	if  ( appBufferFromTextWidget( &(hfChosen->hfFile),
						    lt->ltFileTextWidget ) )
	    { LDEB(1); return;	}

	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );
	}

    return;
    }

static APP_TXTYPING_CALLBACK_H( tedLinkMarkChanged, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);

    if  ( ! lt->ltInProgrammaticChange )
	{
	HyperlinkField *	hfChosen= &(lt->ltLinkChosen);

	const int		adaptToFile= 0;
	const int		adaptToMark= 0;
	const int		search= 1;
	const int		select= 1;

	if  ( appBufferFromTextWidget( &(hfChosen->hfBookmark),
						    bl->blMarkTextWidget ) )
	    { LDEB(1); return;	}

	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Revert' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkRevertPushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;

    const int		adaptToFile= 1;
    const int		adaptToMark= 1;
    const int		search= 1;
    const int		select= 1;

    if  ( docCopyHyperlinkField( &(lt->ltLinkChosen), &(lt->ltLinkSet) ) )
	{ LDEB(1); return;	}

    tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );

    return;
    }

/************************************************************************/
/*									*/
/*  'Change' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkChangePushed, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);
    HyperlinkField *		hfChosen= &(lt->ltLinkChosen);
    EditApplication *		ea= lt->ltApplication;

    if  ( appBufferFromTextWidget( &(hfChosen->hfFile),
						    lt->ltFileTextWidget ) )
	{ LDEB(1); return;	}
    if  ( appBufferFromTextWidget( &(hfChosen->hfBookmark),
						    bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}

    if  ( docAdaptBookmarkName( &(hfChosen->hfBookmark) ) )
	{
	lt->ltInProgrammaticChange++;

	guiBufferToText( bl->blMarkTextWidget,
					    &(hfChosen->hfBookmark) );
	appRefuseTextValue( bl->blMarkTextWidget );

	lt->ltInProgrammaticChange--;
	return;
	}

    if  ( tedAppSetHyperlink( ea, hfChosen ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Follow Link' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkFollowPushed, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);
    HyperlinkField *		hfChosen= &(lt->ltLinkChosen);
    EditApplication *		ea= lt->ltApplication;

    if  ( appBufferFromTextWidget( &(hfChosen->hfFile),
						    lt->ltFileTextWidget ) )
	{ LDEB(1); return;	}
    if  ( appBufferFromTextWidget( &(hfChosen->hfBookmark),
						    bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}

    if  ( tedAppFollowLink( w, ea,
			    &(hfChosen->hfFile), &(hfChosen->hfBookmark) ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Remove Link' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkRemovePushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    EditApplication *	ea= lt->ltApplication;

    if  ( tedAppRemoveHyperlink( ea ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a link tool. I.E. the 'Link' page on the format tool to manage	*/
/*  Hyperlinks.								*/
/*									*/
/************************************************************************/

void tedFillLinkTool(		LinkTool *			lt,
				const LinkToolResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    BookmarkList *	bl= &(lt->ltBookmarkList);
    APP_WIDGET		label;
    APP_WIDGET		row;

    const int		listVisibleItems= 5;

    lt->ltPageResources= lpr;

    /* FULL WIDTH */
    appMakeLabelInColumn( &label, pageWidget, lpr->lprFileText );
    appMakeTextInColumn( &(lt->ltFileTextWidget), pageWidget, 0, 1 );
    appMakeLabelInColumn( &(bl->blMarkLabelWidget),
				pageWidget, lpr->lprMarkText );
    appMakeTextInColumn( &(bl->blMarkTextWidget), pageWidget, 0, 1 );

    appGuiSetTypingCallbackForText( lt->ltFileTextWidget,
					    tedLinkFileChanged, (void *)lt );
    appGuiSetTypingCallbackForText( bl->blMarkTextWidget,
					    tedLinkMarkChanged, (void *)lt );

    appGuiMakeListInColumn( &(bl->blMarkListWidget),
		pageWidget, listVisibleItems,
		tedLinkBookmarkChosen, (APP_BUTTON_CALLBACK_T)0, (void *)lt );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(lt->ltFollowLinkButton), &(lt->ltRemoveLinkButton),
	    isr->isrSelectButtonText, isr->isrDeleteButtonText,
	    tedLinkFollowPushed, tedLinkRemovePushed, (void *)lt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedLinkRevertPushed, tedLinkChangePushed, (void *)lt );

    lt->ltSetLinkButton= is->isApplyButton;
    lt->ltRevertButton= is->isRevertButton;

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a link tool					*/
/*									*/
/************************************************************************/

void tedInitLinkTool(		LinkTool *		lt )
    {
    lt->ltCurrentDocumentId= 0;

    tedInitBookmarkList( &(lt->ltBookmarkList) );

    lt->ltSetLinkButton= (APP_WIDGET)0;
    lt->ltRevertButton= (APP_WIDGET)0;
    lt->ltFollowLinkButton= (APP_WIDGET)0;
    lt->ltRemoveLinkButton= (APP_WIDGET)0;

    docInitHyperlinkField( &(lt->ltLinkChosen) );
    docInitHyperlinkField( &(lt->ltLinkSet) );

    lt->ltInProgrammaticChange= 0;

    return;
    }

void tedFormatCleanLinkTool(		LinkTool *		lt )
    {
    docCleanHyperlinkField( &(lt->ltLinkChosen) );
    docCleanHyperlinkField( &(lt->ltLinkSet) );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the tabs page.						*/
/*									*/
/************************************************************************/

void tedLinkToolFillChoosers(	LinkTool *			lt )
    {
    return;
    }

void tedFinishLinkTool(		LinkTool *			lt )
    {
    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the link tool.						*/
/*									*/
/************************************************************************/

void tedRefreshLinkTool(	LinkTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    DocumentField *		dfHyperlink;

    lt->ltCanChange= cmdEnabled[EDITcmdSET_HYPERLINK];

    if  ( lt->ltCurrentDocumentId != sd->sdDocumentId )
	{
	const int	includeTocMarks= 0;

	lt->ltInProgrammaticChange++;
	tedFillBookmarkList( &(lt->ltBookmarkList), includeTocMarks, bd );
	lt->ltInProgrammaticChange--;

	lt->ltCurrentDocumentId= sd->sdDocumentId;
	}

    docCleanHyperlinkField( &(lt->ltLinkChosen) );
    docInitHyperlinkField( &(lt->ltLinkChosen) );

    docCleanHyperlinkField( &(lt->ltLinkSet) );
    docInitHyperlinkField( &(lt->ltLinkSet) );

    dfHyperlink= docFindTypedFieldForPosition( bd, &(ds->dsHead),
							DOCfkHYPERLINK, 0 );
    if  ( dfHyperlink )
	{
	const int		adaptToFile= 1;
	const int		adaptToMark= 1;
	const int		search= 1;
	const int		select= 1;

	if  ( docGetHyperlinkField( &(lt->ltLinkSet), dfHyperlink ) )
	    { LDEB(1); return;	}
	if  ( docCopyHyperlinkField( &(lt->ltLinkChosen), &(lt->ltLinkSet) ) )
	    { LDEB(1); return;	}

	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );

	*pPref += 3;
	}
    else{
	const int		adaptToFile= 1;
	const int		adaptToMark= 1;
	const int		search= 0;
	const int		select= 1;

	lt->ltBookmarkList.blMarkChosenExists= 0;
	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );
	}

    guiEnableWidget( lt->ltSetLinkButton, 0 && lt->ltCanChange );
    guiEnableWidget( lt->ltRevertButton, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Get link tool resources.						*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedLinkSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolLink",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Hyperlink" ),

    APP_RESOURCE( "hyperlinkSetLink",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set Link" ),
    APP_RESOURCE( "hyperlinkRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),

    APP_RESOURCE( "hyperlinkFollowLink",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Follow Link" ),
    APP_RESOURCE( "hyperlinkRemoveLink",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Remove Link" ),
    };

static AppConfigurableResource TED_TedLinkToolResourceTable[]=
{
    APP_RESOURCE( "hyperlinkDocument",
		offsetof(LinkToolResources,lprFileText),
		"Document" ),
    APP_RESOURCE( "hyperlinkBookmark",
		offsetof(LinkToolResources,lprMarkText),
		"Bookmark" ),
};

void tedLinkToolGetResourceTable(	EditApplication *		ea,
					LinkToolResources *		lpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)lpr,
				TED_TedLinkToolResourceTable,
				sizeof(TED_TedLinkToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedLinkSubjectResourceTable,
				sizeof(TED_TedLinkSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
