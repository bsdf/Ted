/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stddef.h>

#   include	"docListFonts.h"
#   include	"docShape.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of Document Fonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

typedef struct GetAttributesUsed
    {
    IndexSet *	gauAttributesUsed;
    } GetAttributesUsed;

static int docAttributesUsedGotSpan(
				BufferDocument *		bd,
				struct BufferItem *		paraNode,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				from,
				int				upto,
				void *				through )
    {
    GetAttributesUsed *		gau= (GetAttributesUsed *)through;

    if  ( utilIndexSetAdd( gau->gauAttributesUsed, textAttrNr ) )
	{ LDEB(textAttrNr); return -1;	}

    return 0;
    }

int docGetAttributesUsedInTree(	BufferDocument *		bd,
				const DocumentTree *		dt,
				IndexSet *			isUsed )
    {
    ScanDocumentFonts		sdf;
    GetAttributesUsed		gau;

    docInitScanDocumentFonts( &sdf );

    gau.gauAttributesUsed= isUsed;

    sdf.sdfListObjectFonts= (DocListObjectFonts)0;
    sdf.sdfDocListSpanFont= docAttributesUsedGotSpan;
    sdf.sdfThrough= &gau;

    /*  a  */
    if  ( docListTreeFonts( bd, dt, &sdf ) )
	{ LDEB(1); return -1;;	}

    return 0;
    }

int docGetAttributesUsedInShape( BufferDocument *		bd,
				const DrawingShape *		ds,
				IndexSet *			isUsed )
    {
    int		child;

    if  ( docGetAttributesUsedInTree( bd, &(ds->dsDocumentTree), isUsed ) )
	{ LDEB(1); return -1;	}

    for ( child= 0; child < ds->dsChildCount; child++ )
	{
	if  ( docGetAttributesUsedInShape( bd, ds->dsChildren[child], isUsed ) )
	    { LDEB(child); return -1;	}
	}

    return 0;
    }
