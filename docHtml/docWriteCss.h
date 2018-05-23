/************************************************************************/
/*									*/
/*  Save attributes as CSS.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_WRITE_CSS_H
#   define		DOC_WRITE_CSS_H

#   include		<docBuf.h>
#   include		<sioGeneral.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docCssEmitBackgroundStyle(
				int *				pCol,
				SimpleOutputStream *		sos,
				const BufferDocument *		bd,
				const ItemShading *		is );

extern int docCssUseBackgroundStyle(	const ItemShading *		is,
					const BufferDocument *		bd );

extern void docCssEmitBorderStyleByNumber(
			    int *			pCol,
			    SimpleOutputStream *	sos,
			    const BufferDocument *	bd,
			    const char *		whatBorder,
			    int				num );

extern int docCssSaveTextAttributeStyles(
					SimpleOutputStream *	sos,
					const IndexSet *	used,
					const BufferDocument *	bd );

#   endif	/*	DOC_WRITE_CSS_H	*/

