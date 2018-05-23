/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docField.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docMergeField.h>

/************************************************************************/
/*									*/
/*  Evaluate fields to merge in external data.				*/
/*									*/
/************************************************************************/

#   define	FIC_COUNT	15

/************************************************************************/
/*									*/
/*  Calculate the value for a database merge field.			*/
/*									*/
/************************************************************************/

static int docCalculateMergefieldFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int				rval= 0;
    MergeField			mf;

    docInitMergeField( &mf );

    if  ( docGetMergeField( &mf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    if  ( rf->rfFieldDataProvider )
	{
	int		ret= 0;
	const char *	fieldName= utilMemoryBufferGetString( &(mf.mfFieldName) );

	ret= (*rf->rfFieldDataProvider)( pCalculated, mbResult,
					    fieldName, rf->rfMergeThrough );
	if  ( ret )
	    { LDEB(ret); *pCalculated= 0;	}
	}
    else{ *pCalculated= 0;	}

  ready:
    docCleanMergeField( &mf );

    return rval;
    }

int docRecalculateMergeField(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				struct BufferItem *		paraBi,
				int				part,
				int				partCount,
				DocumentField *			df,
				const RecalculateFields *	rf )
    {
    int					rval= 0;
    MemoryBuffer			mbResult;
    int					calculated= 0;

    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    utilInitMemoryBuffer( &mbResult );

    if  ( docCalculateMergefieldFieldString( &calculated, &mbResult, df, rf ) )
	{ SDEB(fki->fkiLabel); rval= -1; goto ready;	}

    if  ( ! calculated )
	{
	*pCalculated= 0;
	*pPartShift= 0;
	/* NO! *pStroffShift= 0; */
	goto ready;
	}

    if  ( docRecalculateFieldParticulesFromString(
				    pCalculated, pPartShift, pStroffShift,
				    paraBi, part, partCount, &mbResult, rf ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &mbResult );

    return rval;
    }


/*
if  ( targetSize < 1+ namelen+ 1 )
    { LLDEB(namelen,targetSize); *pCalculated= 0; return 0; }

size= 0;
target[size++]= ISO1_guillemotleft;
memcpy( target+ size, value, namelen ); size += namelen;
target[size++]= ISO1_guillemotright;
target[size]= '\0';
*/

