/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	"docIntermediaryDocument.h"
#   include	<docPropertiesAdmin.h>

/************************************************************************/
/*									*/
/*  Build the document that will receive the table of contents, before	*/
/*  it is copied over to the target document.				*/
/*									*/
/*  A couple of administrations are copied over to make sure that	*/
/*  properties have the same indexes in the administrations of both	*/
/*  documents.								*/
/*									*/
/************************************************************************/

BufferDocument * docIntermediaryDocument(
				struct BufferItem **		pSectNode,
				const BufferDocument *		bdRef )
    {
    BufferDocument *	rval= (BufferDocument *)0;
    BufferDocument *	bdTo= (BufferDocument *)0;
    struct BufferItem *	sectNode;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    bdTo= docNewDocument( bdRef );
    if  ( ! bdTo )
	{ XDEB(bdTo); goto ready;	}

    if  ( docCopyDocumentProperties( &(bdTo->bdProperties),
					    &(bdRef->bdProperties) ) )
	{ LDEB(1); goto ready;	}

    if  ( docCopyStyleSheet( &(bdTo->bdStyleSheet),
					    &(bdRef->bdStyleSheet), dam0 ) )
	{ LDEB(1); goto ready;	}

    sectNode= docInsertNode( bdTo, bdTo->bdBody.dtRoot, -1, DOClevSECT );
    if  ( ! sectNode )
	{ XDEB(sectNode); goto ready;	}

    rval= bdTo; bdTo= (BufferDocument *)0; /* steal */
    if  ( pSectNode )
	{ *pSectNode= sectNode;	}

  ready:

    if  ( bdTo )
	{ docFreeIntermediaryDocument( bdTo );	}

    return rval;
    }

void docFreeIntermediaryDocument(	BufferDocument *		bd )
    {
    bd->bdPropertyLists= (DocumentPropertyLists *)0;

    docFreeDocument( bd );
    }
