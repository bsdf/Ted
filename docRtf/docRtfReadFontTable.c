/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<textOfficeCharset.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"

/************************************************************************/

void docRtfRestartFont(		RtfReader *	rrc )
    {
    docCleanDocumentFont( &(rrc->rrcCurrentFont) );
    docInitDocumentFont( &(rrc->rrcCurrentFont) );
    docRtfInitEncodedFont( &(rrc->rrcCurrentEncodedFont) );
    if  ( rrc->rrDocument )
	{
	rrc->rrcCurrentEncodedFont.ecCharset=
			    rrc->rrDocument->bdProperties.dpDocumentCharset;
	}

    return ;
    }

/************************************************************************/
/*									*/
/*  Consume a font table.						*/
/*									*/
/************************************************************************/

static int docRtfCommitFaltName(	const RtfControlWord *	rcw,
					RtfReader *		rrc )
    {
    const int	removeSemicolon= 0;
    int		size= 0;

    if  ( docRtfStoreSavedText( &(rrc->rrcCurrentFont.dfAltName), &size,
						    rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    return 0;
    }

static int docRtfCommitFontPanoseText(	const RtfControlWord *	rcw,
					RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( rrs->rrsSavedText.mbSize != FONTlenPANOSE )
	{
	LLDEB(rrs->rrsSavedText.mbSize,FONTlenPANOSE);
	utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );
	return 0;
	}

    memcpy( rrc->rrcCurrentFont.dfPanose,
			rrs->rrsSavedText.mbBytes,
			rrs->rrsSavedText.mbSize );
    utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );

    rrc->rrcCurrentFont.dfPanose[FONTlenPANOSE]= '\0';

    return 0;
    }

static int docRtfAddCurrentFontToList(	RtfReader *	rrc )
    {
    int		rval= 0;
    const int	removeSemicolon= 1;
    int		size= 0;

    if  ( docRtfStoreSavedText( &(rrc->rrcCurrentFont.dfName), &size,
						    rrc, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( rrc->rrcCurrentFont.dfName && rrc->rrcCurrentFont.dfName[0] == '@' )
	{
	if  ( docFontSetFamilyName( &(rrc->rrcCurrentFont),
					    rrc->rrcCurrentFont.dfName+ 1 ) )
	    { LDEB(1); return -1;	}
	}

    if  ( rrc->rrcCurrentFont.dfName )
	{
	DocumentProperties *	dp= &(rrc->rrDocument->bdProperties);
	DocumentFontList *	dfl= dp->dpFontList;
	DocumentFont *		df;

	EncodedFont *		efIn= &(rrc->rrcCurrentEncodedFont);
	EncodedFont *		efOut;

	if  ( ! strcmp( rrc->rrcCurrentFont.dfName, "ZapfDingbats" ) )
	    {
	    if  ( docFontSetFamilyName( &(rrc->rrcCurrentFont),
							"ITC Zapf Dingbats" ) )
		{ LDEB(1); return -1;	}
	    }
	if  ( ! strcmp( rrc->rrcCurrentFont.dfName, "ZapfChancery" ) )
	    {
	    if  ( docFontSetFamilyName( &(rrc->rrcCurrentFont),
							"ITC Zapf Chancery" ) )
		{ LDEB(1); return -1;	}
	    }

	utilRemoveCharsetFromFontName( &(rrc->rrcCurrentFont),
							    efIn->ecCharset );

	efIn->ecBufFontNumber= docGetFontByName( dfl,
						rrc->rrcCurrentFont.dfName );
	if  ( efIn->ecBufFontNumber < 0 )
	    {
	    SLDEB(rrc->rrcCurrentFont.dfName,efIn->ecBufFontNumber);
	    rval= -1; goto ready;
	    }

	df= docFontListGetFontByNumber( dfl, efIn->ecBufFontNumber );
	if  ( ! df )
	    { SXDEB(rrc->rrcCurrentFont.dfName,df); rval= -1; goto ready; }

	if  ( docCopyDocumentFont( df, &(rrc->rrcCurrentFont) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	df->dfDocFontNumber= efIn->ecBufFontNumber;

	efOut= (EncodedFont *)utilPagedListClaimItem(
			&(rrc->rrcEncodedFontList), efIn->ecFileFontNumber );
	if  ( ! efOut )
	    { LXDEB(efIn->ecFileFontNumber,efOut); rval= -1; goto ready; }
	*efOut= *efIn;
	}

  ready:

    docRtfRestartFont( rrc );

    return rval;
    }

static int docRtfFontName(	RtfReader *		rrc,
				const char *		name,
				int			len )
    {
    if  ( docRtfSaveDocEncodedText( rrc, name, len ) )
	{ LDEB(len); return -1;	}

    if  ( rrc->rrcCurrentEncodedFont.ecFileFontNumber < 0 )
	{ LDEB(rrc->rrcCurrentEncodedFont.ecFileFontNumber); return 0;	}

    if  ( docRtfAddCurrentFontToList( rrc ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfCommitCurrentFont(	const RtfControlWord *	rcw,
					RtfReader *	rrc )
    { return docRtfAddCurrentFontToList( rrc );	}

int docRtfFontProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case DFpropFAMILY_STYLE:
	    rrc->rrcCurrentFont.dfStyleInt= arg;
	    break;
	case DFpropCHARSET:
	    rrc->rrcCurrentEncodedFont.ecCharset= arg;
	    break;
	case DFpropCODEPAGE:
	    rrc->rrcCurrentEncodedFont.ecCodepage= arg;
	    break;
	case DFpropPITCH:
	    rrc->rrcCurrentFont.dfPitch= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,arg);
	}

    return 0;
    }

static RtfControlWord	docRtfFontGroupGroups[]=
    {
    RTF_TEXT_GROUP( "panose",	DFpropPANOSE,	docRtfCommitFontPanoseText ),
    RTF_TEXT_GROUP( "falt",	DFpropALT_NAME,	docRtfCommitFaltName ),

    { (char *)0, 0, 0 }
    };

static int docRtfFontGroup(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    docRtfRestartFont( rrc );

    rrc->rrcCurrentEncodedFont.ecFileFontNumber= arg;

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
					    docRtfFontGroupGroups,
					    docRtfSaveDocEncodedText,
					    docRtfCommitCurrentFont ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static int docRtfThemeFontGroup(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    docRtfRestartFont( rrc );

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
					    docRtfFontGroupGroups,
					    docRtfSaveDocEncodedText,
					    docRtfCommitCurrentFont ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static RtfControlWord	docRtfFontTableGroups[]=
    {
	RTF_DEST_XX( "f",	RTFidF,	docRtfFontGroup ),
	RTF_DEST_XX( "flomajor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fhimajor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fdbmajor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fbimajor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "flominor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fhiminor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fdbminor",RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fbiminor",RTFidF,	docRtfThemeFontGroup ),

	{ (char *)0, 0, 0 }
    };

static int docRtfCommitFontTable(	const RtfControlWord *	rcw,
					RtfReader *		rrc )
    {
    BufferDocument *		bd= rrc->rrDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    int				charset;

    if  ( rrc->rrcDefaultFont >= 0 )
	{
	if  ( docRtfReadMapFont( rrc, &(dp->dpDefaultFont), &charset,
					    rrc->rrcDefaultFont ) < 0 )
	    { LDEB(rrc->rrcDefaultFont);	}
	}

    if  ( rrc->rrcDefaultFontDbch >= 0 )
	{
	if  ( docRtfReadMapFont( rrc, &(dp->dpDefaultFontDbch), &charset,
					    rrc->rrcDefaultFontDbch ) < 0 )
	    { LDEB(rrc->rrcDefaultFontDbch);	}
	}

    if  ( rrc->rrcDefaultFontLoch >= 0 )
	{
	if  ( docRtfReadMapFont( rrc, &(dp->dpDefaultFontLoch), &charset,
					    rrc->rrcDefaultFontLoch ) < 0 )
	    { LDEB(rrc->rrcDefaultFontLoch);	}
	}

    if  ( rrc->rrcDefaultFontHich >= 0 )
	{
	if  ( docRtfReadMapFont( rrc, &(dp->dpDefaultFontHich), &charset,
					    rrc->rrcDefaultFontHich ) < 0 )
	    { LDEB(rrc->rrcDefaultFontHich);	}
	}

    if  ( rrc->rrcDefaultFontBi >= 0 )
	{
	if  ( docRtfReadMapFont( rrc, &(dp->dpDefaultFontBi), &charset,
						rrc->rrcDefaultFontBi ) < 0 )
	    { LDEB(rrc->rrcDefaultFontBi);	}
	}

    return 0;
    }

int docRtfFontTable(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
						docRtfFontTableGroups,
						docRtfFontName,
						docRtfCommitFontTable ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/

int docRtfReadMapFont(	const RtfReader *		rrc,
			int *				pDocFontNum,
			int *				pCharset,
			int				fileFontNum )
    {
    EncodedFont *	ef;

    if  ( fileFontNum < 0 )
	{ LDEB(fileFontNum); return -1;	}

    ef= (EncodedFont *)utilPagedListGetItemByNumber(
				    &(rrc->rrcEncodedFontList), fileFontNum );
    if  ( ! ef )
	{ return 1;	}

    if  ( ef->ecCodepage >= 0 )
	{ LDEB(ef->ecCodepage);	}

    *pDocFontNum= ef->ecBufFontNumber;
    *pCharset= ef->ecCharset;

    return 0;
    }
