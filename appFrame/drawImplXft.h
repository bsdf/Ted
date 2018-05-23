#   ifndef		DRAW_IMPL_XFT_H
#   define		DRAW_IMPL_XFT_H

#   include	"drawImpl.h"
#   include	"drawScreenFontImpl.h"
#   include	"appXftColorList.h"

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

#   ifdef USE_XFT

extern int drawStringXft(		const DrawScreenFont *	dsf,
					AppXftColorList *	axcl,
					int			x,
					int			y,
					const char *		s,
					int			len );

extern int drawFillRectangleXft( XftDraw *			draw,
				AppXftColorList *		axcl,
				const DocumentRectangle *	dr );
#   endif

#   endif	/*	DRAW_IMPL_XFT_H	*/
