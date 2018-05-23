/************************************************************************/
/*									*/
/*  Exchange of section properties with RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>

/************************************************************************/
/*									*/
/*  Handle a section property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberSectionProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    SectionProperties *		sp= &(rrc->rrcSectionProperties);

    if  ( rrc->rrcTree						&&
	  rrc->rrcTree->dtRoot					&&
	  rrc->rrcTree->dtRoot->biTreeType != DOCinBODY	)
	{
	/* SSDEB(docTreeTypeStr(rrc->rrcTree->dtRoot->biTreeType),rcw->rcwWord); */
	return 0;
	}

    switch( rcw->rcwID )
	{
	case SPprop_NONE:
	    {
	    const DocumentProperties *	dp;

	    if  ( ! rrc->rrDocument )
		{ XDEB(rrc->rrDocument); return -1;	}

	    dp= &(rrc->rrDocument->bdProperties);

	    docCleanSectionProperties( sp );
	    docInitSectionProperties( sp );
	    rrc->rrcSectionColumn= 0;

	    sp->spDocumentGeometry= dp->dpGeometry;
	    sp->spNotesProperties= dp->dpNotesProps;
	    }
	    return 0;

	case SPpropSTYLE:
	    rrc->rrcStyle.dsLevel= DOClevSECT;
	    break;

	/***/
	case SPprop_COLUMN_NUMBER:
	    rrc->rrcSectionColumn= arg- 1;
	    return 0;

	case SPprop_COLUMN_WIDTH:
	    if  ( sp->spColumnCount < 2				||
		  rrc->rrcSectionColumn < 0			||
		  rrc->rrcSectionColumn >= sp->spColumnCount	)
		{
		/*LLDEB(rrc->rrcSectionColumn,sp->spColumnCount);*/
		return 0;
		}

	    sp->spColumns[rrc->rrcSectionColumn].scColumnWidthTwips= arg;
	    return 0;

	case SPprop_COLUMN_RIGHT:
	    if  ( sp->spColumnCount < 2				||
		  rrc->rrcSectionColumn < 0			||
		  rrc->rrcSectionColumn >= sp->spColumnCount	)
		{ LLDEB(rrc->rrcSectionColumn,sp->spColumnCount); return 0; }

	    sp->spColumns[rrc->rrcSectionColumn].scSpaceToRightTwips= arg;
	    return 0;
	}

    if  ( docSetSectionProperty( sp, rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rrc->rrcSectionPropertyMask), rcw->rcwID );
    PROPmaskADD( &(rrc->rrcStyle.dsSectMask), rcw->rcwID );

    return 0;
    }

