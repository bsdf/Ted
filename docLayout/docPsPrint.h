/************************************************************************/
/*									*/
/*  PostScript printing.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_PS_PRINT_H
#   define		DOC_PS_PRINT_H

#   include	<sioGeneral.h>
#   include	"layoutContext.h"
#   include	<docBuf.h>

struct PrintGeometry;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docPsPrintDocument(
			SimpleOutputStream *		sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const MemoryBuffer *		fontDirectory,
			double				shadingMesh,
			int				emitOutline,
			const LayoutContext *		lc,
			const struct PrintGeometry *	pg );

#   endif	/*	DOC_PS_PRINT_H	*/
