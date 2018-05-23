/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	<docSeqField.h>
#   include	<utilTree.h>
#   include	"docRecalculateFields.h"

/************************************************************************/
/*									*/
/*  Return the number of a seq field to be inserted in the document.	*/
/*									*/
/************************************************************************/

int docCalculateSeqFieldString( int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;
    SeqField		sf;

    docInitSeqField( &sf );

    if  ( docGetSeqField( &sf, df ) )
	{ *pCalculated= 0; goto ready;	}

    utilEmptyMemoryBuffer( mbResult );

    /*  ABUSE dfNoteIndex! */
    if  ( docFieldFormatInteger( mbResult, sf.sfNumberFormat,
						    df->dfNoteIndex ) )
	{ LDEB(1); *pCalculated= 0;	}
    else{ *pCalculated= 1;		}

  ready:
    docCleanSeqField( &sf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Calculate the number of a seq field.				*/
/*									*/
/************************************************************************/

int docRenumberSeqField( 	int *				pChanged,
				DocumentField *			df,
				BufferDocument *		bd )
    {
    int			rval= 0;
    int			changed= 0;
    SeqField		sf;
    int *		pVal= (int *)0;

    docInitSeqField( &sf );

    if  ( docGetSeqField( &sf, df ) )
	{ LDEB(1); goto ready;	}

    if  ( ! bd->bdSeqFieldIdentifiers )
	{
	const int	ownKeys= 1;

	bd->bdSeqFieldIdentifiers= utilTreeMakeTree( ownKeys );
	if  ( ! bd->bdSeqFieldIdentifiers )
	    { XDEB(bd->bdSeqFieldIdentifiers); rval= -1; goto ready; }
	}

    pVal= (int *)utilTreeGetEQ( bd->bdSeqFieldIdentifiers, (const char **)0,
				    utilMemoryBufferGetString( &(sf.sfIdentifier) ) );
    if  ( ! pVal)
	{
	pVal= (int *)malloc( sizeof(int) );
	if  ( ! pVal)
	    { XDEB(pVal); rval= -1; goto ready;	}
	else{
	    *pVal= 0; changed= 1;
	    if  ( utilTreeStoreValue( bd->bdSeqFieldIdentifiers,
			    (void **)0, (const char **)0,
			    utilMemoryBufferGetString( &(sf.sfIdentifier) ), pVal ) )
		{ LDEB(1); free( pVal ); rval= -1; goto ready; }
	    }
	} 

    if  ( sf.sfResetTo >= 0 )
	{ *pVal= sf.sfResetTo;	}
    else{
	if  ( sf.sfIncrement || *pVal == 0 )
	    { (*pVal)++;	}
	}

    if  ( df->dfNoteIndex != *pVal )
	{ df->dfNoteIndex= *pVal; changed= 1;	}

    *pChanged= changed;

  ready:
    docCleanSeqField( &sf );

    return rval;
    }

