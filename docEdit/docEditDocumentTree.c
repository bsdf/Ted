/************************************************************************/
/*									*/
/*  Buffer administration for 'ExternalItems' I.E. headers/footers and	*/
/*  foonotes/endnotes.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docShape.h>
#   include	<docDocumentNote.h>
#   include	"docCopyNode.h"

/************************************************************************/
/*									*/
/*  Copy a note from one paragraph to another.				*/
/*									*/
/************************************************************************/

int docCopyNote(		DocumentCopyJob *	dcj,
				DocumentField *		dfTo,
				const DocumentField *	dfFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    BufferDocument *		bdTo= eo->eoDocument;

    DocumentNote *		dnTo;
    DocumentNote *		dnFrom;

    dnFrom= docGetNoteOfField( dfFrom, dcj->dcjSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); return -1;	}

    dfTo->dfNoteIndex= docInsertNote( &dnTo, bdTo, dfTo,
				    dnFrom->dnNoteProperties.npAutoNumber );
    if  ( dfTo->dfNoteIndex < 0 )
	{ LDEB(dfTo->dfNoteIndex); return -1;	}

    /* realloc */
    dnFrom= docGetNoteOfField( dfFrom, dcj->dcjSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); return -1;	}

    if  ( dnFrom->dnDocumentTree.dtRoot )
	{
	SelectionScope	ssRoot;

	docInitSelectionScope( &ssRoot );

	ssRoot.ssTreeType= dnFrom->dnNoteProperties.npTreeType;
	ssRoot.ssSectNr= 0;
	ssRoot.ssOwnerSectNr= dfTo->dfSelectionScope.ssSectNr;
	ssRoot.ssOwnerNumber= dfTo->dfFieldNumber;

	if  ( docCopyDocumentTree( dcj, &(dnTo->dnDocumentTree), &ssRoot,
						&(dnFrom->dnDocumentTree) ) )
	    { XDEB(dnTo->dnDocumentTree.dtRoot); return -1;	}
	}

    dnTo->dnNoteProperties.npTreeType=
				dnFrom->dnNoteProperties.npTreeType;

    if  ( docCheckSeparatorItemForNoteType( bdTo,
					dnTo->dnNoteProperties.npTreeType ) )
	{ LDEB(dnTo->dnNoteProperties.npTreeType); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a shape.							*/
/*									*/
/*  1)  Children.							*/
/*  2)  Vertices.							*/
/*  2)  Wrap polygon.							*/
/*  4)  Segment information						*/
/*  5)  Text box content.						*/
/*									*/
/************************************************************************/

DrawingShape * docCopyDrawingShape(
				DocumentCopyJob *	dcj,
				DrawingShape *		from )
    {
    DrawingShape *		rval= (DrawingShape *)0;
    DrawingShape *		to;
    int				saveNumber;

    to= docClaimDrawingShape(
		    &(dcj->dcjEditOperation->eoDocument->bdShapeList) );
    if  ( ! to )
	{ XDEB(to); goto ready;	}

    saveNumber= to->dsShapeNumber;
    *to= *from;
    docInitShapeAllocated( to );
    to->dsShapeNumber= saveNumber;

    /*  1  */
    to->dsChildren= (DrawingShape **)malloc(
				from->dsChildCount* sizeof(DrawingShape *) );
    if  ( ! to->dsChildren )
	{ LXDEB(from->dsChildCount,to->dsChildren); goto ready;	}

    /*  2,3,4  */
    if  ( docCopyShapeDrawing( &(to->dsDrawing), &(from->dsDrawing) ) )
	{ LDEB(1); goto ready;	}

    /*  1  */
    while( to->dsChildCount < from->dsChildCount )
	{
	to->dsChildren[to->dsChildCount]= docCopyDrawingShape( dcj,
					from->dsChildren[to->dsChildCount] );
	if  ( ! to->dsChildren[to->dsChildCount] )
	    {
	    LXDEB(to->dsChildCount,to->dsChildren[to->dsChildCount]);
	    goto ready;
	    }
	to->dsChildCount++;
	}

    /*  5  */
    to->dsSelectionScope= dcj->dcjTargetSelectionScope;
    to->dsSelectionScope.ssTreeType= from->dsSelectionScope.ssTreeType;
    to->dsSelectionScope.ssOwnerNumber= to->dsShapeNumber;
    if  ( from->dsDocumentTree.dtRoot )
	{
	if  ( docCopyDocumentTree( dcj, &(to->dsDocumentTree),
			    &(to->dsSelectionScope), &(from->dsDocumentTree) ) )
	    { XDEB(to->dsDocumentTree.dtRoot); goto ready;	}
	}

    rval= to; to= (DrawingShape *)0; /* steal */

  ready:

    if  ( to )
	{
	EditOperation *	eo= dcj->dcjEditOperation;

	docDeleteDrawingShape( eo->eoDocument, to );
	}

    return rval;
    }

