/************************************************************************/
/*									*/
/*  Calculate 'Symbol' fields.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	<docSymbolField.h>
#   include	<uniLegacyEncoding.h>
#   include	<uniUtf8.h>

#   include	"docBuf.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Format the value of a 'symbol' field.				*/
/*									*/
/************************************************************************/

int docCalculateSymbolFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;

    int			step;

    char		scratch[20];
    const int *		unicodes= uniWinCp1252GlyphUnicodes;

    SymbolField		sf;

    docInitSymbolField( &sf );

    if  ( docFieldGetSymbol( &sf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &(sf.sfFontName), "Symbol" ) )
	{ unicodes= uniSymbolGlyphUnicodes;	}

    if  ( utilMemoryBufferEqualsString( &(sf.sfFontName), "ITC Zapf Dingbats" )||
	  utilMemoryBufferEqualsString( &(sf.sfFontName), "ZapfDingbats" )||
	  utilMemoryBufferEqualsString( &(sf.sfFontName), "Dingbats" ) )
	{ unicodes= uniDingbatsGlyphUnicodes;	}

    switch( sf.sfEncoding )
	{
	case SYMBOLencANSI:
	    if  ( sf.sfSymbol >= 0		&&
		  sf.sfSymbol < 256		&&
		  unicodes[sf.sfSymbol] >= 0	)
		{ sf.sfSymbol= unicodes[sf.sfSymbol];	}
	    else{ LDEB(sf.sfSymbol); *pCalculated= 0; goto ready;	}
	    break;
	case SYMBOLencUNICODE:
	    break;

	case SYMBOLencSHIFT_JIS:
	default:
	    LDEB(sf.sfEncoding); *pCalculated= 0; goto ready;
	}

    step= uniPutUtf8( scratch, sf.sfSymbol );
    if  ( step < 1 )
	{ LLDEB(sf.sfSymbol,step); *pCalculated= 0; goto ready;	}
    utilMemoryBufferAppendBytes( mbResult, (unsigned char *)scratch, step );
    *pCalculated= 1;

  ready:
    docCleanSymbolField( &sf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docRecalculateParaSymbolTextParticules(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				struct BufferItem *		paraBi,
				int				part,
				int				partCount,
				DocumentField *			df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;

    char *		allocated= (char *)0;
    SymbolField		sf;

    docInitSymbolField( &sf );

    if  ( docFieldGetSymbol( &sf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    rval= docRecalculateParaStringTextParticules( pCalculated,
		    pPartShift, pStroffShift, paraBi, part, partCount, df, rf );

    if  ( rval )
	{ LDEB(rval); goto ready;	}

    if  ( *pCalculated )
	{
	TextAttribute		taSet;
	PropertyMask		taSetMask;

	utilPropMaskClear( &taSetMask );
	utilInitTextAttribute( &taSet );

	if  ( ! utilMemoryBufferIsEmpty( &(sf.sfFontName) ) )
	    {
	    int				fontNumber;
	    DocumentProperties *	dp= &(rf->rfDocument->bdProperties);

	    allocated= utilMemoryStrdup( &(sf.sfFontName) );
	    if  ( ! allocated )
		{ XDEB(allocated); rval= -1; goto ready;	}

	    fontNumber= docGetFontByName( dp->dpFontList, allocated );
	    if  ( fontNumber < 0 )
		{ SLDEB(allocated,fontNumber);	}
	    else{
		taSet.taFontNumber= fontNumber;
		PROPmaskADD( &taSetMask, TApropFONT_NUMBER );
		}
	    }

	if  ( sf.sfSizePoints > 0 )
	    {
	    taSet.taFontSizeHalfPoints= 2* sf.sfSizePoints;
	    PROPmaskADD( &taSetMask, TApropFONTSIZE );
	    }

	if  ( ! utilPropMaskIsEmpty( &taSetMask ) )
	    {
	    if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
				    rf->rfDocument, paraBi,
				    part+ 1, part+ 1+ partCount+ *pPartShift,
				    &taSet, &taSetMask ) )
		{ LDEB(part); rval= -1; goto ready;	}
	    }
	}

  ready: 

    docCleanSymbolField( &sf );

    return rval;
    }
