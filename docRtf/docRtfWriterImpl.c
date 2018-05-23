/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<textConverter.h>
#   include	<textConverterImpl.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTextConverter.h"

/************************************************************************/

static void docRtfInitWritingContext(	RtfWriter *	rw )
    {
    rw->rwDocument= (BufferDocument *)0;

    utilInitTextAttribute( &(rw->rwTextAttribute) );
    rw->rwTextCharset= FONTcharsetANSI;
    docInitParagraphProperties( &(rw->rwcParagraphProperties) );
    docInitRowProperties( &(rw->rwRowProperties) );

    docInitParagraphProperties( &(rw->rwcOutsideTableParagraphProperties) );

    utilInitPagedList( &(rw->rwcEncodedFontList) );
    utilStartPagedList( &(rw->rwcEncodedFontList),
				sizeof(EncodedFont),
				(InitPagedListItem)docRtfInitEncodedFont,
				(CleanPagedListItem)0 );

    rw->rwcPushedAttribute= (PushedAttribute *)0;

    rw->rwcInFldrslt= 0;
    rw->rwTableNesting= 0;
    rw->rwDeepestTableNesting= 0;

    rw->rwSectionPropertiesSaved= 0;

    rw->rwSaveFlags= 0;

    rw->rwcAfter= RTFafterTEXT;
    rw->rwcLastNodeLevel= DOClevOUT;

    rw->rwCol= 0;
    rw->rwSosOut= (SimpleOutputStream *)0;

    rw->rwRtfTextConverter= (TextConverter *)0;
    rw->rwTextTextConverter= (TextConverter *)0;

    rw->rwPpExtraMask= (const PropertyMask *)0;
    rw->rwCpExtraMask= (const PropertyMask *)0;
    rw->rwRpExtraMask= (const PropertyMask *)0;
    rw->rwSpExtraMask= (const PropertyMask *)0;
    }

static void docRtfCleanWritingContext(	RtfWriter *	rw )
    {
    docCleanParagraphProperties( &rw->rwcParagraphProperties );
    docCleanRowProperties( &rw->rwRowProperties );

    docCleanParagraphProperties( &rw->rwcOutsideTableParagraphProperties );

    utilCleanPagedList( &(rw->rwcEncodedFontList) );

    if  ( rw->rwRtfTextConverter )
	{
	textCleanTextConverter( rw->rwRtfTextConverter );
	free( rw->rwRtfTextConverter );
	}

    if  ( rw->rwTextTextConverter )
	{
	textCleanTextConverter( rw->rwTextTextConverter );
	free( rw->rwTextTextConverter );
	}
    }

/************************************************************************/
/*									*/
/*  Reserve a number of columns in the output file.			*/
/*									*/
/************************************************************************/

void docRtfReserveColumns(	RtfWriter *	rw,
				int			cols )
    {
    if  ( rw->rwCol > 0 && rw->rwCol+ cols > 72 )
	{ docRtfWriteNextLine( rw );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

void docRtfWriteNextLine(	RtfWriter *	rw )
    {
    if  ( rw->rwCol > 0 )
	{
	sioOutPutString( "\r\n", rw->rwSosOut );

	rw->rwCol= 0;
	rw->rwcAfter= RTFafterTEXT;
	}

    return;
    }

int docRtfWriteTag(	RtfWriter *	rw,
			const char *	tag )
    {
    int		len= strlen( tag );

    rw->rwCol += 1+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= len;
	}

    if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    rw->rwcAfter= RTFafterTAG;

    return 0;
    }

int docRtfWriteDestinationBegin(	RtfWriter *		rw,
					const char *		tag )
    {
    int		len= strlen( tag );

    rw->rwCol += 1+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= 1+ len;
	}

    if  ( sioOutPutByte( '{', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( tag && tag[0] )
	{
	if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}
	rw->rwCol += 1;
	if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}
	rw->rwcAfter= RTFafterTAG;
	}
    else{
	rw->rwcAfter= RTFafterTEXT;
	}

    return 0;
    }

int docRtfWriteArgDestinationBegin(	RtfWriter *		rw,
					const char *		tag,
					int			arg )
    {
    char	scratch[20];
    int		len;

    sprintf( scratch, "%d", arg );

    len= strlen( tag )+ strlen( scratch );

    rw->rwCol += 2+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= 1+ len;
	}

    if  ( sioOutPutByte( '{', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( scratch, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}

    rw->rwcAfter= RTFafterARG;

    return 0;
    }

int docRtfWriteDestinationEnd(		RtfWriter *	rw )
    {
    if  ( sioOutPutByte( '}', rw->rwSosOut ) )
	{ LDEB(1); return -1;	}

    rw->rwCol += 1;
    rw->rwcAfter= RTFafterTEXT;

    return 0;
    }

int docRtfWriteArgTag(	RtfWriter *		rw,
			const char *		tag,
			int			arg )
    {
    char	scratch[20];
    int		len;

    sprintf( scratch, "%d", arg );

    len= strlen( tag )+ strlen( scratch );

    rw->rwCol += 1+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= len;
	}

    if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( scratch, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}

    rw->rwcAfter= RTFafterARG;

    return 0;
    }

void docRtfWriteFlagTag(	RtfWriter *		rw,
				const char *		tag,
				int			arg )
    {
    if  ( arg )
	{ docRtfWriteTag( rw, tag );		}
    else{ docRtfWriteArgTag( rw, tag, arg );	}
    }

void docRtfWriteAltTag(		RtfWriter *		rw,
				const char *		yesTag,
				const char *		noTag,
				int			arg )
    {
    if  ( arg )
	{ docRtfWriteTag( rw, yesTag );	}
    else{ docRtfWriteTag( rw, noTag  );	}
    }

void docRtfWriteEnumTag(	RtfWriter *		rw,
				const char * const *	tags,
				int			arg,
				int			tagCount,
				int			enumCount )
    {
    if  ( tagCount != enumCount )
	{ LLSDEB(tagCount,enumCount,tags[0]); return;	}

    if  ( arg < 0 || arg >= tagCount )
	{ LLSDEB(tagCount,arg,tags[0]); return;	}

    if  ( tags[arg] )
	{ docRtfWriteTag( rw, tags[arg] );	}
    }

void docRtfWriteSemicolon(	RtfWriter *	rw )
    {
    sioOutPutString( ";", rw->rwSosOut );
    rw->rwCol += 1;

    rw->rwcAfter= RTFafterTEXT;
    }

/************************************************************************/
/*									*/
/*  Write object data.							*/
/*									*/
/************************************************************************/

int docRtfWriteMemoryBuffer(	RtfWriter *			rw,
				const MemoryBuffer *		mb )
    {
    const unsigned char *	s;
    int				i;

    if  ( rw->rwCol+ mb->mbSize > 78 )
	{ docRtfWriteNextLine( rw ); }

    if  ( rw->rwcAfter != RTFafterTEXT )
	{
	if  ( sioOutPutByte( ' ', rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}
	rw->rwCol += 1;
	rw->rwcAfter= RTFafterTEXT;
	}

    s= mb->mbBytes;
    for ( i= 0; i < mb->mbSize; s++, i++ )
	{
	if  ( rw->rwCol >= 78 )
	    { docRtfWriteNextLine( rw ); }

	if  ( sioOutPutByte( *s, rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}

	rw->rwCol++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a word 2000 style table or cell formatting property.		*/
/*									*/
/************************************************************************/

void docRtfSaveAutoSpace(		RtfWriter *		rw,
					const char *		unitTag,
					int			unit,
					const char *		numberTag,
					int			number )
    {
    if  ( number != 0 || unit != TRautoNONE )
	{ docRtfWriteArgTag( rw, unitTag, unit );	}

    if  ( number != 0 )
	{ docRtfWriteArgTag( rw, numberTag, number );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Write an encoded destination and its contents.			*/
/*									*/
/************************************************************************/

void docRtfWriteDocEncodedStringDestination(
					RtfWriter *		rw,
					const char *		tag,
					const char *		s,
					int			n,
					int			addSemicolon )
    {
    docRtfWriteDestinationBegin( rw, tag );

    if  ( n > 0 )
	{ docRtfWriteDocEncodedString( rw, s, n );	}

    if  ( addSemicolon )
	{ docRtfWriteSemicolon( rw );	}

    docRtfWriteDestinationEnd( rw );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a destination that does not need any transcoding.		*/
/*									*/
/************************************************************************/

void docRtfWriteRawBytesDestination(	RtfWriter *		rw,
					const char *		tag,
					const char *		s,
					int			n )
    {
    docRtfWriteDestinationBegin( rw, tag );

    if  ( n > 0 )
	{ docRtfWriteRawBytes( rw, s, n );	}

    docRtfWriteDestinationEnd( rw );

    return;
    }

/************************************************************************/
/*									*/
/*  Open a writer.							*/
/*									*/
/************************************************************************/

RtfWriter * docRtfOpenWriter(		SimpleOutputStream *	sos,
					BufferDocument *	bd,
					int			flags )
    {
    RtfWriter *		rval= (RtfWriter *)0;
    RtfWriter *		rw= (RtfWriter *)0;

    rw= (RtfWriter *)malloc(sizeof(RtfWriter) );
    if  ( ! rw )
	{ PDEB(rw); goto ready;	}

    docRtfInitWritingContext( rw );

    rw->rwRtfTextConverter= malloc( sizeof(TextConverter) );
    if  ( ! rw->rwRtfTextConverter )
	{ PDEB(rw->rwRtfTextConverter); goto ready;	}
    textInitTextConverter( rw->rwRtfTextConverter );

    rw->rwTextTextConverter= malloc( sizeof(TextConverter) );
    if  ( ! rw->rwTextTextConverter )
	{ PDEB(rw->rwTextTextConverter); goto ready;	}
    textInitTextConverter( rw->rwTextTextConverter );

    rw->rwDocument= bd;
    rw->rwSosOut= sos;
    rw->rwSaveFlags= flags;

    docRtfWriteSetupTextConverters( rw );

    rval= rw; rw= (RtfWriter *)0; /* steal */

  ready:

    if  ( rw )
	{ docRtfCloseWriter( rw );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Close a writer.							*/
/*									*/
/************************************************************************/

void docRtfCloseWriter(		RtfWriter *		rw )
    {
    docRtfCleanWritingContext( rw );
    free( rw );

    return;
    }
