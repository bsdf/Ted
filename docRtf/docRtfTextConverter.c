/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<textOfficeCharset.h>
#   include	"docRtfTextConverter.h"

#   include	<appDebugon.h>

/************************************************************************/

const char DOC_RTF_AnsiCharsetName[]= "CP1252";

/************************************************************************/

void docRtfInitEncodedFont(	EncodedFont *	ef )
    {
    ef->ecFileFontNumber= -1;
    ef->ecBufFontNumber= -1;
    ef->ecCharset= FONTcharsetANSI;
    ef->ecCharsetIdx= -1;
    ef->ecCodepage= -1;
    }

