/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac To PostScript.				*/
/*									*/
/************************************************************************/

#   include	<psPrint.h>
#   include	"drawMetafile.h"

#   ifndef		DRAW_METAFILE_PS_H
#   define		DRAW_METAFILE_PS_H

typedef int (*MetafileWritePs)(	PrintingState *			ps,
				const MetafilePlayer *		player );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfPs(	PrintingState *			ps,
				const MetafilePlayer *		player );

extern int appMacPictPlayFilePs( PrintingState *		ps,
				const MetafilePlayer *		player );

extern int appMetaPlayEmfPs(	PrintingState *			ps,
				const MetafilePlayer *		player );

extern int appMetaDefineProcsetPs(	SimpleOutputStream *	sos );

#   endif	/*	DRAW_METAFILE_PS_H	*/
