/************************************************************************/
/*									*/
/*  Writing RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_WRITER_H
#   define		RTF_WRITER_H

#   include	<sioGeneral.h>
#   include	<docBuf.h>

struct RtfWriter;
typedef struct RtfWriter RtfWriter;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern RtfWriter *	docRtfOpenWriter(	SimpleOutputStream *	sos,
						BufferDocument *	bd,
						int			flags );

extern void docRtfCloseWriter(	RtfWriter *	rw );

#   endif	/*	RTF_WRITER_H	*/
