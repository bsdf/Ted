/************************************************************************/
/*									*/
/*  Ted: Interface to the find tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedAppResources.h"

#   include	<appSpellTool.h>
#   include	<docFind.h>
#   include	<docTreeNode.h>
#   include	"tedFind.h"
#   include	"tedFindTool.h"
#   include	"tedSpellTool.h"
#   include	"tedToolFront.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<ind.h>

#   include	<appDebugon.h>

int tedDocFindNext(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				ret;

    DocumentSelection		dsNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! td->tdFindProg )
	{ XDEB(td->tdFindProg); return -1;	}

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsTail), bd,
				docFindParaFindNext, (void *)td->tdFindProg );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int		lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );
	}

    return ret;
    }

int tedDocFindPrev(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				ret;

    DocumentSelection		dsNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! td->tdFindProg )
	{ XDEB(td->tdFindProg); return -1;	}

    if  ( tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindPrevInDocument( &dsNew, &(ds.dsHead), bd,
				docFindParaFindPrev, (void *)td->tdFindProg );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int		lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );
	}

    return ret;
    }

int tedFindToolSetPattern(	void *		voidea,
				const char *	pattern,
				int		useRegex )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;

    tar->tarFindPattern= (char *)0;

    return docFindSetPattern( &(td->tdFindProg), pattern, useRegex );
    }

void tedShowFindTool(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    /*  To get the correct title  */
    tedShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowFindPage( ea );

    if  ( tar->tarFindPattern )
	{
	tedFormatSetFindPattern( ea, tar->tarFindPattern, tar->tarFindRegex );
	}

    return;
    }

int tedSpellFindNext(	void *			voidea,
			MemoryBuffer *		mbGuess,
			const SpellChecker *	spc,
			SpellDictionary *	spd )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsTail), bd,
					docSpellParaFindNext, (void *)spd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int	lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

	if  ( mbGuess && utilMemoryBufferGetRange(
			mbGuess, &(dsNew.dsTail.dpNode->biParaStringBuffer),
			dsNew.dsHead.dpStroff,
			dsNew.dsTail.dpStroff- dsNew.dsHead.dpStroff ) )
	    { LDEB(1); return -1;	}
	}

    return ret;
    }

