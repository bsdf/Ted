/************************************************************************/
/*									*/
/*  Convert text from/to UTF-8 using iconv.				*/
/*									*/
/************************************************************************/

#   ifndef	TEXT_CONVERTER_IMPL_H
#   define	TEXT_CONVERTER_IMPL_H

#   include	"textConverter.h"

struct TextConverterImpl; /*  read iconv_t */

typedef struct TextConverter
    {
    char *			tcNativeEncodingName;
    struct TextConverterImpl *	tcIconvToUtf8;
    struct TextConverterImpl *	tcIconvFrUtf8;

    TextConverterProduce	tcProduce;
    } TextConverter;

#   endif
