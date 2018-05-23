/************************************************************************/
/*  Ted: interaction with the page layout tool.				*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedAppResources.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docScreenLayout.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reformat and redisplay the document because of a major format	*/
/*  change. etc.							*/
/*									*/
/************************************************************************/

void tedRedoDocumentLayout(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				hasSelection= tedHasSelection( ed );
    int				reachedBottom= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    tedLayoutDocumentBody( &reachedBottom, &lc );

    if  ( tedOpenTreeObjects( &(bd->bdBody), &lc ) )
	{ LDEB(1); 	}

    if  ( reachedBottom )
	{
	BufferItem *	rootNode= bd->bdBody.dtRoot;

	docGetPixelRectangleForPages( &(ed->edFullRect), &lc,
					    rootNode->biTopPosition.lpPage,
					    rootNode->biBelowPosition.lpPage );
	}

    if  ( hasSelection )
	{ tedDelimitCurrentSelection( ed );	}

    appDocSetScrollbarValues( ed );

    if  ( hasSelection )
	{
	DocumentSelection		ds;
	SelectionGeometry		sg;
	SelectionDescription		sd;
	BufferItem *			bodySectNode;

	if  ( tedGetSelection( &ds, &sg, &sd,
				    (DocumentTree **)0, &bodySectNode, ed ) )
	    { LDEB(1); return;	}

	tedDocAdaptTopRuler( ed, &ds, &sg, &sd, bodySectNode );

	tedScrollToSelection( ed, (int *)0, (int *)0 );

	tedDescribeSelection( ed );

	if  ( td->tdSelectionDescription.sdIsObjectSelection )
	    { tedMoveObjectWindows( ed );	}
	}

    guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the page tool to a document.					*/
/*									*/
/************************************************************************/

void tedAdaptPageToolToDocument(	EditApplication *	ea,
					EditDocument *		ed )
    {
    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    if  ( ! ea->eaPageTool )
	{ return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    appPageToolSetProperties( ea->eaPageTool, &(dp->dpGeometry) );

    appEnablePageTool( ea->eaPageTool, ! ed->edIsReadonly );

    return;
    }

/************************************************************************/
/*									*/
/*  (re)Calculate the layout of a whole document.			*/
/*									*/
/************************************************************************/

int tedLayoutDocumentBody(	int *				pReachedBottom,
				const LayoutContext *		lc )
    {
    return docScreenLayoutDocumentBody( pReachedBottom, lc->lcDocument, lc );
    }

/************************************************************************/
/*									*/
/*  Get a suggestion about the line height: Used for the initial value	*/
/*  for space before/after in the format tool.				*/
/*									*/
/************************************************************************/

int tedGetParaLineHeight(	int *			pLineHeight,
				EditDocument *		ed )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    int				fontSize= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    if  ( docLayoutParagraphLineExtents( &fontSize, &lc, ds.dsHead.dpNode ) )
	{ LDEB(1); return -1;	}

    *pLineHeight= fontSize;
    return 0;
    }

