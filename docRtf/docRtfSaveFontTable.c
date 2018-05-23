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
#   include	<psDocumentFontStyle.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfTextConverter.h"
#   include	"docRtfFlags.h"

/************************************************************************/

static const char * docRtfFontFamilyStyleTag(	int styleInt )
    {
    switch( styleInt )
	{
	case DFstyleFNIL:	return "fnil";
	case DFstyleFROMAN:	return "froman";
	case DFstyleFSWISS:	return "fswiss";
	case DFstyleFMODERN:	return "fmodern";
	case DFstyleFSCRIPT:	return "fscript";
	case DFstyleFDECOR:	return "fdecor";
	case DFstyleFTECH:	return "ftech";
	case DFstyleFBIDI:	return "fbidi";

	default:
	    LDEB(styleInt); return "fnil";
	}
    }

/************************************************************************/
/*									*/
/*  Write a font table.							*/
/*									*/
/************************************************************************/

static void docRtfWriteEncodedFont(	RtfWriter *		rw,
					const DocumentFont *	df,
					const char *		suffix,
					int			fontnum,
					int			charset )
    {
    docRtfWriteArgDestinationBegin( rw, "f", fontnum );

    docRtfWriteTag( rw, docRtfFontFamilyStyleTag( df->dfStyleInt ) );

    if  ( charset != FONTcharsetDEFAULT )
	{ docRtfWriteArgTag( rw, "fcharset", charset ); }

    if  ( df->dfPitch != FONTpitchDEFAULT )
	{ docRtfWriteArgTag( rw, "fprq", df->dfPitch ); }

    if  ( df->dfPanose[0] )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedStringDestination( rw, "*\\panose",
		    df->dfPanose, strlen( df->dfPanose ), addSemicolon );
	}

    if  ( df->dfName )
	{
	const char *	name= df->dfName;

	if  ( ! strcmp( name, "ITC Zapf Dingbats" ) )
	    { name= "ZapfDingbats";	}
	if  ( ! strcmp( name, "ITC Zapf Chancery" ) )
	    { name= "ZapfChancery";	}

	docRtfWriteDocEncodedString( rw, name, strlen( name ) );

	if  ( suffix )
	    { docRtfWriteDocEncodedString( rw, suffix, strlen( suffix ) ); }
	}

    if  ( df->dfAltName )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedStringDestination( rw, "*\\falt",
		    df->dfAltName, strlen( df->dfAltName ), addSemicolon );
	}

    docRtfWriteSemicolon( rw );

    docRtfWriteDestinationEnd( rw );

    return;
    }

static const EncodedFont * dowRtfWriteGetEncodedFont(
					RtfWriter *		rw,
					const DocumentFont *	df,
					int			csIdx )
    {
    const EncodedFont *		ef;
    int				key;

    key= CHARSETidx_COUNT* df->dfDocFontNumber+ csIdx;
    ef= (const EncodedFont *)utilPagedListGetItemByNumber(
					&(rw->rwcEncodedFontList), key );
    if  ( ! ef )
	{ SLLLXDEB(df->dfName,df->dfDocFontNumber,csIdx,key,ef);	}

    return ef;
    }

int docRtfWriteGetCharset(	RtfWriter *		rw,
				int *			pCharset,
				const DocumentFont *	df,
				int			symbol )
    {
    int			csIdx;
    const EncodedFont *	ef;
    int			charset= FONTcharsetDEFAULT;
    int			fontNumber= -1;

    csIdx= utilIndexMappingGet( &(df->dfUnicodeToCharset), symbol );
    if  ( csIdx < 0 )
	{ csIdx= df->dfCharsetIdxUsed[0];	}

    ef= dowRtfWriteGetEncodedFont( rw, df, csIdx );
    if  ( ef )
	{
	charset= ef->ecCharset;
	fontNumber= ef->ecFileFontNumber;
	}
    else{ LXXDEB(csIdx,ef,symbol);		}

    *pCharset= charset;
    return fontNumber;
    }

int docRtfWriteGetDefaultFont(	RtfWriter *			rw,
				int				deff )
    {
    const BufferDocument *	bd= rw->rwDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= dp->dpFontList;

    const int			cs= 0;

    const DocumentFont *	df;
    const EncodedFont *		ef;

    if  ( rw->rwSaveFlags & RTFflagUNENCODED )
	{ return deff;	}

    if  ( deff < 0 || deff >= dfl->dflFontCount )
	{ return -1;	}

    df= docFontListGetFontByNumber( dfl, deff );
    if  ( ! df || ! df->dfName || df->dfCharsetIdxUsedCount == 0 )
	{ return -1;	}

    ef= dowRtfWriteGetEncodedFont( rw, df, df->dfCharsetIdxUsed[cs] );
    if  ( ! ef )
	{ LXDEB(cs,ef); return -1;	}

    return ef->ecFileFontNumber;
    }

void docRtfWriteFontTable(	RtfWriter *			rw )
    {
    const BufferDocument *	bd= rw->rwDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= dp->dpFontList;

    int				fnt;

    docRtfWriteDestinationBegin( rw, "fonttbl" );
    docRtfWriteNextLine( rw );

    for ( fnt= 0; fnt < dfl->dflFontCount; fnt++ )
	{
	const DocumentFont *	df= docFontListGetFontByNumber( dfl, fnt );
	int			cs;

	if  ( ! df || ! df->dfName )
	    { continue;	}

	for ( cs= 0; cs < df->dfCharsetIdxUsedCount; cs++ )
	    {
	    const OfficeCharset *	oc;
	    const EncodedFont *		ef;
	    const char *		suffix= (const char *)0;

	    ef= dowRtfWriteGetEncodedFont( rw, df, df->dfCharsetIdxUsed[cs] );
	    if  ( ! ef )
		{ LXDEB(cs,ef); continue;	}

	    oc= utilGetOfficeCharsetByIndex( df->dfCharsetIdxUsed[cs] );
	    if  ( ! oc )
		{ LXDEB(df->dfCharsetIdxUsed[cs],oc); continue; }

	    if  ( oc->ocEmitSuffix )
		{
		suffix= oc->ocOfficeFontnameSuffix;
		}

	    docRtfWriteEncodedFont( rw, df, suffix, ef->ecFileFontNumber,
							oc->ocOfficeCharset );

	    if  ( fnt+ 1 < dfl->dflFontCount		||
		  cs+ 1 < df->dfCharsetIdxUsedCount	)
		{ docRtfWriteNextLine( rw );	}
	    }
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return;
    }

/************************************************************************/

int docRtfWriteBuildFontAdmin(	RtfWriter *		rw )
    {
    BufferDocument *		bd= rw->rwDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= dp->dpFontList;

    int				f;
    int				fileFontNumber= 0;

    utilDocFontListClearCharsUsed( dfl );

    if  ( docGetCharsUsed( bd ) )
	{ LDEB(1); return -1;	}

    for ( f= 0; f < dfl->dflFontCount; f++ )
	{
	DocumentFont *	df= docFontListGetFontByNumber( dfl, f );

	if  ( ! df )
	    { continue;	}

	if  ( docFontFindLegacyEncodings( df ) )
	    { LSDEB(f,df->dfName); return -1;	}

	if  ( df->dfCharsetIdxUsedCount > 0 )
	    {
	    int		cs;

	    for ( cs= 0; cs < df->dfCharsetIdxUsedCount; cs++ )
		{
		EncodedFont		efIn;
		EncodedFont *		efOut;
		const OfficeCharset *	oc;
		int			key;

		oc= utilGetOfficeCharsetByIndex( df->dfCharsetIdxUsed[cs] );
		if  ( ! oc )
		    { XDEB(oc); return -1;	}

		docRtfInitEncodedFont( &efIn );

		efIn.ecFileFontNumber= fileFontNumber++;
		efIn.ecBufFontNumber= df->dfDocFontNumber;
		efIn.ecCharset= oc->ocOfficeCharset;
		efIn.ecCharsetIdx= df->dfCharsetIdxUsed[cs];

		key= CHARSETidx_COUNT* efIn.ecBufFontNumber+
						    df->dfCharsetIdxUsed[cs];

		efOut= (EncodedFont *)utilPagedListClaimItem(
					    &(rw->rwcEncodedFontList), key );
		if  ( ! efOut )
		    { LXDEB(key,efOut); return -1;	}
		*efOut= efIn;
		}
	    }
	}

    return 0;
    }

