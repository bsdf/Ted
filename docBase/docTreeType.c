/************************************************************************/
/*									*/
/*  The various kinds of headers and footers in a document.		*/
/*									*/
/************************************************************************/

#   include "docTreeType.h"

const int DOC_HeaderFooterTypes[]=
    {
    DOCinFIRST_HEADER,		DOCinFIRST_FOOTER,
    DOCinLEFT_HEADER,		DOCinLEFT_FOOTER,
    DOCinRIGHT_HEADER,		DOCinRIGHT_FOOTER,
    };

const int DOC_HeaderFooterTypeCount= sizeof(DOC_HeaderFooterTypes)/sizeof(int);
