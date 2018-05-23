/************************************************************************/
/*									*/
/*  SVG printing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_SVG_DRAW_H
#   define		DOC_SVG_DRAW_H

#   include	<sioGeneral.h>
#   include	<layoutContext.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSvgDrawDocument(
			SimpleOutputStream *		sos,
			const char *			applicationName,
			const char *			applicationReference,
			const LayoutContext *		lc );

#   endif	/*	DOC_SVG_DRAW_H	*/
