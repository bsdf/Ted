/************************************************************************/
/*									*/
/*  Save notes.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	<docBuf.h>
#   include	<docDocumentNote.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

/************************************************************************/

typedef struct SaveNotes
    {
    int				snDone;
    HtmlWritingContext *	snHtmlWritingContext;
    } SaveNotes;

static int docHtmlSaveDeferredNote(	int		from,
					int		to,
					void *		vsn )
    {
    SaveNotes *			sn= (SaveNotes *)vsn;
    HtmlWritingContext *	hwc= sn->snHtmlWritingContext;
    const BufferDocument *	bd= hwc->hwcDocument;

    const DocumentField *	df;
    struct DocumentNote *	dn;

    if  ( sn->snDone == 0 )
	{
	docHtmlPutString( "<div style=\"padding-left: 15%;\">\n", hwc );
	}

    df= docGetFieldByNumber( &(bd->bdFieldList), to );
    if  ( df->dfKind != DOCfkCHFTN )
	{ LDEB(df->dfKind); return -1;	}

    dn= docGetNoteOfField( df, bd );
    if  ( dn )
	{
	if  ( ! dn->dnDocumentTree.dtRoot )
	    { XDEB(dn->dnDocumentTree.dtRoot);	}
	else{
	    if  ( docHtmlSaveSelection( hwc, &(dn->dnDocumentTree),
					(const DocumentSelection *)0 ) )
		{ LDEB(1); return -1; }
	    }
	}

    sn->snDone++;
    return 0;
    }

int docHtmlSaveDeferredNotes(	HtmlWritingContext *		hwc )
    {
    SaveNotes			sn;

    sn.snDone= 0;
    sn.snHtmlWritingContext= hwc;

    if  ( utilIndexMappingForAll( &(hwc->hwcDeferredNotes),
				    docHtmlSaveDeferredNote, (void *)&sn ) )
	{ LDEB(1); return -1;	}

    if  ( sn.snDone > 0 )
	{ docHtmlPutString( "</div>\n", hwc );	}

    utilClearIndexMapping( &(hwc->hwcDeferredNotes) );
    return 0;
    }


int docHtmlStartNote(	const DocumentField *		df,
			HtmlWritingContext *		hwc,
			const BufferItem *		bi,
			int				attNr )
    {
    int				rval= 0;
    const MemoryBuffer *	fileName= (const MemoryBuffer *)0;
    const MemoryBuffer *	markName= (const MemoryBuffer *)0;
    const MemoryBuffer *	refName= (const MemoryBuffer *)0;

    const char *	title= (const char *)0;
    int			titleSize= 0;

    const int		isNote= 1;

    MemoryBuffer	mbRef;
    MemoryBuffer	mbDef;

    utilInitMemoryBuffer( &mbRef );
    utilInitMemoryBuffer( &mbDef );

    if  ( bi->biTreeType == DOCinBODY )
	{
	DocumentNote *	dn;

	utilMemoryBufferPrintf( &mbRef, "_NREF_%d", hwc->hwcNoteRefCount+ 1 );
	utilMemoryBufferPrintf( &mbDef, "_NDEF_%d", hwc->hwcNoteRefCount+ 1 );

	markName= &mbDef;
	refName=  &mbRef;

	dn= docGetNoteOfField( df, hwc->hwcDocument );
	if  ( dn )
	    {
	    DocumentPosition	dpLast;

	    if  ( ! docTailPosition( &dpLast, dn->dnDocumentTree.dtRoot ) )
		{
		int	paraNr= docNumberOfParagraph( dpLast.dpNode );

		if  ( paraNr == 1 )
		    {
		    title= (char *)docParaString( dpLast.dpNode, 0 );
		    titleSize= docParaStrlen( dpLast.dpNode );
		    }
		}
	    }
	}
    else{
	utilMemoryBufferPrintf( &mbRef, "_NREF_%d", hwc->hwcNoteDefCount+ 1 );
	utilMemoryBufferPrintf( &mbDef, "_NDEF_%d", hwc->hwcNoteDefCount+ 1 );

	markName= &mbRef;
	refName=  &mbDef;
	}

    docHtmlChangeAttributes( hwc, -1 );

    docHtmlStartAnchor( hwc, isNote, fileName, markName, refName,
							title, titleSize );

    docHtmlChangeAttributes( hwc, attNr );

    hwc->hwcBytesInLink= 0;

    utilCleanMemoryBuffer( &mbRef );
    utilCleanMemoryBuffer( &mbDef );

    return rval;
    }

