/************************************************************************/
/*									*/
/*  Management of bookmarks related to TOC fields.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docParaParticules.h>
#   include	<docTreeType.h>
#   include	<docBookmarkField.h>
#   include	"docEdit.h"
#   include	"docTocBookmarks.h"

/************************************************************************/
/*									*/
/*  Recalculate TOC fields in a document.				*/
/*									*/
/************************************************************************/

static int docSetTocBookmark(	FieldInstructions *	fiBookmark,
				BufferDocument *	bd )
    {
    int			rval= 0;
    DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int		fieldCount= dfl->dflPagedList.plItemCount;
    int			fieldNr;

    char		markName[DOCmaxBOOKMARK+1];

    long		id0= 0;
    long		id1= 0;

    MemoryBuffer	mbBookmark;

    utilInitMemoryBuffer( &mbBookmark );

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( dfl, fieldNr );
	long			id;

	if  ( ! df )
	    { continue;	}
	if  ( df->dfKind != DOCfkBOOKMARK	||
	      ! docIsTocBookmark( &id, df )	)
	    { continue;	}

	if  ( id0 <= 0 || id0 > id )
	    { id0= id;	}
	if  ( id1 <= 0 || id1 < id )
	    { id1= id;	}
	}

    if  ( id0 > 1 )
	{ sprintf( markName, "_Toc%ld", id0- 1 );	}
    else{ sprintf( markName, "_Toc%ld", id1+ 1 );	}

    if  ( utilMemoryBufferSetString( &mbBookmark, markName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSetBookmarkField( fiBookmark, &mbBookmark ) )
	{ SDEB(markName); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &mbBookmark );

    return rval;
    }

void docRemoveUnbalancedTocBookmarks(	BufferDocument *	bdDoc )
    {
    DocumentFieldList *	dfl= &(bdDoc->bdFieldList);
    const int		fieldCount= dfl->dflPagedList.plItemCount;
    int			fieldNr;

    EditOperation	eo;

    docInitEditOperation( &eo );

    eo.eoTree= &(bdDoc->bdBody);
    eo.eoDocument= bdDoc;

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( dfl, fieldNr );

	if  ( ! df )
	    { continue;		}
	if  ( df->dfSelectionScope.ssTreeType != DOCinBODY	)
	    { continue;	}

	if  ( df->dfKind == DOCfkBOOKMARK				&&
	      df->dfHeadPosition.epParaNr !=
				      df->dfTailPosition.epParaNr	&&
	      docIsTocBookmark( (long *)0, df )				)
	    {
	    DocumentSelection	dsInside;
	    DocumentSelection	dsAround;
	    int			headPart;
	    int			tailPart;
	    DocumentSelection	dsExInside;

	    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					    &headPart, &tailPart, bdDoc, df ) )
		{ LDEB(fieldNr); continue; }

	    if  ( docDeleteField( &dsExInside, &eo,
			dsInside.dsHead.dpNode, dsInside.dsTail.dpNode,
			headPart, tailPart, df ) )
		{ LDEB(fieldNr);	}
	    }
	}

    docCleanEditOperation( &eo );

    }

/************************************************************************/
/*									*/
/*  Find a bookmark field that could be used as a TOC bookmark.		*/
/*  If no field can be found.. fill a selection that can be used to set	*/
/*  a TOC bookmark.							*/
/*									*/
/************************************************************************/

static int docFindParaTocBookmark(	DocumentField **	pDfBookmark,
					DocumentSelection *	dsBookmark,
					BufferDocument *	bd,
					DocumentTree *		dt,
					BufferItem *		paraNode )
    {
    DocumentSelection	ds;
    DocumentField *	dfBookmark;

    if  ( docHeadPosition( &(ds.dsHead), paraNode ) )
	{ LDEB(1); return -1;	}
    docAvoidParaHeadField( &(ds.dsHead), (int *)0, bd );

    {
    int				part;
    const TextParticule *	tp;

    docFindParticuleOfPosition( &part, (int *)0, &(ds.dsHead), PARAfindLAST );

    tp= ds.dsHead.dpNode->biParaParticules+ part;
    while( part < paraNode->biParaParticuleCount	&&
	   ( tp->tpKind == DOCkindPAGEBREAK	||
	     tp->tpKind == DOCkindLINEBREAK	)	)
	{
	ds.dsHead.dpStroff= tp->tpStrlen+ tp->tpStroff;
	tp++; part++;
	}
    }

    if  ( docTailPosition( &(ds.dsTail), paraNode ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( &ds, &(ds.dsHead), &(ds.dsTail), 1 );

    dfBookmark= docFindTypedFieldInSelection( bd, &ds, DOCfkBOOKMARK, 0 );
    if  ( dfBookmark )
	{
	if  ( docSelectionForEditPositionsInTree( dsBookmark, dt,
					    &(dfBookmark->dfHeadPosition),
					    &(dfBookmark->dfTailPosition) ) )
	    { LDEB(1); return -1;	}

	*pDfBookmark= dfBookmark;
	return 0;
	}
    else{
	if  ( ds.dsTail.dpStroff < ds.dsHead.dpStroff )
	    {
	    XXDEB(ds.dsTail.dpNode,ds.dsHead.dpNode);
	    LLDEB(ds.dsTail.dpStroff,ds.dsHead.dpStroff);
	    return -1;
	    }

	*dsBookmark= ds;
	*pDfBookmark= (DocumentField *)0;
	return 1;
	}
    }

/************************************************************************/
/*									*/
/*  Set a TOC bookmark that contains the whole paragraph.		*/
/*									*/
/************************************************************************/

const DocumentField * docGetParaTocBookmark(
					BufferDocument *	bd,
					DocumentTree *		dt,
					BufferItem *		paraNode )
    {
    int			res;
    DocumentSelection	dsBookmark;
    DocumentField *	dfBookmark= (DocumentField *)0;

    FieldInstructions	fiBookmark;

    docInitFieldInstructions( &fiBookmark );

    res= docFindParaTocBookmark( &dfBookmark, &dsBookmark, bd, dt, paraNode );
    if  ( res < 0 )
	{ LDEB(res); goto ready;	}
    if  ( res > 0 )
	{
	if  ( ! docIsIBarSelection( &dsBookmark ) )
	    {
	    if  ( docSetTocBookmark( &fiBookmark, bd ) )
		{ LDEB(1); goto ready;	}

	    if  ( docSurroundTextSelectionByField( bd, dt, &dsBookmark,
			    &dfBookmark,
			    (DocumentSelection *)0, (DocumentSelection *)0,
			    (int *)0, (int *)0,
			    (PropertyMask *)0, (TextAttribute *)0,
			    DOCfkBOOKMARK, &fiBookmark ) )
		{ LDEB(1); goto ready;	}
	    }
	}

  ready:

    docCleanFieldInstructions( &fiBookmark );

    return dfBookmark;
    }

/************************************************************************/
/*									*/
/*  Make sure that all leaves in the outlinelevel tree have a bookmark	*/
/*  that can be used in the table of contents.				*/
/*									*/
/************************************************************************/

typedef struct SetBookmarkThrough
    {
    int			sbtRval;
    BufferDocument *	sbtDocument;
    DocumentTree *	sbtTree;
    } SetBookmarkThrough;

static int docSetTocBookmarkForLevel(	int			ilvl,
					void *			vsbt )
    {
    BufferItem *		paraNode;
    SetBookmarkThrough *	sbt= (SetBookmarkThrough *)vsbt;

    paraNode= docGetParagraphByNumber( sbt->sbtTree, ilvl );
    if  ( ! paraNode )
	{ LXDEB(ilvl,paraNode); sbt->sbtRval= -1;	}
    else{
	/*  Do not check return value:			*/
	/*  No bookmark is set in an empty paragraph	*/

	docGetParaTocBookmark( sbt->sbtDocument, sbt->sbtTree, paraNode );
	}

    return 0;
    }

int docTocSetOutlineBookmarks(	BufferDocument *	bd )
    {
    SetBookmarkThrough	sbt;

    sbt.sbtRval= 0;
    sbt.sbtDocument= bd;
    sbt.sbtTree= &(bd->bdBody);

    if  ( docListNumberTreeForAll( &(sbt.sbtTree->dtOutlineTree),
				docSetTocBookmarkForLevel, (void *)&sbt ) )
	{ LDEB(1); return -1;	}

    return sbt.sbtRval;
    }

int docSetTocBookmarks(	BufferDocument *		bd )
    {
    int			rval= 0;
    DocumentTree *	dt= &(bd->bdBody);

    DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int		fieldCount= dfl->dflPagedList.plItemCount;
    int			fieldNr;

    FieldInstructions	fiBookmark;

    docInitFieldInstructions( &fiBookmark );

    if  ( docTocSetOutlineBookmarks( bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		dfTc= docGetFieldByNumber( dfl, fieldNr );
	DocumentField *		dfBookmark;
	DocumentSelection	dsInsideTc;
	DocumentSelection	dsAroundTc;

	if  ( ! dfTc )
	    { continue;	}
	if  ( ( dfTc->dfKind != DOCfkTC && dfTc->dfKind != DOCfkTCN )	||
	      dfTc->dfSelectionScope.ssTreeType != DOCinBODY		)
	    { continue;	}

	if  ( docDelimitFieldInDoc( &dsInsideTc, &dsAroundTc,
					    (int *)0, (int *)0, bd, dfTc ) )
	    { LDEB(1); rval= -1; goto ready;	}

	dfBookmark= docFindTypedFieldForPosition( bd, &(dsInsideTc.dsHead),
							    DOCfkBOOKMARK, 0 );
	if  ( ! dfBookmark )
	    {
	    if  ( docSetTocBookmark( &fiBookmark, bd ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( docSurroundTextSelectionByField( bd, dt, &dsAroundTc,
			    &dfBookmark,
			    (DocumentSelection *)0, (DocumentSelection *)0,
			    (int *)0, (int *)0,
			    (PropertyMask *)0, (TextAttribute *)0,
			    DOCfkBOOKMARK, &fiBookmark ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}

  ready:

    docCleanFieldInstructions( &fiBookmark );

    return rval;
    }

