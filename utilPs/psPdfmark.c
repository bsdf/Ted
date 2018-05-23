/************************************************************************/
/*									*/
/*  Pdfmark related PostScript generation.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"psPrint.h"
#   include	<geo2DInteger.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Include pdfmark/distiller related emulation in the PostScript	*/
/*  output.								*/
/*									*/
/************************************************************************/

static const char * appPsPdfmarkEmulation[]=
    {
    "%%pdfmark emulation",
    "/pdfmark where",
    "    { pop }",
    "    { userdict /pdfmark /cleartomark load put }",
    "ifelse",
    };

static const char * appPssetdistillerparamsEmulation[]=
    {
    "%%setdistillerparams emulation",
    "/setdistillerparams where",
    "    { pop }",
    "    { /setdistillerparams { pop } bind def }",
    "ifelse",
    };

void psSetPdfmarkEmulation(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, appPsPdfmarkEmulation,
		sizeof(appPsPdfmarkEmulation)/sizeof(const char *) );

    psDefineProcedure( sos, appPssetdistillerparamsEmulation,
		sizeof(appPssetdistillerparamsEmulation)/sizeof(const char *) );

    return;
    }

static const char * appPspsImageQualityDistillerparams[]=
    {
    "<<",
    "  /AutoFilterColorImages false",
    "  /ColorImageFilter /FlateEncode",
    "  /ColorACSImageDict <<",
    "    /QFactor 0.5",
    "    /Blend 1",
    "    /ColorTransform 1",
    "    /HSamples [1 1 1 1]",
    "    /VSamples [1 1 1 1] >>",
    ">> setdistillerparams",
    };

void psImageQualityDistillerparams(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, appPspsImageQualityDistillerparams,
	    sizeof(appPspsImageQualityDistillerparams)/sizeof(const char *) );
    }

/************************************************************************/
/*									*/
/*  Emit the bookmark part of an internal link.				*/
/*									*/
/*  As the link is internal, we save both the source of the and its	*/
/*  destination. This makes it possible to replace strange characters,	*/
/*  rather than to escape them to pass them through the PostScript	*/
/*  interpreter and the pdfmark translator.				*/
/*									*/
/************************************************************************/

static int psEmitDestination(	SimpleOutputStream *	sos,
				const MemoryBuffer *	destName )
    {
    int			i;
    const char *	s= utilMemoryBufferGetString( destName );

    for ( i= 0; i < destName->mbSize; s++, i++ )
	{
	if  ( *s == '('	|| *s == ')'		||
	      *s == '<'	|| *s == '>'		||
	      *s == '['	|| *s == ']'		||
	      *s == '{'	|| *s == '}'		||
	      *s == '/'	|| *s == '%'		||
	      isspace( *s ) || ! isascii( *s )	)
	    {
	    if  ( sioOutPutByte( '_', sos ) < 0 )
		{ return -1;	}
	    continue;
	    }

	if  ( sioOutPutByte( *s, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the destination of a pdfmark jump in the Printout.		*/
/*									*/
/************************************************************************/

int psEmitDestPdfmark(		SimpleOutputStream *		sos,
				const AffineTransform2D *	at,
				int				lineTop,
				const MemoryBuffer *		mbRef )
    {
    int			top;
    int			x= 0;
    int			y= lineTop;

    top= AT2_Y( x, y, at );

    sioOutPrintf( sos, "[ /Dest /" );

    psEmitDestination( sos, mbRef );
    sioOutPrintf( sos, "\n" );

    sioOutPrintf( sos, "  /View [ /XYZ null %d null ]\n", top );

    sioOutPrintf( sos, "/DEST pdfmark\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Emit the bookmark part of an external link.				*/
/*									*/
/*  As the link is external, do not replace characters in the link as	*/
/*  we do for internal links and bookmarks: We mut trust the link as	*/
/*  we do not know that we are going to change the bookmark name.	*/
/*									*/
/************************************************************************/

static int psUriLinkDestination(	SimpleOutputStream *	sos,
					const unsigned char *	fileName,
					int			fileSize,
					const MemoryBuffer *	markName )
    {
    const int	sevenBits= 1;

    psPrintString( sos, fileName, fileSize, sevenBits );

    if  ( markName && ! utilMemoryBufferIsEmpty( markName ) )
	{
	if  ( sioOutPutByte( '#', sos ) < 0 )
	    { return -1;	}

	psPrintString( sos, markName->mbBytes, markName->mbSize, sevenBits );
	}

    return 0;
    }

static void psWebLinkDestination(	SimpleOutputStream *	sos,
					const unsigned char *	fileName,
					int			fileSize,
					const MemoryBuffer *	markName )
    {
    sioOutPrintf( sos, "  /Action << /Subtype /URI /URI (" );

    psUriLinkDestination( sos, fileName, fileSize, markName );

    sioOutPrintf( sos, ") >>\n" );

    return;
    }

static void psFileLinkDestMark(	SimpleOutputStream *	sos,
					const MemoryBuffer *	fileName,
					const MemoryBuffer *	markName )
    {
    const int			sevenBits= 1;
    const unsigned char *	file= fileName->mbBytes;
    int				size= fileName->mbSize;

    if  ( size > 5 && ! strncmp( (const char *)file, "file://", 7 ) )
	{ file += 7; size -= 7; }
    else{
	int	offset= 0;

	while( offset < size && isalpha( file[offset] ) )
	    { offset++;	}

	if  ( offset+ 3 < size					&&
	      ! strncmp( (const char *)file+ offset, "://", 3 )	)
	    {
	    psWebLinkDestination( sos, fileName->mbBytes,
					fileName->mbSize, markName );
	    return;
	    }
	}

    sioOutPrintf( sos, "  /Action /Launch /File (" );

    psPrintString( sos, file, size, sevenBits );

    sioOutPrintf( sos, ")\n" );

    if  ( markName && ! utilMemoryBufferIsEmpty( markName ) )
	{
	sioOutPrintf( sos, "  /URI (" );
	psUriLinkDestination( sos, file, size, markName );
	sioOutPrintf( sos, ")\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Emit a 'Hyperlink' style pdfmark. I.E: Make the rectangle of the	*/
/*  link clickable and make it refer to the destination of the link.	*/
/*									*/
/************************************************************************/

void psSourcePdfmark(		SimpleOutputStream *		sos,
				const DocumentRectangle *	drLink,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName )
    {
    sioOutPrintf( sos, "[ /Rect [ %d %d %d %d ]\n",
				drLink->drX0, drLink->drY0,
				drLink->drX1, drLink->drY1 );
    sioOutPrintf( sos, "  /Border [ 0 0 0 ]\n" );

    if  ( ! fileName || utilMemoryBufferIsEmpty( fileName ) )
	{
	if  ( ! markName || utilMemoryBufferIsEmpty( markName ) )
	    { XDEB(markName);	}
	else{
	    sioOutPrintf( sos, "  /Dest /" );

	    psEmitDestination( sos, markName );
	    sioOutPrintf( sos, "\n" );
	    }
	}
    else{
	psFileLinkDestMark( sos, fileName, markName );
	}

    sioOutPrintf( sos, "  /Subtype /Link\n" );
    sioOutPrintf( sos, "/ANN pdfmark\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Emit a 'PageMode' style pdfmark.					*/
/*									*/
/************************************************************************/

int psPageModePdfmark(		SimpleOutputStream *	sos,
				const char *		pageMode )
    {
    sioOutPrintf( sos, "[ /PageMode /%s /DOCVIEW pdfmark\n", pageMode );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit an 'Outline' style pdfmark.					*/
/*									*/
/************************************************************************/

int psOutlinePdfmark(		PrintingState *		ps,
				int			childCount,
				int			closed,
				const MemoryBuffer *	title,
				const MemoryBuffer *	markName )
    {
    if  ( childCount < 0 )
	{ LDEB(childCount); childCount= -childCount;	}
    if  ( closed )
	{ childCount= -childCount;			}

    sioOutPrintf( ps->psSos, "[" );

    if  ( childCount != 0 )
	{ sioOutPrintf( ps->psSos, " /Count %d", childCount );	}

    sioOutPrintf( ps->psSos, " /Dest /" );
    psEmitDestination( ps->psSos, markName );

    sioOutPrintf( ps->psSos, " /Title " );
    psPrintPdfMarkStringValue( ps, title->mbBytes, title->mbSize );

    sioOutPrintf( ps->psSos, " /OUT pdfmark\n" );

    return 0;
    }

