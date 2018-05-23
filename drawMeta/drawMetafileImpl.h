/************************************************************************/
/*									*/
/*  Common purely administrative functionality.				*/
/*									*/
/************************************************************************/

#   include	"drawWinMetaImpl.h"

#   ifndef		DRAW_METAFILE_IMPL_H
#   define		DRAW_METAFILE_IMPL_H

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaSelectBrushObjectImpl(DeviceContext *		dc,
					void *			through,
					LogicalBrush *		lb );

extern int appMetaSelectPenObjectImpl(	DeviceContext *		dc,
					void *			through,
					LogicalPen *		lp );

extern int appMetaSelectFontObjectImpl(	DeviceContext *		dc,
					void *			through,
					LogicalFont *		lf );

extern int appMetaGetPenDots(		DeviceContext *		dc,
					unsigned char		dots[6],
					const LogicalPen *	lp );

extern int appMetaGetPenWidth(		DeviceContext *		dc,
					const LogicalPen *	lp );

#   endif	/*	DRAW_METAFILE_IMPL_H	*/
