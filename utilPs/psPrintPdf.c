/************************************************************************/
/*									*/
/*  Print Pdf through ghostscript.					*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<utilMemoryBufferPrintf.h>
#   include	"psPrint.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print Pdf through ghostscript.					*/
/*									*/
/************************************************************************/

int psSetToPdfCommand(	MemoryBuffer *			command,
			int				pageTwipsWide,
			int				pageTwipsHigh,
			const MemoryBuffer *		filename )
    {
    if  ( filename && ! utilMemoryBufferIsEmpty( filename ) )
	{
	utilMemoryBufferPrintf( command,
	    "ps2pdf -dDEVICEWIDTHPOINTS=%d -dDEVICEHEIGHTPOINTS=%d - '%s'",
				( pageTwipsWide+ 19)/20,
				( pageTwipsHigh+ 19)/20,
				utilMemoryBufferGetString( filename ) );
	}
    else{
	utilMemoryBufferPrintf( command,
	    "ps2pdf -dDEVICEWIDTHPOINTS=%d -dDEVICEHEIGHTPOINTS=%d - -",
				( pageTwipsWide+ 19)/20,
				( pageTwipsHigh+ 19)/20 );
	}

    return 0;
    }

