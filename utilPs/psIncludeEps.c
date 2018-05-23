/************************************************************************/
/*									*/
/*  Include EPS files in PostScript output.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"psPrint.h"
#   include	<geo2DInteger.h>
#   include	<sioFileio.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Define procedures to Begin/End the inclusion of an EPS graphic.	*/
/*									*/
/*  *)  Copied directly from the EPS spec.				*/
/*  1)  I spent some time wondering why the 'begin' operator was not	*/
/*	matched by an 'end' operator. The matching end is in the loop	*/
/*	as 'op_count' is remembered before the copy of 'userdict' is	*/
/*	pushed.								*/
/*									*/
/************************************************************************/

static const char * appBeginEPSF[]=
    {
    "/BeginEPSF",
    "    {",
    "    /b4_Inc_state save def          % Save state for cleanup",
    "    /dict_count countdictstack def  % Count objects on dict stack",
    "    /op_count count 1 sub def       % Count objects on operand stack",
	 /*  1  */
    "    userdict begin                  % Push userdict on dict stack",
    "    /showpage { } def               % Redefine showpage, { } = null proc",
    "    0 setgray 0 setlinecap          % Prepare graphics state",
    "    1 setlinewidth 0 setlinejoin",
    "    10 setmiterlimit [ ] 0 setdash newpath",
    "    /languagelevel where            % If level not equal to 1 then",
    "    { pop languagelevel             % set strokeadjust and",
    "        1 ne                        % overprint to their defaults.",
    "            { false setstrokeadjust false setoverprint",
    "            } if",
    "        } if",
    "    } bind def",
    };

static const char * appEndEPSF[]=
    {
    "/EndEPSF",
    "    {",
    "    count op_count sub {pop} repeat % Clean up stacks",
	 /*  1  */
    "    countdictstack dict_count sub {end} repeat",
    "    b4_Inc_state restore",
    "    } bind def",
    };

void psDefineEpsProcs(	SimpleOutputStream *		sos )
    {
    psDefineProcedure( sos, appBeginEPSF,
				sizeof(appBeginEPSF)/sizeof(const char *) );

    psDefineProcedure( sos, appEndEPSF,
				sizeof(appEndEPSF)/sizeof(const char *) );

    return;
    }

void psBeginEpsObject(		SimpleOutputStream *		sos,
				const DocumentRectangle *	drTo,
				const DocumentRectangle *	drBBox,
				const char *			file )
    {
    const int		sevenBits= 1;
    AffineTransform2D	at;

    if  ( geoAffineTransformForTriangles( &at,
				    drBBox->drX0, drBBox->drY0,
				    drBBox->drX0, drBBox->drY1,
				    drBBox->drX1, drBBox->drY0,
				    drTo->drX0, drTo->drY0,
				    drTo->drX0, drTo->drY1,
				    drTo->drX1, drTo->drY0 ) )
	{ LDEB(1); return;	}

    if  ( ! file )
	{ file= "?-?";	}

    sioOutPrintf( sos, "BeginEPSF\n" );

    sioOutPrintf( sos, "newpath %d %d moveto ",	drTo->drX0, drTo->drY0 );
    sioOutPrintf( sos, "%d %d lineto ",		drTo->drX0, drTo->drY1 );
    sioOutPrintf( sos, "%d %d lineto ",		drTo->drX1, drTo->drY1 );
    sioOutPrintf( sos, "%d %d lineto ",		drTo->drX1, drTo->drY0 );
    sioOutPrintf( sos, "closepath clip\n" );

    psTransformMatrix( sos, &at );
    sioOutPrintf( sos, " concat\n" );

    sioOutPrintf( sos, "%%%%BeginDocument: (" );
    psPrintString( sos, (unsigned char *)file, strlen( file ), sevenBits );
    sioOutPrintf( sos, ")\n" );

    return;
    }


void psEndEpsObject(	SimpleOutputStream *		sos )
    {
    sioOutPrintf( sos, "%%%%EndDocument\n" );
    sioOutPrintf( sos, "EndEPSF\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Copy an eps file and extract the bounding box.			*/
/*									*/
/*  Validation is noly performed at the end after the file is copied.	*/
/*  If the file is successfully copied but does not look like an EPS	*/
/*  file we return 1.							*/
/*									*/
/************************************************************************/

int psSaveEpsFile(	SimpleOutputStream *	sos,
			DocumentRectangle *	drBBox,
			const MemoryBuffer *	filename )
    {
    int				gotBBox= 0;
    int				gotEpsHeader= 0;

    char			line[512+ 2];

    SimpleInputStream *		sis= (SimpleInputStream *)0;

    int				n;

    int				llx= 0, lly= 0, urx= 0, ury= 0;
    int				i1, i2, i3, i4;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); return -1;	}

    if  ( ! sioInGetString( line, 512+ 1, sis ) )
	{ LDEB(512); sioInClose( sis ); return -1; }
    line[512+ 1]= '\0';

    n= sscanf( line, "%%!PS-Adobe-%d.%d EPSF-%d.%d", &i1, &i2, &i3, &i4 );
    if  ( n == 4 )
	{ gotEpsHeader= 1;	}
    else{ SLDEB(line,n);	}

    sioOutPutString( line, sos );

    while( ! strchr( line, '\n' ) )
	{
	if  ( ! sioInGetString( line, 512+ 1, sis ) )
	    { break;	}
	line[512+ 1]= '\0';

	sioOutPutString( line, sos );
	}

    while( sioInGetString( line, 512+ 1, sis ) )
	{
	line[512+ 1]= '\0';
	sioOutPutString( line, sos );

	if  ( ! gotBBox )
	    {
	    n= sscanf( line, "%%%%BoundingBox: %d %d %d %d",
							&i1, &i2, &i3, &i4 );
	    if  ( n == 4 )
		{ llx= i1; lly= i2; urx= i3; ury= i4; gotBBox= 1; }
	    }

	while( ! strchr( line, '\n' ) )
	    {
	    if  ( ! sioInGetString( line, 512+1, sis ) )
		{ break;	}
	    line[512+ 1]= '\0';

	    sioOutPutString( line, sos );
	    }
	}

    sioInClose( sis );

    if  ( ! gotEpsHeader || ! gotBBox )
	{ LLDEB(gotEpsHeader,gotBBox); return 1;	}

    drBBox->drX0= llx;
    drBBox->drX1= urx;
    drBBox->drY0= lly;
    drBBox->drY1= ury;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include an eps file removing (DSC)comments and empty lines.		*/
/*									*/
/************************************************************************/

int psIncludeEpsFile(	SimpleOutputStream *	sos,
				SimpleInputStream *	sis )
    {
    char	line[512+1];

    while( sioInGetString( line, 512, sis ) )
	{
	int		emit= 1;
	const char *	s= line;

	line[512]= '\0';

	while( isspace( *s ) )
	    { s++;	}

	if  ( ! *s || *s == '%' )
	    { emit= 0;	}

	if  ( emit )
	    { sioOutPutString( line, sos ); }

	while( ! strchr( line, '\n' ) )
	    {
	    if  ( ! sioInGetString( line, 512, sis ) )
		{ break;	}
	    line[512]= '\0';

	    if  ( emit )
		{ sioOutPutString( line, sos ); }
	    }
	}
    return 0;
    }
