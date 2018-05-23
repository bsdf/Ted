/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docParaParticules.h"
#   include	"docRecalculateFields.h"
#   include	<docRefField.h>

/************************************************************************/
/*									*/
/*  Evaluate REF fields.						*/
/*									*/
/************************************************************************/

int docCalculateRefFieldValue(	MemoryBuffer *			mbResult,
				const MemoryBuffer *		markName,
				const BufferDocument *		bd )
    {
    DocumentSelection		dsInside;
    int				headPart;
    int				tailPart;

    docInitDocumentSelection( &dsInside );

    if  ( docFindBookmarkInDocument( &dsInside, &headPart, &tailPart,
							    bd, markName ) )
	{
	const char *	ehh1= "<<? ";
	const char *	ehh2= "?>> ";

	utilMemoryBufferAppendBytes( mbResult,
				    (unsigned char *)ehh1, strlen( ehh1 ) );

	utilMemoryAppendBuffer( mbResult, markName );

	utilMemoryBufferAppendBytes( mbResult,
				    (unsigned char *)ehh2, strlen( ehh2 ) );
	}
    else{
	int			beginMoved= 0;
	int			endMoved= 0;
	int			skipping= 0;

	const BufferItem *	paraBiFrom;
	const TextParticule *	tp;
	int			part;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved,
								&dsInside ); 
	paraBiFrom= dsInside.dsHead.dpNode;
	/*
	Really happens. E.G: In the word 2003 rtf spec.
	if  ( dsInside.dsTail.dpStroff == dsInside.dsHead.dpStroff )
	    { LLDEB(dsInside.dsTail.dpStroff,dsInside.dsHead.dpStroff);	}
	*/

	if  ( docFindParticuleOfPosition( &part, (int *)0,
					    &(dsInside.dsHead), PARAfindLAST ) )
	    { LDEB(dsInside.dsHead.dpStroff); return -1;	}

	tp= paraBiFrom->biParaParticules+ part;
	while( part < paraBiFrom->biParaParticuleCount	&&
	       tp->tpKind != DOCkindSPAN		)
	    { part++; tp++;	}

	for ( ; part < paraBiFrom->biParaParticuleCount; tp++, part++ )
	    {
	    if  ( tp->tpStroff >= dsInside.dsTail.dpStroff )
		{ break;	}

	    if  ( tp->tpKind == DOCkindSPAN )
		{
		if  ( skipping )
		    {
		    utilMemoryBufferAppendBytes( mbResult,
						    (unsigned char *)" ", 1 );
		    }

		utilMemoryBufferAppendBytes( mbResult,
				    docParaString( paraBiFrom, tp->tpStroff ),
				    tp->tpStrlen );
		skipping= 0;
		}
	    else{
		skipping= 1;
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of a ref field.					*/
/*									*/
/************************************************************************/

int docCalculateRefFieldString( int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;
    RefField		reff;

    docInitRefField( &reff );

    if  ( docGetRefField( &reff, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    if  ( docCalculateRefFieldValue( mbResult, &(reff.rfBookmark),
							    rf->rfDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pCalculated= 1;

  ready:

    docCleanRefField( &reff );

    return rval;
    }

