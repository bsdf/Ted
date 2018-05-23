#   include	"appUtilConfig.h"

#   include	<math.h>

#   include	"utilColor.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize an RGB8Color.						*/
/*									*/
/************************************************************************/

void utilInitRGB8Color(	RGB8Color *	rgb8 )
    {
    rgb8->rgb8Red= 0;
    rgb8->rgb8Green= 0;
    rgb8->rgb8Blue= 0;
    rgb8->rgb8Alpha= 255;

    return;
    }

/************************************************************************/

int utilRGB8GetComponent(	const RGB8Color *	rgb8,
				int			prop )
    {
    switch( prop )
	{
	case RGBAcompRED:
	    return rgb8->rgb8Red;
	case RGBAcompGREEN:
	    return rgb8->rgb8Green;
	case RGBAcompBLUE:
	    return rgb8->rgb8Blue;
	case RGBAcompALPHA:
	    return rgb8->rgb8Alpha;

	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/

void utilGetTopShadowColor(		RGB8Color *		top,
					const RGB8Color *	back )
    {
    int				luma;
    int				chroma;
    int				hue;

    utilRGB8LumaChromaHue( &luma, &chroma, &hue, back );
    luma= sqrt( luma* 255.0+ 0.4999 );
    if  ( luma > 255 )
	{ LDEB(luma); luma= 255;	}

    utilRGB8FromLumaChromaHue( top, luma, chroma, hue );

    return;
    }

void utilGetBottomShadowColor(		RGB8Color *		bottom,
					const RGB8Color *	back )
    {
    int				luma;
    int				chroma;
    int				hue;

    utilRGB8LumaChromaHue( &luma, &chroma, &hue, back );
    luma= 255- sqrt( ( 255- luma )* 255.0+ 0.4999 );
    if  ( luma > 255 )
	{ LDEB(luma); luma= 255;	}

    utilRGB8FromLumaChromaHue( bottom, luma, chroma, hue );

    return;
    }

void utilGetForegroundColor(		RGB8Color *		fore,
					const RGB8Color *	back )
    {
    int				luma;
    int				chroma;
    int				hue;

    utilRGB8LumaChromaHue( &luma, &chroma, &hue, back );

    if  ( luma < 128 )
	{ utilRGB8SolidWhite( fore );	}
    else{ utilRGB8SolidBlack( fore );	}

    return;
    }

/************************************************************************/
/*									*/
/*  RGB -> IHS conversion.						*/
/*  (Luma/Chroma/Hue)							*/
/*									*/
/*  See http://en.wikipedia.org/wiki/HSL_and_HSV			*/
/*									*/
/*  From NTSC (Luma)							*/
/*  0.30* 255=  76.50							*/
/*  0.59* 255= 150.45							*/
/*  0.11* 255=  28.05							*/
/*									*/
/************************************************************************/

#   define	F_R	 77
#   define	F_G	150
#   define	F_B	 28

#   define	Y(r,g,b) ( ( F_R* (r)+ F_G* (g)+ F_B* (b)+ 127 )/  \
							    ( F_R+ F_G+ F_B ) )

			/**
			 *  Angle units in 1/6 circle
			 *  Use 1/10t of a degree.
			 */
#   define	A	600

int utilRGB8LumaChromaHue(	int *			pLuma,
				int *			pChroma,
				int *			pHue,
				const RGB8Color *	rgb )
    {
    int		M= rgb->rgb8Red;
    int		m= rgb->rgb8Red;
    int		q= 0;

    int		C;
    int		H= 0;

    if  ( M < rgb->rgb8Green )
	{ M=  rgb->rgb8Green; q= 2;	}
    if  ( m > rgb->rgb8Green )
	{ m=  rgb->rgb8Green;	}

    if  ( M < rgb->rgb8Blue )
	{ M=  rgb->rgb8Blue; q= 4;	}
    if  ( m > rgb->rgb8Blue )
	{ m=  rgb->rgb8Blue;	}

    C= M- m; /* chroma */

    if  ( C != 0 && pHue )
	{
	switch( q )
	    {
	    case 0:
		H= ( A* ( rgb->rgb8Green- rgb->rgb8Blue )+ (C/2)- 1 )/ C+ (0*A);
		break;
	    case 2:
		H= ( A* ( rgb->rgb8Blue- rgb->rgb8Red )+ (C/2)- 1 )/ C+ (2*A);
		break;
	    case 4:
		H= ( A* ( rgb->rgb8Red- rgb->rgb8Green )+ (C/2)- 1 )/ C+ (4*A);
		break;
	    default:
		LDEB(q); return -1;
	    }

	H= ( H+ (6*A) ) % (6*A);
	}

    if  ( pLuma )
	{ *pLuma= Y( rgb->rgb8Red, rgb->rgb8Green, rgb->rgb8Blue );	}
    if  ( pChroma )
	{ *pChroma= C;	}
    if  ( pHue )
	{ *pHue= H;	}

    return 0;
    }

int utilRGB8FromLumaChromaHue(	RGB8Color *	rgb8,
				int		luma,
				int		chroma,
				int		hue )
    {
    int		R1;
    int		G1;
    int		B1;

    int		m;

    if  ( hue < 0 || hue >= (6*A) )
	{ LDEB(hue); return -1;	}
    if  ( chroma < 0 || chroma > 255 )
	{ LDEB(chroma); return -1;	}
    if  ( luma < 0 || luma > 255 )
	{ LDEB(luma); return -1;	}

    if  ( chroma == 0 )
	{ R1= G1= B1= 0;	}
    else{
	int		Hp;
	int		C;
	int		X;

	Hp= hue/ A;
	C= chroma;

	if  ( Hp % 2 )
	    { X= ( C* ( A- hue+ A* Hp ) +(A/2)- 1 )/ A;	}
	else{ X= ( C* (    hue- A* Hp ) +(A/2)- 1 )/ A;	}

	switch( Hp )
	    {
	    case 0: R1= C; G1= X; B1= 0; break;
	    case 1: R1= X; G1= C; B1= 0; break;
	    case 2: R1= 0; G1= C; B1= X; break;
	    case 3: R1= 0; G1= X; B1= C; break;
	    case 4: R1= X; G1= 0; B1= C; break;
	    case 5: R1= C; G1= 0; B1= X; break;

	    default:
		LDEB(Hp); return -1;
	    }
	}

    m= luma- Y( R1, G1, B1 );

    R1 += m; G1 += m; B1 += m;

    if  ( R1 > 255 )
	{ /* LDEB(R1); */ R1= 255;	}
    if  ( G1 > 255 )
	{ /* LDEB(G1); */ G1= 255;	}
    if  ( B1 > 255 )
	{ /* LDEB(B1); */ B1= 255;	}

    rgb8->rgb8Red= R1;
    rgb8->rgb8Green= G1;
    rgb8->rgb8Blue= B1;

    return 0;
    }

# if 0
extern int xxxx( void );

static int tt( int r, int g, int b )
    {
    RGB8Color	in;
    int		luma, chroma, hue;
    RGB8Color	out;

    in.rgb8Red= r;
    in.rgb8Green= g;
    in.rgb8Blue= b;

    if  ( utilRGB8LumaChromaHue( &luma, &chroma, &hue, &in ) )
	{ LLLDEB(in.rgb8Red,in.rgb8Green,in.rgb8Blue); return -1; }
    if  ( utilRGB8FromLumaChromaHue( &out, luma, chroma, hue ) )
	{
	LLLDEB(in.rgb8Red,in.rgb8Green,in.rgb8Blue);
	LLLDEB(luma,chroma,hue);
	return -1;
	}
# define D 0
    if  ( out.rgb8Red < in.rgb8Red- D || out.rgb8Red > in.rgb8Red+ D	||
	  out.rgb8Green < in.rgb8Green- D || out.rgb8Green > in.rgb8Green+ D ||
	  out.rgb8Blue < in.rgb8Blue- D || out.rgb8Blue > in.rgb8Blue+ D )
	{
	LLLDEB(in.rgb8Red,in.rgb8Green,in.rgb8Blue);
	LLLDEB(luma,chroma,hue);
	LLLDEB(out.rgb8Red,out.rgb8Green,out.rgb8Blue);
	return -1;
	}

    return 0;
    }

static int ttt( const char * s )
    {
    unsigned int		r, g, b;

    if  ( sscanf( s, "%02x%02x%02x", &r, &g, &b ) != 3 )
	{ SDEB(s); return -1;	}

    if  ( tt( r, g, b ) )
	{ SDEB(s); return -1;	}

    return 0;
    }

int xxxx( void )
    {
    int		r, g, b;

    SDEB("EXAMPLES");
    if ( ttt( "ffffff" ) ) return -1; /* luma=255 chroma=  0 hue=  0  */
    if ( ttt( "808080" ) ) return -1; /* luma=128 chroma=  0 hue=  0  */
    if ( ttt( "000000" ) ) return -1; /* luma=  0 chroma=  0 hue=  0  */
    if ( ttt( "ff0000" ) ) return -1; /* luma= 77 chroma=255 hue=  0  */
    if ( ttt( "bfbf00" ) ) return -1; /* luma=170 chroma=191 hue= 60  */
    if ( ttt( "008000" ) ) return -1; /* luma= 75 chroma=128 hue=120  */
    if ( ttt( "80ffff" ) ) return -1; /* luma=216 chroma=127 hue=180  */
    if ( ttt( "8080ff" ) ) return -1; /* luma=141 chroma=127 hue=240  */
    if ( ttt( "bf40bf" ) ) return -1; /* luma=116 chroma=127 hue=300  */
    if ( ttt( "a0a424" ) ) return -1; /* luma=148 chroma=128 hue= 62  */
    if ( ttt( "411bea" ) ) return -1; /* luma= 61 chroma=207 hue=251  */
    if ( ttt( "1eac41" ) ) return -1; /* luma=117 chroma=142 hue=134  */
    if ( ttt( "f0c80e" ) ) return -1; /* luma=191 chroma=226 hue= 49  */
    if ( ttt( "b430e5" ) ) return -1; /* luma=107 chroma=181 hue=283  */
    if ( ttt( "ed7651" ) ) return -1; /* luma=149 chroma=156 hue= 14  */
    if ( ttt( "fef888" ) ) return -1; /* luma=237 chroma=118 hue= 56  */
    if ( ttt( "19cb97" ) ) return -1; /* luma=143 chroma=178 hue=162  */
    if ( ttt( "362698" ) ) return -1; /* luma= 55 chroma=114 hue=248  */
    if ( ttt( "7e7eb8" ) ) return -1; /* luma=132 chroma= 58 hue=240  */

    SDEB("WEB SAFE");
    for ( r= 0; r < 256; r += 51 )
	{
	for ( g= 0; g < 256; g += 51 )
	    {
	    for ( b= 0; b < 256; b += 51 )
		{
		if  ( tt( r, g, b ) )
		    { return -1;	}
		}
	    }
	}

    SDEB("ALL");
    for ( r= 0; r < 256; r += 1 )
	{
	for ( g= 0; g < 256; g += 1 )
	    {
	    for ( b= 0; b < 256; b += 1 )
		{
		if  ( tt( r, g, b ) )
		    { return -1;	}
		}
	    }
	}

    return 0;
    }
# endif
