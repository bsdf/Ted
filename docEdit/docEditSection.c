/************************************************************************/
/*									*/
/*  Section manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	"docCopyNode.h"
#   include	<docTreeType.h>

/************************************************************************/

static int docCopySectHdFt(	DocumentTree *		eiTo,
				BufferDocument *	bdTo,
				const BufferItem *	bodySectBi,
				BufferDocument *	bdFrom,
				DocumentTree *		dtFrom,
				int			treeType )
    {
    int			rval= 0;

    EditOperation	eo;
    DocumentCopyJob	dcj;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );

    docEraseDocumentTree( bdTo, eiTo );

    if  ( dtFrom->dtRoot )
	{
	const int	ownerNumber= -1;
	BufferItem *	sectBi;

	sectBi= docMakeTreeRoot( bdTo, eiTo,
					    bodySectBi, ownerNumber, treeType );
	if  ( ! sectBi )
	    { XDEB(sectBi); rval= -1; goto ready;	}

	eo.eoTree= eiTo;
	eo.eoDocument= bdTo;
	eo.eoSelectionScope= sectBi->biSectSelectionScope;

	if  ( bdTo == bdFrom )
	    {
	    const int	copyFields= 1;

	    /*  Not a reorganisation but a copy:	*/
	    if  ( docSet1DocumentCopyJob( &dcj, &eo, copyFields ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    dcj.dcjSourceTree= dtFrom;
	    }
	else{
	    const int	forceAttributeTo= -1;

	    if  ( docSet2DocumentCopyJob( &dcj, &eo, bdFrom, dtFrom,
					(MemoryBuffer *)0, forceAttributeTo ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	if  ( docCopySectChildren( &dcj, sectBi, dtFrom->dtRoot ) )
	    { XDEB(sectBi); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );

    return rval;
    }

int docCopySectHeadersFooters(	BufferItem *		sectBiTo,
				BufferDocument *	bdTo,
				BufferItem *		sectBiFrom,
				BufferDocument *	bdFrom )
    {
    SectHeadersFooters *	shfFrom= sectBiFrom->biSectHeadersFooters;
    SectHeadersFooters *	shfTo;

    if  ( ! shfFrom )
	{
	if  ( sectBiTo->biSectHeadersFooters )
	    {
	    docCleanSectHeadersFooters( bdTo, sectBiTo->biSectHeadersFooters );
	    free( sectBiTo->biSectHeadersFooters );
	    sectBiTo->biSectHeadersFooters= (SectHeadersFooters *)0;
	    }

	return 0;
	}

    if  ( ! sectBiTo->biSectHeadersFooters )
	{
	sectBiTo->biSectHeadersFooters= (SectHeadersFooters *)malloc(
						sizeof(SectHeadersFooters) );
	if  ( ! sectBiTo->biSectHeadersFooters )
	    { XDEB(sectBiTo->biSectHeadersFooters); return -1;	}
	docInitSectHeadersFooters( sectBiTo->biSectHeadersFooters);
	}

    shfTo= sectBiTo->biSectHeadersFooters;

    if  ( docCopySectHdFt( &(shfTo->shfFirstPageHeader),
		bdTo, sectBiTo, bdFrom,
		&(shfFrom->shfFirstPageHeader), DOCinFIRST_HEADER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfLeftPageHeader),
		bdTo, sectBiTo, bdFrom,
		&(shfFrom->shfLeftPageHeader), DOCinLEFT_HEADER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfRightPageHeader),
		bdTo, sectBiTo, bdFrom,
		&(shfFrom->shfRightPageHeader), DOCinRIGHT_HEADER ) )
	{ LDEB(1); return -1;	}

    if  ( docCopySectHdFt( &(shfTo->shfFirstPageFooter),
		bdTo, sectBiTo, bdFrom,
		&(shfFrom->shfFirstPageFooter), DOCinFIRST_FOOTER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfLeftPageFooter),
		bdTo, sectBiTo, bdFrom,
		&(shfFrom->shfLeftPageFooter), DOCinLEFT_FOOTER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfRightPageFooter),
		bdTo, sectBiTo, bdFrom,
		&(shfFrom->shfRightPageFooter), DOCinRIGHT_FOOTER ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docCopySectDescription(	BufferItem *		sectNodeTo,
				BufferDocument *	bdTo,
				BufferItem *		sectNodeFrom,
				BufferDocument *	bdFrom )
    {
    if  ( sectNodeFrom->biTreeType == DOCinBODY			&&
	  docCopySectHeadersFooters( sectNodeTo, bdTo, sectNodeFrom, bdFrom ) )
	{ LDEB(sectNodeFrom->biTreeType); return -1;	}

    if  ( docCopySectionProperties( &(sectNodeTo->biSectProperties),
					&(sectNodeFrom->biSectProperties) ) )
	{ LDEB(1);	}

    return 0;
    }
