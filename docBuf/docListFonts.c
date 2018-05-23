/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*  Manage resources for layout.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stddef.h>

#   include	"docListFonts.h"
#   include	"docFind.h"
#   include	"docTreeNode.h"
#   include	"docDebug.h"
#   include	"docShape.h"
#   include	<docObjectProperties.h>
#   include	<docTextParticule.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of PostScriptFonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

static int docScanParaSpans(	DocumentSelection *		ds,
				BufferItem *			paraNode,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				vsdf );

static int docScanShapeSpans(	const DrawingShape *		ds,
				BufferDocument *		bd,
				ScanDocumentFonts *		sdf )
    {
    int		rval= 0;
    int		i;

    if  ( ds->dsDocumentTree.dtRoot )
	{
	DocumentPosition	dp;

	if  ( ! docHeadPosition( &dp, ds->dsDocumentTree.dtRoot ) )
	    {
	    DocumentSelection	dsIgnored;

	    docInitDocumentSelection( &dsIgnored );

	    if  ( docFindFindNextInCurrentTree( &dsIgnored, &dp, bd,
						docScanParaSpans, sdf ) != 1 )
		{ LDEB(1); rval= -1;	}
	    }
	}

    for ( i= 0; i < ds->dsChildCount; i++ )
	{
	if  ( docScanShapeSpans( ds->dsChildren[i], bd, sdf ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }

/*  a  */
static int docScanParaSpans(	DocumentSelection *		ds,
				BufferItem *			paraNode,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				vsdf )
    {
    ScanDocumentFonts *		sdf= (ScanDocumentFonts *)vsdf;
    int				attributeNumber= -1;
    TextAttribute		ta;

    InsertedObject *		io;

    const TextParticule *	tp;
    int				part;

    utilInitTextAttribute( &ta );
    tp= paraNode->biParaParticules; part= 0;

    while( part < paraNode->biParaParticuleCount )
	{
	int		from;
	int		upto;

	switch( tp->tpKind )
	    {
	    case DOCkindSPAN:
	    case DOCkindTAB:
	    case DOCkindOPT_HYPH:
		from= tp->tpStroff;
		upto= tp->tpStroff+ tp->tpStrlen;
		if  ( tp->tpTextAttrNr >= 0			&&
		      tp->tpTextAttrNr != attributeNumber	)
		    {
		    docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );
		    attributeNumber= tp->tpTextAttrNr;
		    }

		while( part+ 1 < paraNode->biParaParticuleCount )
		    {
		    if  ( tp[1].tpKind != DOCkindSPAN	&&
			  tp[1].tpKind != DOCkindTAB	)
			{ break;	}
		    if  ( tp[1].tpTextAttrNr != attributeNumber )
			{ break;	}

		    part++; tp++;
		    }
		upto= tp->tpStroff+ tp->tpStrlen;
		part++; tp++;

		/* Do invoke the callback for empty spans! */
		if  ( (*sdf->sdfDocListSpanFont)( bd, paraNode,
						attributeNumber, &ta,
						from, upto, sdf->sdfThrough ) )
		    { LLDEB(from,tp->tpStroff); return -1;	}

		break;

	    case DOCkindOBJECT:
		io= docGetObject( bd, tp->tpObjectNumber );
		if  ( ! io )
		    { LPDEB(tp->tpObjectNumber,io); return -1;	}

		if  ( sdf->sdfListObject )
		    {
		    if  ( tp->tpTextAttrNr >= 0			&&
			  tp->tpTextAttrNr != attributeNumber	)
			{
			docGetTextAttributeByNumber( &ta,
							bd, tp->tpTextAttrNr );
			attributeNumber= tp->tpTextAttrNr;
			}

		    if  ( (*sdf->sdfListObject)( bd, paraNode, &ta,
							    sdf->sdfThrough ) )
			{ LDEB(tp->tpKind); return -1;	}
		    }

		if  ( sdf->sdfListObjectFonts )
		    {
		    if  ( (*sdf->sdfListObjectFonts)( io, "f",
							    sdf->sdfThrough ) )
			{ LDEB(tp->tpKind); return -1;	}
		    }

		if  ( io->ioKind == DOCokDRAWING_SHAPE			&&
		      io->ioDrawingShape				&&
		      docScanShapeSpans( io->ioDrawingShape,
							    bd, sdf )	)
		    { LDEB(1);	}

		part++; tp++; break;

	    case DOCkindFIELDHEAD:
	    case DOCkindFIELDTAIL:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
	    case DOCkindLTR_MARK:
	    case DOCkindRTL_MARK:
		part++; tp++; break;

	    default:
		LDEB(tp->tpKind); return -1;
	    }
	}

    return 1;
    }

int docListDocumentFonts(	BufferDocument *		bd,
				ScanDocumentFonts *		sdf )
    {
    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    /*  a  */
    if  ( docDocumentHead( &dpBeginFrom, bd ) )
	{
	docListNode( 0, bd->bdBody.dtRoot, 0 );
	LDEB(1); return -1;
	}

    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bd,
						    docScanParaSpans, sdf );
    if  ( res != 1 )
	{ LDEB(res); return -1;	}

    return 0;
    }

int docListTreeFonts(		BufferDocument *		bd,
				const DocumentTree *		dt,
				ScanDocumentFonts *		sdf )
    {
    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    if  ( docHeadPosition( &dpBeginFrom, dt->dtRoot ) )
	{ return 0;	}

    res= docFindFindNextInCurrentTree( &dsIgnored, &dpBeginFrom, bd,
						    docScanParaSpans, sdf );
    if  ( res != 1 )
	{ LDEB(res); return -1;	}

    return 0;
    }


void docInitScanDocumentFonts(		ScanDocumentFonts *		sdf )
    {
    sdf->sdfListObjectFonts= (DocListObjectFonts)0;
    sdf->sdfDocListSpanFont= (DocListSpanFont)0;
    sdf->sdfListObject= (DocListObject)0;
    sdf->sdfThrough= (void *)0;
    }

