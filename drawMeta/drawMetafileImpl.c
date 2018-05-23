#   include	"drawMetaConfig.h"

#   include	<stddef.h>

#   include	"drawMetafileImpl.h"

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

# if 0
#    define	WMFLOG(x)	(x)
# else
#    define	WMFLOG(x)	/*nothing*/
# endif

int appMetaSelectPenObjectImpl(		DeviceContext *		dc,
					void *			through,
					LogicalPen *		lp )
    {
    /*PrintingState *		ps= (PrintingState *)through;*/

    WMFDEB(appDebug( "SelectObject() PEN Width= %d ", lp->lpWidth ));

    WMFLOG(sioOutPrintf( sos, "%% SelectObject() PEN Width= %d ",
							lp->lpWidth ));

    switch( lp->lpStyle & 0xff )
	{
	case PS_SOLID:
	    WMFDEB(appDebug( "SOLID\n" ));
	    WMFLOG(sioOutPrintf( sos, "SOLID\n" ));
	  solid:
	    dc->dcDrawBorders= 1;
	    break;

	case PS_INSIDEFRAME:
	    WMFDEB(appDebug( "INSIDEFRAME\n" ));
	    WMFLOG(sioOutPrintf( sos, "INSIDEFRAME\n" ));

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DASH:
	    WMFDEB(appDebug( "DASH\n" ));
	    WMFLOG(sioOutPrintf( sos, "DASH\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DOT:
	    WMFDEB(appDebug( "DOT\n" ));
	    WMFLOG(sioOutPrintf( sos, "DOT\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DASHDOT:
	    WMFDEB(appDebug( "DASHDOT\n" ));
	    WMFLOG(sioOutPrintf( sos, "DASHDOT\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DASHDOTDOT:
	    WMFDEB(appDebug( "DASHDOTDOT\n" ));
	    WMFLOG(sioOutPrintf( sos, "DASHDOTDOT\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_NULL:
	    WMFDEB(appDebug( "NULL\n" ));
	    WMFLOG(sioOutPrintf( sos, "NULL\n" ));
	    dc->dcDrawBorders= 0;
	    break;

	default:
	    WMFDEB(appDebug( "style= 0x%x\n", lp->lpStyle ));
	    WMFLOG(sioOutPrintf( sos, "style= 0x%x\n", lp->lpStyle ));
	    dc->dcDrawBorders= 0;
	    break;
	}

    dc->dcPen= *lp;

    return 0;
    }

int appMetaSelectBrushObjectImpl( 	DeviceContext *		dc,
					void *			through,
					LogicalBrush *		lb )
    {
    WMFDEB(appDebug( "SelectObject() " ));
    WMFDEB(appDebug( "BRUSH\n" ));

    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    dc->dcFillInsides= 1;
	    dc->dcFillHatched= 0;
	    dc->dcFillPattern= 0;
	    break;

	case BS_HOLLOW:
	    dc->dcFillInsides= 0;
	    dc->dcFillHatched= 0;
	    dc->dcFillPattern= 0;
	    break;

	case BS_HATCHED:
	    dc->dcFillInsides= 1;
	    dc->dcFillHatched= 1;
	    dc->dcFillPattern= 0;
	    break;

	case BS_PATTERN:
	default:
	    dc->dcFillInsides= 1;
	    dc->dcFillHatched= 0;
	    dc->dcFillPattern= 0;
	    LDEB(lb->lbStyle);
	    break;
	}

    dc->dcBrush= *lb;

    return 0;
    }

int appMetaSelectFontObjectImpl(	DeviceContext *		dc,
					void *			through,
					LogicalFont *		lf )
    {
    DocumentFontList *		dfl= &(dc->dcFontList);
    const MetafilePlayer *	player= dc->dcPlayer;
    const IndexSet *		unicodesWanted;

    dc->dcAfi= (*player->mpGetFontForAttribute)( &unicodesWanted,
		&(lf->lfTextAttribute), dfl, player->mpPostScriptFontList );
    if  ( ! dc->dcAfi )
	{ XDEB(dc->dcAfi); return -1;	}

    return 0;
    }

int appMetaGetPenWidth(		DeviceContext *		dc,
				const LogicalPen *	lp )
    {
    int		widthTwips= appWinMetaOutputSize( dc, lp->lpWidth );

    if  ( widthTwips < 10 )
	{ widthTwips = 10;	}

    switch( lp->lpStyle & 0xff )
	{
	case PS_SOLID:
	    break;

	case PS_INSIDEFRAME:
	    break;

	case PS_DASH:
	    if  ( lp->lpWidth > 1 )
		{ break;;	}

	    widthTwips= 10;
	    break;

	case PS_DOT:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    widthTwips= 10;
	    break;

	case PS_DASHDOT:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    widthTwips= 10;
	    break;

	case PS_DASHDOTDOT:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    widthTwips= 10;
	    break;

	case PS_NULL:
	    widthTwips= 0;
	    break;

	default:
	    WMFDEB(appDebug( "style= 0x%x\n", lp->lpStyle ));
	    break;
	}

    return widthTwips;
    }

int appMetaGetPenDots(		DeviceContext *		dc,
				unsigned char		dots[6],
				const LogicalPen *	lp )
    {
    int				outputWidth;
    int				dash;
    int				betdash;
    int				dot;
    int				betdot;
    int				dotCount= 0;

    outputWidth= appWinMetaOutputSize( dc, lp->lpWidth );
    dot= 2* outputWidth;
    if  ( dot < 20 )
	{ dot= 20;	}
    dash= 6* dot;
    betdash= 4* dot;
    betdot= 2* dot;

    switch( lp->lpStyle & 0xff )
	{
	case PS_SOLID:
	    break;
	case PS_INSIDEFRAME:
	    break;

	case PS_DASH:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    dots[dotCount++]= dash;
	    dots[dotCount++]= betdash;
	    break;

	case PS_DOT:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    dots[dotCount++]= dot;
	    dots[dotCount++]= betdot;
	    break;

	case PS_DASHDOT:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    dots[dotCount++]= dash;
	    dots[dotCount++]= betdot;
	    dots[dotCount++]= dot;
	    dots[dotCount++]= betdot;
	    break;

	case PS_DASHDOTDOT:
	    if  ( lp->lpWidth > 1 )
		{ break;	}

	    dots[dotCount++]= dash;
	    dots[dotCount++]= betdot;
	    dots[dotCount++]= dot;
	    dots[dotCount++]= betdot;
	    dots[dotCount++]= dot;
	    dots[dotCount++]= betdot;
	    break;

	case PS_NULL:
	    break;

	default:
	    WMFDEB(appDebug( "style= 0x%x\n", lp->lpStyle ));
	    break;
	}

    return dotCount;
    }

