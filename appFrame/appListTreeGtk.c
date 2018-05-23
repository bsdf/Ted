#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"
#   include	"appListGtk.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  List specific functionality. (Implemented with a GtkTreeView)	*/
/*									*/
/************************************************************************/

static gboolean appGuiGtkHandlePotentialListAction(
					GtkTreeView *		treev,
					GtkTreePath *		path,
					GtkTreeViewColumn *	column,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la && la->laActionCallback )
	{
	(*la->laActionCallback)( la->laWidget, la->laThrough );
	return TRUE;
	}

    return FALSE;
    }

static gboolean appGuiGtkHandlePotentialListCallback(
					GtkTreeSelection *	selection,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la && la->laListCallback )
	{
	int	position;

	position= appGuiGetPositionFromListCallback( la->laWidget, selection );

	appGuiGtkSetListAdjustment( voidla, position );

	(*la->laListCallback)( la->laWidget, selection, la->laThrough );
	return TRUE;
	}

    return FALSE;
    }

static gboolean appGuiGtkListCursorChanged(
					GtkTreeView *		treev,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la )
	{
	GtkTreeSelection *	selection;
	int			position;

	selection= gtk_tree_view_get_selection( treev );
	position= appGuiGetPositionFromListCallback(
					    GTK_WIDGET( treev ), selection );
	appGuiGtkSetListAdjustment( voidla, position );

	return TRUE;
	}

    return FALSE;
    }

void appGuiMakeListInColumn(	APP_WIDGET *		pList,
				APP_WIDGET		column,
				int			visibleItems,
				APP_LIST_CALLBACK_T	listCallback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    GtkListStore *	store= gtk_list_store_new( 1, G_TYPE_STRING );
    APP_WIDGET		tree;

    GtkAdjustment *	vadjustment= (GtkAdjustment *)0;
    APP_WIDGET		scrolled_window= (APP_WIDGET)0;

    GtkTreeSelection *	selection;
    void *		voidla;

    tree= gtk_tree_view_new_with_model( GTK_TREE_MODEL( store ) );
    gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( tree ), FALSE );

    if  ( visibleItems >= 0 )
	{
	scrolled_window= appListGtkMakeParent( &vadjustment, column );

	gtk_scrolled_window_add_with_viewport(
			    GTK_SCROLLED_WINDOW( scrolled_window ), tree );

	/*
	gtk_scrollable_set_vadjustment( GTK_SCROLLABLE( tree ), vadjustment );
	gtk_tree_view_set_vadjustment( GTK_TREE_VIEW( tree ), vadjustment );
	*/
	}
    else{
	gboolean	fill= TRUE;
	gboolean	expand= TRUE;

	gtk_box_pack_start( GTK_BOX( column ), tree, expand, fill, 0 );
	}

    gtk_widget_show( tree );

    gtk_tree_view_insert_column_with_attributes(
				GTK_TREE_VIEW( tree ),
				-1,
				"col",
				gtk_cell_renderer_text_new(),
				"text", 0, NULL );

    voidla= appListGtkSetCallbacks( tree, scrolled_window, vadjustment,
				    listCallback, actionCallback, through );
    if  ( ! voidla )
	{ XDEB(voidla); return;	}

    selection= gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );
    gtk_tree_selection_set_mode( selection, GTK_SELECTION_SINGLE );

    if  ( listCallback )
	{
	g_signal_connect( G_OBJECT( selection ), "changed",
		(GtkSignalFunc)appGuiGtkHandlePotentialListCallback, voidla );
	}

    if  ( actionCallback )
	{
	gtk_signal_connect( GTK_OBJECT( tree ), "row-activated",
		(GtkSignalFunc)appGuiGtkHandlePotentialListAction, voidla );
	}

    gtk_signal_connect( GTK_OBJECT( tree ), "cursor-changed",
		(GtkSignalFunc)appGuiGtkListCursorChanged, voidla );

    if  ( visibleItems > 0 )
	{
	appListGtkSetVisibleSize( tree, scrolled_window, visibleItems );
	}

    *pList= tree;
    }

void appGuiAddValueToListWidget(	APP_WIDGET		tree,
					int			position,
					const char *		value )
    {
    const int		col= 0;

    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkTreeModel *	model= gtk_tree_view_get_model( treev );
    GtkListStore *	store= GTK_LIST_STORE( model );

    GtkTreePath *	ppath= (GtkTreePath *)0;
    GtkTreeIter		iter;

    ListAction *	la= appListGtkGetListAction( tree );

    if  ( position >= 0 && position < la->laItemCount )
	{
	GtkTreeIter	positer;

	ppath= gtk_tree_path_new_from_indices( position, -1 );
	gtk_tree_model_get_iter( model, &positer, ppath );

	gtk_list_store_insert_before( store, &iter, &positer );
	}
    else{
	gtk_list_store_append( store, &iter );
	}

    if  ( la )
	{ la->laItemCount++;	}

    gtk_list_store_set( store, &iter, col, value, -1 );

    if  ( ppath )
	{ gtk_tree_path_free( ppath );	}
    }

void appGuiSelectPositionInListWidget(		APP_WIDGET	tree,
						int		position )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkTreePath *	path;
    GtkTreeSelection *	selection;
    ListAction *	la= appListGtkGetListAction( tree );

    selection= gtk_tree_view_get_selection( treev );

    path= gtk_tree_path_new_from_indices( position, -1 );
    gtk_tree_selection_select_path( selection, path );

    if  ( la && la->laAdjustment && la->laItemCount > 0 )
	{ appGuiGtkSetListAdjustment( la, position );	}

    gtk_tree_path_free( path );
    }

void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	tree )
    {
    GtkTreeSelection *	selection;
    GtkTreeModel *	model;
    GtkTreeIter		iter;

    ListAction *	la= appListGtkGetListAction( tree );

    if  ( la )
	{
	la->laSelectedPosition= -1;
	}

    selection= gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    if  ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
        {
	gtk_tree_selection_unselect_iter( selection, &iter );
	}

    gtk_tree_selection_unselect_all( selection );
    }

void appGuiEmptyListWidget(	APP_WIDGET		tree )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkListStore *	store;
    ListAction *	la= appListGtkGetListAction( tree );

    if  ( la )
	{
	la->laItemCount= 0;
	la->laSelectedPosition= -1;
	}

    store= GTK_LIST_STORE( gtk_tree_view_get_model( treev ) );

    gtk_list_store_clear( store );
    }

int appGuiGetPositionFromListCallback(	APP_WIDGET		tree,
					GtkTreeSelection *	selection )
    {
    GtkTreeModel *	model;
    GtkTreeIter		iter;

    if  ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
        {
	GtkTreePath *	path= gtk_tree_model_get_path( model, &iter );
	int		idx;

	idx= gtk_tree_path_get_indices( path )[0];

	gtk_tree_path_free( path );
	     
	return idx;
	}

    return -1;
    }

void appFreeStringFromListCallback(	char *		s )
    { g_free( s );	}

char * appGuiGetStringFromListCallback(	APP_WIDGET		tree,
					GtkTreeSelection *	selection )
    {
    GtkTreeModel *	model;
    GtkTreeIter		iter;

    if  ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
        {
	gchar *		name;

	gtk_tree_model_get( model, &iter, 0, &name, -1 );
	return name;
	}

    return (char *)0;
    }

void appGuiReplaceValueInListWidget(	APP_WIDGET	tree,
					int		position,
					const char *	value )
    {
    const int		col= 0;

    GtkTreeView *	treev;
    GtkTreeModel *	model;
    GtkListStore *	store;

    GtkTreePath *	path;
    GtkTreeIter		iter;

    treev= GTK_TREE_VIEW( tree );
    model= gtk_tree_view_get_model( treev );
    store= GTK_LIST_STORE( model );

    path= gtk_tree_path_new_from_indices( position, -1 );
    gtk_tree_model_get_iter( model, &iter, path );

    gtk_list_store_set( store, &iter, col, value, -1 );

    gtk_tree_path_free( path );
    }

typedef struct FindValue
    {
    gboolean		fvFound;
    GtkTreeIter		fvIter;
    const char *	fvValue;
    } FindValue;

static gboolean appGuiGtkFindValue(		GtkTreeModel *	model,
						GtkTreePath *	path,
						GtkTreeIter *	iter,
						void *		vfv )
    {
    FindValue *	fv= (FindValue *)vfv;
    gchar *	value;

    gtk_tree_model_get( model, iter, 0, &value, -1);
    fv->fvFound= ! strcmp( fv->fvValue, value );
    if  ( fv->fvFound )
	{ fv->fvIter= *iter;	}
    g_free( value );

    return fv->fvFound;
    }

void appGuiSelectValueInListWidget(		APP_WIDGET	tree,
						const char *	value )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkTreeModel *	model= gtk_tree_view_get_model( treev );
    GtkTreeSelection *	selection= gtk_tree_view_get_selection( treev );
    ListAction *	la= appListGtkGetListAction( tree );

    FindValue		fv;

    fv.fvFound= FALSE;
    fv.fvValue= value;

    gtk_tree_model_foreach( model, appGuiGtkFindValue, (void *)&fv );

    if  ( fv.fvFound )
	{
	gtk_tree_selection_select_iter( selection, &(fv.fvIter) );

	if  ( la && la->laAdjustment && la->laItemCount > 0 )
	    {
	    GtkTreePath *	path;
	    int			position;

	    path= gtk_tree_model_get_path( model, &(fv.fvIter) );
	    position= gtk_tree_path_get_indices( path )[0];

	    appGuiGtkSetListAdjustment( la, position );

	    gtk_tree_path_free( path );
	    }
	}
    }

#   endif
