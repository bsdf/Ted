/************************************************************************/
/*									*/
/*  Read the the list override table of a document.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"

#   include	<docListAdmin.h>

#   include	<appDebugon.h>

int docRtfRememberListOverrideLevelProperty(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    ListOverrideLevel *		lol= &(rrc->rrcListOverrideLevel);

    switch( rcw->rcwID )
	{
	case LOLpropSTARTAT:
	    lol->lolOverrideStartAt= arg != 0;
	    break;

	case LOLpropFORMAT:
	    lol->lolOverrideFormat= arg != 0;
	    break;

	/* LOLpropLEVEL */

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfRememberListOverrideProperty(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    ListOverride *		lo= &(rrc->rrcListOverride);

    if  ( docSetListOverrideProperty( lo, rcw->rcwID, arg ) )
	{ SDEB(rcw->rcwWord);	}

    return 0;
    }

static int docRtfListOverrideLevelLevel(	const RtfControlWord *	rcw,
						int			arg,
						RtfReader *	rrc )
    {
    const int		copyIds= 1;
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    if  ( docRtfReadListLevelGroup( rcw, rrc ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docCopyDocumentListLevel( &(rrc->rrcListOverrideLevel.lolListLevel),
				    &(rrc->rrcDocumentListLevel),
				    copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideLevelGroups[]=
    {
	RTF_DEST_XX( "listlevel", LOLpropLEVEL,	docRtfListOverrideLevelLevel ),

	{ (char *)0, 0, 0 }
    };

static int docRtfListOverrideLevel(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    docCleanListOverrideLevel( &(rrc->rrcListOverrideLevel) );
    docInitListOverrideLevel( &(rrc->rrcListOverrideLevel) );

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfListOverrideLevelGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docListOverrideAddLevel( &(rrc->rrcListOverride),
				    &(rrc->rrcListOverrideLevel),
				    fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    docCleanListOverrideLevel( &(rrc->rrcListOverrideLevel) );
    docInitListOverrideLevel( &(rrc->rrcListOverrideLevel) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideGroups[]=
    {
	RTF_DEST_XX( "lfolevel", DLpropLEVELS,	docRtfListOverrideLevel ),

	{ (char *)0, 0, 0 }
    };

static int docRtfListOverride(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    DocumentProperties *	dp= &(rrc->rrDocument->bdProperties);

    docCleanListOverride( &(rrc->rrcListOverride) );
    docInitListOverride( &(rrc->rrcListOverride) );

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfListOverrideGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docListOverrideTableSetOverride(
				    &(dp->dpListAdmin->laListOverrideTable),
				    &(rrc->rrcListOverride),
				    fontMap, colorMap, rulerMap ) < 0 )
	{ LDEB(1); return -1;	}

    docCleanListOverride( &(rrc->rrcListOverride) );
    docInitListOverride( &(rrc->rrcListOverride) );

    return 0;
    }

static RtfControlWord	docRtfListtableGroups[]=
    {
	RTF_DEST_XX( "listoverride", DPpropLISTTABLE, docRtfListOverride ),

	{ (char *)0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Read the list override table of a document.				*/
/*									*/
/*  1)  Read the overrides in the table.				*/
/*  2)  Allocate memory to manage the paragraphs in the lists in the	*/
/*	body. Allocation for the trees in external items is done	*/
/*	lazily.								*/
/*									*/
/************************************************************************/

int docRtfListOverrideTable(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    BufferDocument *		bd= rrc->rrDocument;
    DocumentTree *		dt= &(rrc->rrDocument->bdBody);
    DocumentProperties *	dp= &(bd->bdProperties);

    /*  1  */
    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfListtableGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    /*  2  */
    if  ( docClaimListNumberTrees( &(dt->dtListNumberTrees),
		       dp->dpListAdmin->laListOverrideTable.lotOverrideCount ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a list override table to rtf.					*/
/*									*/
/************************************************************************/

static void docRtfWriteListOverrideLevels(
				RtfWriter *		rwc,
				const ListOverride *		lo )
    {
    const ListOverrideLevel *	lol;
    int				lev;

    docRtfWriteNextLine( rwc );

    lol= lo->loLevels;
    for ( lev= 0; lev < lo->loLevelCount; lol++, lev++ )
	{
	docRtfWriteDestinationBegin( rwc, "lfolevel" );

	if  ( lol->lolOverrideStartAt && ! lol->lolOverrideFormat )
	    {
	    docRtfWriteArgTag( rwc, "listoverridestartat\\levelstartat",
						lol->lolListLevel.llStartAt );
	    }

	if  ( lol->lolOverrideFormat )
	    {
	    docRtfWriteTag( rwc, "listoverrideformat" );

	    docRtfWriteListLevel( rwc, &(lol->lolListLevel) );
	    }

	docRtfWriteDestinationEnd( rwc );
	docRtfWriteNextLine( rwc );
	}
    return;
    }

/************************************************************************/
/*									*/
/*  Write the list override table for a document.			*/
/*									*/
/************************************************************************/

void docRtfWriteListOverrideTable(	RtfWriter *			rwc,
					const ListOverrideTable *	lot )
    {
    int				ov;
    const ListOverride *	lo;
    int				overrideCount;

    docRtfWriteDestinationBegin( rwc, "*\\listoverridetable" );
    docRtfWriteNextLine( rwc );

    overrideCount= lot->lotOverrideCount;
    while( lot->lotOverrideCount > 0				&&
	   lot->lotOverrides[overrideCount- 1].loIndex < 0	)
	{ overrideCount--;	}

    lo= lot->lotOverrides;
    for ( ov= 0; ov < overrideCount; lo++, ov++ )
	{
	if  ( lo->loIndex < 1 )
	    { continue;	}

	docRtfWriteDestinationBegin( rwc, "listoverride" );

	if  ( lo->loListID != -1 )
	    { docRtfWriteArgTag( rwc, "listid", lo->loListID );	}

	docRtfWriteArgTag( rwc, "listoverridecount", lo->loOverrideCount );

	if  ( lo->loLevelCount > 0 )
	    { docRtfWriteListOverrideLevels( rwc, lo ); }

	docRtfWriteArgTag( rwc, "ls", lo->loIndex );

	docRtfWriteDestinationEnd( rwc );
	if  ( ov+ 1 < overrideCount )
	    { docRtfWriteNextLine( rwc );	}
	}

    docRtfWriteDestinationEnd( rwc );
    docRtfWriteNextLine( rwc );

    return;
    }

