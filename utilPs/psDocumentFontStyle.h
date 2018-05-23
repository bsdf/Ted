/************************************************************************/
/*									*/
/*  Styles of fonts as userd on MS-Windows.				*/
/*									*/
/************************************************************************/

#   ifndef	PS_DOCUMENT_FONT_STYLE_H
#   define	PS_DOCUMENT_FONT_STYLE_H

typedef enum DocumentFontStyle
    {
    DFstyleFNIL= 0,
    DFstyleFROMAN,
    DFstyleFSWISS,
    DFstyleFMODERN,
    DFstyleFSCRIPT,
    DFstyleFDECOR,
    DFstyleFTECH,
    DFstyleFBIDI,

    DFstyle_COUNT
    } DocumentFontStyle;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int psFontFamilyStyle( const char * fontFamilyName );

#   endif	/*  PS_DOCUMENT_FONT_STYLE_H	*/
