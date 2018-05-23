/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*  Manage resources for layout.					*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<drawMetafilePsList.h>
#   include	"docParticuleData.h"
#   include	"docPsPrintImpl.h"
#   include	<docListFonts.h>
#   include	<sioHex.h>
#   include	<sioMemory.h>
#   include	<docObjectProperties.h>
#   include	<docTreeNode.h>
#   include	"docMetafileObject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of PostScriptFonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Find the fonts of an image.						*/
/*									*/
/************************************************************************/

int docPsListImageFonts(	PostScriptTypeList *		pstl,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				const LayoutContext *		lc,
				const char *			prefix )
    {
    int				rval= 0;
    MetafileWriteListPs		listFontsPs;

    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisMeta= (SimpleInputStream *)0;

    MetafilePlayer		mp;

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	    listFontsPs= appWmfListFontsPs;
	    break;

	case DOCokMACPICT:
	    listFontsPs= appMacPictListFontsPs;
	    break;

	case DOCokPICTEMFBLIP:
	    listFontsPs= appEmfListFontsPs;
	    break;

	default:
	    LDEB(pip->pipType); goto ready;
	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMeta); rval= -1; goto ready;	}

    docSetMetafilePlayer( &mp, sisMeta, lc, pip, 0, 0 );

    if  ( (*listFontsPs)( pstl, &mp, prefix ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:
    if  ( sisMeta )
	{ sioInClose( sisMeta );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    return rval;
    }

/************************************************************************/

typedef struct GetDocumentFonts
    {
    const LayoutContext *		gdfLayoutContext;
    PostScriptTypeList *		gdfPostScriptTypeList;
    } GetDocumentFonts;

/************************************************************************/

static int docPsListObjectFonts( const InsertedObject *		io,
				const char *			prefix,
				void *				through )
    {
    GetDocumentFonts *		gdf= (GetDocumentFonts *)through;
    const LayoutContext *	lc= gdf->gdfLayoutContext;
    PostScriptTypeList *	pstl= gdf->gdfPostScriptTypeList;

    switch( io->ioKind )
	{
	case DOCokMACPICT:
	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	    return docPsListImageFonts( pstl,
				&(io->ioPictureProperties),
				&(io->ioObjectData), lc, prefix );
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		||
		  io->ioResultKind == DOCokMACPICT		)
		{
		return docPsListImageFonts( pstl,
				&(io->ioPictureProperties),
				&(io->ioResultData), lc, prefix );
		}
	    else{ /*LDEB(io->ioResultKind);*/ return 0;	}

	case DOCokEPS_FILE:
	    LDEB(io->ioKind);
	    return 0;

	case DOCokDRAWING_SHAPE:
	    return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}
    }

static int docPsPrintGotSpan(	BufferDocument *		bd,
				BufferItem *			paraNode,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				from,
				int				upto,
				void *				through )
    {
    GetDocumentFonts *		gdf= (GetDocumentFonts *)through;
    const LayoutContext *	lc= gdf->gdfLayoutContext;
    const DocumentFontList *	dfl= bd->bdProperties.dpFontList;
    PostScriptTypeList *	pstl= gdf->gdfPostScriptTypeList;
    const IndexSet *		unicodesWanted;

    const AfmFontInfo *		afi;

    const int			appearsInText= 1;

    afi= (*lc->lcGetFontForAttribute)( &unicodesWanted,
					ta, dfl, lc->lcPostScriptFontList );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    if  ( psRememberPostsciptFace( pstl, afi, ta, "f", appearsInText ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docPsPrintGetDocumentFonts(	PostScriptTypeList *		pstl,
				const LayoutContext *		lc )
    {
    ScanDocumentFonts		sdf;
    GetDocumentFonts		gdf;

    docInitScanDocumentFonts( &sdf );

    gdf.gdfPostScriptTypeList= pstl;
    gdf.gdfLayoutContext= lc;

    sdf.sdfListObjectFonts= docPsListObjectFonts;
    sdf.sdfDocListSpanFont= docPsPrintGotSpan;
    sdf.sdfThrough= &gdf;

    /*  a  */
    if  ( docListDocumentFonts( lc->lcDocument, &sdf ) )
	{ LDEB(1); return -1;;	}

    return 0;
    }

