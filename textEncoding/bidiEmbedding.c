#   include	"textEncodingConfig.h"

#   include	<stdio.h>
#   include	"bidiEmbedding.h"

const char * bidiEmbeddingStr(	int	embedding )
    {
    static char	scratch[20];

    switch( embedding )
	{
	case BIDIembedLRE:	return "LRE";
	case BIDIembedRLE:	return "RLE";
	case BIDIembedLRO:	return "LRO";
	case BIDIembedRLO:	return "RLO";
	case BIDIembedPDF:	return "PDF";

	case BIDIembedEMBEDDING:return "EMB";
	case BIDIembedIMPLICIT: return "IMP";

	case BIDIembedREMOVED: 	return "REM";
	case BIDIembedREJECTED:	return "REJ";

	default:
	    sprintf( scratch, "%d", embedding );
	    return scratch;
	}
    }
