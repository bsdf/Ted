/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/*  Find the object in the document for an x,y position.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	<docBuf.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

int docFindHeaderFooterForY(		DocumentTree **		pTree,
					struct BufferItem **	pSelSectNode,
					int *			pColumn,
					const LayoutContext *	lc,
					struct BufferItem *	bodySectNode,
					BufferDocument *	bd,
					int			page,
					int			docY )
    {
    int				isEmpty;
    DocumentTree *		dt;

    DocumentRectangle		drExtern;
    const int			justUsed= 0;

    /*  4  */
    dt= (DocumentTree *)0;
    docWhatPageHeader( &dt, &isEmpty, bodySectNode, page, bd );
    if  ( dt && dt->dtRoot )
	{
	const int		headerColumn= 0;

	if  ( docGetBoxAroundTree( &drExtern, bodySectNode, dt, justUsed,
						    page, headerColumn, lc ) )
	    { LDEB(1);	}
	else{
	    if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		{
		*pTree= dt;
		*pSelSectNode= dt->dtRoot;
		*pColumn= headerColumn;

		return 0;
		}
	    }
	}

    /*  5  */
    dt= (DocumentTree *)0;
    docWhatPageFooter( &dt, &isEmpty, bodySectNode, page, bd );
    if  ( dt && dt->dtRoot )
	{
	const int		footerColumn= 0;

	if  ( docGetBoxAroundTree( &drExtern, bodySectNode, dt, justUsed,
						    page, footerColumn, lc ) )
	    { LDEB(1);	}
	else{
	    if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		{
		*pTree= dt;
		*pSelSectNode= dt->dtRoot;
		*pColumn= footerColumn;

		return 0;
		}
	    }
	}

    return 1;
    }

