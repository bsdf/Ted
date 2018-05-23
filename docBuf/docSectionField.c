/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Return the number of the current section.				*/
/*									*/
/************************************************************************/

int docCalculateSectionFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    char		scratch[100+1];
    DocumentTree  *	dt;
    BufferItem *	bodySectBi= (BufferItem *)0;

    if  ( docGetRootOfSelectionScope( &dt, &bodySectBi,
					(BufferDocument *)rf->rfDocument,
					&(df->dfSelectionScope) )	||
	  ! bodySectBi							)
	{ XDEB(bodySectBi); *pCalculated= 0; return 0;	}

    sprintf( scratch, "%d", bodySectBi->biNumberInParent+ 1 );
    utilMemoryBufferAppendBytes( mbResult,
			    (unsigned char *)scratch, strlen( scratch ) );

    *pCalculated= 1; return 0;
    }

int docCalculateSectionPagesFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    char		scratch[100+1];
    DocumentTree  *	dt;
    BufferItem *	bodySectBi= (BufferItem *)0;

    if  ( docGetRootOfSelectionScope( &dt, &bodySectBi,
					(BufferDocument *)rf->rfDocument,
					&(df->dfSelectionScope) )	||
	  ! bodySectBi							)
	{ XDEB(bodySectBi); *pCalculated= 0; return 0;	}

    sprintf( scratch, "%d",
			bodySectBi->biBelowPosition.lpPage-
			bodySectBi->biTopPosition.lpPage+ 1 );
    utilMemoryBufferAppendBytes( mbResult,
			    (unsigned char *)scratch, strlen( scratch ) );

    *pCalculated= 1; return 0;
    }

