/************************************************************************/
/*									*/
/*  Text Editor Buffer structure of the string buffer in a paragraph.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_STRING_H
#   define	DOC_PARA_STRING_H

#   include	"docBuf.h"
#   include	"docTreeNode.h"

struct TextConverter;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docParaAppendText(	BufferDocument *	bd,
				BufferItem *		paraBi,
				const TextAttribute *	ta,
				struct TextConverter *	tc,
				const char *		text,
				int			len );

extern int docParaDivideAppendedText(
				BufferItem *		paraBi,
				int			textAttributeNumber,
				int			stroff,
				int			upto );

extern int docRedivideStringInParticules(
				BufferItem *		bi,
				int			strOff,
				int			strLen,
				int			part,
				int			partsFree,
				int			textAttributeNumber );

extern int docParaStringReplace(	int *			pSizeShift,
					BufferItem *		bi,
					int			stroffBegin,
					int			stroffTail,
					const char *		addedString,
					int			addedStrlen );

extern int docParaFixStroff(	const BufferItem *	paraBi,
				int			stroff );

extern int docParaNextStroff(	const BufferItem *	paraBi,
				int			stroff );

extern int docParaPrevStroff(	const BufferItem *	paraBi,
				int			stroff );

extern int docParaFirstStroff(	const BufferItem *	paraBi );

extern int docParaLastStroff(	const BufferItem *	paraBi );

extern void docParaSetupTextConverter(	struct TextConverter *	tc );

#   endif
