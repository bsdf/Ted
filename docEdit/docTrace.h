/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TRACE_H
#   define	DOC_TRACE_H

#   include	<docBuf.h>

#   include	"docEditOperation.h"
#   include	"docEditStep.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docReinsertNodes(	EditOperation *			eo,
				struct BufferItem *		parentTo,
				const struct BufferItem *	parentFrom,
				int				to,
				const EditStep *		es );

extern const DocumentField * docTraceGetFromField(
						const EditStep * 	es );

extern const PictureProperties * docTraceGetFromPictureProps(
						const EditStep * 	es );

#    endif	/*  DOC_TRACE_H	*/
