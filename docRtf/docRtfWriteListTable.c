/************************************************************************/
/*									*/
/*  Read the list table of a document.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	<docListLevel.h>
#   include	<docDocumentList.h>

/************************************************************************/
/*									*/
/*  Write a list level to rtf.						*/
/*									*/
/************************************************************************/

void docRtfWriteListLevel(	RtfWriter *		rwc,
				const ListLevel *	ll )
    {
    MemoryBuffer		mbtext;
    MemoryBuffer		mbnumbers;

    utilInitMemoryBuffer( &mbtext );
    utilInitMemoryBuffer( &mbnumbers );

    docRtfWriteDestinationBegin( rwc, "listlevel" );

    docRtfWriteArgTag( rwc, "levelnfc", ll->llNumberStyle );
    docRtfWriteArgTag( rwc, "levelnfcn", ll->llNumberStyle );
    docRtfWriteArgTag( rwc, "leveljc", ll->llJustification );
    docRtfWriteArgTag( rwc, "leveljcn", ll->llJustification );

    docRtfWriteArgTag( rwc, "levelfollow", ll->llFollow );
    docRtfWriteArgTag( rwc, "levelstartat", ll->llStartAt );

    if  ( ll->llPrevToDigits )
	{ docRtfWriteTag( rwc, "levellegal" );	}
    if  ( ll->llNoRestart )
	{ docRtfWriteTag( rwc, "levelnorestart" );	}

    if  ( ll->llFromOld )
	{ docRtfWriteTag( rwc, "levelold" );	}
    if  ( ll->llUsePrevText )
	{ docRtfWriteTag( rwc, "levelprev" );	}
    if  ( ll->llUsePrevSpace )
	{ docRtfWriteTag( rwc, "levelprevspace" );	}

    if  ( ll->llIndent != 0 )
	{ docRtfWriteArgTag( rwc, "levelindent", ll->llIndent ); }
    if  ( ll->llSpace != 0 )
	{ docRtfWriteArgTag( rwc, "levelspace", ll->llSpace ); }

    if  ( docListLevelToRtfStrings( &mbtext, &mbnumbers, ll ) )
	{ LDEB(1);	}
    else{
	int			size= 0;
	const unsigned char *	bytes;

	bytes= utilMemoryBufferGetBytes( &size, &mbtext );
	if  ( size > 0 )
	    {
	    const int	addSemicolon= 1;
	    char	tag[50];

	    strcpy( tag, "leveltext" );

	    if  ( ll->llTemplateID < -5 || ll->llTemplateID > -1 )
		{
		sprintf( tag, "leveltext\\leveltemplateid%ld",
						    ll->llTemplateID );
		}

	    docRtfWriteDocEncodedStringDestination( rwc, tag,
					(char *)bytes, size, addSemicolon );
	    }

	bytes= utilMemoryBufferGetBytes( &size, &mbnumbers );
	if  ( size > 0 )
	    {
	    docRtfWriteRawBytesDestination( rwc, "levelnumbers",
							(char *)bytes, size );
	    }
	}

    if  ( ll->llPictureNumber >= 0 )
	{ docRtfWriteArgTag( rwc, "levelpicture", ll->llPictureNumber ); }

    docRtfSaveTextAttribute( rwc, &(ll->llTextAttributeMask),
						    &(ll->llTextAttribute) );

    if  ( ! utilPropMaskIsEmpty( &(ll->llParaPropertyMask) ) )
	{
	ParagraphProperties	pp;

	docInitParagraphProperties( &pp );

	/* HACK */
	pp.ppTabStopListNumber= ll->llTabStopListNumber;
	pp.ppLeftIndentTwips= ll->llLeftIndentTwips;
	pp.ppFirstIndentTwips= ll->llFirstIndentTwips;
	pp.ppTabStopListNumber= ll->llTabStopListNumber;

	docRtfSaveParagraphProperties( rwc, &(ll->llParaPropertyMask), &pp );
	}

    docRtfWriteDestinationEnd( rwc );

    utilCleanMemoryBuffer( &mbtext );
    utilCleanMemoryBuffer( &mbnumbers );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a list to rtf.						*/
/*									*/
/************************************************************************/

int docRtfWriteListProps(	RtfWriter *		rw,
				const DocumentList *	dl )
    {
    const ListLevel *	ll;
    int			lev;

    if  ( dl->dlListTemplateID != -1 )
	{ docRtfWriteArgTag( rw, "listtemplateid", dl->dlListTemplateID ); }

    docRtfWriteAltTag( rw, "listhybrid", "listsimple", dl->dlListHybrid );

    if  ( dl->dlLevelCount > 0 )
	{ docRtfWriteNextLine( rw );	}

    ll= dl->dlLevels;
    for ( lev= 0; lev < dl->dlLevelCount; ll++, lev++ )
	{
	docRtfWriteListLevel( rw, ll );
	docRtfWriteNextLine( rw );
	}

    if  ( dl->dlListID != -1 )
	{ docRtfWriteArgTag( rw, "listid", dl->dlListID );	}

    {
    const int		addSemicolon= 1;

    docRtfWriteDocEncodedStringDestination( rw, "listname",
			    (const char *)dl->dlListName.mbBytes,
			    dl->dlListName.mbSize, addSemicolon );
    }

    if  ( dl->dlListStyleID != -1 )
	{ docRtfWriteArgTag( rw, "liststyleid", dl->dlListStyleID ); }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a listtable to rtf.						*/
/*									*/
/************************************************************************/

void docRtfWriteListTable(	RtfWriter *			rw,
				const DocumentListTable *	dlt )
    {
    int				i;
    const DocumentList *	dl= dlt->dltLists;

    docRtfWriteDestinationBegin( rw, "*\\listtable" );
    docRtfWriteNextLine( rw );

    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	{
	docRtfWriteDestinationBegin( rw, "list" );

	docRtfWriteListProps( rw, dl );

	docRtfWriteDestinationEnd( rw );
	if  ( i+ 1 < dlt->dltListCount )
	    { docRtfWriteNextLine( rw );	}
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return;
    }

