/************************************************************************/
/*									*/
/*  Postscript printing procedures.					*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"psPrint.h"
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Emulate the PS level 2 rectfill operator (for 1 rectangle only)	*/
/*  if needed.								*/
/*									*/
/************************************************************************/

static const char * psRectfillEmulation[]=
{
"%%rectfill emulation for one rectangle only",
"/rectfill where",
"    { pop }",
"    { /rectfill",
"        { 4 2 roll moveto 2 copy",
"            0 exch rlineto 0 rlineto ",
"            neg 0 exch rlineto pop closepath fill",
"        } bind def",
"    }",
"ifelse",
};

void psSetRectfillEmulation(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, psRectfillEmulation,
		    sizeof(psRectfillEmulation)/sizeof(const char *) );

    return;
    }

/************************************************************************/
/*									*/
/*  Emulate the PS level 1 1/2 selectfont operator if needed.		*/
/*									*/
/************************************************************************/

static const char * psSelectfontEmulation[]=
{
"%%selectfont emulation",
"/selectfont where",
"    { pop }",
"    { /selectfont",
"        { exch findfont exch dup type /arraytype eq",
"          { makefont } { scalefont } ifelse",
"          setfont",
"        } bind def",
"    }",
"ifelse",
};

void psSetSelectfontEmulation(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, psSelectfontEmulation,
		    sizeof(psSelectfontEmulation)/sizeof(const char *) );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate an utf8 string to 8/8 encoding and then show it.		*/
/*									*/
/************************************************************************/

# if 0
    For hex debugging: (Actually checked this way)
    (1) "16 2 string cvrs putinterval" for "put"
    (2) "1 add" -> "2 add"
    (3) "2 add" -> "4 add"
    (4) "2 mul" -> "4 mul"

    Also change font setting to an ordinary font.
# endif

static const char * psUtf8ShowImplementation[]=
{
"%%%%",
"%%%%    Emit a single big endian utf16 value. We assume that ",
"%%%%    the value fits in 16 bits.",
"%%%%",

"/e_utf16",
"    {",
"    %dup (EMIT 0x) print 16 10 string cvrs print (\n) print",

"				% s_to l_to val",
"    dup 256 idiv",
"				% s_to l_to val val/256",
"    3 index 3 index 3 2 roll",
"				% s_to l_to val s_to l_to val/256",
"    put",			/* (1) */
"				% s_to l_to val",
"    256 mod",
"				% s_to l_to val%256",
"    2 index 2 index 1 add 3 2 roll",	/* (2) */
"				% s_to l_to s_to l_to+1 val%256",
"    put",			/* (1) */
"				% s_to l_to",
"    2 add",			/* (3) */
"    } bind def",

"%%%%",
"%%%%    Translate an utf-8 coded string to big endian utf16 and then ",
"%%%%    show it. The assumption is that the current font is an 8/8 ",
"%%%%    encoded composite font.",
"%%%%",

"/utf8show",
"    {",
"    dup length 2 mul string exch", /* (4) */
"				    % s_to s_from",
"    0 exch 0 exch 0 exch",
"				    % s_to l_to val togo s_from",

"    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
"    {",
"				% val togo c",
"    dup 128 ge",
"	{",
"	dup 192 ge",
"	    { %%%% Start a multibyte sequence",
"	    exch pop 1 exch",
"				    % val togo=1 c",

"	    4 mul 256 mod",
"				    % val togo (4*c)%256",
"		{",
"				    % while ( c >= 128 )",
"		dup 128 lt { exit } if",
"				    % val togo c -> togo (2*c)%256",
"		1 bitshift 256 mod",
"				    % val togo c -> val togo+1 c",
"		exch 1 add exch",
"		} loop",
"				    % val togo c -> val togo c togo",
"	    1 index",
"				    % val togo c togo",
"	    1 add neg bitshift",
"				    % val togo c>>(togo+1)",
"	    3 1 roll exch pop",
"				    % val=c togo",
"	    }",
"	    { %%%% Continuation of a multibyte sequence",
"	    63 and",
"				    % val togo c%0x3f -> togo c%0x3f val",
"	    3 2 roll",
"	    64 mul add",
"				    % togo c%0x3f+64*val",
"	    1 index 1 sub",
"				    % togo c%0x3f+64*val togo-1",
"	    0 eq {",
"				    % togo val -> val",
"		exch pop",
"		e_utf16",
"				    % val=0 togo=0",
"		0 0",
"		}",
"		{",
"				    % togo val",
"		exch 1 sub",
"		} ifelse",
"	    } ifelse",
"	}",
"	{ %%%% An ascii character",
"				    % val togo c -> c",
"	3 1 roll pop pop",
"				    % val",
"	e_utf16",
"				    % val=0 togo=0",
"	0 0",
"	} ifelse",
"    } forall",
"    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
"				    % s_to l_to val togo",
"    pop pop",
"				    % s_to l_to",
"    0 exch getinterval show",
"    } bind def",
};

void psSetUtf8ShowImplementation(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, psUtf8ShowImplementation,
		    sizeof(psUtf8ShowImplementation)/sizeof(const char *) );

    return;
    }

void psSetMvsImplementation(		SimpleOutputStream *	sos )
    {
    sioOutPrintf( sos, "\n/mvs { moveto utf8show } bind def\n" );
    }

static const char * psStartStdCodePageImplementation[]=
{
"%%%%",
"%%%%    Build the array that will hold a standard code page to use in a ",
"%%%%    font definition. ",
"%%%%",

"/sstdcp",
"  {",
"  256 array 0 1 255",
"    {",
"    1 index exch /.notdef put",
"    } for",
"  } bind def",
};

static const char * psDefinePageFontImplementation[]=
{
"%%%%",
"%%%%    Define a code page font with a particular code page ",
"%%%%    The caller provides the following on the stack: ",
"%%%%    /encodedFontName codePage /baseFontName ",
"%%%%",

"/dcpf",
"  {",
"  findfont dup length dict begin",
"    {",
"    1 index /FID ne",
"      { def } { pop pop } ifelse",
"    } forall",
"  /Encoding exch def currentdict",
"  end",
"  definefont pop",
"  } bind def",
};

void psSetDefinePageFontImplementation(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, psStartStdCodePageImplementation,
	    sizeof(psStartStdCodePageImplementation)/sizeof(const char *) );

    psDefineProcedure( sos, psDefinePageFontImplementation,
	    sizeof(psDefinePageFontImplementation)/sizeof(const char *) );

    return;
    }

/************************************************************************/
/*									*/
/*  The 12 shadings that come with MS-Word. Correspond to Windows GDI	*/
/*  shading patterns.							*/
/*									*/
/************************************************************************/

static const char * psPatternImplementation[]=
{
"%%%%",
"%%%%   Primitives for filling rectangles the way the MS-Windows ",
"%%%%   GDI does.",
"%%%%",
"",
"% linewidth (plain)",
"/l-ortho    { 0.175 setlinewidth } bind def",
"/l-cross    { 0.125 setlinewidth } bind def",
"/l-diag     { 0.150 setlinewidth } bind def",
"/l-dcross   { 0.075 setlinewidth } bind def",
"",
"% linewidth (dark)",
"/l-dkortho  { 0.350 setlinewidth } bind def",
"/l-dkcross  { 0.250 setlinewidth } bind def",
"/l-dkdiag   { 0.300 setlinewidth } bind def",
"/l-dkdcross { 0.150 setlinewidth } bind def",
"",
"% Extend to grid (4 points); Step in grid",
"/h-grid { ceiling cvi } bind def",
"/h-step { 1 sub } bind def",
"",
"/h-matrix",
"    {",
"    matrix defaultmatrix",
"    h-mesh h-mesh matrix scale ",
"    matrix concatmatrix",
"    } bind def",
"",
"/h-mesh { 3 } bind def",
"",
"/h-horiz",
"    {",
"    % llx lly urx+i ury+i",
"    {",
"	dup 3 index lt { exit } if",
"	2 copy moveto 3 index 1 index lineto",
"	h-step",
"    } loop",
"    pop pop pop pop",
"    } bind def",
"",
"/h-vert",
"    {",
"    % llx lly urx+i ury+i",
"    {",
"	1 index 4 index lt { exit } if",
"	2 copy moveto 1 index 3 index lineto",
"	exch h-step exch",
"    } loop",
"    pop pop pop pop",
"    } bind def",
"",
"/h-prep",
"    {",
"    gsave pathbbox clip",
"",
"    % llx lly urx ury",
"    h-matrix matrix invertmatrix",
"    5 1 roll",
"    4 2 roll transform 4 index transform",
"    4 2 roll transform 4 index transform",
"    5 -1 roll pop",
"",
"    % yswap? llx lly urx ury",
"    dup 3 index lt { 3 1 roll exch } if",
"    % xswap? llx lly urx ury",
"    1 index 4 index lt { 4 2 roll 3 1 roll exch } if",
"",
"    h-matrix setmatrix",
"",
"    % clipping takes care of the linecaps: avoid white gaps",
"    2 setlinecap",
"    } bind def",
"",
"/h-prep-ortho",
"    {",
"    h-prep",
"",
"    % Extend upper right to grid: llx lly urx ury -> llx lly urx+i ury+i",
"    exch h-grid exch h-grid",
"    } bind def",
"",
"/h-fdiag",
"    {",
"    % add width to ury",
"    exch 4 index add",
"    % extend extended ury to grid",
"    h-grid			% w llx lly urx+i ury+i",
"",
"    % w llx lly urx+i ury+i",
"    {",
"	dup 3 index lt { exit } if",
"	2 copy",
"	moveto 4 index neg dup ",
"	rlineto",
"	h-step",
"    } loop",
"    pop pop pop pop pop",
"    } bind def",
"",
"/h-bdiag",
"    {",
"    % subtract witdh from lly",
"    3 2 roll 4 index sub 3 -2 roll",
"    % extend extended ury to grid",
"    exch h-grid		% w llx lly urx+i ury+i",
"",
"    % w llx lly urx+i ury+i",
"    {",
"	dup 3 index lt { exit } if",
"	2 copy",
"	moveto 4 index dup neg exch",
"	rlineto",
"	h-step",
"    } loop",
"    pop pop pop pop pop",
"    } bind def",
"",
"/h-prep-diag",
"    {",
"    h-prep",
"",
"    % extend urx to grid",
"    exch h-grid					% llx lly ury urx+i",
"    % save width at the stack bottom",
"    dup 4 index exch sub neg 5 1 roll		% w llx lly ury urx+i",
"    } bind def",
"",
"/f-horiz",
"    {",
"    h-prep-ortho l-ortho",
"",
"    newpath",
"	h-horiz",
"    stroke grestore",
"    } bind def",
"",
"/f-dkhoriz",
"    {",
"    h-prep-ortho l-dkortho",
"",
"    newpath",
"	h-horiz",
"    stroke grestore",
"    } bind def",
"",
"/f-vert",
"    {",
"    h-prep-ortho l-ortho",
"",
"    newpath",
"	h-vert",
"    stroke grestore",
"    } bind def",
"",
"/f-dkvert",
"    {",
"    h-prep-ortho l-dkortho",
"",
"    newpath",
"	h-vert",
"    stroke grestore",
"    } bind def",
"",
"/f-cross",
"    {",
"    h-prep-ortho l-cross",
"",
"    newpath",
"	4 copy h-horiz h-vert",
"    stroke grestore",
"    } bind def",
"",
"/f-dkcross",
"    {",
"    h-prep-ortho l-dkcross",
"",
"    newpath",
"	4 copy h-horiz h-vert",
"    stroke grestore",
"    } bind def",
"",
"/f-fdiag",
"    {",
"    h-prep-diag l-diag",
"",
"    newpath",
"	h-fdiag",
"    stroke grestore",
"    } bind def",
"",
"/f-dkfdiag",
"    {",
"    h-prep-diag l-dkdiag",
"",
"    newpath",
"	h-fdiag",
"    stroke grestore",
"    } bind def",
"",
"/f-bdiag",
"    {",
"    h-prep-diag l-diag",
"",
"    newpath",
"	h-bdiag",
"    stroke grestore",
"    } bind def",
"",
"/f-dkbdiag",
"    {",
"    h-prep-diag l-dkdiag",
"",
"    newpath",
"	h-bdiag",
"    stroke grestore",
"    } bind def",
"",
"/f-dcross",
"    {",
"    h-prep-diag l-dcross",
"",
"    newpath",
"	5 copy h-fdiag h-bdiag",
"    stroke grestore",
"    } bind def",
"",
"/f-dkdcross",
"    {",
"    h-prep-diag l-dkdcross",
"",
"    newpath",
"	5 copy h-fdiag h-bdiag",
"    stroke grestore",
"    } bind def",
};

void psSetPatternImplementation(	SimpleOutputStream *	sos,
					double			shadingMesh )
    {
    psDefineProcedure( sos, psPatternImplementation,
		    sizeof(psPatternImplementation)/sizeof(const char *) );

    if  ( shadingMesh >= 1e-4 )
	{
	sioOutPrintf( sos, "\n/h-mesh { %g } bind def\n", shadingMesh );
	}

    return;
    }

