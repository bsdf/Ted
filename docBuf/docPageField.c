/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<utilBase26.h>
#   include	<utilRoman.h>
#   include	<docPagerefField.h>
#   include	"docBuf.h"
#   include	"docDebug.h"
#   include	"docEvalField.h"
#   include	"docParaParticules.h"
#   include	"docRecalculateFields.h"
#   include	<docTreeType.h>

/************************************************************************/
/*									*/
/*  Evaluate page number related fields.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Format a page number to be used in a certain paragraph.		*/
/*									*/
/************************************************************************/

static int docGetPageNumberOffset(	const BufferItem *	sectNode )
    {
    if  ( ! sectNode->biParent )
	{ XDEB(sectNode->biParent); return 0;	}

    while( sectNode->biNumberInParent > 0 )
	{
	if  ( sectNode->biSectRestartPageNumbers )
	    { break;	}

	sectNode= sectNode->biParent->biChildren[sectNode->biNumberInParent- 1];
	}

    if  ( sectNode->biSectRestartPageNumbers )
	{
	return sectNode->biTopPosition.lpPage- sectNode->biSectStartPageNumber;
	}
    else{ return sectNode->biTopPosition.lpPage; }
    }

static void docFormatPageNumber(	char *			target,
					int			targetSize,
					const BufferItem *	sectNode,
					int			pageNumber )
    {
    int			style= sectNode->biSectPageNumberStyle;

    if  ( targetSize < 20 )
	{ LDEB(targetSize); strcpy( target, "?" ); return;	}

    pageNumber -= docGetPageNumberOffset( sectNode );

    switch( style )
	{
	default:
	    LDEB(style);
	    /*FALLTHROUGH*/

	case DOCpgnDEC:
	    sprintf( target, "%d", pageNumber+ 1 );
	    break;

	case DOCpgnUCLTR:
	    if  ( utilBase26String( target, targetSize, pageNumber+ 1, 1 ) )
		{ LDEB(pageNumber); return ;	}
	    break;

	case DOCpgnLCLTR:
	    if  ( utilBase26String( target, targetSize, pageNumber+ 1, 0 ) )
		{ LDEB(pageNumber); return ;	}
	    break;

	case DOCpgnUCRM:
	    if  ( utilRomanString( target, targetSize, pageNumber+ 1, 1 ) )
		{ sprintf( target, "UCRM:%d", pageNumber+ 1 );	}
	    break;

	case DOCpgnLCRM:
	    if  ( utilRomanString( target, targetSize, pageNumber+ 1, 0 ) )
		{ sprintf( target, "lcrm:%d", pageNumber+ 1 );	}
	    break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Return the value of a pageref field.				*/
/*									*/
/************************************************************************/

int docCalculatePagerefFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		dfRef,
				const RecalculateFields *	rf )
    {
    char			scratch[100+1];
    BufferDocument *		bd= rf->rfDocument;
    BufferItem *		bodyNode= bd->bdBody.dtRoot;
    DocumentField *		dfMark;
    int				pageNumber;
    int				i;
    const BufferItem *		sectNode= (const BufferItem *)0;

    PagerefField		pf;

    int				n;

    docInitPagerefField( &pf );

    if  ( docGetPagerefField( &pf, dfRef ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    n= docFindBookmarkField( &dfMark, &(bd->bdFieldList), &(pf.pfBookmark) );
    if  ( n < 0 )
	{
	/* SLDEB(utilMemoryBufferGetString(&(pf.pfBookmark)),n); */
	*pCalculated= 0; goto ready;
	}

    pageNumber= dfMark->dfPage;

    for ( i= 0; i < bodyNode->biChildCount; i++ )
	{
	sectNode= bodyNode->biChildren[i];
	if  ( sectNode->biBelowPosition.lpPage >= pageNumber )
	    { break;	}
	}

    if  ( i >= bodyNode->biChildCount )
	{ LDEB(pageNumber); *pCalculated= 0; goto ready;	}

    docFormatPageNumber( scratch, sizeof(scratch)-1, sectNode, pageNumber );

    utilMemoryBufferAppendBytes( mbResult,
			    (unsigned char *)scratch, strlen( scratch ) );
    *pCalculated= 1;

  ready:

    docCleanPagerefField( &pf );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of a page/numpages field.				*/
/*									*/
/************************************************************************/

int docCalculatePageFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    char		scratch[100+1];
    DocumentTree  *	dt;
    BufferItem *	bodySectNode= (BufferItem *)0;
    int			page= rf->rfBodySectPage;

    if  ( ! docIsHeaderType( df->dfSelectionScope.ssTreeType )	&&
	  ! docIsFooterType( df->dfSelectionScope.ssTreeType )	)
	{
	DocumentPosition		dp;

	if  ( docPositionForEditPosition( &dp, &(df->dfHeadPosition),
							    rf->rfTree ) )
	    { LDEB(df->dfHeadPosition.epParaNr);	}
	else{
	    int			line;
	    int			flags= 0;
	    const int		lastOne= PARAfindFIRST;

	    if  ( docFindLineOfPosition( &line, &flags, &dp, lastOne ) )
		{ LDEB(dp.dpStroff);	}
	    else{ page= dp.dpNode->biParaLines[line].tlTopPosition.lpPage; }
	    }
	}

    if  ( docGetRootOfSelectionScope( &dt, &bodySectNode,
					(BufferDocument *)rf->rfDocument,
					&(df->dfSelectionScope) )	||
	  ! bodySectNode					)
	{ XDEB(bodySectNode); *pCalculated= 0; return 0;	}

    docFormatPageNumber( scratch, sizeof(scratch)-1, bodySectNode, page );
    utilMemoryBufferAppendBytes( mbResult,
			    (unsigned char *)scratch, strlen( scratch ) );

    *pCalculated= 1; return 0;
    }

int docCalculateNumpagesFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    char		scratch[100+1];
    BufferItem *	bodyNode= rf->rfDocument->bdBody.dtRoot;

    sprintf( scratch, "%d", bodyNode->biBelowPosition.lpPage+ 1 );
    utilMemoryBufferAppendBytes( mbResult,
			    (unsigned char *)scratch, strlen( scratch ) );

    *pCalculated= 1; return 0;
    }

