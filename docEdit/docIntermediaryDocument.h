/************************************************************************/
/*									*/
/*  Intermediate documents: Some pieces are shallow copies.		*/
/*									*/
/************************************************************************/

#   ifndef		DOC_ITERMEDIATE_DOC_H
#   define		DOC_ITERMEDIATE_DOC_H

#   include		<docBuf.h>
#   include		<docDebug.h>

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern BufferDocument * docIntermediaryDocument(
				struct BufferItem **		pSectBi,
				const BufferDocument *		bdFrom );

extern void docFreeIntermediaryDocument(
				BufferDocument *		bd );

#   endif	/*	DOC_ITERMEDIATE_DOC_H	*/
