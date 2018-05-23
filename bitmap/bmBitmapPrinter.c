#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<sioHex.h>
#   include	<sioBase85.h>
#   include	<sioFlate.h>

#   include	"bmBitmapPrinter.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine how to print.						*/
/*									*/
/*  Note that bmPsSetBitmapPrinter() has a knowledge of the limitations	*/
/*  of bmPsWriteBitmapData().						*/
/*									*/
/************************************************************************/

int bmPsOpenBitmapPrinter(	BitmapPrinter *			bp,
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				useFilters,
				int				indexedImages )
    {
    bp->bpOutput= (SimpleOutputStream *)0;
    bp->bpHexed= (SimpleOutputStream *)0;
    bp->bpBase85= (SimpleOutputStream *)0;
    bp->bpFlate= (SimpleOutputStream *)0;

    if  ( useFilters )
	{
	const int	gzipEmbedded= 0;

	bp->bpBase85= sioOutBase85Open( sos );

	bp->bpFlate= sioOutFlateOpen( bp->bpBase85, gzipEmbedded );
	bp->bpOutput= bp->bpFlate;
	}
    else{
	const int	wide= 72;
	const int	lastNL= 1;

	bp->bpHexed= sioOutHexOpenFolded( sos, wide, lastNL );
	bp->bpOutput= bp->bpHexed;
	}

    bp->bpUseFilters= useFilters;
    bp->bpIndexedImages= indexedImages;

    return 0;
    }

void bmCloseBitmapPrinter(	BitmapPrinter *		bp )
    {
    if  ( bp->bpFlate )
	{ sioOutClose( bp->bpFlate );	}

    if  ( bp->bpBase85 )
	{ sioOutClose( bp->bpBase85 );	}

    if  ( bp->bpHexed )
	{ sioOutClose( bp->bpHexed );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Print Black/White/Color data in hexadecimal format.			*/
/*									*/
/*  1)  The caller of bmPsWriteShortRow() promises to pass even numbers	*/
/*	of shorts to the routine. So there is no issue about odd	*/
/*	counts.								*/
/*									*/
/************************************************************************/

static int bmPsWriteShortRow(	SimpleOutputStream *	sos,
				int			invert,
				int			count,
				const BmUint16 *	from )
    {
    int			col;
    int			val0;
    int			val1;

    if  ( invert )
	{
	/*  1  */
	for ( col= 0; col+ 1 < count; col += 2 )
	    {
	    val0= ~*(from++);
	    val1= ~*(from++);

	    val1= val1 >> 4;

	    if  ( sioOutPutByte( ( val0 >> 8 ), sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte(
			( val0 & 0xf0 ) | ( ( val1 & 0xf00 ) >> 8 ), sos ) < 0 )
			    { return -1;	}
	    if  ( sioOutPutByte( ( val1 & 0xff ), sos ) < 0 )
		{ return -1;	}
	    }

	if  ( col < count )
	    {
	    val0= ~*(from++);

	    if  ( sioOutPutByte( ( val0 >> 8 ), sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( ( val0 & 0xf0 ), sos ) < 0 )
		{ return -1;	}
	    }
	}
    else{
	/*  1  */
	for ( col= 0; col+ 1 < count; col += 2 )
	    {
	    val0= *(from++);
	    val1= *(from++);

	    val1= val1 >> 4;

	    if  ( sioOutPutByte( ( val0 >> 8 ), sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte(
			( val0 & 0xf0 ) | ( ( val1 & 0xf00 ) >> 8 ), sos ) < 0 )
			    { return -1;	}
	    if  ( sioOutPutByte( ( val1 & 0xff ), sos ) < 0 )
		{ return -1;	}
	    }

	if  ( col < count )
	    {
	    val0= *(from++);

	    if  ( sioOutPutByte( ( val0 >> 8 ), sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( ( val0 & 0xf0 ), sos ) < 0 )
		{ return -1;	}
	    }
	}

    return 0;
    }

static int bmPsWriteByteRow0(	SimpleOutputStream *	sos,
				int			invert,
				int			bytesPerRow,
				const unsigned char *	from )
    {
    int		col;

    if  ( invert )
	{
	for ( col= 0; col < bytesPerRow; col++ )
	    {
	    if  ( sioOutPutByte( ( ~*(from) ) & 0xff, sos ) < 0 )
		{ return -1;	}
	    from++;
	    }
	}
    else{
	for ( col= 0; col < bytesPerRow; col++ )
	    {
	    if  ( sioOutPutByte( *(from) & 0xff, sos ) < 0 )
		{ return -1;	}
	    from++;
	    }
	}

    return 0;
    }

static int bmPsWriteByteRowX(	SimpleOutputStream *	sos,
				int			invert,
				int			originBit,
				int			rightShift,
				int			bytesPerRow,
				const unsigned char *	from )
    {
    int				col;
    unsigned char		val;

    if  ( invert )
	{
	for ( col= 1; col < bytesPerRow; col++ )
	    {
	    val=
		( ~( ( from[0] << originBit )	|
		     ( from[1] >> rightShift )	) ) &0xff;

	    if  ( sioOutPutByte( val, sos ) < 0 )
		{ return -1;	}
	    from++;
	    }

	val= ( ~( from[0] << originBit ) ) & 0xff;
	if  ( sioOutPutByte( val, sos ) < 0 )
	    { return -1;	}
	}
    else{
	for ( col= 1; col < bytesPerRow; col++ )
	    {
	    val=	( from[0] << originBit )	|
			( from[1] >> rightShift )	;

	    if  ( sioOutPutByte( val, sos ) < 0 )
		{ return -1;	}

	    from++;
	    }

	val= from[0] << originBit;
	if  ( sioOutPutByte( val, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

static int bmPsWritePal8Row(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				bytesPerRow,
				const unsigned char *		from )
    {
    int		col;
    const	ColorPalette *	cp= &(bd->bdPalette);

    for ( col= 0; col < bytesPerRow; col++ )
	{
	if  ( sioOutPutByte( cp->cpColors[*from].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[*from].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[*from].rgb8Blue, sos ) < 0 )
	    { return -1;	}

	from++;
	}

    return 0;
    }

static int bmPsWritePal4Row0(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				const unsigned char *		from )
    {
    int		col;
    int		val;
    const	ColorPalette *	cp= &(bd->bdPalette);

    for ( col= 0; col+ 1 < selWidePix; col += 2 )
	{
	val= ( *from >> 4 ) & 0x0f;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
	    { return -1;	}

	val= *from & 0x0f;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
	    { return -1;	}

	from++;
	}

    if  ( selWidePix % 2 )
	{
	val= ( *from >> 4 ) & 0x0f;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

static int bmPsWritePal4RowX(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				bytesPerRow,
				const unsigned char *		from )
    {
    int		col;
    int		val;
    const	ColorPalette *	cp= &(bd->bdPalette);

    val= *from & 0x0f;

    if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
	{ return -1;	}

    from++;

    for ( col= 1; col < bytesPerRow; col++ )
	{
	val= ( *from >> 4 ) & 0x0f;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos) < 0 )
	    { return -1;	}

	val= *from & 0x0f;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos) < 0 )
	    { return -1;	}

	from++;
	}

    val= ( *from >> 4 ) & 0x0f;

    if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos) < 0 )
	{ return -1;	}

    return 0;
    }

static int bmPsWritePal2Row0(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				const unsigned char *		from )
    {
    int		col;
    int		shift;
    const	ColorPalette *	cp= &(bd->bdPalette);

    for ( col= 0; col+ 3 < selWidePix; col += 4 )
	{
	for ( shift= 6; shift >= 0; shift -= 2 )
	    {
	    int		val= ( *from >> shift ) & 0x03;

	    if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
		{ return -1;	}
	    }

	from++;
	}

    shift= 6;
    for ( col= 0; col < selWidePix % 4; shift -= 2, col++ )
	{
	int	val= ( *from >> shift ) & 0x03;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

static int bmPsWritePal1Row0(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				const unsigned char *		from )
    {
    int		col;
    int		shift;
    const	ColorPalette *	cp= &(bd->bdPalette);

    for ( col= 0; col+ 7 < selWidePix; col += 8 )
	{
	for ( shift= 7; shift >= 0; shift-- )
	    {
	    int		val= ( *from >> shift ) & 0x01;

	    if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
		{ return -1;	}
	    }

	from++;
	}

    shift= 7;
    for ( col= 0; col < selWidePix % 8; shift--, col++ )
	{
	int	val= ( *from >> shift ) & 0x01;

	if  ( sioOutPutByte( cp->cpColors[val].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( cp->cpColors[val].rgb8Blue, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

int bmPsWriteBitmapData(	    BitmapPrinter *		bp,
				    const DocumentRectangle *	drSel,
				    const BitmapDescription *	bd,
				    const unsigned char *	inputBuffer )
    {
    SimpleOutputStream *	sos= bp->bpOutput;
    int				indexedImages= bp->bpIndexedImages;
    int				directPixels;

    int				row;

    int				originBit;
    int				originByte;
    int				rightShift;

    int				bytesPerRow;

    const unsigned char *	from;
    int				selWidePix= drSel->drX1- drSel->drX0+ 1;
    int				selHighPix= drSel->drY1- drSel->drY0+ 1;

    const			ColorPalette *	cp= &(bd->bdPalette);

    directPixels= 0;
    if  ( bd->bdColorEncoding == BMcoWHITEBLACK )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoBLACKWHITE )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoRGB && ! bd->bdHasAlpha )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE && indexedImages )
	{ directPixels= 1;	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdBitsPerPixel == 1			&&
	  cp->cpColors[0].rgb8Red == 255		&&
	  cp->cpColors[0].rgb8Green == 255		&&
	  cp->cpColors[0].rgb8Blue == 255		)
	{ directPixels= 1;	}

    if  ( bd->bdBitsPerSample == 16 && directPixels )
	{
	int		invert= bd->bdColorEncoding == BMcoBLACKWHITE;
	int		shortsPerRow;

	originByte= 2* drSel->drX0;
	shortsPerRow= ( selWidePix* bd->bdBitsPerPixel+ 15 )/ 16;

	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWriteShortRow( sos, invert, shortsPerRow,
						    (const BmUint16 *)from );
	    }

	return 0;
	}

    originBit=   ( drSel->drX0*   bd->bdBitsPerPixel )% 8;
    originByte=  ( drSel->drX0*   bd->bdBitsPerPixel )/ 8;
    bytesPerRow= ( selWidePix* bd->bdBitsPerPixel+ 7 )/ 8;
    rightShift= 8- originBit;

    if  ( originBit == 0 && directPixels )
	{
	int	invert= bd->bdColorEncoding == BMcoBLACKWHITE;

	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWriteByteRow0( sos, invert, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB	&&
	  bd->bdHasAlpha			)
	{
	const int	invert= 0;
	unsigned char *	scratch;

	scratch= (unsigned char *)malloc( bd->bdBytesPerRow );
	if  ( ! scratch )
	    { LXDEB(bd->bdBytesPerRow,scratch); return -1;	}

	if  ( bd->bdBitsPerSample == 8 )
	    {
	    for ( row= 0; row < selHighPix; row++ )
		{
		int		col;
		unsigned char *	to= scratch;

		from= inputBuffer+
			    ( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

		for ( col= 0; col < selWidePix; col++ )
		    {
		    if  ( from[3] < 127 )
			{
			*(to++)= 0xff;
			*(to++)= 0xff;
			*(to++)= 0xff;
			from += 4;
			}
		    else{
			*(to++)= *(from++);
			*(to++)= *(from++);
			*(to++)= *(from++);
			from++;
			}
		    }

		bmPsWriteByteRow0( sos, invert, 3* selWidePix, scratch );
		}
	    }
	else{
	    LDEB(bd->bdBitsPerSample); free( scratch ); return -1;
	    }

	free( scratch );
	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoWHITEBLACK	||
	  bd->bdColorEncoding == BMcoBLACKWHITE )
	{
	const int	invert= bd->bdColorEncoding == BMcoBLACKWHITE;

	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWriteByteRowX( sos, invert,
				originBit, rightShift, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 8			)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal8Row( sos, bd, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 4			&&
	  originBit == 0				)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal4Row0( sos, bd, selWidePix, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 4			&&
	  originBit == 4				)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal4RowX( sos, bd, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 2			&&
	  originBit == 0				)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal2Row0( sos, bd, selWidePix, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 1			)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ drSel->drY0 )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal1Row0( sos, bd, selWidePix, from );
	    }

	return 0;
	}

    LDEB(directPixels);
    LDEB(bd->bdBitsPerPixel);
    LLDEB(originBit,bd->bdColorEncoding); return -1;
    }

