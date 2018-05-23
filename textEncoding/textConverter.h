/************************************************************************/
/*									*/
/*  Convert text from/to UTF-8. (using iconv)				*/
/*									*/
/************************************************************************/

#   ifndef	TEXT_CONVERTER_H
#   define	TEXT_CONVERTER_H

struct TextConverter;

/************************************************************************/
/*									*/
/*  Convert bytes.. Return the size of the result.			*/
/*									*/
/************************************************************************/

/**
 *
 */
typedef int (*TextConverterProduce)(	void *			through,
					int			produced,
					const char *		bytes,
					int			count );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void textConverterSetNativeEncodingName(
					struct TextConverter *	tc,
					const char *		encodingName );

extern void textInitTextConverter(	struct TextConverter *	tc );
extern void textCleanTextConverter(	struct TextConverter *	tc );

extern int textConverterConvertToUtf8(	struct TextConverter *	tc,
					void *			through,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len );

extern int textConverterConvertFromUtf8( struct TextConverter *	tc,
					void *			through,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len );

extern void textConverterSetProduce(	struct TextConverter *	tc,
					TextConverterProduce	produce );

#   endif
