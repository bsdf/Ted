#   include	"drawMetaConfig.h"

#   include	<stddef.h>

#   include	"drawMetafilePsList.h"
#   include	"drawWinMetaImpl.h"
#   include	<utilMatchFont.h>
#   include	<bmWmfIo.h>
#   include	<bmEmfIo.h>
#   include	"drawMacPictImpl.h"
#   include	<sioGeneral.h>

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


typedef struct ListMetaFonts
    {
    PostScriptTypeList *	lmfPstl;
    const char *		lmfPrefix;
    } ListMetaFonts;

static int appDrawMetaBitmapImagePsList(
				DeviceContext *			dc,
				void *				through,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest )
    { return 0;	}

static int appMetaSelectPenObjectPsList(DeviceContext *		dc,
					void *			through,
					LogicalPen *		lp )
    { return 0;	}

static int appCleanObjectPsList(	DeviceContext *		dc,
					void *			through,
					MetaFileObject *	mfo )
    { return 0;	}

static int appMetaSelectBrushObjectPsList(DeviceContext *		dc,
					void *			through,
					LogicalBrush *		lb )
    { return 0;	}

static int appMetaDrawRoundedRectanglePsList(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				wide,
				int				high,
				int				fillInside,
				int				drawBorder )
    { return 0;	}

static int appMeta_PolyPolygonPsList(DeviceContext *	dc,
				void *			through,
				int			polyCount,
				const int *		pointCounts,
				const Point2DI *	points,
				int			fillInsides,
				int			drawBorders,
				int			closePath )
    { return 0;	}

static int appMeta_PolyBezierPsList(DeviceContext *	dc,
				void *			through,
				int			count,
				const Point2DI *	points,
				int			startPath,
				int			fillInsides,
				int			drawBorders,
				int			closePath )
    { return 0;	}

static int appMetaDrawArcPsList(DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    { return 0;	}

static int appMetaSelectFontObjectPsList( DeviceContext *	dc,
					void *			through,
					LogicalFont *		lf )
    {
    const MetafilePlayer *	player= dc->dcPlayer;
    const ListMetaFonts *	lmf= (const ListMetaFonts *)through;
    DocumentFontList *		dfl= &(dc->dcFontList);
    const PostScriptFontList *	psfl= player->mpPostScriptFontList;
    const int			appearsInText= 0;

    const IndexSet *		unicodesWanted;

    dc->dcAfi= (*player->mpGetFontForAttribute)( &unicodesWanted,
					&(lf->lfTextAttribute), dfl, psfl );
    if  ( ! dc->dcAfi )
	{ XDEB(dc->dcAfi); return -1;	}

    if  ( psRememberPostsciptFace( lmf->lmfPstl, dc->dcAfi,
		    &(lf->lfTextAttribute), lmf->lmfPrefix, appearsInText ) )
	{ SDEB(dc->dcAfi->afiFontName); return -1;	}

    return 0;
    }

static int appMetaSelectPatternBrushObjectPsList( DeviceContext *	dc,
						void *			through,
						PatternBrush *		pb )
    { return 0;	}

static int appMetaDrawRectanglePsList(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder )
    { return 0;	}

static int appMetaDrawStringPsList(	DeviceContext *		dc,
					void *			through,
					int			x0,
					int			y0,
					const MemoryBuffer *	text )
    { return 0;	}

static int appMetaPatBltPsList(	DeviceContext *			dc,
				void *				through,
				long				rop,
				const DocumentRectangle *	dr )
    { return 0;	}

static int appMetaIgnoreColorPsList(	DeviceContext *		dc,
					void *			through,
					int			r,
					int			g,
					int			b )
    { return 0;	}

static int appMetaSetPolyFillModePsList( DeviceContext *	dc,
					void *			through,
					int			mode )
    { return 0;	}

static int appMetaStartPsList(	DeviceContext *			dc,
				int				objectCount,
				const MetafilePlayer *		player,
				const DocumentRectangle *	drLogical,
				int				mapMode )
    {
    if  ( appMetaInitDeviceContext( dc, player, objectCount, drLogical, mapMode,
				player->mpTwipsWide, player->mpTwipsHigh,
				player->mpTwipsWide, player->mpTwipsHigh ) )
	{ LDEB(objectCount); return -1;	}

    dc->dcCleanObject= appCleanObjectPsList;
    dc->dcDrawRasterImage= appDrawMetaBitmapImagePsList;
    dc->dcSelectPenObject= appMetaSelectPenObjectPsList;
    dc->dcSelectBrushObject= appMetaSelectBrushObjectPsList;
    dc->dcSelectFontObject= appMetaSelectFontObjectPsList;
    dc->dcSelectPatternBrushObject= appMetaSelectPatternBrushObjectPsList;
    dc->dcDrawPolyPolygon= appMeta_PolyPolygonPsList;
    dc->dcDrawPolyBezier= appMeta_PolyBezierPsList;
    dc->dcDrawString= appMetaDrawStringPsList;
    dc->dcPatBlt= appMetaPatBltPsList;
    dc->dcDrawPie= appMetaDrawArcPsList;
    dc->dcDrawArc= appMetaDrawArcPsList;
    dc->dcDrawChord= appMetaDrawArcPsList;
    dc->dcDrawEllipse= appMetaDrawRectanglePsList;
    dc->dcDrawRoundedRectangle= appMetaDrawRoundedRectanglePsList;
    dc->dcSetTextColor= appMetaIgnoreColorPsList;
    dc->dcSetBkColor= appMetaIgnoreColorPsList;
    dc->dcSetPolyFillMode= appMetaSetPolyFillModePsList;

    return 0;
    }

/************************************************************************/
/*									*/
/*  List the fonts in a windows metafile.				*/
/*									*/
/*  The geometry parameters are absolutely irrelevant, but required by	*/
/*  appMetaInitDeviceContext().						*/
/*									*/
/************************************************************************/

int appWmfListFontsPs( PostScriptTypeList *		pstl,
			const MetafilePlayer *		player,
			const char *			prefix )
    {
    int			rval= 0;

    WmfHeader		wh;
    DeviceContext	dc;

    ListMetaFonts	lmf;

    DocumentRectangle	drLogical;

    drLogical.drX0= 0;
    drLogical.drY0= 0;
    drLogical.drX1= player->mpXWinExt;
    drLogical.drY1= player->mpYWinExt;

    lmf.lmfPstl= pstl;
    lmf.lmfPrefix= prefix;

    if  ( sioInGetByte( player->mpInputStream ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( player->mpInputStream );

    if  ( bmMetaReadWmfHeader( &wh, player->mpInputStream ) )
	{ LDEB(1); return -1;		}

    if  ( appMetaStartPsList( &dc, wh.wh_objectCount, player,
					    &drLogical, player->mpMapMode ) )
	{ LDEB(wh.wh_objectCount); return -1;	}

    WMFDEB(appDebug( "PSFONTS WMF\n" ));

    if  ( appMetaPlayWmf( &dc, (void *)&lmf ) )
	{ rval= -1;	}

    appMetaCleanDeviceContext( &dc, (void *)&lmf );

    return rval;
    }

int appEmfListFontsPs(	PostScriptTypeList *		pstl,
			const MetafilePlayer *		player,
			const char *			prefix )
    {
    int			rval= 0;

    int			done;
    EmfHeader		eh;
    DeviceContext	dc;

    ListMetaFonts	lmf;

    lmf.lmfPstl= pstl;
    lmf.lmfPrefix= prefix;

    done= bmMetaReadEmfHeader( &eh, player->mpInputStream );
    if  ( done < 0 )
	{ LDEB(done); return -1;		}
    while( done < eh.eh_headerSize )
	{
	if  ( sioInGetByte( player->mpInputStream ) < 0 )
	    { LDEB(1); return -1;	}

	done++;
	}

    if  ( appMetaStartPsList( &dc, eh.eh_numOfHandles, player,
					    &(eh.ehBounds), MM_ANISOTROPIC ) )

	{ LDEB(eh.eh_numOfHandles); return -1;	}

    WMFDEB(appDebug(
	"PSFONTS EMF LOGICAL: [%d..%d x %d..%d] TWIPS: [%d x %d]\n",
				    eh.ehBounds.drX0, eh.ehBounds.drX1,
				    eh.ehBounds.drY0, eh.ehBounds.drY1,
				    player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaPlayEmf( &dc, (void *)&lmf ) )
	{ rval= -1;	}

    appMetaCleanDeviceContext( &dc, (void *)&lmf );

    return rval;
    }


int appMacPictListFontsPs(	PostScriptTypeList *		pstl,
				const MetafilePlayer *		player,
				const char *			prefix )
    {
    MacPictHeader	mph;
    int			rval= 0;

    int			done;
    DeviceContext	dc;
    const int		handleCount= MACPICThandle_COUNT;

    ListMetaFonts	lmf;

    lmf.lmfPstl= pstl;
    lmf.lmfPrefix= prefix;

    done= appMacPictGetDeviceHeader( &mph, player->mpInputStream );
    if  ( done < 0 )
	{ LDEB(done); return -1;		}

    if  ( appMetaStartPsList( &dc, handleCount, player, &(mph.mphFrame),
							    MM_ANISOTROPIC ) )
	{ LDEB(handleCount); return -1;	}

    dc.dcMacPictVersion= mph.mphVersion;

    WMFDEB(appDebug( "PSFONTS PICT ...\n" ));

    if  ( appMacPictPlayPict( &dc, (void *)&lmf ) )
	{ rval= -1;	}

    appMetaCleanDeviceContext( &dc, (void *)&lmf );

    return rval;
    }

