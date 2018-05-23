/************************************************************************/
/*									*/
/*  Read RTF shapes.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfShpTab.h"
#   include	<docShape.h>
#   include	<docShapeProp.h>
#   include	<docObjectProperties.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docParaParticules.h>

/************************************************************************/

static int docRtfReadChildShape(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

/************************************************************************/
/*									*/
/*  Property control words inside a shape.				*/
/*									*/
/************************************************************************/

int docRtfShpProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    DrawingShape *	ds= rr->rrDrawingShape;
    ShapeProperties *	sp;

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rr->rrCurrentLine,ds); return 0;	}

    sp= &(ds->dsShapeProperties);

    if  ( rcw->rcwID != SHPpropLID	&&
	  ds->dsIsChildShape		)
	{ SLDEB(rcw->rcwWord,ds->dsIsChildShape);	}

    if  ( docSetShapeProperty( sp, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Content inside a shape.						*/
/*									*/
/************************************************************************/

static int docRtfShpText(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    DrawingShape *	ds= rr->rrDrawingShape;

    int			treeType= DOCinSHPTXT;
    const int		ignoreEmpty= 0;

    /* No! to deal with faulty documents.
    rr->rrDrawingShape= (DrawingShape *)0;
    */

    if  ( docRtfReadDocumentTree( rcw, &(ds->dsDocumentTree), &treeType,
				rr, ignoreEmpty, &(ds->dsSelectionScope) ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ds->dsSelectionScope.ssTreeType != treeType )
	{
	LLDEB(ds->dsSelectionScope.ssTreeType,treeType);
	ds->dsSelectionScope.ssTreeType= treeType;
	docSetTreeTypeOfNode( ds->dsDocumentTree.dtRoot, treeType );
	}

    /* No! to deal with faulty documents.
    rr->rrDrawingShape= ds;
    */
    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an {\sn .... } {\sv .... } pair.				*/
/*									*/
/*  NOTE that the concept of an 'RtfControlWord' is abused for		*/
/*	 the shape property.						*/
/*									*/
/************************************************************************/

static int docRtfShpCollectNumbers(	int *			numbers,
					int			count,
					char *			from )
    {
    int		rval= 0;
    int		i;

    for ( i= 0; i < count; i++ )
	{
	char *		past;
	long		val;

	while( isspace( *from ) )
	    { from++;	}
	if  ( from[0] != ';' )
	    { SDEB(from); rval= -1; goto ready;	}
	from++;
	while( isspace( *from ) )
	    { from++;	}

	past= from;
	val= strtol( from, &past, 10 );
	if  ( past == from )
	    { SDEB(from); rval= -1; goto ready;	}
	from= past;

	numbers[i]= val;
	}

    if  ( count > 0 )
	{ numbers[count]= numbers[0]; }

  ready:

    return rval;
    }

static int docRtfShpCollectVertices(	Point2DI *		vertices,
					int			count,
					char *			from )
    {
    int			rval= 0;
    int			i;

    for ( i= 0; i < count; i++ )
	{
	char *		past;
	long		val;

	while( isspace( *from ) )
	    { from++;	}
	if  ( from[0] != ';' )
	    { SDEB(from); rval= -1; goto ready;	}
	from++;
	while( isspace( *from ) )
	    { from++;	}
	if  ( from[0] != '(' )
	    { SDEB(from); rval= -1; goto ready;	}
	from++;
	while( isspace( *from ) )
	    { from++;	}

	past= from;
	val= strtol( from, &past, 10 );
	if  ( past == from )
	    { SDEB(from); rval= -1; goto ready;	}
	from= past;
	vertices[i].x= val;

	while( isspace( *from ) )
	    { from++;	}
	if  ( from[0] != ',' )
	    { SDEB(from); rval= -1; goto ready;	}
	from++;
	while( isspace( *from ) )
	    { from++;	}

	past= from;
	val= strtol( from, &past, 10 );
	if  ( past == from )
	    { SDEB(from); rval= -1; goto ready;	}
	from= past;
	vertices[i].y= val;

	while( isspace( *from ) )
	    { from++;	}
	if  ( from[0] != ')' )
	    { SDEB(from); rval= -1; goto ready;	}
	from++;
	while( isspace( *from ) )
	    { from++;	}
	}

    if  ( count > 0 )
	{ vertices[count]= vertices[0]; }

ready:

    return rval;
    }

int docRtfShpArray(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    int			rval= 0;

    DrawingShape *	ds= rr->rrDrawingShape;
    ShapeDrawing *	sd;

    int			unitSize;
    int			count;

    const int		removeSemicolon= 0;
    char *		text= (char *)0;
    char *		from;
    char *		past;
    int			size;

    long		val;

    Point2DI *		vertices= (Point2DI *)0;
    int *		numbers= (int *)0;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready;	}

    from= text;
    while( isspace( *from ) )
	{ from++;	}

    past= from;
    val= strtol( from, &past, 10 );
    if  ( past == from )
	{ SDEB(from); rval= -1; goto ready;	}
    from= past;
    unitSize= val;

    while( isspace( *from ) )
	{ from++;	}
    if  ( *from != ';' )
	{ SDEB(from); rval= -1; goto ready;	}
    from++;
    while( isspace( *from ) )
	{ from++;	}

    past= from;
    val= strtol( from, &past, 10 );
    if  ( past == from )
	{ SDEB(from); rval= -1; goto ready;	}
    from= past;
    count= val;

    switch( unitSize )
	{
	case 8:
	    vertices= (Point2DI *)malloc( (count+ 1)* sizeof( Point2DI ) );
	    if  ( ! vertices )
		{ LXDEB(count,vertices); rval= -1; goto ready; }
	    if  ( docRtfShpCollectVertices( vertices, count, from ) )
		{ LDEB(count); rval= -1; goto ready;	}
	    break;

	case 2:
	case 4:
	    numbers= (int *)malloc( (count+ 1)* sizeof( int ) );
	    if  ( ! numbers )
		{ LXDEB(count,numbers); rval= -1; goto ready; }
	    if  ( docRtfShpCollectNumbers( numbers, count, from ) )

		{ LDEB(count); rval= -1; goto ready;	}
	    break;

	default:
	    LLDEB(count,unitSize); rval= -1; goto ready;
	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); goto ready;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_pVerticies:
	    sd->sdVertices= vertices;
	    vertices= (Point2DI *)0;	/* steal */
	    sd->sdVertexCount= count;
	    break;

	case DSHPprop_pWrapPolygonVertices:
	    sd->sdWrapPolygonVertices= vertices;
	    vertices= (Point2DI *)0;	/* steal */
	    sd->sdWrapPolygonVertexCount= count;
	    break;

	case DSHPprop_pSegmentInfo:
	    sd->sdSegmentInfos= numbers;
	    numbers= (int *)0;	/* steal */
	    sd->sdSegmentInfoCount= count;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

  ready:
    if  ( text )
	{ free( text );	}
    if  ( vertices )
	{ free( vertices );	}
    if  ( numbers )
	{ free( numbers );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Consume a picture value attached to a shape.			*/
/*									*/
/************************************************************************/

static int docRtfSaveShapeData(	RtfReader *	rr,
				const char *		text,
				int			len )
    {
    DrawingShape *	ds= rr->rrDrawingShape;

    if  ( ! ds )
	{ XDEB(ds); return 0;	}

    if  ( rr->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( utilMemoryBufferSetBytes( &(ds->dsPictureData),
					(const unsigned char *)text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }


static int docRtfReadShapePicture(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState		internRrs;

    int				rval;

    docRtfPushReadingState( rr, &internRrs );

    utilPropMaskClear( &(rr->rrcPicturePropMask) );
    docInitPictureProperties( &(rr->rrcPictureProperties) );

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1, rr,
			    (const RtfControlWord *)0, docRtfSaveShapeData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    docRtfPopReadingState( rr );

    return rval;
    }

int docRtfShpPicture(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    DrawingShape *	ds= rr->rrDrawingShape;

    if  ( ! ds )
	{ XDEB(ds);						}
    else{ ds->dsPictureProperties= rr->rrcPictureProperties;	}

    return 0;
    }

int docRtfShpString(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    const int	removeSemicolon= 0;
    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    /*
    SSDEB(rcw->rcwWord,text);
    */

    if  ( text )
	{ free( text );	}

    return 0;
    }

static int docRtfShpGetNumber(	long *			pVal,
				RtfReader *		rr )
    {
    int			rval= 0;
    long		val;

    const int		removeSemicolon= 0;
    char *		text= (char *)0;
    char *		from;
    char *		past;
    int			size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready;	}

    from= text;
    while( isspace( *from ) )
	{ from++;	}
    past= from;
    val= strtol( from, &past, 10 );
    if  ( past == from )
	{ SDEB(from); rval= -1; goto ready;	}
    from= past;

    *pVal= val;

  ready:

    if  ( text )
	{ free( text );	}

    return rval;
    }

int docRtfShapeNumber(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    DrawingShape *	ds= rr->rrDrawingShape;
    long		val;

    if  ( docRtfShpGetNumber( &val, rr ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}

    if  ( docSetShapeDrawingProperty( &(ds->dsDrawing), rcw->rcwID, val ) )
	{ SLDEB(rcw->rcwWord,val);	}

    return 0;
    }

int docRtfShpColor(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    DrawingShape *	ds= rr->rrDrawingShape;
    ShapeDrawing *	sd;
    long		val;
    RGB8Color		rgb8;

    if  ( docRtfShpGetNumber( &val, rr ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    utilInitRGB8Color( &rgb8 );
    rgb8.rgb8Red= val & 255; val /= 256;
    rgb8.rgb8Green= val & 255; val /= 256;
    rgb8.rgb8Blue= val & 255; val /= 256;

    switch( rcw->rcwID )
	{
	case DSHPprop_fillColor:
	    sd->sdFillColor= rgb8;
	    break;
	case DSHPprop_fillBackColor:
	    sd->sdFillBackColor= rgb8;
	    break;
	case DSHPprop_fillCrMod:
	    sd->sdFillCrModColor= rgb8;
	    break;

	case DSHPprop_lineColor:
	    sd->sdLineColor= rgb8;
	    break;
	case DSHPprop_lineBackColor:
	    sd->sdLineBackColor= rgb8;
	    break;
	case DSHPprop_lineCrMod:
	    sd->sdLineCrModColor= rgb8;
	    break;

	case DSHPprop_shadowColor:
	    sd->sdShadowColor= rgb8;
	    break;
	case DSHPprop_shadowHighlight:
	    sd->sdShadowHighlightColor= rgb8;
	    break;
	case DSHPprop_shadowCrMod:
	    sd->sdShadowCrModColor= rgb8;
	    break;
	case DSHPprop_c3DExtrusionColor:
	    sd->sdShadowc3DExtrusionColor= rgb8;
	    break;
	case DSHPprop_c3DCrMod:
	    sd->sdShadowc3DCrModColor= rgb8;
	    break;

	case DSHPprop_pictureTransparent:
	    /* ? */
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShapeProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int				rval= 0;

    if  ( rr->rrShapeProperty )
	{ SDEB(rr->rrShapeProperty->rcwWord);	}

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    rr->rrShapeProperty= (struct RtfControlWord *)0;
    utilEmptyMemoryBuffer( &(rr->rrShapePropertyName) );
    utilEmptyMemoryBuffer( &(rr->rrShapePropertyValue) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
		(RtfControlWord *)0, docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SDEB(rcw->rcwWord); rval= -1;	}

    rr->rrShapeProperty= (struct RtfControlWord *)0;
    utilEmptyMemoryBuffer( &(rr->rrShapePropertyName) );
    utilEmptyMemoryBuffer( &(rr->rrShapePropertyValue) );

    return rval;
    }

static int docRtfCommitShapePropertyName(
					const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    const int	removeSemicolon= 0;
    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    if  ( text )
	{
	rr->rrShapeProperty= docRtfFindShapePropertyWord( text );
	if  ( ! rr->rrShapeProperty )
	    { SXDEB(text,rr->rrShapeProperty);	}

	free( text );
	}
    else{ XDEB(text);		}

    return 0;
    }

static int docRtfCommitShapePropertyValue(
					const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    if  ( rr->rrShapeProperty )
	{
	if  ( (*rr->rrShapeProperty->rcwApply)( rr->rrShapeProperty, 0, rr ) )
	    {  SDEB(rr->rrShapeProperty->rcwWord); return -1;	}
	}
    else{
	const int	removeSemicolon= 0;
	char *		text= (char *)0;
	int		size;

	if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	    { LDEB(1); return -1;	}

	if  ( text )
	    { free( text );	}
	}

    return 0;
    }

int docRtfShapePropertyName(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    return docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
					    docRtfSaveDocEncodedText,
					    docRtfCommitShapePropertyName );
    }

static RtfControlWord	docRtfReadPictValueGroups[]=
    {
	RTF_DEST_XX( RTFtag_pict,	0,	docRtfReadShapePicture ),

	{ (char *)0, 0, 0 }
    };

int docRtfShapePropertyValue(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    return docRtfReadGroup( rcw, 0, 0, rr, docRtfReadPictValueGroups,
					    docRtfSaveDocEncodedText,
					    docRtfCommitShapePropertyValue );
    }

/************************************************************************/
/*									*/
/*  Read shape instructions.						*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShpinstGroups[]=
    {
	RTF_DEST_XX( "shptxt",	DOCinSHPTXT,	docRtfShpText ),
	RTF_DEST_XX( "sp",	1,		docRtfShapeProperty ),
	RTF_DEST_XX( "shpgrp",	SHPtyGROUP,	docRtfReadChildShape ),
	RTF_DEST_XX( "shp",	SHPtyUNKNOWN,	docRtfReadChildShape ),

	{ (char *)0, 0, 0 }
    };

static int docRtfShpinst(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int		res;

    res= docRtfReadGroup( rcw, 0, 0, rr,
		    docRtfShpinstGroups, docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Read the shape result and immediately discard it.			*/
/*									*/
/************************************************************************/

static int docRtfShprslt(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int		res= 0;

    BufferItem *	sectBi;
    SelectionScope	ss;
    int			treeType= DOCinSHPTXT;
    const int		ignoreEmpty= 0;

    DocumentTree	dt;

    rr->rrcInIgnoredGroup++;

    docInitDocumentTree( &dt );
    docInitSelectionScope( &ss );

    sectBi= docGetSectNode( rr->rrcNode );
    if  ( ! sectBi )
	{ XDEB(sectBi); res= -1; goto ready;	}

    ss.ssTreeType= DOCinSHPTXT;
    ss.ssSectNr= 0;
    ss.ssOwnerSectNr= sectBi->biNumberInParent;

    if  ( docRtfReadDocumentTree( rcw, &dt, &treeType,
						rr, ignoreEmpty, &ss ) )
	{ SDEB(rcw->rcwWord); res= -1; goto ready;	}

    docSetTreeTypeOfNode( dt.dtRoot, treeType );

  ready:

    docCleanDocumentTree( rr->rrDocument, &dt );
    rr->rrcInIgnoredGroup--;

    return res;
    }

/************************************************************************/
/*									*/
/*  Read a shape or a drawing object that is translayed to a shape.	*/
/*									*/
/************************************************************************/

static int docRtfReadShapeIntern(	DrawingShape **		pDs,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr,
					const RtfControlWord *	shapeGroups )
    {
    int			res;
    DrawingShape *	parent= rr->rrDrawingShape;
    DrawingShape *	ds= (DrawingShape *)0;

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    if  ( ! rr->rrcInIgnoredGroup )
	{
	const BufferItem *		bodySectBi;

	ds= docClaimDrawingShape( &(rr->rrDocument->bdShapeList) );
	if  ( ! ds )
	    { XDEB(ds); return -1;	}

	ds->dsDrawing.sdShapeType= rcw->rcwID;

	bodySectBi= docGetBodySectNode( rr->rrcNode, rr->rrDocument );
	if  ( ! bodySectBi )
	    { XDEB(bodySectBi); return -1;	}

	ds->dsSelectionScope.ssTreeType= DOCinSHPTXT;
	ds->dsSelectionScope.ssSectNr= 0;
	ds->dsSelectionScope.ssOwnerSectNr= bodySectBi->biNumberInParent;
	ds->dsSelectionScope.ssOwnerNumber= ds->dsShapeNumber;

	rr->rrDrawingShape= ds;
	}

    res= docRtfReadGroup( rcw, 0, 0, rr,
			shapeGroups, docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{
	SLDEB(rcw->rcwWord,res);
	if  ( ds )
	    { docDeleteDrawingShape( rr->rrDocument, ds );	}
	}
    else{
	if  ( ! rr->rrcInIgnoredGroup )
	    {
	    BufferItem *	paraBi= rr->rrcNode;
	    TextParticule *	tp;
	    RtfReadingState *	rrs= rr->rrcState;

	    if  ( rrs->rrsTextShadingChanged )
		{ docRtfRefreshTextShading( rr, rrs );	}

	    tp= docAppendObject( rr->rrDocument, paraBi,
						    &(rrs->rrsTextAttribute) );
	    if  ( ! tp )
		{
		LDEB(paraBi->biParaParticuleCount);
		docDeleteDrawingShape( rr->rrDocument, ds );
		res= -1;
		}
	    else{
		InsertedObject *	io;

		io= docGetObject( rr->rrDocument, tp->tpObjectNumber );
		if  ( ! io )
		    { LXDEB(tp->tpObjectNumber,io); res= -1;	}
		else{
		    io->ioKind= DOCokDRAWING_SHAPE;
		    io->ioDrawingShape= ds;
		    io->ioInline= 0;
		    }

		}
	    *pDs= ds;
	    }
	}

    rr->rrDrawingShape= parent;
    return res;
    }

/************************************************************************/
/*									*/
/*  The actual shape as opposed to the result in simpler RTF.		*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShapeGroups[]=
    {
	RTF_DEST_XX( "shptxt",	DOCinSHPTXT,	docRtfShpText ),
	RTF_DEST_XX( "shprslt",	DOCinSHPTXT,	docRtfShprslt ),
	RTF_DEST_XX( "shpgrp",	SHPtyGROUP,	docRtfReadChildShape ),
	RTF_DEST_XX( "shp",	SHPtyUNKNOWN,	docRtfReadChildShape ),
	RTF_DEST_XX( "shpinst",	SHPtyUNKNOWN,	docRtfShpinst ),

	{ (char *)0, 0, 0, }
    };

static int docRtfReadChildShape(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    int			res;
    DrawingShape *	parent= rr->rrDrawingShape;
    DrawingShape *	ds;

    if  ( ! rr->rrcInIgnoredGroup )
	{
	ds= docClaimShapeInParent( &(rr->rrDocument->bdShapeList),
						    parent, -1, rcw->rcwID );
	if  ( ! ds )
	    { XDEB(ds); return -1;	}

	ds->dsDrawing.sdShapeType= rcw->rcwID;

	ds->dsSelectionScope= parent->dsSelectionScope;
	ds->dsSelectionScope.ssOwnerNumber= ds->dsShapeNumber;

	rr->rrDrawingShape= ds;
	}

    res= docRtfReadGroup( rcw, 0, 0, rr,
		    docRtfShapeGroups, docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rr->rrDrawingShape= parent;
    return res;
    }

int docRtfReadShape(			const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    DrawingShape *	ds= (DrawingShape *)0;

    if  ( ! docRtfGetParaNode( rr ) )
	{ SDEB(rcw->rcwWord); return -1; }

    return docRtfReadShapeIntern( &ds, rcw, arg, rr, docRtfShapeGroups );
    }

/************************************************************************/
/*									*/
/*  Make fixes to a shape that originates from reading a drawing	*/
/*  object.								*/
/*									*/
/*  1)  Word only uses the vertex in a drawing object to determine	*/
/*	what diagonal to draw.						*/
/*  2)  Word swaps the arrow heads. Why?				*/
/*									*/
/************************************************************************/

static int docRtfDrawingObjectToShape(	DrawingShape *	ds )
    {
    ShapeDrawing *		sd= &(ds->dsDrawing);

    if  ( ds->dsDrawing.sdShapeType == SHPtyLINE	&&
	  ! ds->dsIsChildShape				&&
	  sd->sdVertexCount == 2			)
	{
	ShapeArrow		sa;

	/*  1  */
	if  ( sd->sdVertices[0].x <= sd->sdVertices[1].x )
	    { sd->sd_fFlipH= 0;	}
	else{ sd->sd_fFlipH= 1;	}
	if  ( sd->sdVertices[0].y <= sd->sdVertices[1].y )
	    { sd->sd_fFlipV= 0;	}
	else{ sd->sd_fFlipV= 1;	}

	/*  2  */
	sa= sd->sdLineHeadArrow;
	sd->sdLineHeadArrow= sd->sdLineTailArrow;
	sd->sdLineTailArrow= sa;
	}

    return 0;
    }

static RtfControlWord	docRtfDoGroups[]=
    {
	RTF_DEST_XX( "dptxbxtext",	DOCinSHPTXT,	docRtfShpText ),

	{ (char *)0, 0, 0 }
    };

int docRtfReadDrawingObject(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rr )
    {
    DrawingShape *	ds= (DrawingShape *)0;

    if  ( docRtfReadShapeIntern( &ds, rcw, arg, rr, docRtfDoGroups ) )
	{ SDEB(rcw->rcwWord);	}

    if  ( ! rr->rrcInIgnoredGroup )
	{
	if  ( docRtfDrawingObjectToShape( ds ) )
	    { LDEB(1);	}
	}

    return 0;
    }

