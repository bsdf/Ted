/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>


#   include	<appDebugon.h>

#   include	"docDocumentField.h"

/************************************************************************/
/*									*/
/*  Extract a numeric rtf tag from the field data.			*/
/*									*/
/************************************************************************/

static void docLookupGetTag(	int *			pVal,
				const DocumentField *	df,
				const char *		tag )
    {
    int			taglen= strlen( tag );
    int			pos;

    const char *	bytes= utilMemoryBufferGetString( &(df->dfData) );
    int			size= df->dfData.mbSize;

    if  ( tag[0] != '\\' )
	{ SDEB(tag); return;	}

    pos= 0;
    while( pos < size && bytes[pos] != '\\' )
	{ pos++;	}

    while( pos < size )
	{
	int		past= pos+ 1;
	int		end= past;

	while( past < size && isalnum( bytes[past] ) )
	    {
	    if  ( isalpha( bytes[past] ) )
		{ end= past+ 1;	}

	    past++;
	    }

	if  ( end- pos == taglen			&&
	      ! strncmp( bytes+ pos, tag, taglen )	)
	    {
	    if  ( end < past )
		{
		char *	p; 
		*pVal= strtol( bytes+ end, &p, 10 );
		}

	    return;
	    }

	pos= past;
	while( pos < size && bytes[pos] != '\\' )
	    { pos++;	}
	}

    return;
    }

int docFieldGetTc(		const DocumentField *	df,
				char *			pFlag,
				int *			pLevel,
				int *			pNumbered )
    {
    int		level= -1;
    int		flag= -1;

    if  ( df->dfKind != DOCfkTC && df->dfKind != DOCfkTCN )
	{ return -1;	}

    docLookupGetTag( &level, df, "\\tcl" ); level--;
    docLookupGetTag( &flag, df, "\\tcf" );

    if  ( level < 0 )
	{ level= 0;	}
    if  ( flag < 0 )
	{ flag= 'C';	}

    *pLevel= level;
    *pFlag= flag;
    *pNumbered= df->dfKind == DOCfkTC;
    return 0;
    }

