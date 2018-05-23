/************************************************************************/
/*									*/
/*  Ted: Bookmark dialog.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"tedAppFront.h"
#   include	"tedBookmarkTool.h"
#   include	"tedToolUtil.h"
#   include	<appInspector.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docField.h>
#   include	<docBookmarkField.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  A)  A Bookmark was chosen in the list.				*/
/*  B)  Something was typed in the 'Destination' text widgets.		*/
/*									*/
/************************************************************************/

static void tedBookmarkAdaptToText(	const BookmarkTool *	bt,
					int			adaptString )
    {
    const BookmarkList *	bl= &(bt->btBookmarkList);

    int				gotChosen;
    int				changed;

    changed= utilMemoryCompareBuffers( &(bt->btMarkChosen), &(bt->btMarkSet) );
    gotChosen= ! utilMemoryBufferIsEmpty( &(bt->btMarkChosen) );

    guiEnableWidget( bt->btSetBookmarkButton,
		    ! bl->blMarkChosenExists		&&
		    bt->btCanChange			&&
		    changed && gotChosen		);

    guiEnableWidget( bt->btRevertButton, changed );

    guiEnableWidget( bt->btRemoveBookmarkButton,
		     bt->btCanChange			&&
		     ! changed && gotChosen		);

    guiEnableWidget( bt->btGoToBookmarkButton,
		    bl->blMarkChosenExists && gotChosen );

    guiEnableWidget( bl->blMarkListWidget, bl->blIsLocal );
    guiEnableText( bl->blMarkTextWidget, bt->btCanChange );

    if  ( adaptString )
	{
	guiBufferToText( bl->blMarkTextWidget, &(bt->btMarkChosen) );
	}

    return;
    }

static void tedBookmarkAdaptToBookmark(	BookmarkTool *		bt,
					int			adaptString,
					int			select )
    {
    tedBookmarkFindChosen( &(bt->btBookmarkList), bt->btApplication,
							&(bt->btMarkChosen) );

    if  ( select )
	{
	tedBookmarkUpdateSelectionInList( &(bt->btBookmarkList),
							&(bt->btMarkChosen) );
	}

    tedBookmarkAdaptToText( bt, adaptString );
    }

/************************************************************************/
/*									*/
/*  'Revert' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedBookmarkRevertPushed, w, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const int			adaptString= 1;
    const int			select= 1;

    utilCopyMemoryBuffer( &(bt->btMarkChosen), &(bt->btMarkSet) );

    bt->btInProgrammaticChange++;

    tedBookmarkAdaptToBookmark( bt, adaptString, select );

    bt->btInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Reset the document id to force the tool to refresh.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedBookmarkChangePushed, button, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const BookmarkList *	bl= &(bt->btBookmarkList);

    if  ( appBufferFromTextWidget( &(bt->btMarkChosen),
						bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}
    else{
	if  ( docAdaptBookmarkName( &(bt->btMarkChosen) ) )
	    {
	    bt->btInProgrammaticChange++;

	    guiBufferToText( bl->blMarkTextWidget,
						    &(bt->btMarkChosen) );
	    appRefuseTextValue( bl->blMarkTextWidget );

	    bt->btInProgrammaticChange--;
	    return;
	    }

	/*  1  */
	bt->btCurrentDocumentId= 0;

	if  ( tedAppSetBookmark( bt->btApplication, &(bt->btMarkChosen) ) )
	    { LDEB(1);	}
	}
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkGotoPushed, button, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const BookmarkList *	bl= &(bt->btBookmarkList);

    if  ( appBufferFromTextWidget( &(bt->btMarkChosen),
						bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}
    else{
	if  ( tedAppGoToBookmark( bt->btApplication, &(bt->btMarkChosen) ) )
	    { LDEB(1);	}
	}
    }

/************************************************************************/
/*									*/
/*  Remove the selected bookmark.					*/
/*									*/
/*  1)  Reset the document id to force the tool to refresh.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedBookmarkRemovePushed, w, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;

    /*  1  */
    bt->btCurrentDocumentId= 0;

    if  ( tedAppRemoveBookmark( bt->btApplication ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Typing happened in the 'Bookmark' text widget.			*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedBookmarkDestinationChanged, w, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const BookmarkList *	bl= &(bt->btBookmarkList);

    const int			adaptString= 0;
    const int			select= 1;

    guiEnableWidget( bt->btRemoveBookmarkButton, 0 );

    if  ( appBufferFromTextWidget( &(bt->btMarkChosen),
						bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}

    tedBookmarkAdaptToBookmark( bt, adaptString, select );

    return;
    }

/************************************************************************/
/*									*/
/*  A Bookmark was chosen in the list.					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedBookmarkChosen, w, voidbt, voidlcs )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    BookmarkList *		bl= &(bt->btBookmarkList);
    const int			adaptString= 0;

    char *			markName= (char *)0;

    if  ( bt->btInProgrammaticChange )
	{ return;	}

    bt->btInProgrammaticChange++;

    if  ( bt->btCanChange )
	{
	tedBookmarkListToText( bl, &(bt->btMarkChosen), voidlcs, w );
	tedBookmarkAdaptToText( bt, adaptString );
	}
    else{
	markName= appGuiGetStringFromListCallback( w, voidlcs );
	if  ( ! markName )
	    { XDEB(markName); goto ready;	}

	if  ( utilMemoryBufferSetString( &(bt->btMarkChosen), markName ) )
	    { SDEB(markName); goto ready;	}

	if  ( tedAppGoToBookmark( bt->btApplication, &(bt->btMarkChosen) ) )
	    { LDEB(1); goto ready;	}
	}

  ready:

    if  ( markName )
	{ appFreeStringFromListCallback( markName );	}

    bt->btInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  Make a bookmark tool. I.E. the 'Bookmark' page on the format tool	*/
/*  to manage Hyperlinks.						*/
/*									*/
/************************************************************************/

void tedFillBookmarkTool(	BookmarkTool *			bt,
				const BookmarkPageResources *	bpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    BookmarkList *	bl= &(bt->btBookmarkList);
    APP_WIDGET		row;

    const int		listVisibleItems= 5;

    bt->btResources= bpr;

    /* FULL WIDTH */
    appMakeLabelInColumn( &(bl->blMarkLabelWidget),
				pageWidget, bpr->bprMarkText );
    appMakeTextInColumn( &(bl->blMarkTextWidget), pageWidget, 0, 1 );

    appGuiSetTypingCallbackForText( bl->blMarkTextWidget,
				tedBookmarkDestinationChanged, (void *)bt );

    appGuiMakeListInColumn( &(bl->blMarkListWidget),
		pageWidget, listVisibleItems,
		tedBookmarkChosen, (APP_BUTTON_CALLBACK_T)0, (void *)bt );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(bt->btGoToBookmarkButton), &(bt->btRemoveBookmarkButton),
	    isr->isrSelectButtonText, isr->isrDeleteButtonText,
	    tedBookmarkGotoPushed, tedBookmarkRemovePushed, (void *)bt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedBookmarkRevertPushed, tedBookmarkChangePushed, (void *)bt );

    bt->btSetBookmarkButton= is->isApplyButton;
    bt->btRevertButton= is->isRevertButton;

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a bookmark tool					*/
/*									*/
/************************************************************************/

void tedInitBookmarkTool(		BookmarkTool *		bt )
    {
    bt->btCurrentDocumentId= 0;

    tedInitBookmarkList( &(bt->btBookmarkList) );
    bt->btBookmarkList.blIsLocal= 1;

    bt->btSetBookmarkButton= (APP_WIDGET)0;
    bt->btRevertButton= (APP_WIDGET)0;
    bt->btGoToBookmarkButton= (APP_WIDGET)0;
    bt->btRemoveBookmarkButton= (APP_WIDGET)0;

    utilInitMemoryBuffer( &(bt->btMarkSet) );
    utilInitMemoryBuffer( &(bt->btMarkChosen) );

    bt->btInProgrammaticChange= 0;

    return;
    }

void tedFormatCleanBookmarkTool(		BookmarkTool *		bt )
    {
    utilCleanMemoryBuffer( &(bt->btMarkSet) );
    utilCleanMemoryBuffer( &(bt->btMarkChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the bookmarks page.						*/
/*									*/
/************************************************************************/

void tedBookmarkToolFillChoosers( BookmarkTool *		bt )
    {
    return;
    }

void tedFinishBookmarkTool(	BookmarkTool *			bt )
    {
    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the bookmark tool.						*/
/*									*/
/************************************************************************/

void tedRefreshBookmarkTool(	BookmarkTool *			bt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				DocumentTree *			ei,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    const int			adaptString= 1;

    bt->btInProgrammaticChange++;

    if  ( bt->btCurrentDocumentId != sd->sdDocumentId )
	{
	const BookmarkPageResources *	bpr= bt->btResources;
	int				includeTocMarks= 0;

	if  ( bpr->bprShowTocMarks && ! strcmp( bpr->bprShowTocMarks, "1" ) )
	    { includeTocMarks= 1;	}

	tedFillBookmarkList( &(bt->btBookmarkList), includeTocMarks, bd );

	bt->btCurrentDocumentId= sd->sdDocumentId;
	}

    bt->btCanChange= cmdEnabled[EDITcmdSET_BOOKMARK];

    utilCleanMemoryBuffer( &(bt->btMarkSet) );
    utilInitMemoryBuffer( &(bt->btMarkSet) );
    utilCleanMemoryBuffer( &(bt->btMarkChosen) );
    utilInitMemoryBuffer( &(bt->btMarkChosen) );

    if  ( sd->sdHeadInBookmark )
	{
	DocumentField *		dfBookmark;

	const MemoryBuffer *	markName= (const MemoryBuffer *)0;

	dfBookmark= docFindTypedFieldForPosition( bd, &(ds->dsHead),
							    DOCfkBOOKMARK, 0 );
	if  ( docFieldGetBookmark( &markName, dfBookmark ) )
	    { LDEB(1); *pEnabled= 0; goto ready;	}

	if  ( utilCopyMemoryBuffer( &(bt->btMarkSet), markName ) )
	    { LDEB(1); *pEnabled= 0; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(bt->btMarkChosen), &(bt->btMarkSet) ) )
	    { LDEB(1); *pEnabled= 0; goto ready;	}

	bt->btBookmarkList.blMarkChosenExists= 1;
	tedBookmarkAdaptToText( bt, adaptString );

	if  ( ! docIsTocBookmark( (long *)0, dfBookmark ) )
	    {
	    *pPref += 3;

	    tedBookmarkUpdateSelectionInList( &(bt->btBookmarkList),
							&(bt->btMarkChosen) );
	    }
	}
    else{
	const int	select= 1;

	if  ( ! sd->sdIsIBarSelection )
	    { docSuggestNewBookmarkName( &(bt->btMarkChosen), bd, ds );	}

	tedBookmarkAdaptToBookmark( bt, adaptString, select );
	}

  ready:

    bt->btInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedBookmarkSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolBookmark",
	    offsetof(InspectorSubjectResources,isrSubjectName),
	    "Bookmark" ),

    APP_RESOURCE( "bookmarkSetBookmark",
	    offsetof(InspectorSubjectResources,isrApplyToSubject),
	    "Set Bookmark" ),
    APP_RESOURCE( "bookmarkRevert",
	    offsetof(InspectorSubjectResources,isrRevert),
	    "Revert" ),

    APP_RESOURCE( "bookmarkGoToBookmark",
	    offsetof(InspectorSubjectResources,isrSelectButtonText),
	    "Go To Bookmark" ),
    APP_RESOURCE( "bookmarkRemoveBookmark",
	    offsetof(InspectorSubjectResources,isrDeleteButtonText),
	    "Remove Bookmark" ),
    };

static AppConfigurableResource TED_TedBookmarkToolResourceTable[]=
    {
    APP_RESOURCE( "bookmarkBookmark",
	    offsetof(BookmarkPageResources,bprMarkText),
	    "Bookmark" ),
    APP_RESOURCE( "bookmarkShowTocMarks",
	    offsetof(BookmarkPageResources,bprShowTocMarks),
	    "0" ),
    };

void tedBookmarkToolGetResourceTable(	EditApplication *		ea,
					BookmarkPageResources *		bpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)bpr,
				TED_TedBookmarkToolResourceTable,
				sizeof(TED_TedBookmarkToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedBookmarkSubjectResourceTable,
				sizeof(TED_TedBookmarkSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
