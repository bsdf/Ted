#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<math.h>

#   include	<sioGeneral.h>
#   include	"psFontName.h"
#   include	"psReadWriteFontInfo.h"

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  A primitive table driven parser for AFM files.			*/
/*									*/
/************************************************************************/

typedef struct AfmReader
    {
    AfmFontInfo *	arAfi;
    SimpleInputStream *	arSis;
    int			arAfmFlags;
    } AfmReader;

#   define	AFMlenLINE	256

typedef struct AfmKeyword
    {
    const char *	akString;
    int			(*akConsumer)(	AfmReader *		ar,
					int			valPos,
					char *			input );
    } AfmKeyword;

static int psAfmConsumeLines(	AfmReader *		ar,
				char *			input,
				const AfmKeyword *	ak,
				int			keywordCount );

static int psAfmComment(		AfmReader *, int, char *);
static int psAfmIgnore(			AfmReader *, int, char *);
static int psAfmStartFontMetrics(	AfmReader *, int, char *);
static int psAfmEndFontMetrics(		AfmReader *, int, char *);
static int psAfmFontName(		AfmReader *, int, char *);
static int psAfmFullName(		AfmReader *, int, char *);
static int psAfmNotice(			AfmReader *, int, char *);
static int psAfmVersion(		AfmReader *, int, char *);
static int psAfmFamilyName(		AfmReader *, int, char *);
static int psAfmWeight(			AfmReader *, int, char *);
static int psAfmFontBBox(		AfmReader *, int, char *);
static int psAfmEncodingScheme(		AfmReader *, int, char *);
static int psAfmMappingScheme(		AfmReader *, int, char *);
static int psAfmCharacterSet(		AfmReader *, int, char *);
static int psAfmCharacters(		AfmReader *, int, char *);
static int psAfmIsFixedV(		AfmReader *, int, char *);
static int psAfmCapHeight(		AfmReader *, int, char *);
static int psAfmXHeight(		AfmReader *, int, char *);
static int psAfmAscender(		AfmReader *, int, char *);
static int psAfmDescender(		AfmReader *, int, char *);
static int psAfmStartDirection(		AfmReader *, int, char *);
static int psAfmUnderlinePosition(	AfmReader *, int, char *);
static int psAfmUnderlineThickness(	AfmReader *, int, char *);
static int psAfmItalicAngle(		AfmReader *, int, char *);
static int psAfmCharWidth(		AfmReader *, int, char *);
static int psAfmIsFixedPitch(		AfmReader *, int, char *);
static int psAfmStartCharMetrics(	AfmReader *, int, char *);
static int psAfmStartKernData(		AfmReader *, int, char *);
static int psAfmStartComposites(	AfmReader *, int, char *);
static int psAfmStartTrackKern(		AfmReader *, int, char *);
static int psAfmStartKernPairs(		AfmReader *, int, char *);
static int psAfmResourceName(		AfmReader *, int, char *);
static int psAfmFontFileName(		AfmReader *, int, char *);
static int psAfmFontFileIndex(		AfmReader *, int, char *);
static int psAfmX11Font(		AfmReader *, int, char *);

# define psAfmStdHW psAfmIgnore
# define psAfmStdVW psAfmIgnore

static const AfmKeyword	psAfmFileKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "StartFontMetrics",	psAfmStartFontMetrics,	},
    };

static const AfmKeyword	psAfmMetricsKeywords[]=
    {
	{ "Comment",		psAfmComment,		},
	{ "FontName",		psAfmFontName,		},
	{ "FullName",		psAfmFullName,		},
	{ "FamilyName",		psAfmFamilyName,	},
	{ "Weight",		psAfmWeight,		},
	{ "FontBBox",		psAfmFontBBox,		},
	{ "Version",		psAfmVersion,		},
	{ "Notice",		psAfmNotice,		},
	{ "EncodingScheme",	psAfmEncodingScheme,	},
	{ "MappingScheme",	psAfmMappingScheme,	},
	{ "EscChar",		psAfmIgnore,		},
	{ "CharacterSet",	psAfmCharacterSet,	},
	{ "Characters",		psAfmCharacters,	},
	{ "IsBaseFont",		psAfmIgnore,		},
	{ "VVector",		psAfmIgnore,		},
	{ "IsFixedV",		psAfmIsFixedV,		},
	{ "CapHeight",		psAfmCapHeight,		},
	{ "XHeight",		psAfmXHeight,		},
	{ "Ascender",		psAfmAscender,		},
	{ "Descender",		psAfmDescender,		},
	{ "StartDirection",	psAfmStartDirection,	},
	{ "UnderlinePosition",	psAfmUnderlinePosition,	},
	{ "UnderlineThickness",	psAfmUnderlineThickness,},
	{ "ItalicAngle",	psAfmItalicAngle,	},
	{ "CharWidth",		psAfmCharWidth,		},
	{ "IsFixedPitch",	psAfmIsFixedPitch,	},
	{ "StartCharMetrics",	psAfmStartCharMetrics,	},
	{ "StartKernData",	psAfmStartKernData,	},
	{ "StartComposites",	psAfmStartComposites,	},
	{ "StdHW",		psAfmStdHW,		},
	{ "StdVW",		psAfmStdVW,		},

	{ "StartKernPairs",	psAfmStartKernPairs,	},
				/****************************************/
				/*  Should not appear directly in the	*/
				/*  file but embedded between		*/
				/*  "StartKernData" and "EndKernData".	*/
				/*  As some messy afm files forget the	*/
				/*  embeding of the kerning data in a	*/
				/*  separate section, the information	*/
				/*  is accepted here as well.		*/
				/****************************************/

	{ "EndFontMetrics",	psAfmEndFontMetrics,	},
    };

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
				AfmReader *			ar )
    {
    s= sioInGetString( s, size, ar->arSis );

    if  ( s )
	{
	int		lineLength;

	s[size]= '\0';
	lineLength= strlen( s );

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

/************************************************************************/

int psAfmReadAfm(		SimpleInputStream *	sisAfm,
				AfmFontInfo *		afi,
				int			afmFlags )
    {
    int			res;
    char		input[AFMlenLINE+1];

    AfmReader		ar;

    ar.arSis= sisAfm;
    ar.arAfi= afi;
    ar.arAfmFlags= afmFlags;

    res= psAfmConsumeLines( &ar, input, psAfmFileKeywords,
				sizeof(psAfmFileKeywords)/sizeof(AfmKeyword) );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    afi->afiUnitsPerEm= 1000;

    return res;
    }

/************************************************************************/
/*									*/
/*  Consume a series of lines.						*/
/*									*/
/************************************************************************/

static int psAfmConsumeLines(	AfmReader *		ar,
				char *			input,
				const AfmKeyword *	keywords,
				int			keywordCount )
    {
    for (;;)
	{
	int			k;
	int			b;
	int			p;
	const AfmKeyword *	ak;

	int			res;

	if  ( ! psGetInputLine( input, &p, AFMlenLINE, ar ) )
	    { return 1;	}

	b= 0;
	while( input[b] &&   isspace( input[b] ) )
	    { b++;	}

	if  ( ! input[b] )
	    { continue;	}

	p= b;
	while( input[p] && ! isspace( input[p] ) )
	    { p++;	}
	if  ( input[p] )
	    { input[p++]= '\0';	}

	while( isspace( input[p] ) )
	    { p++;	}

	ak= keywords;
	for ( k= 0; k < keywordCount; ak++, k++ )
	    {
	    if  ( ! strcmp( input+ b, ak->akString ) )
		{ break;	}
	    }

	if  ( k >= keywordCount )
	    { SDEB(input+ b); return -1;	}
	if  ( ! ak->akConsumer )
	    { return 0;	}

	res= (*ak->akConsumer)( ar, p, input );
	if  ( res < 0 )
	    { SDEB(input+ b); return -1;	}
	if  ( res > 0 )
	    { break;				}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Ignore an input line.						*/
/*									*/
/************************************************************************/

static int psAfmIgnore(		AfmReader *		ar,
				int			valPos,
				char *			input )
    { return 0; }

/************************************************************************/
/*									*/
/*  Consume font metrics.						*/
/*									*/
/*  1)  Not interested in the version.					*/
/*  1)  Not interested in the rest of the file: Return 1.		*/
/*									*/
/************************************************************************/

static int psAfmStartFontMetrics(	AfmReader *		ar,
					int			valPos,
					char *			input )
    {
    int		res;

    res= psAfmConsumeLines( ar, input, psAfmMetricsKeywords,
			    sizeof(psAfmMetricsKeywords)/sizeof(AfmKeyword) );
    if  ( res < 0 )
	{ LDEB(res); return res;	}

    return 1;
    }

static int psAfmEndFontMetrics(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return 1;		}

/************************************************************************/
/*									*/
/*  Extract various types of values.					*/
/*									*/
/************************************************************************/

static int psAfmSaveString(	char **		pTarget,
				char *		source )
    {
    if  ( *pTarget )
	{ free( *pTarget );	}

    *pTarget= strdup( source );
    if  ( ! *pTarget )
	{ XDEB(*pTarget); return -1;	}

    return 0;
    }

static int psAfmGetNumber(	double *	pNumber,
				char *		input,
				int		valPos )
    {
    char *	s= input+ valPos;
    char *	past;
    double	d;

    d= strtod( s, &past );
    if  ( past != s )
	{
	while( isspace( *past ) )
	    { past++;	}
	}

    if  ( past == s || *past )
	{ SSDEB(input+valPos,past); return -1;	}

    *pNumber= d;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract various strings.						*/
/*									*/
/************************************************************************/

static const AfmKeyword	psAfmSpecialComments[]=
    {
	{ "ResourceName",		psAfmResourceName,		},
	{ "FontFileName",		psAfmFontFileName,		},
	{ "FontFileIndex",		psAfmFontFileIndex,		},
	{ "X11Font",			psAfmX11Font,			},
    };

static int	psAfmComment(	AfmReader *		ar,
				int			valPos,
				char *			input )
    {
    int			kwCount= sizeof(psAfmSpecialComments)/sizeof(AfmKeyword);
    const AfmKeyword *	keywords= psAfmSpecialComments;
    int			b;

    b= valPos;
    while( input[b] &&   isspace( input[b] ) )
	{ b++;	}

    if  ( input[b] )
	{
	const AfmKeyword *	ak;
	int			p= b;
	int			k;
	int			res;

	while( input[p] && ! isspace( input[p] ) )
	    { p++;	}
	if  ( input[p] )
	    { input[p++]= '\0';	}

	while( isspace( input[p] ) )
	    { p++;	}

	ak= keywords;
	for ( k= 0; k < kwCount; ak++, k++ )
	    {
	    if  ( ! strcmp( input+ b, ak->akString ) )
		{ break;	}
	    }

	if  ( k >= kwCount )
	    { return 0;	}
	if  ( ! ak->akConsumer )
	    { return 0;	}

	res= (*ak->akConsumer)( ar, p, input );
	if  ( res < 0 )
	    { SDEB(input+ b); return -1;	}
	return res;
	}

    return 0;
    }

static int psAfmFontName(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiFontName), input+ valPos ); }

static int psAfmFontFileName(	AfmReader *		ar,
				int			valPos,
				char *			input )
    {
    return utilMemoryBufferSetString( &(ar->arAfi->afiFontFileName),
							    input+ valPos );
    }

static int psAfmX11Font(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAddX11FontToInfo( ar->arAfi, input+ valPos );	}

static int psAfmResourceName(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiResourceName), input+ valPos ); }

static int psAfmFullName(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiFullName), input+ valPos ); }

static int psAfmNotice(		AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiNotice), input+ valPos ); }

static int psAfmVersion(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiVersion), input+ valPos ); }

static int psAfmFamilyName(	AfmReader *		ar,
				int			valPos,
				char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiFamilyName), input+ valPos ); }

static int	psAfmWeight(	AfmReader *		ar,
				int			valPos,
				char *			input )
    {
    int		weight;
    int		start;
    int		length;

    if  ( utilFontWeightFromString( &weight, &start, &length, input+ valPos ) )
	{ ar->arAfi->afiWeightInt= weight;	}

    return psAfmSaveString( &(ar->arAfi->afiWeightStr), input+ valPos );
    }

static int	psAfmEncodingScheme(	AfmReader *	ar,
					int		valPos,
					char *		input )
    { return psAfmSaveString( &(ar->arAfi->afiEncodingScheme), input+ valPos ); }

/************************************************************************/
/*									*/
/*  Unimplemented..							*/
/*									*/
/************************************************************************/

static int	psAfmBBox(	int			valPos,
				char *			input,
				DocumentRectangle *	dr )
    {
    char *	s= input+ valPos;
    char *	past;
    double	left, bottom, right, top;

    int		done= 0;

    left= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    bottom= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    right= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    top= strtod( s, &past );
    if  ( past == s )
	{ SSDEB(input,input+ valPos); return -1;	}
    done += past- s; s= past;

    dr->drX0= (int)left;
    dr->drY0= (int)bottom;
    dr->drX1= (int)right;
    dr->drY1= (int)top;

    return done;
    }

static int psAfmFontBBox(	AfmReader *	ar,
				int		valPos,
				char *		input )
    {
    DocumentRectangle	abb;
    int		res= psAfmBBox( valPos, input, &abb );
    char *	s;

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    s= input+ valPos+ res;
    while( isspace( *s ) )
	{ s++;	}
    if  ( *s )
	{ SSDEB(input,input+ valPos+ res); return -1;	}

    ar->arAfi->afiFontBBox= abb;

    return 0;
    }

static int	psAfmMappingScheme(	AfmReader *	ar,
					int		valPos,
					char *		input )
    { LDEB(1); return -1; }

static int	psAfmCharacterSet(	AfmReader *		ar,
					int			valPos,
					char *			input )
    { return psAfmSaveString( &(ar->arAfi->afiCharacterSet), input+ valPos ); }

static int	psAfmCharacters(	AfmReader *		ar,
					int			valPos,
					char *			input )
    {
    long	count;
    char *	past;
    char *	s= input+ valPos;

    count= strtol( s, &past, 10 );
    if  ( past != s )
	{
	while( isspace( *past ) )
	    { past++;	}
	}

    if  ( past == s || *past )
	{ SSDEB(input+valPos,past); return -1;	}
    if  ( count < 0 )
	{ LDEB(count);	}

    return 0;
    }

static int	psAfmIsFixedV(		AfmReader *	ar,
					int		valPos,
					char *		input )
    {
    LDEB(1); return -1;
    }

static int	psAfmStartDirection(	AfmReader *	ar,
					int		valPos,
					char *		input )
    {
    LDEB(1); return -1;
    }

static int	psAfmFontFileIndex(	AfmReader *			ar,
					int			valPos,
					char *			input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiFontFileIndex= (int)d; return 0;
    }

static int	psAfmCapHeight(	AfmReader *			ar,
				int			valPos,
				char *			input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiCapHeight= (int)d; return 0;
    }

static int	psAfmXHeight(	AfmReader *			ar,
				int			valPos,
				char *			input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiXHeight= (int)d; return 0;
    }

static int	psAfmAscender(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiAscender= (int)d; return 0;
    }

static int	psAfmDescender(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiDescender= (int)d; return 0;
    }

static int	psAfmUnderlinePosition(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiUnderlinePosition= (int)d; return 0;
    }

static int	psAfmUnderlineThickness(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    double	d;

    if  ( psAfmGetNumber( &d, input, valPos ) )
	{ LDEB(1); return -1;	}

    ar->arAfi->afiUnderlineThickness= (int)d; return 0;
    }

static int	psAfmItalicAngle(	AfmReader *			ar,
					int		valPos,
					char *		input )
    {
    if  ( psAfmGetNumber( &(ar->arAfi->afiItalicAngle), input, valPos ) )
	{ return -1;	}

    ar->arAfi->afiTanItalicAngle= tan( ( M_PI* ar->arAfi->afiItalicAngle ) / 180  );

    return 0;
    }

static int	psAfmCharWidth(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    LDEB(1); return -1;
    }

static int	psAfmIsFixedPitch(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    if  ( ! strcmp( input+ valPos, "false" ) )
	{ ar->arAfi->afiIsFixedPitch= 0; return 0;	}
    if  ( ! strcmp( input+ valPos, "true" ) )
	{ ar->arAfi->afiIsFixedPitch= 1; return 0;	}

    SSDEB(input,input+ valPos); return -1;
    }

static int	psAfmStartCharMetrics(	AfmReader *		ar,
					int			valPos,
					char *			input )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, ar ) )
	{
	char *			b;
	char *			p;

	int			C= -1;
	double			WX= 0.0;
	char *			N= (char *)0;
	DocumentRectangle	abb;

	abb.drX0= abb.drX1= abb.drY1= abb.drY0= 0;

	if  ( ! strncmp( input, "EndCharMetrics", 14 ) )
	    {
	    b= input+ 14;

	    while( isspace (*b ) )
		{ b++;	}

	    if  ( *b )
		{ SDEB(input); return -1;	}

	    return 0;
	    }

	if  ( ! ( ar->arAfmFlags & PSflagDEFER_METRICS ) )
	    {
	    b= input;
	    while( isspace( *b ) )
		{ b++;	}

	    while( *b )
		{
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( isspace( *p ) )
		    { *(p++)= '\0';	}

		if  ( ! strcmp( b, "C" ) )
		    {
		    b= p;
		    C= strtol( b, &p, 10 );
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    goto nextToken;
		    }

		if  ( ! strcmp( b, "CX" ) )
		    {
		    b= p;
		    C= strtol( b, &p, 16 );
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    goto nextToken;
		    }

		if  ( ! strcmp( b, "WX" ) )
		    {
		    b= p;
		    WX= strtod( b, &p );
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    goto nextToken;
		    }

		if  ( ! strcmp( b, "N" ) )
		    {
		    b= p; while( isspace( *b ) )
			{ b++;	}
		    p= b; while( *p && ! isspace( *p ) )
			{ p++;	}
		    if  ( b == p )
			{ SDEB(b); return -1;	}

		    N= b;
		    goto nextToken;
		    }

		if  ( ! strcmp( b, "L" ) )
		    {
		    b= p; while( isspace( *b ) )
			{ b++;	}
		    p= b; while( *p && ! isspace( *p ) )
			{ p++;	}
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    b= p; while( isspace( *b ) )
			{ b++;	}
		    p= b; while( *p && ! isspace( *p ) )
			{ p++;	}
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    goto nextToken;
		    }

		if  ( ! strcmp( b, "B" ) )
		    {
		    int	done;

		    b= p;
		    done= psAfmBBox( 0, b, &abb );

		    if  ( done < 0 )
			{ SLDEB(b,done); return -1;	}

		    p= b+ done; goto nextToken;
		    }

		SSDEB(b,N); return -1;

		nextToken:
		    while( isspace( *p ) )
			{ *(p++)= '\0';	}
		    if  ( *p != ';' )
			{ SDEB(b); return -1;	}
		    *(p++)= '\0'; b= p;
		    while( isspace( *b ) )
			{ b++;	}
		    continue;
		}

	    if  ( psFontInfoAddMetric( ar->arAfi, C, WX, N, &abb ) )
		{ SDEB(N); return -1;	}
	    }
	}

    LDEB(AFMlenLINE); return -1;
    }

static int psAfmStartKernPairs(	AfmReader *		ar,
				int			valPos,
				char *			input )
{
    int		lineLength;

    if  ( ar->arAfmFlags & PSflagIGNORE_KERNING )
	{ ar->arAfi->afiIgnoreKerning= 1;	}

    while( psGetInputLine( input, &lineLength, AFMlenLINE, ar ) )
	{
	AfmCharMetric *		acm;
	AfmKerningPair *	akp;
	char *			b;
	char *			p;

	char *	N1= (char *)0;
	char *	N2= (char *)0;
	double	XVec= 0.0;

	int	pos1;
	int	pos2;

	if  ( ! strncmp( input, "EndKernPairs", 12 ) )
	    {
	    b= input+ 12;

	    while( isspace (*b ) )
		{ b++;	}

	    if  ( *b )
		{ SDEB(input); return -1;	}

	    return 0;
	    }

	if  ( ! ( ar->arAfmFlags & PSflagDEFER_METRICS )	&&
	      ! ( ar->arAfmFlags & PSflagIGNORE_KERNING )	)
	    {
	    b= input;
	    while( isspace( *b ) )
		{ b++;	}

	    if  ( ! *b || *b == '\n' )
		{ continue;	}

	    while( *b )
		{
		p= b; while( *p && ! isspace( *p ) )
		    { p++;	}
		if  ( isspace( *p ) )
		    { *(p++)= '\0';	}

		if  ( ! strcmp( b, "KPX" ) )
		    {
		    b= p; while( isspace( *b ) )
			{ b++;	}
		    p= b; while( *p && ! isspace( *p ) )
			{ p++;	}
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    if  ( isspace( *p ) )
			{ *(p++)= '\0';	}
		    N1= b;
		    b= p; while( isspace( *b ) )
			{ b++;	}
		    p= b; while( *p && ! isspace( *p ) )
			{ p++;	}
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    if  ( isspace( *p ) )
			{ *(p++)= '\0';	}
		    N2= b;
		    b= p;
		    XVec= strtod( b, &p );
		    if  ( b == p )
			{ SDEB(b); return -1;	}
		    goto nextToken;
		    }

		SDEB(b); return -1;

		nextToken:
		    while( isspace( *p ) )
			{ p++;	}
		    if  ( *p && *p != ';' )
			{ SDEB(b); return -1;	}
		    if  ( *p == ';' )
			{ b= p+ 1; }
		    else{ b= p   ; }

		    while( isspace( *b ) )
			{ b++;	}
		    continue;
		}

	    pos1= psFontInfoGetGlyphIndex( ar->arAfi, N1 );
	    if  ( pos1 < 0 )
		{ SLDEB(N1,pos1); return -1;	}
	    pos2= psFontInfoGetGlyphIndex( ar->arAfi, N2 );
	    if  ( pos2 < 0 )
		{ SLDEB(N2,pos2); return -1;	}

	    acm= ar->arAfi->afiMetrics[pos1];
	    akp= (AfmKerningPair *)realloc( acm->acmKernPairs,
			( acm->acmKernPairCount+ 1 )* sizeof(AfmKerningPair) );
	    if  ( ! akp )
		{ LXDEB(acm->acmKernPairCount,akp); return -1;	}
	    acm->acmKernPairs= akp;

	    akp += acm->acmKernPairCount++;
	    akp->akpPosition= pos2;
	    akp->akpXVec= (int)XVec;
	    }
	}

    LDEB(AFMlenLINE); return -1;
    }

static AfmKeyword	psAfmKernDataKeywords[]=
    {
	{ "Comment",		psAfmIgnore,		},
	{ "StartKernPairs",	psAfmStartKernPairs,	},
	{ "StartTrackKern",	psAfmStartTrackKern,	},
	{ "EndKernData",				},
    };

static int	psAfmStartKernData(	AfmReader *			ar,
					int	valPos,
					char *	input )
    {
    return psAfmConsumeLines( ar, input, psAfmKernDataKeywords,
		    sizeof(psAfmKernDataKeywords)/sizeof(AfmKeyword) );
    }

static int	psAfmStartComposites(	AfmReader *	ar,
					int		valPos,
					char *		input )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, ar ) )
	{
	if  ( ! strncmp( input, "EndComposites", 13 ) )
	    { break;					}
	}

    return 0;
    }

static int	psAfmStartTrackKern(	AfmReader *	ar,
					int		valPos,
					char *		input )
    {
    int		lineLength;

    while( psGetInputLine( input, &lineLength, AFMlenLINE, ar ) )
	{
	if  ( ! strncmp( input, "EndTrackKern", 13 ) )
	    { break;					}
	}

    return 0;
    }

