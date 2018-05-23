/************************************************************************/
/*									*/
/*  Renumber notes as the result of a modification to the document.	*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docRecalculateFields.h"
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Renumber the notes of a document.					*/
/*									*/
/************************************************************************/

typedef struct RenumberNotes
    {
    int				rnChanged;

    int				rnFootN;
    int				rnEndN;
    int				rnSect;
    int				rnPage;

    const FootEndNotesProperties *	rnNotesProperties;
    } RenumberNotes;

static void docRenumberNote(	DocumentNote *		dn,
				const DocumentField *	df,
				RenumberNotes *		rn )
    {
    const NotesProperties *	np;

    if  ( NOTE_IS_DELETED( dn ) )
	{ LDEB(df->dfFieldNumber); return;	}

    if  ( ! dn->dnNoteProperties.npAutoNumber )
	{
	if  ( dn->dnNoteNumber != 0 )
	    { rn->rnChanged= 1;	}

	dn->dnNoteNumber= 0;
	return;
	}

    /****/
    np= &(rn->rnNotesProperties->fepFootnotesProps);

    if  ( np->npRestart == FTN_RST_PER_SECTION		&&
	  df->dfSelectionScope.ssSectNr != rn->rnSect	)
	{ rn->rnFootN= 0;	}

    if  ( np->npRestart == FTN_RST_PER_PAGE		&&
	  dn->dnReferringPage != rn->rnPage		)
	{ rn->rnFootN= 0;	}

    /****/
    np= &(rn->rnNotesProperties->fepEndnotesProps);

    if  ( np->npRestart == FTN_RST_PER_SECTION		&&
	  df->dfSelectionScope.ssSectNr != rn->rnSect	)
	{ rn->rnEndN= 0;	}

    if  ( np->npRestart == FTN_RST_PER_PAGE		&&
	  dn->dnReferringPage != rn->rnPage		)
	{ rn->rnEndN= 0;	}

    /****/

    switch( dn->dnNoteProperties.npTreeType )
	{
	case DOCinFOOTNOTE:
	    if  ( dn->dnNoteNumber != rn->rnFootN )
		{ dn->dnNoteNumber=   rn->rnFootN; rn->rnChanged= 1;	}
	    rn->rnFootN++;
	    break;

	case DOCinENDNOTE:
	    if  ( dn->dnNoteNumber != rn->rnEndN )
		{ dn->dnNoteNumber=   rn->rnEndN; rn->rnChanged= 1;	}
	    rn->rnEndN++;
	    break;

	default:
	    LDEB(dn->dnNoteProperties.npTreeType);
	    break;
	}

    rn->rnSect= df->dfSelectionScope.ssSectNr;
    rn->rnPage= dn->dnReferringPage;
    return;
    }

/************************************************************************/
/*									*/
/*  Renumber notes: Scan the fields in preorder and change the numbers.	*/
/*									*/
/************************************************************************/

void docRenumberNotes(		int *			pChanged,
				BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    RenumberNotes		rn;

    DocumentField *		df;

    rn.rnChanged= 0;
    rn.rnFootN= 0;
    rn.rnEndN= 0;
    rn.rnPage= -1;
    rn.rnSect= -1;
    rn.rnNotesProperties= &(dp->dpNotesProps);

    df= docGetFirstField( &(bd->bdBody.dtRootFields) );
    while( df )
	{
	if  ( df->dfKind == DOCfkCHFTN )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn); continue;	}

	    docRenumberNote( dn, df, &rn );
	    }

	df= docGetNextField( &(bd->bdBody.dtRootFields), df );
	}

    *pChanged= rn.rnChanged;
    return;
    }

