#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	"utilPalette.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize a palette.						*/
/*									*/
/************************************************************************/

void utilInitColorPalette(	ColorPalette *		cp )
    {
    cp->cpColorCount= 0;
    cp->cpColors= (RGB8Color *)0;
    }

void utilCleanColorPalette(	ColorPalette *		cp )
    {
    if  ( cp->cpColors )
	{ free( cp->cpColors );	}
    }

int utilCopyColorPalette(	ColorPalette *		to,
				const ColorPalette *	from )
    {
    if  ( utilPaletteSetCount( to, from->cpColorCount ) )
	{ LDEB(from->cpColorCount); return -1;	}

    if  ( from->cpColorCount > 0 )
	{
	memcpy( to->cpColors, from->cpColors,
				from->cpColorCount* sizeof( RGB8Color ) );
	}

    return 0;
    }

int utilPaletteSetCount(	ColorPalette *		cp,
				int			colorCount )
    {
    if  ( colorCount == 0 )
	{
	if  ( cp->cpColors )
	    {
	    free( cp->cpColors );
	    cp->cpColors= (RGB8Color *)0;
	    }
	}
    else{
	RGB8Color *	fresh;

	fresh= realloc( cp->cpColors, colorCount* sizeof(RGB8Color) );
	if  ( ! fresh )
	    { LXDEB(colorCount,fresh); return -1;	}

	cp->cpColors= fresh;
	while( cp->cpColorCount < colorCount )
	    { utilInitRGB8Color( cp->cpColors+ cp->cpColorCount++ );	}
	}

    cp->cpColorCount= colorCount;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find/Allocate a color in the palette of a palette image. Only do so	*/
/*  if there is space in the palette.					*/
/*									*/
/************************************************************************/

int utilPaletteColorIndex(	ColorPalette *		cp,
				int			maxCount,
				int			r,
				int			g,
				int			b,
				int			a )
    {
    int		col;

    for ( col= 0; col < cp->cpColorCount; col++ )
	{
	if  ( cp->cpColors[col].rgb8Red == r	&&
	      cp->cpColors[col].rgb8Green == g	&&
	      cp->cpColors[col].rgb8Blue == b	&&
	      cp->cpColors[col].rgb8Alpha == a	)
	    { break;	}
	}

    if  ( col >= cp->cpColorCount && cp->cpColorCount >= maxCount )
	{ LLDEB(cp->cpColorCount,maxCount); return -1; }

    if  ( col >= cp->cpColorCount )
	{
	RGB8Color *	fresh;

	fresh= (RGB8Color *)realloc( cp->cpColors,
			( cp->cpColorCount+ 1 )*sizeof(RGB8Color) );
	if  ( ! fresh )
	    { LXDEB(cp->cpColorCount,fresh); return -1; }

	cp->cpColors= fresh;
	fresh += cp->cpColorCount++;
	fresh->rgb8Red= r;
	fresh->rgb8Green= g;
	fresh->rgb8Blue= b;
	fresh->rgb8Alpha= a;
	}

    return col;
    }

/************************************************************************/

static int utilPaletteColorDistance(	const RGB8Color *	rgb1,
					const RGB8Color *	rgb2 )
    {
    int		dd= 0;
    int		ddd;

    ddd= rgb1->rgb8Red- rgb2->rgb8Red;
    if  ( ddd < 0 )
	{ ddd= -ddd;	}
    if  ( dd < ddd )
	{ dd=  ddd;	}

    ddd= rgb1->rgb8Green- rgb2->rgb8Green;
    if  ( ddd < 0 )
	{ ddd= -ddd;	}
    if  ( dd < ddd )
	{ dd=  ddd;	}

    ddd= rgb1->rgb8Blue- rgb2->rgb8Blue;
    if  ( ddd < 0 )
	{ ddd= -ddd;	}
    if  ( dd < ddd )
	{ dd=  ddd;	}

    return dd;
    }

/************************************************************************/

static int utilPaletteFindNearestColor(	const ColorPalette *	cp,
					const RGB8Color *	rgb8,
					int			avoidZero )
    {
    int		d= 257;
    int		i;
    int		color= -1;

    for ( i= avoidZero; i < cp->cpColorCount; i++ )
	{
	int		dd= 0;

	dd= utilPaletteColorDistance( rgb8, &(cp->cpColors[i]) );
	if  ( dd < d )
	    { d= dd; color= i;	}
	}

    return color;
    }

/************************************************************************/
/*									*/
/*  Insert a color in a palette. (Or just return its index)		*/
/*									*/
/*  1)  As a HACK return the index of the nearest color if the palette	*/
/*	is full.							*/
/*									*/
/************************************************************************/

int utilPaletteInsertColor(	ColorPalette *		cp,
				int			avoidZero,
				int			maxColors,
				const RGB8Color *	rgb8 )
    {
    int		color;
    int		extra= 0;

    avoidZero= ( avoidZero != 0 );

    for ( color= avoidZero; color < cp->cpColorCount; color++ )
	{
	if  ( ! bmRGB8ColorsDiffer( &(cp->cpColors[color]), rgb8 ) )
	    { return color;	}
	}

    /*  1  */
    if  ( maxColors > 0 && cp->cpColorCount >= maxColors )
	{
	LLDEB(cp->cpColorCount,maxColors);

	color= utilPaletteFindNearestColor( cp, rgb8, avoidZero );
	if  ( color < 0 )
	    { color= avoidZero;	}

	return color;
	}

    if  ( avoidZero && cp->cpColorCount == 0 )
	{ extra= 1;	}

    if  ( utilPaletteSetCount( cp, extra+ cp->cpColorCount+ 1 ) )
	{ LDEB(extra+ cp->cpColorCount+ 1); return -1;	}

    if  ( extra > 0 )
	{
	utilInitRGB8Color( cp->cpColors+ 0 );
	}

    cp->cpColors[cp->cpColorCount- 1]= *rgb8;

    return cp->cpColorCount- 1;
    }

/************************************************************************/
/*									*/
/*  Merge two palettes.							*/
/*									*/
/*  1)  Allocate memory for the mapping.				*/
/*  2)  Map exact matches.						*/
/*									*/
/************************************************************************/

int utilMergeColorPalettes(	int **				pColorMap,
				ColorPalette *			cpTo,
				const ColorPalette *		cpFrom,
				int				avoidZero,
				int				maxColors )
    {
    int				rval= 0;

    const RGB8Color *		rgb8From;

    int *			colorMap= (int *)0;

    int				from;
    int				to;

    avoidZero= avoidZero != 0;

    /*****/

    /*  1  */
    if  ( cpFrom->cpColorCount > avoidZero )
	{
	colorMap= (int *)malloc( cpFrom->cpColorCount* sizeof( int ) );
	if  ( ! colorMap )
	    { LXDEB(cpFrom->cpColorCount,colorMap); rval= -1; goto ready; }

	if  ( avoidZero )
	    { colorMap[0]= 0;	}
	}
    for ( from= avoidZero; from < cpFrom->cpColorCount; from++ )
	{ colorMap[from]= -1;	}

    /*****/

# if 1
    /*  2  */
    for ( from= avoidZero; from < cpFrom->cpColorCount; from++ )
	{
	for ( to= avoidZero; to < cpTo->cpColorCount; to++ )
	    {
	    if  ( ! bmRGB8ColorsDiffer( &(cpFrom->cpColors[from]),
						    &(cpTo->cpColors[to]) ) )
		{ colorMap[from]= to; break;	}
	    }
	}
# endif

    /*  1  */
    rgb8From= cpFrom->cpColors+ avoidZero;
    for ( from= avoidZero; from < cpFrom->cpColorCount; rgb8From++, from++ )
	{
	if  ( colorMap[from] >= 0 )
	    { continue;	}

	/*  2  */
	to= utilPaletteInsertColor( cpTo, avoidZero, maxColors, rgb8From );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}

	colorMap[from]= to;
	}

    /*  steal */
    if  ( *pColorMap )
	{ free( *pColorMap );	}
    *pColorMap= colorMap; colorMap= (int *)0;

  ready:

    if  ( colorMap )
	{ free( colorMap );	}

    return rval;
    }

