/************************************************************************/
/*									*/
/*  Read a plain text file into a BufferDocument			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<sioMemory.h>
#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	<textConverter.h>
#   include	<textConverterImpl.h>
#   include	<sioUtf8.h>
#   include	"docPlainReadWrite.h"

/************************************************************************/
/*									*/
/*  Read a document as a whole.						*/
/*									*/
/************************************************************************/

typedef struct PlainReadingContext
    {
    TextAttribute	prcTextAttribute;
    BufferItem *	prcParaNode;
    int			prcHasOpenEnd;
    int			prcLongestParagraph;
    MemoryBuffer	prcCollected;

    TextConverter	prcTextConverter;
    } PlainReadingContext;

static void docPlainInitReadingContext( PlainReadingContext *	prc )
    {
    utilInitTextAttribute( &(prc->prcTextAttribute) );

    prc->prcParaNode= (BufferItem *)0;
    prc->prcHasOpenEnd= 0;
    prc->prcLongestParagraph= 0;

    utilInitMemoryBuffer( &(prc->prcCollected) );
    textInitTextConverter( &(prc->prcTextConverter) );
    docParaSetupTextConverter( &(prc->prcTextConverter) );
    }

static void docPlainCleanReadingContext( PlainReadingContext *	prc )
    {
    /* utilCleanTextAttribute( &(prc->prcTextAttribute) ); */

    utilCleanMemoryBuffer( &(prc->prcCollected) );
    textCleanTextConverter( &(prc->prcTextConverter) );
    }

static int docPlainReadParaContents(	BufferDocument *	bd,
					BufferItem *		paraNode,
					int			c,
					SimpleInputStream *	sis,
					PlainReadingContext *	prc )
    {
    int				rval= 0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    utilEmptyMemoryBuffer( &(prc->prcCollected) );

    for (;;)
	{
	int		done;

	if  ( c == '\f' )
	    {
	    /*  Ignored inside the paragraph */
	    if  ( docParaStrlen( paraNode ) == 0 )
		{ paraNode->biParaBreakKind= DOCibkPAGE;	}

	    c= sioInGetUtf8( sis );
	    if  ( c == EOF )
		{ break;	}

	    continue;
	    }

	if  ( c == '\t' )
	    {
	    if  ( docSaveSpecialParticule( bd, paraNode,
				    &(prc->prcTextAttribute), DOCkindTAB ) )
		{ LDEB(docParaStrlen(paraNode)); rval= -1; goto ready;	}

	    c= sioInGetUtf8( sis );
	    if  ( c == EOF )
		{ break;	}

	    continue;
	    }

	utilEmptyMemoryBuffer( &(prc->prcCollected) );
	sos= sioOutMemoryOpen( &(prc->prcCollected) );
	if  ( ! sos )
	    { XDEB(sos); rval= -1; goto ready;	}
	done= 0;

	for (;;)
	    {
	    if  ( c != '\r' )
		{
		done++;
		if  ( sioOutPutUtf8( c, sos ) < 0 )
		    { LCDEB(c,c); rval= -1; goto ready;	}
		}

	    c= sioInGetUtf8( sis );
	    if  ( c == EOF )
		{ prc->prcHasOpenEnd= 1; break;	}

	    if  ( c == '\n' || c == '\t' )
		{ break;	}
	    }

	sioOutClose( sos ); sos= (SimpleOutputStream *)0;

	if  ( done > 0 )
	    {
	    const unsigned char *	bytes;
	    int				size;

	    bytes= utilMemoryBufferGetBytes( &size, &(prc->prcCollected) );

	    if  ( docParaAppendText( bd, paraNode, &(prc->prcTextAttribute),
			    &(prc->prcTextConverter), (char *)bytes, size ) )
		{ LDEB(size); rval= -1; goto ready;	}
	    }

	if  ( c == EOF )
	    { prc->prcHasOpenEnd= 1; break;	}
	if  ( c == '\n' )
	    { prc->prcHasOpenEnd= 0; break;	}
	}

    /*  HACK: fix first paragraph of document!	*/
    if  ( paraNode->biParaParticuleCount > 1			&&
	  paraNode->biParaParticules[0].tpKind == DOCkindSPAN	&&
	  paraNode->biParaParticules[0].tpStrlen == 0		)
	{ docDeleteParticules( paraNode, 0, 1 );	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a paragraph of text.						*/
/*									*/
/*  1)  Read the first byte.						*/
/*									*/
/************************************************************************/

static int docPlainReadParagraph(	BufferDocument *	bd,
					SimpleInputStream *	sis,
					PlainReadingContext *	prc )
    {
    int			c;
    BufferItem *	paraNode;

    int			textAttrNr;

    /*  1  */
    c= sioInGetUtf8( sis );
    if  ( c == EOF )
	{ return 1;	}

    if  ( ! prc->prcParaNode )
	{
	DocumentPosition	dp;

	if  ( docDocumentHead( &dp, bd ) )
	    { LDEB(1);	}

	paraNode= dp.dpNode;
	prc->prcParaNode= paraNode;
	}
    else{
	paraNode= docInsertNode( bd, prc->prcParaNode->biParent,
							    -1, DOClevPARA );
	if  ( ! paraNode )
	    { XDEB(paraNode); return -1;	}

	prc->prcParaNode= paraNode;
	}

    if  ( docPlainReadParaContents( bd, paraNode, c, sis, prc ) )
	{ LDEB(1); return -1;	}

    if  ( paraNode->biParaParticuleCount == 0 )
	{
	textAttrNr= docTextAttributeNumber( bd, &(prc->prcTextAttribute) );

	if  ( ! docInsertTextParticule( paraNode, 0, 0, 0,
						    DOCkindSPAN, textAttrNr ) )
	    { LDEB( paraNode->biParaParticuleCount); return -1;	}
	}

    if  ( prc->prcLongestParagraph < docParaStrlen( paraNode ) )
	{ prc->prcLongestParagraph=  docParaStrlen( paraNode );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a plain text file.						*/
/*									*/
/*  1)  Assume 12 cpi and courier. ( 12 cpi .. 6pt wide .. 10 pt high. )*/
/*	But make it one less: 9 pt to match the rfc1234.txt make up.	*/
/*	Now if we want 80 characters on a line we need 80*6=480 pt. on	*/
/*	a line. A4 is 595 points wide, so we have 115 points left for	*/
/*	the margins. Round to 3/4 inch= 48 pt= 960 twips.		*/
/*  2)  Tab every 8 characters. 12 cpi .. 8/12 inch= 960 twips.		*/
/*									*/
/************************************************************************/

BufferDocument * docPlainReadFile(
			SimpleInputStream *		sis,
			int *				pMxL,
			const DocumentGeometry *	dgPaper )
    {
    BufferDocument *	rval= (BufferDocument *)0;
    BufferDocument *	bd;
    DocumentGeometry	dgDoc= *dgPaper;
    /*
    const char *	lc_ctype= nl_langinfo( CODESET );
    */
    const char *	lc_ctype= "UTF-8";
    const char *	font= "Courier";

    PlainReadingContext	prc;

    docPlainInitReadingContext( &prc );

    dgDoc.dgLeftMarginTwips= 960;
    dgDoc.dgRightMarginTwips= 960;

    if  ( lc_ctype )
	{
	textConverterSetNativeEncodingName(
					&(prc.prcTextConverter), lc_ctype );
	}

    bd= docNewFile( &(prc.prcTextAttribute),
				    font, 2* 9,
				    (PostScriptFontList *)0, &dgDoc );
    if  ( ! bd )
	{ XDEB(bd); return bd;	}

    bd->bdProperties.dpTabIntervalTwips= 960;

    for (;;)
	{
	int	res;

	res= docPlainReadParagraph( bd, sis, &prc );

	if  ( res > 0 )
	    { break;	}
	if  ( res < 0 )
	    { LDEB(res); goto ready; }
	}

    *pMxL= prc.prcLongestParagraph;
    bd->bdProperties.dpHasOpenEnd= prc.prcHasOpenEnd;
    rval= bd; bd= (BufferDocument *)0; /* steal */

  ready:

    docPlainCleanReadingContext( &prc );
    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }
