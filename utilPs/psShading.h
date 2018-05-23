/************************************************************************/
/*									*/
/*  Item Shading.							*/
/*									*/
/************************************************************************/

#   ifndef	PS_SHADING_H
#   define	PS_SHADING_H

typedef enum ShadingPattern
    {
    DOCspSOLID= 0,

    PSshdHORIZ,
    PSshdVERT,
    PSshdFDIAG,
    PSshdBDIAG,
    PSshdCROSS,
    PSshdDCROSS,
    PSshdDKHORIZ,
    PSshdDKVERT,
    PSshdDKFDIAG,
    PSshdDKBDIAG,
    PSshdDKCROSS,
    PSshdDKDCROSS,

    PSshd_COUNT
    } ShadingPattern;

extern const char * const PsShadingNames[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  PS_SHADING_H  */
