/************************************************************************/
/*									*/
/*  Evaluate fields to document properties.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docDateField.h>
#   include	<docFileNameField.h>

/************************************************************************/
/*									*/
/*  Return the value of one of the document dates.			*/
/*									*/
/************************************************************************/

int docCalculateDocDateFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int				rval= 0;

    DateField			dtf;

    const DocumentProperties *	dp= &(rf->rfDocument->bdProperties);
    const struct tm *		tm;

    const char *		defaultFormat= (const char *)0;

    time_t			t;
    struct tm			tmNow;

    docInitDateField( &dtf );

    if  ( docGetDateField( &dtf, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( df->dfKind )
	{
	case DOCfkCREATEDATE:
	    tm= &(dp->dpCreatim);
	    defaultFormat= "%c";
	    break;
	case DOCfkSAVEDATE:
	    tm= &(dp->dpRevtim);
	    defaultFormat= "%c";
	    break;
	case DOCfkPRINTDATE:
	    tm= &(dp->dpPrintim);
	    defaultFormat= "%c";
	    break;

	case DOCfkDATE:
	    t= time( (time_t *)0 );
	    tmNow= *localtime( &t );
	    tm= &tmNow;
	    defaultFormat= "%x";
	    break;
	case DOCfkTIME:
	    t= time( (time_t *)0 );
	    tmNow= *localtime( &t );
	    tm= &tmNow;
	    defaultFormat= "%X";
	    break;

	default:
	    LDEB(df->dfKind); *pCalculated= 0; goto ready;
	}

    if  ( tm->tm_mday == 0 )
	{ /*LDEB(tm->tm_mday);*/ *pCalculated= 0; goto ready;	}

    if  ( ! utilMemoryBufferIsEmpty( &(dtf.dtfPicture) ) )
	{
	char		target[50+1];
	char		scratch[50+1];

	if  ( dtf.dtfPicture.mbSize > sizeof(scratch)- 1 )
	    {
	    LLDEB(dtf.dtfPicture.mbSize,sizeof(scratch));
	    *pCalculated= 0; goto ready;
	    }

	memcpy( scratch, dtf.dtfPicture.mbBytes, dtf.dtfPicture.mbSize );
	scratch[dtf.dtfPicture.mbSize]= '\0';

	appWordFormatDate( target, sizeof(target)-1, tm, scratch );
	utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)target, strlen( target ) );
	*pCalculated= 1; goto ready;
	}
    else{
	char		target[50+1];

	if  ( strftime( (char *)target, sizeof(target)-1,
						    defaultFormat, tm ) < 1 )
	    { *pCalculated= 0; goto ready;	}

	utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)target, strlen( target ) );
	*pCalculated= 1; goto ready;
	}


    *pCalculated= 0;

ready:
    
    docCleanDateField( &dtf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Return the value of one of the document properties.			*/
/*									*/
/************************************************************************/

int docCalculateDocStringFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    const DocumentProperties *	dp= &(rf->rfDocument->bdProperties);
    const MemoryBuffer *	val;
    
    switch( df->dfKind )
	{
	case DOCfkAUTHOR:	val= &(dp->dpAuthor);	break;
	case DOCfkCOMMENTS:	val= &(dp->dpDoccomm);	break;
	case DOCfkDOCCOMM:	val= &(dp->dpDoccomm);	break;
	case DOCfkKEYWORDS:	val= &(dp->dpKeywords);	break;
	case DOCfkSUBJECT:	val= &(dp->dpSubject);	break;
	case DOCfkTITLE:	val= &(dp->dpTitle);	break;
	case DOCfkFILENAME:	val= &(dp->dpFilename);	break;

	default:
	    LDEB(df->dfKind); *pCalculated= 0; return 0;
	}

    if  ( utilMemoryAppendBuffer( mbResult, val ) )
	{ LDEB(1); return -1;	}

    *pCalculated= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Return the file name of the document.				*/
/*									*/
/*  The file name on the document proprties is assumed to be a full	*/
/*  absolute name. (From "/")						*/
/*									*/
/************************************************************************/

int docCalculateFilenameFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int				rval= 0;
    FileNameField		fnf;

    const DocumentProperties *	dp= &(rf->rfDocument->bdProperties);

    char *			relative= (char *)0;

    docInitFileNameField( &fnf );

    if  ( docGetFileNameField( &fnf, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! fnf.fnfIncludePath					&&
	  ! utilMemoryBufferIsEmpty( &(dp->dpFilename) )	)
	{ relative= strrchr( (char *)dp->dpFilename.mbBytes, '/' );	}

    if  ( fnf.fnfIncludePath || ! relative )
	{
	if  ( utilMemoryAppendBuffer( mbResult, &(dp->dpFilename) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	relative++;

	if  ( utilMemoryBufferAppendBytes( mbResult,
			    (unsigned char *)relative, strlen( relative ) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    *pCalculated= 1;

  ready:

    docCleanFileNameField( &fnf );

    return rval;
    }

