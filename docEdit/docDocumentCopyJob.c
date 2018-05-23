/************************************************************************/
/*									*/
/*  Copy part of one document to another (or the same) document.	*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<docPropertiesAdmin.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	"docDocumentCopyJob.h"
#   include	<docParaRulerAdmin.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docItemShadingAdmin.h>
#   include	<docCellPropertyAdmin.h>

void docInitDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    dcj->dcjEditOperation= (EditOperation *)0;
    docInitSelectionScope( &(dcj->dcjTargetSelectionScope) );
    dcj->dcjTargetTree= (DocumentTree *)0;
    dcj->dcjSourceDocument= (BufferDocument *)0;
    dcj->dcjSourceTree= (DocumentTree *)0;
    dcj->dcjCopyFields= 0;
    dcj->dcjFieldMap= (int *)0;

    docInitDocumentAttributeMap( &(dcj->dcjAttributeMap) );
    dcj->dcjForceAttributeTo= -1;

    utilInitMemoryBuffer( &(dcj->dcjRefFileName) );

    dcj->dcjFieldStack= (FieldCopyStackLevel *)0;

    dcj->dcjInExternalTree= 0;

    dcj->dcjCurrentTextAttributeNumberFrom= -1;
    dcj->dcjCurrentTextAttributeNumberTo= -1;

    utilInitIndexSet( &(dcj->dcjNoteFieldsCopied) );
    dcj->dcjBulletsCopied= 0;

    dcj->dcjCopyHeadParaProperties= 0;
    dcj->dcjCopyTailParaProperties= 0;

    return;
    }

void docCleanDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    docCleanDocumentAttributeMap( &(dcj->dcjAttributeMap) );

    utilCleanMemoryBuffer( &(dcj->dcjRefFileName) );

    while( dcj->dcjFieldStack )
	{
	FieldCopyStackLevel *	prev= dcj->dcjFieldStack->fcslPrev;

	free( dcj->dcjFieldStack );

	dcj->dcjFieldStack= prev;
	}

    utilCleanIndexSet( &(dcj->dcjNoteFieldsCopied) );

    return;
    }

int docPushFieldOnCopyStack(		DocumentCopyJob *	dcj,
					DocumentField *		df )
    {
    FieldCopyStackLevel *	fcsl;

    fcsl= (FieldCopyStackLevel *)malloc( sizeof(FieldCopyStackLevel) );
    if  ( ! fcsl )
	{ XDEB(fcsl); return -1;	}

    fcsl->fcslPrev= dcj->dcjFieldStack;
    fcsl->fcslField= df;
    dcj->dcjFieldStack= fcsl;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a mapping for field numbers.				*/
/*									*/
/************************************************************************/

static int * docAllocateFieldMap(	const BufferDocument *	bdFrom )
    {
    int *	fieldMap;
    int		i;
    const int	fieldCount= bdFrom->bdFieldList.dflPagedList.plItemCount;

    fieldMap= (int *)malloc( fieldCount* sizeof(int) );
    if  ( ! fieldMap )
	{ LXDEB(fieldCount,fieldMap); return (int *)0; }
    for ( i= 0; i < fieldCount; i++ )
	{ fieldMap[i]= -1;	}

    return fieldMap;
    }

/************************************************************************/

static int docSetAttributeMap(	DocumentAttributeMap *		dam,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom )
    {
    int		rval= 0;

    int *	fontMap= (int *)0;
    int *	colorMap= (int *)0;
    int *	borderMap= (int *)0;
    int *	shadingMap= (int *)0;
    int *	frameMap= (int *)0;
    int *	cellMap= (int *)0;
    int *	lsmap= (int *)0;
    int *	rulerMap= (int *)0;

    const DocumentPropertyLists *	dplFrom= bdFrom->bdPropertyLists;
    DocumentPropertyLists *		dplTo= bdTo->bdPropertyLists;

    if  ( docMergeColorTables( &colorMap, bdTo, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeBorderPropertiesLists( &borderMap, colorMap,
					&(dplTo->dplBorderPropertyList),
					&(dplFrom->dplBorderPropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeItemShadingLists( &shadingMap, colorMap,
					&(dplTo->dplItemShadingList),
					&(dplFrom->dplItemShadingList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeFramePropertyLists( &frameMap,
					&(dplTo->dplFramePropertyList),
					&(dplFrom->dplFramePropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeTabstopListLists( &rulerMap,
					&(dplTo->dplTabStopListList),
					&(dplFrom->dplTabStopListList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeCellPropertiesLists( &cellMap, borderMap, shadingMap,
					&(dplTo->dplCellPropertyList),
					&(dplFrom->dplCellPropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeDocumentLists( &fontMap, &lsmap, bdTo, bdFrom,
						    colorMap, rulerMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dam->damFontMap )
	{ free( dam->damFontMap );	}
    dam->damFontMap= fontMap; fontMap= (int *)0; /* steal */

    if  ( dam->damColorMap )
	{ free( dam->damColorMap );	}
    dam->damColorMap= colorMap; colorMap= (int *)0; /* steal */

    if  ( dam->damRulerMap )
	{ free( dam->damRulerMap );	}
    dam->damRulerMap= rulerMap; rulerMap= (int *)0; /* steal */

    if  ( dam->damBorderMap )
	{ free( dam->damBorderMap );	}
    dam->damBorderMap= borderMap; borderMap= (int *)0; /* steal */

    if  ( dam->damShadingMap )
	{ free( dam->damShadingMap );	}
    dam->damShadingMap= shadingMap; shadingMap= (int *)0; /* steal */

    if  ( dam->damFrameMap )
	{ free( dam->damFrameMap );	}
    dam->damFrameMap= frameMap; frameMap= (int *)0; /* steal */

    if  ( dam->damCellMap )
	{ free( dam->damCellMap );	}
    dam->damCellMap= cellMap; cellMap= (int *)0; /* steal */

    if  ( dam->damListStyleMap )
	{ free( dam->damListStyleMap );	}
    dam->damListStyleMap= lsmap; lsmap= (int *)0;/* steal */

  ready:

    if  ( fontMap )
	{ free( fontMap );	}
    if  ( colorMap )
	{ free( colorMap );	}
    if  ( borderMap )
	{ free( borderMap );	}
    if  ( shadingMap )
	{ free( shadingMap );	}
    if  ( frameMap )
	{ free( frameMap );	}
    if  ( cellMap )
	{ free( cellMap );	}
    if  ( lsmap )
	{ free( lsmap );	}
    if  ( rulerMap )
	{ free( rulerMap );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying within a document.			*/
/*									*/
/************************************************************************/

int docSet1DocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				int			copyFields )
    {
    int *	fieldMap;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= eo->eoDocument;
    dcj->dcjSourceTree= eo->eoTree;
    dcj->dcjCopyFields= copyFields;

    fieldMap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    dcj->dcjFieldMap= fieldMap;

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying within a document.			*/
/*									*/
/************************************************************************/

int docSetTraceDocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferDocument *	bdFrom )
    {
    int *	fieldMap= (int *)0;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= bdFrom;
    dcj->dcjSourceTree= &(bdFrom->bdBody);
    dcj->dcjCopyFields= 1;

    fieldMap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    dcj->dcjFieldMap= fieldMap;

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying from one document to another.		*/
/*									*/
/************************************************************************/

int docSet2DocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferDocument *	bdFrom,
				DocumentTree *		eiFrom,
				const MemoryBuffer *	refFileName,
				int			forceAttributeTo )
    {
    int *		fldmap;

    BufferDocument *	bdTo= eo->eoDocument;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= bdFrom;
    dcj->dcjSourceTree= eiFrom;
    dcj->dcjCopyFields= 1;
    dcj->dcjForceAttributeTo= forceAttributeTo;

    fldmap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fldmap )
	{ XDEB(fldmap); return -1;	}
    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}
    dcj->dcjFieldMap= fldmap;

    if  ( docSetAttributeMap( &(dcj->dcjAttributeMap), bdTo, bdFrom ) )
	{ LDEB(1); return -1;	}

    if  ( refFileName )
	{
	if  ( utilCopyMemoryBuffer( &(dcj->dcjRefFileName), refFileName ) )
	    { LDEB(1); return -1;	}
	}
    else{
	utilEmptyMemoryBuffer( &(dcj->dcjRefFileName) );
	}

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField )	)
	{ LDEB(1); return -1;	}

    return 0;
    }
