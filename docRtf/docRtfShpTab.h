/************************************************************************/
/*									*/
/*  Handle shapes while reading RTF.					*/
/*									*/
/************************************************************************/

#   ifndef		RTF_SHP_TAB_H
#   define		RTF_SHP_TAB_H

#   include	<docRtfReaderImpl.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfShpArray(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfShpString(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfShpPicture(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfShapeNumber(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfShpColor(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

#   endif	/*	RTF_SHP_TAB_H	*/
