/************************************************************************/
/*									*/
/*  Converting text encodings while reading/writing RTF.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TEXT_CONVERTER_H
#   define	DOC_RTF_TEXT_CONVERTER_H

typedef struct EncodedFont
    {
    int		ecFileFontNumber;
    int		ecBufFontNumber;
    int		ecCharset;
    int		ecCharsetIdx;
    int		ecCodepage;
    } EncodedFont;

extern const char DOC_RTF_AnsiCharsetName[];	/*  Ansi= CP1252	*/

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRtfInitEncodedFont(	EncodedFont *		ef );

#    endif	/*  DOC_RTF_TEXT_CONVERTER_H	*/
