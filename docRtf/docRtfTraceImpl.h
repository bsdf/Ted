/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TRACE_IMPL_H
#   define	DOC_RTF_TRACE_IMPL_H

typedef enum TraceEditRangeProperty
    {
    TRACEposOLD_HEAD_COL= 0,
    TRACEposOLD_HEAD_PARA,
    TRACEposOLD_HEAD_STROFF,

    TRACEposOLD_TAIL_COL,
    TRACEposOLD_TAIL_PARA,
    TRACEposOLD_TAIL_STROFF,

    TRACEposNEW_HEAD_COL,
    TRACEposNEW_HEAD_PARA,
    TRACEposNEW_HEAD_STROFF,

    TRACEposNEW_TAIL_COL,
    TRACEposNEW_TAIL_PARA,
    TRACEposNEW_TAIL_STROFF,

    TRACEposSELECTED,

    TRACEposFIELD_KIND,

    TRACEposPROP_LEVEL,

    DOCtrace_COUNT
    } TraceEditRangeProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char *	docRtfGetTraceTag(	int	command );

#    endif	/*  DOC_RTF_TRACE_IMPL_H	*/
