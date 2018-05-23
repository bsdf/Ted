/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<string.h>
#   include	<ctype.h>
#   include	<stdlib.h>
#   include	<uniUtf8.h>
#   include	<ucdGeneralCategory.h>

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docBookmarkField.h"

/************************************************************************/
/*									*/
/*  Extract the name from a bookmark field.				*/
/*									*/
/*  Return the bookmark name.						*/
/*									*/
/************************************************************************/

int docFieldGetBookmark(	const MemoryBuffer **		pMarkName,
				const DocumentField *		df )
    {
    if  ( df->dfKind != DOCfkBOOKMARK			||
	  df->dfInstructions.fiComponentCount < 2	)
	{ return -1;	}

    *pMarkName= &(df->dfInstructions.fiComponents[1].icBuffer);

    if  ( df->dfInstructions.fiComponentCount > 2 )
	{ LDEB(df->dfInstructions.fiComponentCount);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is this field a Toc bookmark?					*/
/*									*/
/************************************************************************/

int docIsTocBookmark(	long *				pId,
			const DocumentField *		df )
    {
    char *			past;
    long			id;

    const char *		markName;
    int				markSize;

    if  ( df->dfKind != DOCfkBOOKMARK			||
	  df->dfInstructions.fiComponentCount < 2	)
	{ return -1;	}

    markName= (const char *)df->dfInstructions.fiComponents[1].icBuffer.mbBytes;
    markSize= df->dfInstructions.fiComponents[1].icBuffer.mbSize;

    if  ( strncmp( markName, "_Toc", 4 ) )
	{ return 0;	}

    id= strtod( markName+ 4, &past );
    if  ( past == markName+ 4 )
	{ return 0;	}
    if  ( past- markName < markSize )
	{ return 0;	}

    if  ( pId )
	{ *pId= id;	}

    return 1;
    }

int docFieldMatchesBookmark(	const DocumentField *		df,
				const MemoryBuffer *		markName )
    {
    int				rval= 0;
    const MemoryBuffer *	foundName;

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	if  ( ! docFieldGetBookmark( &foundName, df )		&&
	      ! utilMemoryCompareBuffers( foundName, markName )	)
	    { rval= 1;	}
	}

    return rval;
    }

int docSetBookmarkField(	FieldInstructions *	fi,
				const MemoryBuffer *	mb )
    {
    if  ( docStartFieldInstructions( fi, "BOOKMARK", 8 ) )
	{ LDEB(8); return -1;	}

    if  ( docFieldInstructionsAddComponent( fi, mb ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

# define BKMK( sym ) ( ucdIsL( (sym) ) || ucdIsNd( (sym) ) || (sym) == '_' )

static int docBookmarkNormalizeBytes(	int *			pChanged,
					char *			to,
					const char *		from,
					int			left )
    {
    int			done= 0;
    int			past= 0;

    unsigned short	symbol;
    int			step;

    *pChanged= 0;

    while( left > 0 )
	{
	step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	if  ( BKMK( symbol ) )
	    { break;	}

	from += step; left -= step;
	*pChanged= 1;
	}

    while( left > 0 )
	{
	while( left > 0 )
	    {
	    step= uniGetUtf8( &symbol, from );
	    if  ( step < 1 )
		{ LDEB(step); return -1;	}

	    if  ( ! BKMK( symbol ) )
		{ break;	}
	    if  ( done+ step > DOCmaxBOOKMARK )
		{ goto ready;	}

	    memcpy( to, from, step );

	    to += step; done += step;
	    past= done;
	    from += step; left -= step;
	    }

	if  ( left <= 0 || done+ 1 >= DOCmaxBOOKMARK )
	    { goto ready;	}

	*(to++)= '_'; done++;
	from += step; left -= step;
	*pChanged= 1;

	while( left > 0 )
	    {
	    step= uniGetUtf8( &symbol, from );
	    if  ( step < 1 )
		{ LDEB(step); return -1;	}

	    if  ( BKMK( symbol ) )
		{ break;	}

	    from += step; left -= step;
	    *pChanged= 1;
	    }
	}

  ready:

    if  ( left > 0 )
	{ *pChanged= 1;	}

    return past;
    }

/************************************************************************/
/*									*/
/*  Bring a bookmark in a generally accepted format.			*/
/*  I.E: Replace everything that is not isalnum() and truncate to	*/
/*  DOCmaxBOOKMARK.							*/
/*									*/
/************************************************************************/

int docAdaptBookmarkName(	MemoryBuffer *		markName )
    {
    char *		buf= (char *)markName->mbBytes;
    int			changed= 0;
    int			done;

    done= docBookmarkNormalizeBytes( &changed, buf, buf, markName->mbSize );

    utilMemoryBufferSetSize( markName, done );
    return changed;
    }

/************************************************************************/
/*									*/
/*  Derive a nice bookmark from the document text.			*/
/*									*/
/************************************************************************/

int docBookmarkFromText(	MemoryBuffer *		markName,
				const char *		text,
				int			len )
    {
    char	scratch[DOCmaxBOOKMARK+ 1];
    int		done;
    int		changed= 0;

    done= docBookmarkNormalizeBytes( &changed, scratch, text, len );

    if  ( done < 4 )
	{ memcpy( scratch, "bkmk", 5 ); done= 4;	}

    if  ( utilMemoryBufferSetBytes( markName, (unsigned char *)scratch, done ) )
	{ LDEB(done); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return a suitable index for inserting a suffix to make the bookmark	*/
/*  unique.								*/
/*									*/
/*  We want to use 'wanted' bytes for the suffix.			*/
/*									*/
/*  1)  If the suffix fits after the bookmark, return the end of the	*/
/*	bookmark.							*/
/*  2)  Otherwise, look for the last position that leaves space.	*/
/*									*/
/************************************************************************/

int docBookmarkSuffixIndex(		const MemoryBuffer *	markName,
					int			wanted )
    {
    const char *	buf= (char *)markName->mbBytes;
    int			offset= 0;

    /*  1  */
    if  ( markName->mbSize+ wanted < DOCmaxBOOKMARK )
	{ return markName->mbSize;	}

    while( offset < markName->mbSize )
	{
	unsigned short	symbol;
	int		step= uniGetUtf8( &symbol, buf );

	if  ( step < 1 )
	    { LDEB(step); return offset;	}
	if  ( offset+ step+ wanted >= DOCmaxBOOKMARK )
	    { return offset;	}

	offset += step; buf += step;
	}

    LLLDEB(markName->mbBytes,wanted,DOCmaxBOOKMARK);
    return -1;
    }
