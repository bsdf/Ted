/************************************************************************/
/*									*/
/*  The various kinds of trees in a document.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TREE_TYPE_H
#   define	DOC_TREE_TYPE_H

typedef enum TreeType
    {
    DOCinUNKNOWN= 0,

    DOCinBODY,

    /*  As of Word 11, the right (odd) header is used without facing pages.
    DOCinSECT_HEADER, */
    DOCinFIRST_HEADER,
    DOCinLEFT_HEADER,
    DOCinRIGHT_HEADER,

    /*  As of Word 11, the right (odd) footer is used without facing pages.
    DOCinSECT_FOOTER, */
    DOCinFIRST_FOOTER,
    DOCinLEFT_FOOTER,
    DOCinRIGHT_FOOTER,

    DOCinFOOTNOTE,
    DOCinENDNOTE,
    DOCinANNOTATION,

    DOCinFTNSEP,
    DOCinFTNSEPC,
    DOCinFTNCN,

    DOCinAFTNSEP,
    DOCinAFTNSEPC,
    DOCinAFTNCN,

    DOCinSHPTXT,

    DOCin_COUNT
    } TreeType;

extern const int DOC_HeaderFooterTypes[];
extern const int DOC_HeaderFooterTypeCount;

# define docIsHeaderType( t )	( (t) == DOCinFIRST_HEADER || \
				  (t) == DOCinLEFT_HEADER || \
				  (t) == DOCinRIGHT_HEADER )

# define docIsFooterType( t )	( (t) == DOCinFIRST_FOOTER || \
				  (t) == DOCinLEFT_FOOTER || \
				  (t) == DOCinRIGHT_FOOTER )

# define docIsSeparatorType( t )( (t) == DOCinFTNSEP || \
				  (t) == DOCinFTNSEPC || \
				  (t) == DOCinFTNCN || \
				  (t) == DOCinAFTNSEP || \
				  (t) == DOCinAFTNSEPC || \
				  (t) == DOCinAFTNCN )

# define docTreeInColumnType( t ) \
		    ( (t) == DOCinBODY		|| \
		      (t) == DOCinFOOTNOTE	|| \
		      (t) == DOCinENDNOTE	|| \
		      docIsSeparatorType( t )	)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif
