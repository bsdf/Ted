#   ifndef	UTIL_RANGES_H
#   define	UTIL_RANGES_H

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int utilParseRanges(	unsigned char *		pages,
				int			count,
				const char *		str );

extern int utilParseRange(	int *			pPage0,
				int *			pPage1,
				const char *		str );

extern int utilRangesString(	char *			str,
				int			maxSize,
				const unsigned char *	pages,
				int			size );

extern int utilRangesGetFirst(	const unsigned char *		pages,
				int				count );

extern int utilRangesGetLast(	const unsigned char *		pages,
				int				count );

extern int utilRangesCountOn(	const unsigned char *		pages,
				int				count );

#   endif	/*  UTIL_RANGES_H  */
