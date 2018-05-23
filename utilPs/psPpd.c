#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psPpd.h"
#   include	<sioFileio.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read PPD files.							*/
/*									*/
/************************************************************************/

#   define	AFMlenLINE	256

typedef struct PpdKeyword
    {
    const char *	pkString;

    int			(*pkConsumer)(	SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd );
    } PpdKeyword;

static int psPpdConsumeStatements(
				SimpleInputStream *	sisPpd,
				char *			input,
				Ppd *			ppd,
				PpdKeyword *		pk,
				int			keywordCount );

static int psPpdIgnore(		SimpleInputStream *, int, char *,Ppd * );
static int psPpdAdobe(		SimpleInputStream *, int, char *,Ppd * );
static int psPpdModelName(	SimpleInputStream *, int, char *,Ppd * );
static int psPpdShortNickName(	SimpleInputStream *, int, char *,Ppd * );
static int psPpdNickName(	SimpleInputStream *, int, char *,Ppd * );
static int psPpdProduct(	SimpleInputStream *, int, char *,Ppd * );
static int psPpdManufacturer(	SimpleInputStream *, int, char *,Ppd * );
static int psPpdPSVersion(	SimpleInputStream *, int, char *,Ppd * );

static PpdKeyword	psPpdFileKeywords[]=
    {
	{ "PPD-Adobe:",			psPpdAdobe,		},
	{ "ModelName:",			psPpdModelName,		},
	{ "ShortNickName:",		psPpdShortNickName,	},
	{ "NickName:",			psPpdNickName,		},
	{ "Product:",			psPpdProduct,		},
	{ "Manufacturer:",		psPpdManufacturer,	},
	{ "PSVersion:",			psPpdPSVersion,	},

	{ "ColorDevice:",		psPpdIgnore,		},
	{ "DefaultColorSpace:",		psPpdIgnore,		},
	{ "FileSystem:",		psPpdIgnore,		},
	{ "LanguageLevel:",		psPpdIgnore,		},
	{ "Throughput:",		psPpdIgnore,		},
	{ "TTRasterizer:",		psPpdIgnore,		},

	{ "FileVersion:",		psPpdIgnore,		},
	{ "FormatVersion:",		psPpdIgnore,		},
	{ "LanguageEncoding:",		psPpdIgnore,		},
	{ "LanguageVersion:",		psPpdIgnore,		},
	{ "PCFileName:",		psPpdIgnore,		},
	{ "APDialogExtension:",		psPpdIgnore,		},
	{ "APPrinterIconPath:",		psPpdIgnore,		},
    };

/************************************************************************/

int psReadPpdFile(	Ppd *		ppd,
			const char *	filename )
    {
    int				rval= 0;
    SimpleInputStream *		sisPpd= (SimpleInputStream *)0;

    char			input[AFMlenLINE+1];

    sisPpd= sioInFileioOpenS( filename );
    if  ( ! sisPpd )
	{ SXDEB(filename,sisPpd); rval= -1; goto ready;	}

    rval= psPpdConsumeStatements( sisPpd, input, ppd, psPpdFileKeywords,
				sizeof(psPpdFileKeywords)/sizeof(PpdKeyword) );

    if  ( rval < 0 )
	{ LDEB(rval);	}

  ready:
    if  ( sisPpd )
	{ sioInClose( sisPpd );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get a line of input. (Essentially fgets())				*/
/*									*/
/*  3)  Remove a second carriage return. Some files really have this.	*/
/*									*/
/************************************************************************/

static char *	psGetInputLine(	char *				s,
				int *				pLen,
				int				size,
				SimpleInputStream *		sisPpd )
    {
    s= sioInGetString( s, size, sisPpd );

    if  ( s )
	{
	int		lineLength= strlen( s );

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\n'	)
	    { s[--lineLength]= '\0'; 	}

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	/*  3  */
	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	while( lineLength > 0 && isspace( s[lineLength-1] ) )
	    { s[--lineLength]= '\0';	}

	*pLen= lineLength;
	}

    return s;
    }

static int psPpdCollectValue(	char **			pValue,
				SimpleInputStream *	sisPpd,
				int			valPos,
				char *			input )
    {
    char *		s= input+ valPos;
    char *		value;

    while( isspace( *s ) )
	{ s++;	}

    if  ( ! *s )
	{
	value= (char *)realloc( *pValue, 1 );
	if  ( ! value )
	    { XDEB(value); return -1;	}
	value[0]= '\0';

	*pValue= value; return 0;
	}

    if  ( *s != '"' )
	{
	value= (char *)realloc( *pValue, strlen( input )+ 1 );
	if  ( ! value )
	    { XDEB(value); return -1; }
	*pValue= value; return 0;
	}

LDEB(1);
    return -1;
    }

/************************************************************************/
/*									*/
/*  Consume a series of lines.						*/
/*									*/
/************************************************************************/

static int psPpdConsumeStatements(
				SimpleInputStream *	sisPpd,
				char *			input,
				Ppd *			ppd,
				PpdKeyword *		keywords,
				int			keywordCount )
    {
    int			rval= 0;
    char *		value= (char *)0;

    for (;;)
	{
	int		k;
	int		p;
	PpdKeyword *	pk;

	if  ( ! psGetInputLine( input, &p, AFMlenLINE, sisPpd ) )
	    { rval= 1; goto ready;	}

	if  ( ! input[0] )
	    { continue;	}
	if  ( input[0] == '*' && input[1] == '%' )
	    { continue;	}

	if  ( input[0] != '*' )
	    { SDEB(input); rval= -1; goto ready;	}

	p= 1;
	while( input[p] && ! isspace( input[p] ) )
	    { p++;	}
	if  ( input[p] )
	    { input[p++]= '\0';	}

	while( isspace( input[p] ) )
	    { p++;	}

	pk= keywords;
	for ( k= 0; k < keywordCount; pk++, k++ )
	    {
	    if  ( ! strcmp( input+ 1, pk->pkString ) )
		{ break;	}
	    }

	if  ( k >= keywordCount )
	    { SDEB(input); continue;	}
	if  ( ! pk->pkConsumer )
	    { break;	}

	if  ( pk->pkString[strlen(pk->pkString)- 1] == ':' )
	    {
	    if  ( psPpdCollectValue( &value, sisPpd, p, input ) )
		{ SDEB(pk->pkString); rval= -1; goto ready;	}
	    }

	if  ( (*pk->pkConsumer)( sisPpd, p, input, ppd ) )
	    { SDEB(input); rval= -1; goto ready;	}

	if  ( ! ppd->ppdAdobe )
	    { XDEB(ppd->ppdAdobe); rval= -1; goto ready;	}
	}

  ready:
    if  ( value )
	{ free( value );	}

    return rval;
    }

/************************************************************************/

static int psPpdSaveString(	char **			pTarget,
				SimpleInputStream *	sisPpd,
				int			valPos,
				char *			input )
    {
    *pTarget= strdup( input );

    if  ( ! *pTarget )
	{ XDEB(*pTarget); return -1;	}

    return 0;
    }

static int	psPpdAdobe(		SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    { return psPpdSaveString( &(ppd->ppdAdobe), sisPpd, valPos, input ); }

static int	psPpdModelName(		SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    { return psPpdSaveString( &(ppd->ppdModelName), sisPpd, valPos, input ); }

static int	psPpdShortNickName(	SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    {
    return psPpdSaveString( &(ppd->ppdShortNickName), sisPpd, valPos, input );
    }

static int	psPpdNickName(	SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    { return psPpdSaveString( &(ppd->ppdNickName), sisPpd, valPos, input ); }

static int	psPpdProduct(	SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    { return psPpdSaveString( &(ppd->ppdProduct), sisPpd, valPos, input ); }

static int	psPpdManufacturer(	SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    { return psPpdSaveString( &(ppd->ppdManufacturer), sisPpd, valPos, input ); }

static int	psPpdPSVersion(		SimpleInputStream *	sisPpd,
					int			valPos,
					char *			input,
					Ppd *			ppd )
    { return psPpdSaveString( &(ppd->ppdPSVersion), sisPpd, valPos, input ); }

/************************************************************************/
/*									*/
/*  Ignore an input line.						*/
/*									*/
/************************************************************************/

static int	psPpdIgnore(	SimpleInputStream *		sisPpd,
				int		valPos,
				char *		input,
				Ppd *		ppd )
    { return 0;	}

/************************************************************************/

void psInitPpd(		Ppd *		ppd )
    {
    ppd->ppdAdobe= (char *)0;

    ppd->ppdModelName= (char *)0;
    ppd->ppdShortNickName= (char *)0;
    ppd->ppdNickName= (char *)0;
    ppd->ppdProduct= (char *)0;
    ppd->ppdManufacturer= (char *)0;
    ppd->ppdPSVersion= (char *)0;

    ppd->ppdColorDevice= 0;
    ppd->ppdDefaultColorSpace= 0;
    ppd->ppdFileSystem= 0;
    }

void psCleanPpd(		Ppd *		ppd )
    {
    if  ( ppd->ppdAdobe )
	{ free( ppd->ppdAdobe ); }

    if  ( ppd->ppdModelName )
	{ free( ppd->ppdModelName ); }
    if  ( ppd->ppdShortNickName )
	{ free( ppd->ppdShortNickName ); }
    if  ( ppd->ppdNickName )
	{ free( ppd->ppdNickName ); }
    if  ( ppd->ppdProduct )
	{ free( ppd->ppdProduct ); }
    if  ( ppd->ppdManufacturer )
	{ free( ppd->ppdManufacturer ); }
    if  ( ppd->ppdPSVersion )
	{ free( ppd->ppdPSVersion ); }

    return;
    }
