/************************************************************************/
/*									*/
/*  Read/Write tab stops from/to rtf.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"

/************************************************************************/
/*									*/
/*  Save a tab stop.							*/
/*									*/
/************************************************************************/

void docRtfSaveTabStopList( 	RtfWriter *		rw,
				const TabStopList *	tsl )
    {
    int			i;
    const TabStop *	ts;

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	if  ( rw->rwCol >= 65 )
	    { docRtfWriteNextLine( rw );	}

	if  ( ts->tsAlignment != DOCtaLEFT )
	    {
	    docRtfWriteEnumTag( rw, DOCrtf_TabAlignTags, ts->tsAlignment,
				    DOCrtf_TabAlignTagCount, DOCta_COUNT );
	    }

	if  ( ts->tsLeader != DOCtlNONE )
	    {
	    docRtfWriteEnumTag( rw, DOCrtf_TabLeaderTags, ts->tsLeader,
				    DOCrtf_TabLeaderTagCount, DOCtl_COUNT );
	    }

	if  ( ts->tsFromStyleOrList )
	    { docRtfWriteTag( rw, "jclisttab" );	}

	docRtfWriteArgTag( rw, "tx", ts->tsTwips );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a tab stop property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberTabStopProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    TabStop *		ts= &(rrc->rrcTabStop);

    if  ( docTabStopSetProperty( ts, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return 0;	}

    if  ( rcw->rcwID == TABpropX )
	{
	if  ( docAddTabToListTwips( &(rrs->rrsTabStopList), ts ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return -1;	}

	docInitTabStop( ts );

	PROPmaskADD( &(rrc->rrcStyle.dsParaMask), PPpropTAB_STOPS );
	}

    return 0;
    }
