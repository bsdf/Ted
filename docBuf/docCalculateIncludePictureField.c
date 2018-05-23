/************************************************************************/
/*									*/
/*  Evaluate an 'IncludePicture' field.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appSystem.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docParaParticules.h"
#   include	"docRecalculateFields.h"
#   include	<docIncludePictureField.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate an 'INCLUDEPICTURE' field.					*/
/*									*/
/************************************************************************/

int docRecalculateIncludePictureField(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraBi,
				int				part,
				int				partCount,
				DocumentField *			df,
				const RecalculateFields *	rf )
    {
    int				rval= 0;
    IncludePictureField		ipf;

    const DocumentProperties *	dp= &(rf->rfDocument->bdProperties);

    TextParticule *		tp;

    InsertedObject *		io;
    int				objectNumber;

    int				oldPartCount= paraBi->biParaParticuleCount;
    int				oldStrlen= docParaStrlen( paraBi );
    int				stroff;

    int				stroffShift= 0;

    int				textAttributeNumber;
    int				res= -1;

    MemoryBuffer		fullName;

    utilInitMemoryBuffer( &fullName );

    docInitIncludePictureField( &ipf );

    if  ( docGetIncludePictureField( &ipf, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tp= paraBi->biParaParticules+ part+ 1;

    if  ( partCount == 1		&&
	  tp->tpKind == DOCkindOBJECT	)
	{ *pCalculated= 0; goto ready; }

    if  ( docGetIncludePictureField( &ipf, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ipf.ipfFilename.mbSize < 1 )
	{ LDEB(ipf.ipfFilename.mbSize); *pCalculated= 0; goto ready; }

    {
    const int		relativeIsFile= 1;

    if  ( appAbsoluteName( &fullName, &(ipf.ipfFilename),
				    relativeIsFile, &(dp->dpFilename) ) < 0 )
	{ LDEB(1); *pCalculated= 0; rval= -1; goto ready; }
    }

    io= docClaimObject( &objectNumber, rf->rfDocument );
    if  ( ! io )
	{ XDEB(io); rval= -1; goto ready;	}

    res= docReadFileObject( &fullName, io );
    
    if  ( res )
	{
	docDeleteObject( rf->rfDocument, objectNumber );

	if  ( docFieldReplaceContents( &stroff,
		&stroffShift, &textAttributeNumber, paraBi, part, partCount,
		    *pStroffShift,
		    (const char *)fullName.mbBytes, fullName.mbSize, rf ) )
	    { LDEB(1); rval= -1; goto ready;	}

	tp= docInsertTextParticule( paraBi, part+ 1, stroff, fullName.mbSize,
				    DOCkindSPAN, textAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}
	}
    else{
	if  ( docFieldReplaceContents( &stroff,
			&stroffShift, &textAttributeNumber,
			paraBi, part, partCount,
			*pStroffShift, " ", 1, rf ) )
	    { LDEB(1); rval= -1; goto ready;	}

	tp= docInsertTextParticule( paraBi, part+ 1, stroff, 1,
				    DOCkindOBJECT, textAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}

	tp->tpObjectNumber= objectNumber;
	}

    *pCalculated= 1;
    *pPartShift= paraBi->biParaParticuleCount- oldPartCount;
    *pStroffShift= docParaStrlen( paraBi )- oldStrlen;

  ready:

    docCleanIncludePictureField( &ipf );
    utilCleanMemoryBuffer( &fullName );

    return rval;
    }

