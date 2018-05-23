/************************************************************************/
/*									*/
/*  Calculate 'Hyperlink' fields.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docHyperlinkField.h"

void docInitHyperlinkField(	HyperlinkField *	hf )
    {
    utilInitMemoryBuffer( &(hf->hfFile) );
    utilInitMemoryBuffer( &(hf->hfBookmark) );
    utilInitMemoryBuffer( &(hf->hfFont) );
    }

void docCleanHyperlinkField(	HyperlinkField *	hf )
    {
    utilCleanMemoryBuffer( &(hf->hfFile) );
    utilCleanMemoryBuffer( &(hf->hfBookmark) );
    utilCleanMemoryBuffer( &(hf->hfFont) );
    }

/************************************************************************/
/*									*/
/*  Make a hyperlink to the same document relative: Remove the absolute	*/
/*  document name if it is equal to the name of the document that the	*/
/*  link appears in.							*/
/*									*/
/************************************************************************/

int docMakeHyperlinkRelative(	DocumentField *		dfTo,
				const MemoryBuffer *	refFileName )
    {
    int			rval= 0;
    HyperlinkField	hf;

    docInitHyperlinkField( &hf );

    if  ( docGetHyperlinkField( &hf, dfTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! utilMemoryBufferIsEmpty( &(hf.hfBookmark) )		||
	  ! utilEqualMemoryBuffer( &(hf.hfFile), refFileName )	)
	{ goto ready;	}

    utilEmptyMemoryBuffer( &(hf.hfFile) );

    if  ( docSetHyperlinkField( &(dfTo->dfInstructions), &hf ) )
	{ rval= -1; goto ready;	}

  ready:

    docCleanHyperlinkField( &hf );

    return rval;
    }

/************************************************************************/

int docSetHyperlinkField(	FieldInstructions *	fi,
				const HyperlinkField *	hf )
    {
    int			rval= 0;

    if  ( docStartFieldInstructions( fi, "HYPERLINK", 9 ) )
	{ LDEB(9); return -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &(hf->hfFile) ) )
	{
	if  ( docFieldInstructionsAddComponent( fi, &(hf->hfFile) ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 'l', &(hf->hfBookmark) ) )
	{ CDEB('l'); return -1;	}

    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 'o', &(hf->hfFont) ) )
	{ CDEB('o'); return -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract the destination from a hyperlink field.			*/
/*									*/
/*  Return the file name and the bookmark.				*/
/*  The strings are NOT '\0' terminated.				*/
/*									*/
/************************************************************************/

int docGetHyperlinkField(	HyperlinkField *	hf,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkHYPERLINK )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( comp == 1 && ! ic->icIsFlag )
	    {
	    if  ( utilCopyMemoryBuffer( &(hf->hfFile), &(ic->icBuffer) ) )
		{ CDEB('o'); return -1;	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'o' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(hf->hfFont), &(ic->icBuffer) ) )
		{ CDEB('o'); return -1;	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'l' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(hf->hfBookmark), &(ic->icBuffer) ) )
		{ CDEB('l'); return -1;	}

	    continue;
	    }

	LLSDEB(comp,ic->icIsFlag,utilMemoryBufferGetString(&(ic->icBuffer)));
	}

    return 0;
    }

int docHyperlinkFieldIsLink(	const HyperlinkField *		hf )
    {
    return ! utilMemoryBufferIsEmpty( &(hf->hfFile) ) ||
	   ! utilMemoryBufferIsEmpty( &(hf->hfBookmark) );
    }

int docHyperlinkFieldIsExtern(	const HyperlinkField *		hf )
    {
    return ! utilMemoryBufferIsEmpty( &(hf->hfFile) );
    }

int docEqualHyperlinkFields(	const HyperlinkField *		hf1,
				const HyperlinkField *		hf2 )
    {
    if  ( ! utilEqualMemoryBuffer( &(hf1->hfFile), &(hf2->hfFile) ) )
	{ return 0;	}
    if  ( ! utilEqualMemoryBuffer( &(hf1->hfBookmark), &(hf2->hfBookmark) ) )
	{ return 0;	}
    if  ( ! utilEqualMemoryBuffer( &(hf1->hfFont), &(hf2->hfFont) ) )
	{ return 0;	}

    return 1;
    }

int docCopyHyperlinkField(	HyperlinkField *		to,
				const HyperlinkField *		from )
    {
    if  ( utilCopyMemoryBuffer( &(to->hfFile), &(from->hfFile) ) )
	{ return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->hfBookmark), &(from->hfBookmark) ) )
	{ return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->hfFont), &(from->hfFont) ) )
	{ return -1;	}

    return 0;
    }
