#   include	"drawMetaConfig.h"

#   include	<stddef.h>

#   include	"drawMetafilePs.h"
#   include	<psPrint.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Include the procedures that are used for WMF rendering in		*/
/*  PostScript.								*/
/*									*/
/*  sed -e 's/^/"/' -e 's/$/",/' file					*/
/*									*/
/************************************************************************/

static const char *	APPMETAPS_FillPrep[]=
{
"/fill-prep",
"    {",
"    pathbbox",
"    clip newpath",
"			% llx_usr  lly_usr  urx_usr  ury_usr",
"    [ 0 0 0 0 0 0 ] currentmatrix",
"    transform",
"    [ 0 0 0 0 0 0 ] defaultmatrix [ 0 0 0 0 0 0 ] invertmatrix",
"    transform",
"			% llx_usr  lly_usr  urx_dev  ury_dev",
"    4 2 roll",
"			% urx_dev  ury_dev  llx_usr  lly_usr",
"    [ 0 0 0 0 0 0 ] currentmatrix",
"    transform",
"    [ 0 0 0 0 0 0 ] defaultmatrix [ 0 0 0 0 0 0 ] invertmatrix",
"    transform",
"			% urx_dev  ury_dev  llx_dev  lly_dev",
"    3 -1 roll",
"			% urx_dev  llx_dev  lly_dev  ury_dev",
"    exch",
"			% urx_dev  llx_dev  ury_dev  lly_dev",
"    2 copy lt { exch } if",
"			% urx_dev  llx_dev  ury=d  lly=d",
"    /fill-high load div floor   /fill-high load mul",
"			% urx_dev  llx_dev  ury=d  LLY_dev",
"    exch",
"    /fill-high load div ceiling /fill-high load mul",
"			% urx_dev  llx_dev  LLY_dev URY_dev",
"    4 2 roll",
"			% LLY_dev URY_dev urx_dev  llx_dev",
"    2 copy lt { exch } if",
"			% LLY_dev URY_dev urx=d  llx=d",
"    /fill-wide load div floor   /fill-wide load mul",
"			% LLY_dev URY_dev urx=d  LLX_dev",
"    exch",
"    /fill-wide load div ceiling /fill-wide load mul",
"			% LLY_dev URY_dev LLX_dev URX_dev",
"    4 2 roll",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    [ 0 0 0 0 0 0 ] defaultmatrix setmatrix",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillPattern[]=
{
"/fill-pattern",
"    {",
"    gsave fill-prep",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    /fill-high load exch",
"			% LLX_dev URX_dev LLY_dev <fill-high> URY_dev",
"	{",
"	gsave",
"			% LLX_dev URX_dev y",
"	0 exch translate",
"			% LLX_dev URX_dev",
"	2 copy /fill-wide load exch",
"	    {",
"	    gsave",
"	    0 translate",
"	    fill-cell",
"	    grestore",
"	    } for",
"",
"	grestore",
"	} for",
"",
"    pop pop",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_Rl[]=
{
"/rl",
"    {",
"    rlineto",
"    } bind def",
"",
};

static const char *	APPMETAPS_Mt[]=
{
"/mt",
"    {",
"    moveto",
"    } bind def",
"",
};

static const char *	APPMETAPS_Cp[]=
{
"/cp",
"    {",
"    closepath",
"    } bind def",
"",
};

static const char *	APPMETAPS_Bp[]=
{
"/bp",
"    {",
"    newpath moveto",
"    } bind def",
"",
};

int appMetaDefineProcsetPs(	SimpleOutputStream *		sos )
    {
    sioOutPrintf( sos, "%%%%BeginProcSet: AppWmfToPs 1.0 1\n\n" );

    psDefineProcedure( sos, APPMETAPS_Bp,
		    sizeof(APPMETAPS_Bp)/sizeof(const char *) );

    psDefineProcedure( sos, APPMETAPS_Mt,
		    sizeof(APPMETAPS_Mt)/sizeof(const char *) );

    psDefineProcedure( sos, APPMETAPS_Rl,
		    sizeof(APPMETAPS_Rl)/sizeof(const char *) );

    psDefineProcedure( sos, APPMETAPS_Cp,
		    sizeof(APPMETAPS_Cp)/sizeof(const char *) );

    psDefineProcedure( sos, APPMETAPS_FillPrep,
		    sizeof(APPMETAPS_FillPrep)/sizeof(const char *) );

    psDefineProcedure( sos, APPMETAPS_FillPattern,
		    sizeof(APPMETAPS_FillPattern)/sizeof(const char *) );

    sioOutPrintf( sos, "%%%%EndProcSet\n\n" );
    return 0;
    }

