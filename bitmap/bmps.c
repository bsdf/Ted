#   include	"bitmapConfig.h"

#   include	<stdio.h>

#   include	"bmBitmapPrinter.h"

#   include	<appDebugon.h>

#   define	MAX_BPS		12

/************************************************************************/
/*									*/
/*  Emit code for the emulation of color images on gray scale printers.	*/
/*									*/
/*  I do not understand this either but it works.			*/
/*									*/
/************************************************************************/

static void bmEpsColorEmulation( SimpleOutputStream *	sos )
    {
    sioOutPutString( ""
"	/bwproc {\n"
"	    rgbproc\n"
"	    dup length 3 idiv string 0 3 0\n"
"	    5 -1 roll {\n"
"		add 2 1 roll 1 sub dup 0 eq {\n"
"		    pop 3 idiv\n"
"		    3 -1 roll\n"
"		    dup 4 -1 roll\n"
"		    dup 3 1 roll\n"
"		    5 -1 roll put\n"
"		    1 add 3 0\n"
"		} { 2 1 roll } ifelse\n"
"	    } forall\n"
"	    pop pop pop\n"
"	} def\n"
"	/colorimage where {pop} {\n"
"	    /colorimage {pop pop /rgbproc exch def {bwproc} image} bind def\n"
"	} ifelse\n", sos );
    }

/************************************************************************/
/*									*/
/*  Return the size of a PostScript string that can hold one row of	*/
/*  data for the bitmap.						*/
/*									*/
/************************************************************************/

int bmPsRowStringSize(		const BitmapDescription *	bd,
				int				pixelsWide,
				int				indexedImages )
    {
    switch( bd->bdColorEncoding )
	{
	case	BMcoWHITEBLACK:
	case	BMcoBLACKWHITE:
	case	BMcoRGB:

	    if  ( bd->bdBitsPerSample == 16 )
		{
		int bpp= MAX_BPS* bd->bdSamplesPerPixel;

		return ( pixelsWide* bpp+ 7 )/ 8;
		}
	    else{
		if  ( bd->bdColorEncoding == BMcoRGB	&&
		      bd->bdHasAlpha			&&
		      bd->bdBitsPerSample == 8		)
		    { return pixelsWide* 3;				}
		else{ return ( pixelsWide* bd->bdBitsPerPixel+ 7 )/ 8;	}
		}

	case	BMcoRGB8PALETTE:

	    if  ( bd->bdBitsPerPixel == 1		&&
		  bd->bdPalette.cpColors[0].rgb8Red == 255	&&
		  bd->bdPalette.cpColors[0].rgb8Green == 255	&&
		  bd->bdPalette.cpColors[0].rgb8Blue == 255	)
		{ return ( pixelsWide+ 7 )/ 8;				}

	    if  ( indexedImages )
		{ return ( pixelsWide* bd->bdBitsPerPixel+ 7 )/ 8;	}
	    else{ return ( pixelsWide* 24+ 7 )/ 8;			}

	default:

	    LDEB(bd->bdColorEncoding);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Print one Encapsulated postsctipt page.				*/
/*									*/
/************************************************************************/

int bmPsPrintBitmap(	SimpleOutputStream *		sos,
			int				level,
			double				xscale,
			double				yscale,
			int				ox,
			int				oy,
			const DocumentRectangle *	drSel,
			int				useFilters,
			int				indexedImages,
			const BitmapDescription *	bd,
			const unsigned char *		buffer	)
    {
    int			rectangleWideTwips;
    int			rectangleHighTwips;
    const int		onWhite= 0;

    DocumentRectangle	drAll;
    drAll.drX0= 0;
    drAll.drY0= 0;
    drAll.drX1= bd->bdPixelsWide- 1;
    drAll.drY1= bd->bdPixelsHigh- 1;

    if  ( ! drSel )
	{ drSel= &drAll;	}

    bmRectangleSizeTwips( &rectangleWideTwips, &rectangleHighTwips, bd,
						drSel->drX1- drSel->drX0+ 1,
						drSel->drY1- drSel->drY0+ 1 );

    xscale= ( xscale* rectangleWideTwips )/ 20;
    yscale= ( yscale* rectangleHighTwips )/ 20;

    return bmPsPrintBitmapImage( sos, level, xscale, yscale, ox, oy, drSel,
			    onWhite, useFilters, indexedImages, bd, buffer );
    }

/************************************************************************/
/*									*/
/*  Write image instructions.						*/
/*									*/
/************************************************************************/

static void bmPsWriteMonoImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdPalette.cpColorCount == 2		)
	{
	const RGB8Color *	rgb8= &(bd->bdPalette.cpColors[1]);

	if  ( rgb8->rgb8Red == rgb8->rgb8Green	&&
	      rgb8->rgb8Red == rgb8->rgb8Blue	)
	    {
	    sioOutPrintf( sos, "%g setgray\n",
					rgb8->rgb8Red/255.0 );
	    }
	else{
	    sioOutPrintf( sos, "%g %g %g setrgbcolor\n",
					rgb8->rgb8Red/255.0,
					rgb8->rgb8Green/255.0,
					rgb8->rgb8Blue/255.0 );
	    }
	}
    else{
	sioOutPrintf( sos, "0 setgray\n" );
	}

    sioOutPrintf( sos, "%u %u", selWidePix, selHighPix );

    sioOutPrintf( sos, " false\n" );

    sioOutPrintf( sos, "[ %u 0 0 -%u 0 %u ]\n",
				selWidePix, selHighPix, selHighPix );

    sioOutPrintf( sos, "%s\n", source );

    sioOutPrintf( sos, "imagemask\n" );

    return;
    }

static void bmPsWriteGrayImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    int		bitsPerSample= bd->bdBitsPerSample;

    if  ( bitsPerSample > MAX_BPS )
	{ bitsPerSample= MAX_BPS;	}

    sioOutPrintf( sos, "%u %u", selWidePix, selHighPix );

    sioOutPrintf( sos, " %d\n", bitsPerSample );

    sioOutPrintf( sos, "[ %u 0 0 -%u 0 %u ]\n",
				selWidePix, selHighPix, selHighPix );

    sioOutPrintf( sos, "%s\n", source );

    sioOutPrintf( sos, "image\n" );

    return;
    }

static void bmPsWriteRgbImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    int		bitsPerSample= bd->bdBitsPerSample;

    if  ( bitsPerSample > MAX_BPS )
	{ bitsPerSample= MAX_BPS;	}

    bmEpsColorEmulation( sos );

    sioOutPrintf( sos, "%u %u %d\n",
			    selWidePix, selHighPix, bitsPerSample );

    sioOutPrintf( sos, "[ %u 0 0 -%u 0 %u ]\n",
			    selWidePix, selHighPix, selHighPix );

    sioOutPrintf( sos, "%s\n", source );
    sioOutPrintf( sos, "false %d colorimage\n", 3 );

    return;
    }

static void bmPsWriteIndexedImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    int		i;

    /*  Done by caller
    sioOutPrintf( sos, "gsave\n" );
    */

    sioOutPrintf( sos, "[ /Indexed /DeviceRGB %d\n",
					    bd->bdPalette.cpColorCount- 1 );
    sioOutPrintf( sos, "<\n" );

    for ( i= 0; i < bd->bdPalette.cpColorCount; i++ )
	{
	sioOutPrintf( sos, " %02x%02x%02x\n", 
				bd->bdPalette.cpColors[i].rgb8Red,
				bd->bdPalette.cpColors[i].rgb8Green,
				bd->bdPalette.cpColors[i].rgb8Blue );
	}

    sioOutPrintf( sos, ">\n" );

    sioOutPrintf( sos, "] setcolorspace\n" );

    sioOutPrintf( sos, "10 dict\n" );
    sioOutPrintf( sos, "  dup /ImageType 1 put\n" );
    sioOutPrintf( sos, "  dup /Width %u put\n",
					selWidePix );
    sioOutPrintf( sos, "  dup /Height %u put\n",
					selHighPix );
    sioOutPrintf( sos, "  dup /ImageMatrix [ %u 0 0 -%u 0 %u ] put\n",
					selWidePix, selHighPix, selHighPix );
    sioOutPrintf( sos, "  dup /BitsPerComponent %d put\n",
					bd->bdBitsPerPixel );
    sioOutPrintf( sos, "  dup /DataSource %s put\n",
					source  );
    sioOutPrintf( sos, "  dup /Decode [ 0 %d ] put\n",
					( 1 << bd->bdBitsPerPixel )- 1  );
    sioOutPrintf( sos, "image\n" );

    /*  Done by caller after image data
    sioOutPrintf( sos, "grestore\n" );
    */

    return;
    }

void bmPsWriteImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				onWhite,
				int				selWidePix,
				int				selHighPix,
				const char *			source,
				int				indexedImages )
    {
    int		bitsPerSample= bd->bdBitsPerSample;

    if  ( bitsPerSample > MAX_BPS )
	{ bitsPerSample= MAX_BPS;	}

    switch( bd->bdColorEncoding )
	{
	case BMcoWHITEBLACK:
	case BMcoBLACKWHITE:

	    if  ( bd->bdBitsPerSample == 1 )
		{
		bmPsWriteMonoImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		}
	    else{
		bmPsWriteGrayImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		}

	    break;
	
	case BMcoRGB:

	    bmPsWriteRgbImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
	    break;

	case BMcoRGB8PALETTE:

	    if  ( onWhite				&&
	    	  bd->bdBitsPerPixel == 1		&&
		  bd->bdPalette.cpColors[0].rgb8Red == 255	&&
		  bd->bdPalette.cpColors[0].rgb8Green == 255	&&
		  bd->bdPalette.cpColors[0].rgb8Blue == 255	)
		{
		bmPsWriteMonoImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		break;
		}

	    if  ( indexedImages )
		{
		bmPsWriteIndexedImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		break;
		}

	    bmPsWriteRgbImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	}

    return;
    }

int bmPsPrintBitmapImage(	SimpleOutputStream *		sos,
				int				level,
				double				xscale,
				double				yscale,
				int				ox,
				int				oy,
				const DocumentRectangle *	drSel,
				int				onWhite,
				int				useFilters,
				int				indexedImages,
				const BitmapDescription *	bd,
				const unsigned char *		buffer )
    {
    BitmapPrinter	bp;

    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE )
	{ indexedImages= 0;	}

    sioOutPrintf( sos, "gsave 10 dict begin %% {> raster image\n" );

    if  ( ox != 0 || oy != 0 || xscale != 1.0 || yscale != 1.0 )
	{
	if  ( ox != 0 || oy != 0 )
	    { sioOutPrintf( sos, "%d %d translate ", ox, oy );	}
	if  ( xscale != 1.0 || yscale != 1.0 )
	    { sioOutPrintf( sos, "%f %f scale ", xscale, yscale ); }

	sioOutPrintf( sos, "\n" );
	}

    if  ( useFilters )
	{
#	ifdef	USE_LZW
	bmPsWriteImageInstructions( sos, bd, onWhite,
		drSel->drX1- drSel->drX0+ 1, drSel->drY1- drSel->drY0+ 1,
		"currentfile /ASCII85Decode filter /LZWDecode filter",
		indexedImages );
#	else
	bmPsWriteImageInstructions( sos, bd, onWhite,
		drSel->drX1- drSel->drX0+ 1, drSel->drY1- drSel->drY0+ 1,
		"currentfile /ASCII85Decode filter /FlateDecode filter",
		indexedImages );
#	endif
	}
    else{
	int bytesPerRow= bmPsRowStringSize( bd,
				drSel->drX1- drSel->drX0+ 1, indexedImages );
	if  ( bytesPerRow < 0 )
	    { LDEB(bytesPerRow); return -1;	}

	sioOutPrintf( sos, "/line %d string def\n", bytesPerRow );

	bmPsWriteImageInstructions( sos, bd, onWhite,
		    drSel->drX1- drSel->drX0+ 1, drSel->drY1- drSel->drY0+ 1,
		    "{ currentfile line readhexstring pop } bind",
		    indexedImages );
	}

    bmPsOpenBitmapPrinter( &bp, sos, bd, useFilters, indexedImages );

    if  ( bmPsWriteBitmapData( &bp, drSel, bd, buffer ) )
	{ LDEB(1); return -1;	}

    bmCloseBitmapPrinter( &bp );

    sioOutPrintf( sos, "end grestore %% }< raster image\n");

    return 0;
    }

