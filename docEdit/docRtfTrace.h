/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TRACE_H
#   define	DOC_RTF_TRACE_H

#   include	<stdio.h>

#   include	<docBuf.h>
#   include	<docNoteProperties.h>
#   include	"docEditOperation.h"
#   include	"docEditStep.h"
#   include	"docEditTrace.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfTraceOldContents(
				DocumentSelection *		ds,
				const EditOperation *		eo,
				int				level,
				unsigned int			flags );

extern int docRtfTraceOldContentsLow(
				const EditOperation *		eo,
				const DocumentSelection *	ds,
				unsigned int			flags );

extern int docRtfTraceOldProperties(
				DocumentSelection *		ds,
				EditOperation *			eo,
				int				level,
				const PropertyMask *		taSetMask,
				const PropertyMask *		ppSetMask,
				const PropertyMask *		cpSetMask,
				const PropertyMask *		rpSetMask,
				const PropertyMask *		spSetMask,
				const PropertyMask *		dpSetMask );

extern int docRtfTraceNewProperties(
				EditOperation *			eo,

				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,

				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,

				const PropertyMask *		cpSetMask,
				const CellProperties *		cpSet,

				const PropertyMask *		rpSetMask,
				const RowProperties *		rpSet,

				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet,

				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet );

extern int docRtfTraceHeaderFooter(
				EditOperation *			eo,
				const DocumentTree *		dt );

extern int docRtfTraceNewContents(
				EditOperation *			eo,
				int				posWhere );

extern int docRtfTraceNewPosition(
				EditOperation *			eo,
				const SelectionScope *		ssNew,
				int				posWhere );

extern int docTraceStartStep(	EditOperation *			eo,
				EditTrace *			et,
				int				command,
				int				fieldKind );

extern int docTraceStartReplace( DocumentSelection *		dsTraced,
				EditOperation *			eo,
				EditTrace *			et,
				int				command,
				int				level,
				unsigned int			flags );

extern int docTraceExtendReplace( EditOperation *		eo,
				EditTrace *			et,
				int				command,
				int				level,
				unsigned int			flags );

extern int docRtfTraceFieldKind(	EditOperation *		eo,
					int			kind );

extern int docRtfTraceOldField(	EditOperation *			eo,
				const DocumentField *		df );
extern int docRtfTraceNewField(	EditOperation *			eo,
				const DocumentField *		df );

extern int docEditReadTraceStep( EditStep *			es,
				int *				pIsRepeat,
				int				direction,
				const EditTrace *		et,
				const BufferDocument *		bd );

extern int docRtfTraceCloseTrace( EditOperation *		eo,
				EditTrace *			et );

extern int docRtfTraceOldImageProperties(
				EditOperation *			eo,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet );

extern int docRtfTraceNewImageProperties(
				EditOperation *			eo,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet );

extern int docRtfTraceOldNoteProperties(
				EditOperation *			eo,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet );

extern int docRtfTraceNewNoteProperties(
				EditOperation *			eo,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet );

extern int docRtfTraceOldList(	EditOperation *			eo,
				const DocumentList *		dl );

extern int docRtfTraceNewList(	EditOperation *			eo,
				const DocumentList *		dl );

extern int docRtfTraceVersion(	int				command,
				const MemoryBuffer *		filename,
				const char *			digest64,
				const struct tm *		revtim,
				EditTrace *			et );

#    endif	/*  DOC_RTF_TRACE_H	*/
