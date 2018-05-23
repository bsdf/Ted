#   include	"docBufConfig.h"

#   include	<stdio.h>

#   include	"docBuf.h"
#   include	"docDebug.h"
#   include	"docField.h"
#   include	<docTreeType.h>
#   include	<docObjectProperties.h>
#   include	<docTextParticule.h>
#   include	<appDebugon.h>

const char * docKindStr( int kind )
    {
    static char	scratch[12];

    switch( kind )
	{
	case DOCkindSPAN:		return "SPAN";
	case DOCkindTAB:		return "TAB ";
	case DOCkindOBJECT:		return "OBJ ";
	case DOCkindFIELDHEAD:		return "<FLD";
	case DOCkindFIELDTAIL:		return "FLD>";
	case DOCkindLINEBREAK:		return "LINE";
	case DOCkindPAGEBREAK:		return "PAGE";
	case DOCkindCOLUMNBREAK:	return "COL ";
	case DOCkindCHFTNSEP:		return "SEP ";
	case DOCkindCHFTNSEPC:		return "SEPC";
	case DOCkindOPT_HYPH:		return "HYPH";
	case DOCkindLTR_MARK:		return "LTR ";
	case DOCkindRTL_MARK:		return "RTL ";

	default:
	    sprintf( scratch, "%-4d", kind );
	    return scratch;
	}
    }

const char * docLevelStr( int level )
    {
    static char	scratch[12];

    switch( level )
	{
	case DOClevANY:		return "ANY ";
	case DOClevOUT:		return "OUT ";

	case DOClevBODY:	return "BODY";
	case DOClevSECT:	return "SECT";
	case DOClevROW:		return "ROW ";
	case DOClevCELL:	return "CELL";
	case DOClevPARA:	return "PARA";
	case DOClevSPAN:	return "SPAN";

	case DOClevCOLUMN:	return "COLM";
	case DOClevTABLE:	return "TABL";

	default:
	    sprintf( scratch, "%-4d", level );
	    return scratch;
	}
    }

const char * docTreeTypeStr( int treeType )
    {
    static char	scratch[12];

    switch( treeType )
	{
	case DOCinBODY:			return "BODY";

	case DOCinFIRST_HEADER:		return "FIRST_HEADER";
	case DOCinLEFT_HEADER:		return "LEFT_HEADER";
	case DOCinRIGHT_HEADER:		return "RIGHT_HEADER";

	case DOCinFIRST_FOOTER:		return "FIRST_FOOTER";
	case DOCinLEFT_FOOTER:		return "LEFT_FOOTER";
	case DOCinRIGHT_FOOTER:		return "RIGHT_FOOTER";

	case DOCinFOOTNOTE:		return "FOOTNOTE";
	case DOCinENDNOTE:		return "ENDNOTE";

	case DOCinFTNSEP:		return "FTNSEP";
	case DOCinFTNSEPC:		return "FTNSEPC";
	case DOCinFTNCN:		return "FTNCN";

	case DOCinAFTNSEP:		return "AFTNSEP";
	case DOCinAFTNSEPC:		return "AFTNSEPC";
	case DOCinAFTNCN:		return "AFTNCN";

	case DOCinSHPTXT:		return "SHPTXT";

	default:
	    sprintf( scratch, "%d", treeType );
	    return scratch;
	}
    }

const char * docObjectKindStr(	int	objectKind )
    {
    static char	scratch[12];

    switch( objectKind )
	{
	case DOCokUNKNOWN:		return "UNKNOWN";
	case DOCokPICTWMETAFILE:	return "PICTWMETAFILE";
	case DOCokPICTPNGBLIP:		return "PICTPNGBLIP";
	case DOCokPICTJPEGBLIP:		return "PICTJPEGBLIP";
	case DOCokPICTEMFBLIP:		return "PICTEMFBLIP";
	case DOCokMACPICT:		return "MACPICT";
	case DOCokPMMETAFILE:		return "PMMETAFILE";
	case DOCokDIBITMAP:		return "DIBITMAP";
	case DOCokWBITMAP:		return "WBITMAP";
	case DOCokOLEOBJECT:		return "OLEOBJECT";
	case DOCokEPS_FILE:		return "EPS_FILE";
	case DOCokDRAWING_SHAPE:	return "DRAWING_SHAPE";

	default:
	    sprintf( scratch, "%d", objectKind );
	    return scratch;
	}
    }

const char * docFieldKindStr( int kind )
    {
    static char	scratch[12];

    if  ( kind >= 0 && kind < DOC_FieldKindCount )
	{ return DOC_FieldKinds[kind].fkiLabel;	}

    sprintf( scratch, "%4d", kind );
    return scratch;
    }

const char * docAttributeStr(	const TextAttribute *		ta )
    {
    static char	scratch[5];
    char *	to= scratch;

    if  ( ta->taFontIsBold )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}
    if  ( ta->taFontIsSlanted )
	{ *(to++)= 'I';	}
    else{ *(to++)= 'i';	}
    if  ( ta->taTextIsUnderlined )
	{ *(to++)= 'U';	}
    else{ *(to++)= 'u';	}

    *(to)= '\0';
    return scratch;
    }

const char * docParticuleFlagsStr(	int	flags )
    {
    static char	scratch[7];
    char *	to= scratch;

    if  ( flags & TPflagXMATCH_LEFT )
	{ *(to++)= '[';	}
    else{ *(to++)= '.';	}

    if  ( flags & TPflagLEFT_BORDER )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}

    if  ( flags & TPflagRIGHT_BORDER )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}

    if  ( flags & TPflagXMATCH_RIGHT )
	{ *(to++)= ']';	}
    else{ *(to++)= '.';	}

    *(to)= '\0';
    return scratch;
    }

void docLogRectangle(	const char *			label,
			const DocumentRectangle *	dr )
    {
    appDebug( "%s: [%4d+%4d=%4d]x[%4d+%4d=%4d]\n", label,
		dr->drX0, dr->drX1- dr->drX0+ 1, dr->drX1,
		dr->drY0, dr->drY1- dr->drY0+ 1, dr->drY1 );

    return;
    }

void docLogRectangles(	const char *			label1,
			const DocumentRectangle *	dr1,
			const char *			label2,
			const DocumentRectangle *	dr2 )
    {
    appDebug( "%s: [%4d+%4d]x[%4d+%4d] %s [%4d+%4d]x[%4d+%4d]\n",
		label1,
		dr1->drX0, dr1->drX1- dr1->drX0+ 1,
		dr1->drY0, dr1->drY1- dr1->drY0+ 1,
		label2,
		dr2->drX0, dr2->drX1- dr2->drX0+ 1,
		dr2->drY0, dr2->drY1- dr2->drY0+ 1 );

    return;
    }

const char * docBreakKindStr( int kind )
    {
    static char scratch[20];

    switch( kind )
	{
	case DOCibkNONE:	return "NONE";
	case DOCibkCOL:		return "COL";
	case DOCibkPAGE:	return "PAGE";
	case DOCibkEVEN:	return "EVEN";
	case DOCibkODD:		return "ODD";

	default:
	    sprintf( scratch, "%d", kind );
	    return scratch;
	}
    }
