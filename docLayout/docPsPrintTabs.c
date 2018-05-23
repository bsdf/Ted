/************************************************************************/
/*									*/
/*  Save PostScript procedures for the implementation of tab stops.	*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<sioGeneral.h>
#   include	"docDraw.h"
#   include	"docPsPrintImpl.h"
#   include	<docTabStop.h>
#   include	<docTextLine.h>

#   include	<appDebugon.h>

static int psPrintDrawTab(	DrawingContext *	dc,
				const DrawTextLine *	dtl,
				PrintingState *		ps,
				const TextAttribute *	ta,
				int			x0,
				int			x1,
				int			baseLine,
				int			step,
				const char *		tabProc )
    {
    x0= step* ( ( x0+ step- 1 )/ step );
    if  ( x1 <= x0 )
	{ return 0;	}

    docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

    sioOutPrintf( ps->psSos, "%d %d %d %s\n", x1- x0, x0, baseLine, tabProc );

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

int docPsPrintTab(	const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				leader,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseline )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;

    const TextLine *		tl= dtl->dtlTextLine;
    int				lineHeight= tl->tlDescY1- tl->tlAscY0;

    int				x0= x0Twips+ lineHeight/ 4;
    int				x1= x1Twips- lineHeight/ 2;

    int				spanBaseline= baseline->lpPageYTwips;

    switch( leader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( ta->taFontIsBold )
		{
		psPrintDrawTab( dc, dtl, ps, ta, x0, x1, spanBaseline,
							60, "dot-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, dtl, ps, ta, x0, x1, spanBaseline,
							60, "dot-tab" );
		}

	    break;

	case DOCtlUNDERLINE:

	    if  ( ta->taFontIsBold )
		{
		psPrintDrawTab( dc, dtl, ps, ta, x0, x1, spanBaseline,
							20, "ul-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, dtl, ps, ta, x0, x1, spanBaseline,
							20, "ul-tab" );
		}

	    break;

	case DOCtlHYPH:

	    if  ( ta->taFontIsBold )
		{
		psPrintDrawTab( dc, dtl, ps, ta, x0, x1, spanBaseline,
							140, "dash-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, dtl, ps, ta, x0, x1, spanBaseline,
							140, "dash-tab" );
		}

	    break;

	case DOCtlTHICK:
	    LDEB(leader);
	    break;

	case DOCtlEQUAL:
	    LDEB(leader);
	    break;

	default:
	    LDEB(leader);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save procedures to use implement tab leades in PostScript.		*/
/*									*/
/************************************************************************/

static const char *	DOC_PS_dot_tab[]=
    {
    "/dot-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth [ 1 59 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dot_tab_bold[]=
    {
    "/dot-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth [ 1 59 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dash_tab[]=
    {
    "/dash-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth [ 40 100 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dash_tab_bold[]=
    {
    "/dash-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth [ 40 100 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_ul_tab[]=
    {
    "/ul-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_ul_tab_bold[]=
    {
    "/ul-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

void docPsSaveTabLeaderProcedures(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, DOC_PS_dot_tab,
				sizeof(DOC_PS_dot_tab)/sizeof(char *) );

    psDefineProcedure( sos, DOC_PS_dot_tab_bold,
				sizeof(DOC_PS_dot_tab_bold)/sizeof(char *) );

    psDefineProcedure( sos, DOC_PS_dash_tab,
				sizeof(DOC_PS_dash_tab)/sizeof(char *) );

    psDefineProcedure( sos, DOC_PS_dash_tab_bold,
				sizeof(DOC_PS_dash_tab_bold)/sizeof(char *) );

    psDefineProcedure( sos, DOC_PS_ul_tab,
				sizeof(DOC_PS_ul_tab)/sizeof(char *) );

    psDefineProcedure( sos, DOC_PS_ul_tab_bold,
				sizeof(DOC_PS_ul_tab_bold)/sizeof(char *) );

    return;
    }

