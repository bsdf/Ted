/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docShape.h"
#   include	"docParaParticules.h"

/************************************************************************/
/*									*/
/*  Manage inserted objects.						*/
/*									*/
/************************************************************************/

static void docCleanObject(	BufferDocument *	bd,
				InsertedObject *	io )
    {
    if  ( io->ioDrawingShape )
	{ docDeleteDrawingShape( bd, io->ioDrawingShape );	}

    docCleanInsertedObject( io );

    return;
    }

/************************************************************************/
/*									*/
/*  Do cleanup provoked by the deletion of a particule.			*/
/*									*/
/************************************************************************/

void docDeleteObject(		BufferDocument *	bd,
				int			n )
    {
    InsertedObject *	io;

    io= docGetObject( bd, n );
    if  ( ! io )
	{ LPDEB(n,io);	}
    else{
	/* Needed because of the bd dependent clean: */
	docCleanObject( bd, io );
	docInitInsertedObject( io );

	utilPagedListDeleteItemByNumber( &(bd->bdObjectList.iolPagedList), n );
	}

    return;
    }

void docCleanParticuleObject(	BufferDocument *	bd,
				TextParticule *		tp )
    {
    if  ( tp->tpKind != DOCkindOBJECT )
	{ return;	}

    docDeleteObject( bd, tp->tpObjectNumber );

    return;
    }

InsertedObject * docClaimObjectCopy(	BufferDocument *	bd,
					int *			pNr,
					const InsertedObject *	ioFrom )
    {
    InsertedObject *	ioTo;
    int			objectNumber;

    ioTo= docClaimObject( &objectNumber, bd );
    if  ( ! ioTo )
	{ XDEB(ioTo); return (InsertedObject *)0;	}

    if  ( ioFrom->ioObjectName						&&
	  docSetObjectName( ioTo, ioFrom->ioObjectName,
			    strlen( (char *)ioFrom->ioObjectName ) )	)
	{
	LDEB(1);
	docDeleteObject( bd, objectNumber );
	return (InsertedObject *)0;
	}

    if  ( ioFrom->ioObjectClass						&&
	  docSetObjectClass( ioTo, ioFrom->ioObjectClass,
			    strlen( (char *)ioFrom->ioObjectClass ) )	)
	{
	LDEB(1);
	docDeleteObject( bd, objectNumber );
	return (InsertedObject *)0;
	}

    if  ( utilCopyMemoryBuffer( &ioTo->ioObjectData, &ioFrom->ioObjectData ) ||
	  utilCopyMemoryBuffer( &ioTo->ioResultData, &ioFrom->ioResultData ) )
	{
	LDEB(1);
	docDeleteObject( bd, objectNumber );
	return (InsertedObject *)0;
	}

    ioTo->ioKind= ioFrom->ioKind;
    ioTo->ioResultKind= ioFrom->ioResultKind;
    ioTo->ioRtfResultKind= ioFrom->ioRtfResultKind;
    ioTo->ioRtfEmbedKind= ioFrom->ioRtfEmbedKind;
    ioTo->ioTwipsWide= ioFrom->ioTwipsWide;
    ioTo->ioTwipsHigh= ioFrom->ioTwipsHigh;
    ioTo->ioScaleXSet= ioFrom->ioScaleXSet;
    ioTo->ioScaleYSet= ioFrom->ioScaleYSet;
    ioTo->ioScaleXUsed= ioFrom->ioScaleXUsed;
    ioTo->ioScaleYUsed= ioFrom->ioScaleYUsed;
    ioTo->ioPixelsWide= ioFrom->ioPixelsWide;
    ioTo->ioPixelsHigh= ioFrom->ioPixelsHigh;

    ioTo->ioPictureProperties= ioFrom->ioPictureProperties;

    ioTo->ioTopCropTwips= ioFrom->ioTopCropTwips;
    ioTo->ioBottomCropTwips= ioFrom->ioBottomCropTwips;
    ioTo->ioLeftCropTwips= ioFrom->ioLeftCropTwips;
    ioTo->ioRightCropTwips= ioFrom->ioRightCropTwips;

    *pNr= objectNumber;
    return ioTo;
    }

