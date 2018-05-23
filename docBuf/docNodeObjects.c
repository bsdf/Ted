/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	"docTreeScanner.h"
#   include	<docListDepth.h>

/************************************************************************/
/*									*/
/*  Insert an object.							*/
/*									*/
/*  NOTE that objects have text attributes. This is for the following	*/
/*  reasons:								*/
/*  *)  The calculation of the descender height below the object.	*/
/*  *)  This makes editing around the object and saving it MUCH easier.	*/
/*									*/
/************************************************************************/

static TextParticule *	docInsertObject( BufferDocument *	bd,
					BufferItem *		paraBi,
					int			n,
					int			off,
					const TextAttribute *	ta )
    {
    TextParticule *	tp;
    InsertedObject *	io;

    int			objectNumber;
    int			textAttributeNumber;

    textAttributeNumber= docTextAttributeNumber( bd, ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return (TextParticule *)0;	}

    io= docClaimObject( &objectNumber, bd );
    if  ( ! io )
	{ XDEB(io); return (TextParticule *)0; }

    tp= docInsertTextParticule( paraBi, n, off, 1,
					DOCkindOBJECT, textAttributeNumber );
    if  ( ! tp )
	{
	XDEB(tp);
	docDeleteObject( bd, objectNumber );
	return tp;
	}

    tp->tpObjectNumber= objectNumber;

    return tp;
    }

TextParticule *	docAppendObject(	BufferDocument *	bd,
					BufferItem *		paraBi,
					const TextAttribute *	ta )
    {
    TextParticule *	tp;
    int			stroffShift= 0;
    int			off= docParaStrlen( paraBi );

    if  ( docParaStringReplace( &stroffShift, paraBi, off, off, " ", 1 ) )
	{ LLDEB(docParaStrlen(paraBi),1); return (TextParticule *)0; }

    tp= docInsertObject( bd, paraBi, -1, off, ta );
    if  ( ! tp )
	{ LDEB(paraBi->biParaParticuleCount); return tp;	}

    return tp;
    }

/************************************************************************/
/*									*/
/*  Close the objects in a buffer node.					*/
/*									*/
/************************************************************************/

typedef struct CleanNodeObjects
    {
    int			cnoBulletsDeleted;
    int			cnoParagraphCount;
    DOC_CLOSE_OBJECT	cnoCloseObject;
    DocumentTree *	cnoTree;
    BufferDocument *	cnoDocument;
    } CleanNodeObjects;

static void docInitCleanNodeObjects(	CleanNodeObjects *	cno )
    {
    cno->cnoBulletsDeleted= 0;
    cno->cnoParagraphCount= 0;
    cno->cnoCloseObject= (DOC_CLOSE_OBJECT)0;
    cno->cnoTree= (DocumentTree *)0;
    cno->cnoDocument= (BufferDocument *)0;
    }

static void docCleanParaObjects(	CleanNodeObjects *	cno,
					BufferItem *		paraNode )
    {
    int			part;
    TextParticule *	tp;

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind == DOCkindOBJECT )
	    { (*cno->cnoCloseObject)( cno->cnoDocument, tp );	}
	}

    if  ( paraNode->biParaListOverride > 0			||
	  paraNode->biParaOutlineLevel < PPoutlineBODYTEXT	)
	{
	if  ( docRemoveParagraphFromList( paraNode,
					cno->cnoTree, cno->cnoDocument ) )
	    { LDEB(1);	}

	if  ( paraNode->biParaListOverride > 0 )
	    { cno->cnoBulletsDeleted++;	}
	}

    return;
    }

static void docCleanTreeObjects(
				int *			pBulletsDeleted,
				int *			pParagraphCount,
				DocumentTree *		dt,
				BufferDocument *	bd,
				DOC_CLOSE_OBJECT	closeObject )
    {
    if  ( dt->dtRoot )
	{
	docCleanNodeObjects( pBulletsDeleted, pParagraphCount,
					    dt, bd, dt->dtRoot, closeObject );
	}
    }

static void docCleanHeaderFooterObjects(	SectHeadersFooters *	shf,
						CleanNodeObjects *	cno )
    {
    docCleanTreeObjects( &(cno->cnoBulletsDeleted), &(cno->cnoParagraphCount),
				&(shf->shfFirstPageHeader),
				cno->cnoDocument, cno->cnoCloseObject );

    docCleanTreeObjects( &(cno->cnoBulletsDeleted), &(cno->cnoParagraphCount),
				&(shf->shfLeftPageHeader),
				cno->cnoDocument, cno->cnoCloseObject );

    docCleanTreeObjects( &(cno->cnoBulletsDeleted), &(cno->cnoParagraphCount),
				&(shf->shfRightPageHeader),
				cno->cnoDocument, cno->cnoCloseObject );

    docCleanTreeObjects( &(cno->cnoBulletsDeleted), &(cno->cnoParagraphCount),
				&(shf->shfFirstPageFooter),
				cno->cnoDocument, cno->cnoCloseObject );

    docCleanTreeObjects( &(cno->cnoBulletsDeleted), &(cno->cnoParagraphCount),
				&(shf->shfLeftPageFooter),
				cno->cnoDocument, cno->cnoCloseObject );

    docCleanTreeObjects( &(cno->cnoBulletsDeleted), &(cno->cnoParagraphCount),
				&(shf->shfRightPageFooter),
				cno->cnoDocument, cno->cnoCloseObject );

    return;
    }

static int docVisitCleanNode(	BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				vcno )
    {
    CleanNodeObjects *	cno= (CleanNodeObjects *)vcno;

    switch( node->biLevel )
	{
	case DOClevPARA:
	    docCleanParaObjects( cno, node );
	    cno->cnoParagraphCount++;
	    break;

	case DOClevSECT:
	    if  ( node->biSectHeadersFooters )
		{
		docCleanHeaderFooterObjects( node->biSectHeadersFooters, cno );
		}
	    break;

	default:
	    break;
	}

    return ADVICEtsOK;
    }

void docCleanNodeObjects(	int *			pBulletsDeleted,
				int *			pParagraphCount,
				DocumentTree *		dt,
				BufferDocument *	bd,
				BufferItem *		node,
				DOC_CLOSE_OBJECT	closeObject )
    {
    const int		flags= 0;

    CleanNodeObjects	cno;

    docInitCleanNodeObjects( &cno );
    cno.cnoCloseObject= closeObject;
    cno.cnoDocument= bd;
    cno.cnoTree= dt;

    if  ( docScanTreeNode( bd, node,
			    docVisitCleanNode, (NodeVisitor)0, flags, &cno ) )
	{ LDEB(1);	}
    else{
	if  ( pBulletsDeleted )
	    { *pBulletsDeleted += cno.cnoBulletsDeleted;	}
	if  ( pParagraphCount )
	    { *pParagraphCount += cno.cnoParagraphCount;	}
	}

    return;
    }

void docCleanDocumentObjects(	BufferDocument *	bd,
				DOC_CLOSE_OBJECT	closeObject )
    {
    int		bulletsDeleted= 0;
    int		paragraphCount= 0;

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdBody), bd, closeObject );

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdEiFtnsep), bd, closeObject );

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdEiFtnsepc), bd, closeObject );

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdEiFtncn), bd, closeObject );

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdEiAftnsep), bd, closeObject );

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdEiAftnsepc), bd, closeObject );

    docCleanTreeObjects( &bulletsDeleted, &paragraphCount,
				    &(bd->bdEiAftncn), bd, closeObject );

    return;
    }
