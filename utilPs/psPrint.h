#   ifndef	UTIL_PS_H
#   define	UTIL_PS_H

#   include	<sioGeneral.h>
#   include	<geo2DInteger.h>
#   include	<utilDocumentGeometry.h>
#   include	"psFace.h"
#   include	<geoAffineTransform.h>
#   include	"psNup.h"
#   include	<utilColor.h>
#   include	<utilMemoryBuffer.h>
#   include	"psPostScriptFontList.h"

/************************************************************************/
/*									*/
/*  The state of a printing job.					*/
/*									*/
/************************************************************************/

typedef struct PrintingState
    {
    SimpleOutputStream *	psSos;

    int				psLastPageMarked;
    int				psLastSheetMarked;
    int				psPagesPrinted;
    int				psSheetsPrinted;
    int				psSheetsStarted;

    const PostScriptFontList *	psPostScriptFontList;
    PrintGeometry		psPrintGeometry;
    NupSchema			psNupSchema;
    int				psRotateSheetGrid;

    AffineTransform2D		psCurrentTransform;
    const char *		psOrientation;

    DocumentRectangle		psSheetBoundingBox;

    int				psInsideLink;
    int				psLinkParticulesDone;
    int				psLinkRectLeft;

    MemoryBuffer		psNoteRef;
    MemoryBuffer		psNoteDef;

    MemoryBuffer		psLinkFile;
    MemoryBuffer		psLinkMark;

    unsigned char		psUsePostScriptFilters;
    unsigned char		psUsePostScriptIndexedImages;
    unsigned char		ps7Bits;
    } PrintingState;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void psStartDSCDocument(
			const PrintingState *		ps,
			const PostScriptTypeList *	psfl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference );

extern void psRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage );

extern int psSetNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter );

extern int psPrintString(	SimpleOutputStream *	sos,
				const unsigned char *	s,
				int			len,
				int			sevenBits );

extern int psPrintPdfMarkStringValue(	PrintingState *		ps,
					const unsigned char *	s,
					int			len );

extern int psPrintStringValue(		PrintingState *		ps,
					const unsigned char *	s,
					int			len );

extern int psMoveShowString(		PrintingState *		ps,
					const unsigned char *	s,
					int			len,
					int			x,
					int			y );

extern int psShowString(		PrintingState *		ps,
					const unsigned char *	s,
					int			len );

extern int psSetFont(		PrintingState *		ps,
				const AfmFontInfo *	afi,
				const TextAttribute *	ta );

extern void psInitPrintingState(	PrintingState *	ps );
extern void psCleanPrintingState(	PrintingState *	ps );

extern int psSetRgb8Color(	PrintingState *		ps,
				const RGB8Color *	rgb8 );

extern int psStartPage(		PrintingState *		ps,
				int			documentPage );

extern int psFinishPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern void psAbortPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern void psSelectFontProcedures(
				SimpleOutputStream *		sos,
				const PostScriptTypeList *	psfl,
				int				allFonts );

extern int psIncludeFonts(	SimpleOutputStream *		sos,
				const PostScriptTypeList *	psfl );

extern void psStartEpsFile(		SimpleOutputStream *	sos,
					const char *		creator,
					const char *		title,
					int			pointsWide,
					int			pointsHigh );

extern void psFinishEpsFile(	SimpleOutputStream *	sos );

extern void psSetPdfmarkEmulation(		SimpleOutputStream *	sos );
extern void psImageQualityDistillerparams(	SimpleOutputStream *	sos );
extern void psSetRectfillEmulation(		SimpleOutputStream *	sos );
extern void psSetSelectfontEmulation(		SimpleOutputStream *	sos );
extern void psSetDefinePageFontImplementation(	SimpleOutputStream *	sos );
extern void psSetUtf8ShowImplementation(	SimpleOutputStream *	sos );
extern void psSetMvsImplementation(		SimpleOutputStream *	sos );
extern void psSetPatternImplementation(	SimpleOutputStream *	sos,
					double			shadingMesh );

extern void psDefineProcedure(	SimpleOutputStream *	sos,
					const char **		lines,
					int			count );

extern void psDefineEpsProcs(	SimpleOutputStream *	sos );

extern void psBeginEpsObject(		SimpleOutputStream *		sos,
					const DocumentRectangle *	drTo,
					const DocumentRectangle *	drBBox,
					const char *			file );

extern void psEndEpsObject(		SimpleOutputStream *	sos );

extern int psDestPdfmark(		PrintingState *		ps,
					int			lineTop,
					const MemoryBuffer *	mbRef );

extern int psEmitDestPdfmark(	SimpleOutputStream *		sos,
				const AffineTransform2D *	at,
				int				lineTop,
				const MemoryBuffer *		mbRef );

extern void psTransformMatrix(		SimpleOutputStream *		sos,
					const AffineTransform2D *	at );

extern void psFillRectangle(		PrintingState *		ps,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void psRectanglePath(		PrintingState *		ps,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void psFlushLink(		PrintingState *		ps,
					int			x0,
					int			wide,
					int			lineTop,
					int			lineHeight );

extern void psSourcePdfmark(	SimpleOutputStream *		sos,
				const DocumentRectangle *	drLink,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName );

extern int psPageModePdfmark(		SimpleOutputStream *	sos,
					const char *		pageMode );

extern int psSaveEpsFile(		SimpleOutputStream *	sos,
					DocumentRectangle *	drBBox,
					const MemoryBuffer *	filename );

extern int psIncludeEpsFile(	SimpleOutputStream *	sos,
					SimpleInputStream *	sis );


extern void psDscListFontNames(	SimpleOutputStream *		sos,
				int				embedFonts,
				const PostScriptTypeList *	pstl );

extern void psSetFontName(	char *				fontName,
				const AfmFontInfo *		afi );

extern int psSetToPdfCommand(	MemoryBuffer *			command,
				int				pageTwipsWide,
				int				pageTwipsHigh,
				const MemoryBuffer *		filename );

extern int psOutlinePdfmark(	PrintingState *		ps,
				int			childCount,
				int			closed,
				const MemoryBuffer *	title,
				const MemoryBuffer *	markName );

#   endif	/*  UTIL_PS_H  */
