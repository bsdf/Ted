/************************************************************************/
/*									*/
/*  Text Drawing primitives for X11.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   ifndef		DRAW_TEXT_IMPL_X11_H
#   define		DRAW_TEXT_IMPL_X11_H

#   ifdef USE_X11_FONTS

typedef struct TextProgress
    {
    int			tpX;
    int			tpY;
    } TextProgress;

typedef void (*HandleText8)(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const char *		s,
				int			l );

typedef void (*HandleText16)(	TextProgress *		tp,
				DrawScreenFont *	dsf,
				APP_FONT *		xfs,
				const XChar2b *		ws,
				int			wl );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void drawTextSegment8X11(	TextProgress *		tp,
					DrawScreenFont *	dsf,
					APP_FONT *		xfs,
					const char *		s,
					int			l );

extern void drawTextSegment16X11(	TextProgress *		tp,
					DrawScreenFont *	dsf,
					APP_FONT *		xfs,
					const XChar2b *		ws,
					int			wl );

extern void drawTextSegment8LengthX11(	TextProgress *		tp,
					DrawScreenFont *	dsf,
					APP_FONT *		xfs,
					const char *		s,
					int			l );

extern void drawTextSegment16LengthX11(	TextProgress *		tp,
					DrawScreenFont *	dsf,
					APP_FONT *		xfs,
					const XChar2b *		ws,
					int			wl );

extern void drawHandleTextSegmentsX11( TextProgress *		tp,
				DrawScreenFont *		dsf,
				const char *			s,
				int				len,
				HandleText8			handleText8,
				HandleText16			handleText16,
				const EncodedScreenFontList *	esfl );

extern void drawVerticalTextExtentsX11(
				DocumentRectangle *		drText,
				int				y,
				const EncodedScreenFontList *	esfl );
#   endif

#   endif	/*	DRAW_TEXT_IMPL_X11_H	*/
