/************************************************************************/
/*									*/
/*  Reading of Rtf files. (Various utility functions.)			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<textOfficeCharset.h>
#   include	<textConverter.h>
#   include	<textConverterImpl.h>
#   include	<docShape.h>

#   include	"docRtfReaderImpl.h"

/************************************************************************/
/*									*/
/*  Reset the attributes in an RtfReader to the defaults.	*/
/*									*/
/************************************************************************/

static void docRtfInitReader(	RtfReader *	rr )
    {
    rr->rrcAddParticule= (RtfAddTextParticule)0;
    rr->rrcSis= (SimpleInputStream *)0;

    rr->rrcLevel= DOClevOUT;
    rr->rrcState= (RtfReadingState *)0;
    rr->rrcInIgnoredGroup= 0;

				/****************************************/
				/*  The depth in the document		*/
				/*  == Type of BufferItem.		*/
				/****************************************/
    rr->rrcComplainUnknown= 1;
				/****************************************/
				/*  Complain about unknown control.	*/
				/****************************************/
    rr->rrcCharacterAhead= EOF;
    rr->rrcAfterNoteref= 0;

    rr->rrReadFlags= 0;

    rr->rrCurrentLine= 1;
    rr->rrDocument= (BufferDocument *)0;
    rr->rrcTree= (DocumentTree *)0;
    rr->rrcNode= (struct BufferItem *)0;
				/****************************************/
				/*  The current position in the input.	*/
				/****************************************/
				/****************************************/
				/* Attributes of the current position.	*/
				/****************************************/
    docInitFrameProperties( &(rr->rrcRowFrameProperties) );
    docInitCellProperties( &(rr->rrcCellProperties) );
    docInitItemShading( &(rr->rrcCellShading) );
    docInitSectionProperties( &(rr->rrcSectionProperties) );
    rr->rrcSectionColumn= 0;
    docInitRowProperties( &(rr->rrcRowProperties) );
    docInitItemShading( &(rr->rrcRowShading) );
    utilPropMaskClear( &(rr->rrcRowPropertyMask) );
    utilPropMaskClear( &(rr->rrcCellPropertyMask) );

    docInitSelectionScope( &(rr->rrcSelectionScope) );

    docInitDocumentList( &(rr->rrcDocumentList) );
    docInitListOverride( &(rr->rrcListOverride) );
    docInitDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docInitListOverrideLevel( &(rr->rrcListOverrideLevel) );
    utilInitMemoryBuffer( &(rr->rrcListLevelText) );
    utilInitMemoryBuffer( &(rr->rrcListLevelNumbers) );

    docInitBorderProperties( &(rr->rrcBorderProperties) );
    rr->rrDrawingShape= (struct DrawingShape *)0;
    rr->rrShapeProperty= (struct RtfControlWord *)0;
    utilInitMemoryBuffer( &(rr->rrShapePropertyName) );
    utilInitMemoryBuffer( &(rr->rrShapePropertyValue) );
    rr->rrcNextObjectVertex= 0;
    docInitTabStop( &(rr->rrcTabStop) );

    docInitParagraphNumber( &(rr->rrcParagraphNumber) );
    rr->rrcParagraphNumbers= (ParagraphNumber *)0;
    rr->rrcParagraphNumberCount= 0;

    rr->rrcColor.rgb8Red= rr->rrcColor.rgb8Green= rr->rrcColor.rgb8Blue= 0;
    rr->rrcGotComponent= 0;

    utilInvalidateTime( &(rr->rrcTm) );
    rr->rrcInfoText= (unsigned char *)0;
				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    docInitDocumentFont( &(rr->rrcCurrentFont) );
    utilInitPagedList( &(rr->rrcEncodedFontList) );
    docRtfInitEncodedFont( &(rr->rrcCurrentEncodedFont) );
    utilStartPagedList( &(rr->rrcEncodedFontList),
				sizeof(EncodedFont),
				(InitPagedListItem)docRtfInitEncodedFont,
				(CleanPagedListItem)0 );

    rr->rrcDefaultFont= -1;
    rr->rrcDefaultFontDbch= -1;
    rr->rrcDefaultFontLoch= -1;
    rr->rrcDefaultFontHich= -1;
    rr->rrcDefaultFontBi= -1;

    docInitDocumentStyle( &(rr->rrcStyle) );
    docInitDocumentProperties( &(rr->rrcDocumentProperties) );
    utilPropMaskClear( &(rr->rrcDocPropertyMask) );
				/****************************************/
				/*  For reading the font table.		*/
				/****************************************/
    rr->rrcInsertedObjectNr= -1;
    utilInitMemoryBuffer( &(rr->rrcBookmark) );
    rr->rrcFieldStack= (struct RtfFieldStackLevel *)0;
    rr->rrcLastFieldNumber= -1;
				/****************************************/
				/*  For reading 'objects' and pictures.	*/
				/*  For reading 'fields'.		*/
				/****************************************/
    rr->rrAfterParaHeadField= 0;
    rr->rrParagraphBreakOverride= -1;
    rr->rrcGotDocGeometry= 0;
				/****************************************/
				/*  For coping with the way word saves	*/
				/*  {\pntext ... }			*/
				/****************************************/

    rr->rrRtfTextConverter= (struct TextConverter *)0;
    rr->rrTextTextConverter= (struct TextConverter *)0;

				/*
				 * Only used for reading Undo/Redo traces.
				 */
    rr->rrcTraceReadWhat= 0;
    rr->rrcTraceCommand= -1;
    rr->rrcTraceSelectionPosition= -1;
    rr->rrcTraceInProps= 0;

    docInitSelectionScope( &(rr->rrcTraceOldSelectionScope) );
    docInitEditRange( &(rr->rrcTraceOldRange) );
    rr->rrcTraceOldCol0= -1;
    rr->rrcTraceOldCol1= -1;
    rr->rrcTraceOldPage= -1;
    rr->rrcTraceOldColumn= -1;

    docInitSelectionScope( &(rr->rrcTraceNewSelectionScope) );
    docInitEditRange( &(rr->rrcTraceNewRange) );
    rr->rrcTraceNewCol0= -1;
    rr->rrcTraceNewCol1= -1;
    rr->rrcTraceNewPage= -1;
    rr->rrcTraceNewColumn= -1;

    docInitNoteProperties( &(rr->rrcNoteProperties) );
    utilPropMaskClear( &(rr->rrcNotePropertyMask) );
    }

static void docRtfCleanReader(	RtfReader *	rr )
    {
    while( rr->rrcState )
	{
	XDEB(rr->rrcState);
	docRtfPopReadingState( rr );
	}

    /*docCleanCellProperties( &(rr->rrcRowProperties) );*/
    docCleanSectionProperties( &(rr->rrcSectionProperties) );
    docCleanRowProperties( &(rr->rrcRowProperties) );
    /*docCleanShapeProperties( &(rr->rrcShapeProperties) );*/

    if  ( rr->rrDrawingShape )
	{ docDeleteDrawingShape( rr->rrDocument, rr->rrDrawingShape ); }

    docCleanDocumentFont( &(rr->rrcCurrentFont) );
    utilCleanPagedList( &(rr->rrcEncodedFontList) );

    docCleanDocumentList( &(rr->rrcDocumentList) );
    docCleanListOverride( &(rr->rrcListOverride) );
    docCleanDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docCleanListOverrideLevel( &(rr->rrcListOverrideLevel) );
    utilCleanMemoryBuffer( &(rr->rrcListLevelText) );
    utilCleanMemoryBuffer( &(rr->rrcListLevelNumbers) );

    docCleanDocumentStyle( &(rr->rrcStyle) );
    docCleanDocumentProperties( &(rr->rrcDocumentProperties) );

    if  ( rr->rrcParagraphNumbers )
	{ free( rr->rrcParagraphNumbers );	}

    docCleanParagraphNumber( &(rr->rrcParagraphNumber) );

    utilCleanMemoryBuffer( &(rr->rrcBookmark) );

    docCleanNoteProperties( &(rr->rrcNoteProperties) );

    if  ( rr->rrRtfTextConverter )
	{
	textCleanTextConverter( rr->rrRtfTextConverter );
	free( rr->rrRtfTextConverter );
	}

    if  ( rr->rrTextTextConverter )
	{
	textCleanTextConverter( rr->rrTextTextConverter );
	free( rr->rrTextTextConverter );
	}
    }

void docRtfPushReadingState(	RtfReader *		rr,
				RtfReadingState *	rrs )
    {
    BufferDocument *		bd= rr->rrDocument;

    docPlainTextAttribute( &(rrs->rrsTextAttribute), bd );
    rrs->rrsTextCharset= -1;
    docInitItemShading( &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitTabStopList( &(rrs->rrsTabStopList) );
    docInitItemShading( &(rrs->rrsParagraphShading) );
    docInitFrameProperties( &(rrs->rrsParaFrameProperties) );

    utilInitMemoryBuffer( &(rrs->rrsSavedText) );

    rrs->rrsBytesPerUnicode= 1;
    rrs->rrsUnicodeBytesToSkip= 0;

    if  ( rr->rrcState )
	{
	const RtfReadingState *	above= rr->rrcState;

	rrs->rrsBytesPerUnicode= above->rrsBytesPerUnicode;
	rrs->rrsUnicodeBytesToSkip= 0;

	rrs->rrsTextAttribute= above->rrsTextAttribute;
	rrs->rrsTextShading= above->rrsTextShading;
	rrs->rrsTextShadingChanged= above->rrsTextShadingChanged;

	docCopyParagraphProperties( &(rrs->rrsParagraphProperties),
				    &(above->rrsParagraphProperties) );
	docCopyTabStopList( &(rrs->rrsTabStopList), &(above->rrsTabStopList) );
	rrs->rrsParagraphShading= above->rrsParagraphShading;
	}

    rrs->rrsPrev= rr->rrcState;
    rr->rrcState= rrs;

    return;
    }

void docRtfPopReadingState(	RtfReader *	rr )
    {
    RtfReadingState *	rrs= rr->rrcState;

    if  ( ! rrs )
	{ XDEB(rrs); return;	}

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docCleanTabStopList( &(rrs->rrsTabStopList) );

    /*  Copy unused data in child to parent */
    if  ( rrs->rrsPrev )
	{
	if  ( utilMemoryAppendBuffer( &(rrs->rrsPrev->rrsSavedText),
						    &(rrs->rrsSavedText) ) )
	    { LDEB(rrs->rrsSavedText.mbSize);	}
	}

    utilCleanMemoryBuffer( &(rrs->rrsSavedText) );

    /**/
    rr->rrcState= rrs->rrsPrev;
    if  ( rr->rrcState )
	{ rr->rrcState->rrsUnicodeBytesToSkip= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Open a reader.							*/
/*									*/
/************************************************************************/

RtfReader * docRtfOpenReader(		SimpleInputStream *	sis,
					BufferDocument *	bd,
					int			flags )
    {
    RtfReader *		rval= (RtfReader *)0;
    RtfReader *		rr= (RtfReader *)0;

    rr= (RtfReader *)malloc(sizeof(RtfReader) );
    if  ( ! rr )
	{ PDEB(rr); goto ready;	}

    docRtfInitReader( rr );

    rr->rrRtfTextConverter= malloc( sizeof(TextConverter) );
    if  ( ! rr->rrRtfTextConverter )
	{ PDEB(rr->rrRtfTextConverter); goto ready;	}
    textInitTextConverter( rr->rrRtfTextConverter );

    rr->rrTextTextConverter= malloc( sizeof(TextConverter) );
    if  ( ! rr->rrTextTextConverter )
	{ PDEB(rr->rrTextTextConverter); goto ready;	}
    textInitTextConverter( rr->rrTextTextConverter );

    rr->rrDocument= bd;
    rr->rrcSis= sis;
    rr->rrReadFlags= flags;

    docRtfReadSetupTextConverters( rr );

    rval= rr; rr= (RtfReader *)0; /* steal */

  ready:

    if  ( rr )
	{ docRtfCloseReader( rr );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Close a reader.							*/
/*									*/
/************************************************************************/

void docRtfCloseReader(		RtfReader *		rr )
    {
    docRtfCleanReader( rr );
    free( rr );

    return;
    }
