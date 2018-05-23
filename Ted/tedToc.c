/************************************************************************/
/*									*/
/*  Ted: Manipulation of TOC fields.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedEdit.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	"tedLayout.h"
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docRecalculateTocField.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh a table of contents.					*/
/*									*/
/************************************************************************/

int tedRefreshTocField(		DocumentSelection *	dsAroundToc,
				TedEditOperation *	teo,
				DocumentField *		dfToc )
    {
    const LayoutContext *	lc= &(teo->teoLayoutContext);
    EditOperation *		eo= &(teo->teoEo);

    int				headPart= -1;
    int				tailPart= -1;

    int				reachedBottom= 0;
    DocumentSelection		dsInsideToc;

    if  ( docRecalculateOneTocField( eo->eoDocument, dfToc ) )
	{ LDEB(1); return -1;	}

    if  ( tedLayoutDocumentBody( &reachedBottom, &(teo->teoLayoutContext) ) )
	{ LDEB(1); return -1;	}

    {
    int			page= 0;
    BufferDocument *	bd= eo->eoDocument;
    RecalculateFields	rf;

    docInitRecalculateFields( &rf );

    rf.rfDocument= bd;
    rf.rfCloseObject= lc->lcCloseObject;
    rf.rfUpdateFlags= FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoDOC_INFO;
    rf.rfFieldsUpdated= 0;

    rf.rfBodySectNode= bd->bdBody.dtRoot->biChildren[0];

    if  ( docRecalculateTextLevelFieldsInDocumentTree( &rf, &(bd->bdBody),
				    bd->bdBody.dtRoot->biChildren[0], page ) )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0					&&
	  tedLayoutDocumentBody( &reachedBottom,
					  &(teo->teoLayoutContext) )	)
	{ LDEB(1); return -1;	}
    }

    if  ( docDelimitFieldInDoc( &dsInsideToc, dsAroundToc,
				&headPart, &tailPart, eo->eoDocument, dfToc ) )
	{ LDEB(1); return -1; }

    docEditIncludeNodeInReformatRange( eo, dsAroundToc->dsHead.dpNode );
    docEditIncludeNodeInReformatRange( eo, dsAroundToc->dsTail.dpNode );

    docSetEditPosition( &(eo->eoAffectedRange.erTail),
						&(dsAroundToc->dsTail) );

    if  ( reachedBottom )
	{ teo->teoRefreshScreenRectangle= 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a table of contents.						*/
/*									*/
/************************************************************************/

void tedDocAddTocField(		EditDocument *		ed,
				const TocField *	tf,
				int			traced )
    {
    DocumentSelection		dsInsideToc;
    DocumentSelection		dsAroundToc;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    SelectionGeometry		sg;
    SelectionDescription	sd;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    int				headPart;
    int				tailPart;

    DocumentField *		dfToc;
    DocumentSelection		dsTraced;

    FieldInstructions		fi;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fi );

    if  ( docTocFieldSetTocInstructions( &fi, tf ) )
	{ LDEB(1); goto ready;	}

    utilInitTextAttribute( &taSet );
    utilPropMaskClear( &taSetMask );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( eo->eoSelectionScope.ssTreeType != DOCinBODY )
	{ LDEB(eo->eoSelectionScope.ssTreeType); return;	}

    if  ( tedEditStartReplace( &dsTraced, &teo,
				    EDITcmdREPLACE_BODY_LEVEL, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    if  ( tedDocReplaceSelectionWithField( &teo, &dfToc,
					&headPart, &tailPart,
					&dsInsideToc,
					&dsAroundToc,
					&fi, DOCfkTOC,
					&taSetMask, &taSet ) )
	{ LDEB(1); goto ready;	}

    if  ( tedRefreshTocField( &dsAroundToc, &teo, dfToc ) )
	{ LDEB(1);	}

    /*  5  */
    tedEditFinishSelection( &teo, &dsAroundToc );

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposALL ) )
	    { LDEB(1); goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:

    docCleanFieldInstructions( &fi );

    tedCleanEditOperation( &teo );

    return;
    }

/************************************************************************/
/*									*/
/*  Set and/or refresh a table of contents field.			*/
/*									*/
/************************************************************************/

int tedDocSetTocField(		EditDocument *		ed,
				const TocField *	tf,
				int			traced )
    {
    int				rval= 0;
    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentField *		dfToc;

    FieldInstructions		fi;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fi );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( docTocFieldSetTocInstructions( &fi, tf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    dfToc= docFindTypedFieldForPosition( eo->eoDocument,
					    &(eo->eoHeadDp), DOCfkTOC, 0 );
    if  ( ! dfToc )
	{ XDEB(dfToc); rval=-1; goto ready;	}

    if  ( tedDocUpdField( &teo, dfToc, &fi ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanFieldInstructions( &fi );
    tedCleanEditOperation( &teo );

    return rval;
    }
