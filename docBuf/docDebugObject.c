#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docDebug.h"
#   include	"docShape.h"
#   include	"docFind.h"
#   include	<docObjectProperties.h>
#   include	<docTextParticule.h>
#   include	<appDebugon.h>

static int docListObject( int n, void * vio, void * through )
    {
    InsertedObject *	io= (InsertedObject *)vio;
    const char *	kind= docObjectKindStr(io->ioKind);
    const char *	reskind= docObjectKindStr(io->ioResultKind);

    int			size= io->ioObjectData.mbSize;
    int			ressize= io->ioResultData.mbSize;

    LSSLLDEB(n,kind,reskind,size,ressize);

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	const DrawingShape *	ds= io->ioDrawingShape;

	if  ( ! io->ioDrawingShape )
	    { LXDEB(n,io->ioDrawingShape);	}
	else{
	    const SelectionScope *	ss= &(ds->dsSelectionScope);

	    SLDEB(docTreeTypeStr(ss->ssTreeType),ss->ssOwnerSectNr);

	    if  ( ss->ssOwnerNumber != ds->dsShapeNumber )
		{ SLLDEB("###",ss->ssOwnerNumber,ds->dsShapeNumber);	}
	    }
	}

    if  ( size > 4 )
	{ size= 4;	}
    if  ( ressize > 4 )
	{ ressize= 4;	}

    if  ( size > 0 )
	{
	appDebug( "    OBJ BYTES: \"%.*s\"\n", size,
				utilMemoryBufferGetString( &(io->ioObjectData) ) );
	}

    if  ( ressize > 0 )
	{
	appDebug( "    RES BYTES: \"%.*s\"\n", ressize,
				utilMemoryBufferGetString( &(io->ioResultData) ) );
	}

    return 0;
    }

void docListObjects( BufferDocument * bd )
    {
    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
						docListObject, (void *)0 );
    }

static void docListShape(	int				indent,
				const DrawingShape *		ds )
    {
    appDebug( "%*sSHAPE %4d\n", indent, "", ds->dsShapeNumber );

    if  ( ds->dsChildCount > 0 )
	{
	int		ch;

	appDebug( "%*s{\n", indent+ 4, "" );

	for ( ch= 0; ch < ds->dsChildCount; ch++ )
	    { docListShape( indent+ 4, ds->dsChildren[ch] ); }

	appDebug( "%*s}\n", indent+ 4, "" );
	}
    }

static int docListShapeX(	int				n,
				void *				vds,
				void *				through )
    {
    docListShape( 0, (const DrawingShape *)vds );
    return 0;
    }

void docListShapes(		BufferDocument *		bd )
    {
    DrawingShapeList *	dsl= &(bd->bdShapeList);
    PagedList *		pl= &(dsl->dslPagedList);

    utilPagedListForAll( pl, docListShapeX, (void *)0 );
    }

static int docFindParaObjects(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

static void docListShapeObjects(	const DrawingShape *	dshp,
					BufferDocument *	bd )
    {
    DocumentSelection		ds;

    if  ( docHeadPosition( &(ds.dsHead), dshp->dsDocumentTree.dtRoot ) )
	{ LDEB(1); return;	}

    docFindFindNextInCurrentTree( &ds, &(ds.dsHead), bd,
					    docFindParaObjects, (void *)0 );
    }

static void docListParaObjects(	const BufferItem *	paraBi,
				BufferDocument *	bd )
    {
    const TextParticule *	tp=  paraBi->biParaParticules;
    int				part;

    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	const InsertedObject *	io;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io= docGetObject( bd, tp->tpObjectNumber );
	if  ( ! io )
	    { LXDEB(tp->tpObjectNumber,io); continue;	}

	switch( io->ioKind )
	    {
	    DrawingShape *	dshp;

	    case DOCokDRAWING_SHAPE:
		dshp= io->ioDrawingShape;

		appDebug( "%s - OBJECT %4d: %s %4d\n",
			docTreeTypeStr( paraBi->biTreeType ),
			tp->tpObjectNumber, docObjectKindStr( io->ioKind ),
			dshp->dsShapeNumber );

		docListShape( 4, dshp );

		if  ( dshp->dsDocumentTree.dtRoot )
		    { docListShapeObjects( dshp, bd );	}
		break;

	    case DOCokOLEOBJECT:
		switch( io->ioResultKind )
		    {
		    case DOCokDRAWING_SHAPE:
			dshp= io->ioDrawingShape;

			appDebug( "%s - OBJECT %4d: %s -> %s %4d\n",
			    docTreeTypeStr( paraBi->biTreeType ),
			    tp->tpObjectNumber,
			    docObjectKindStr( io->ioKind ),
			    docObjectKindStr( io->ioResultKind ),
			    dshp->dsShapeNumber );

			docListShape( 4, dshp );

			if  ( dshp->dsDocumentTree.dtRoot )
			    { docListShapeObjects( dshp, bd );	}
			break;

		    default:
			appDebug( "%s - OBJECT %4d: %s -> %s\n",
			    docTreeTypeStr( paraBi->biTreeType ),
			    tp->tpObjectNumber,
			    docObjectKindStr( io->ioKind ),
			    docObjectKindStr( io->ioResultKind ) );
			break;
		    }
		break;

	    default:
		appDebug( "%s - OBJECT %4d: %s\n",
			docTreeTypeStr( paraBi->biTreeType ),
			tp->tpObjectNumber, docObjectKindStr( io->ioKind ) );
		break;
	    }
	}

    return;
    }

static int docFindParaObjects(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    docListParaObjects( paraBi, bd );
    return 1;
    }

void docListDocumentObjects(	BufferDocument *	bd )
    {
    DocumentSelection		ds;

    if  ( docDocumentHead( &(ds.dsHead), bd ) )
	{ LDEB(1); return;	}

    docFindFindNextInDocument( &ds, &(ds.dsHead), bd,
				docFindParaObjects, (void *)0 );
    return;
    }
