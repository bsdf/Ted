/************************************************************************/
/*									*/
/*  Calculate statistics in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<docStatistics.h>
#   include	"docBuf.h"
#   include	"docCollectStatistics.h"
#   include	"docTreeNode.h"
#   include	<docTextLine.h>
#   include	"docTreeScanner.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

static int docCollectNodeStatistics(
				struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voiddocs )
    {
    DocumentStatistics *	docs= (DocumentStatistics *)voiddocs;
    int				i;

    if  ( node->biLevel == DOClevPARA )
	{
	docs->dsParagraphCount++;
	docs->dsCharacterCount += docParaStrlen( node );
	docs->dsLineCount += node->biParaLineCount;

	for ( i= 0; i < node->biParaLineCount; i++ )
	    { docs->dsWordCount += node->biParaLines[i].tlWordCount;	}
	}

    return 0;
    }

void docCollectDocumentStatistics(	DocumentStatistics *	ds,
					const BufferDocument *	bd )
    {
    const int	flags= 0;

    docInitDocumentStatistics( ds );

    if  ( docScanTreeNode( (BufferDocument *)bd, bd->bdBody.dtRoot,
				    docCollectNodeStatistics, (NodeVisitor)0,
				    flags, (void *)ds ) )
	{ LDEB(1);	}

    ds->dsPageCount= bd->bdBody.dtRoot->biBelowPosition.lpPage+ 1;

    return;
    }

