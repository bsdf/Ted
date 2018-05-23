/************************************************************************/
/*									*/
/*  Obtain the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdio.h>

#   include	<sioMemory.h>
#   include	<sioFileio.h>
#   include	<sioHex.h>
#   include	<bitmap.h>
#   include	<bmEmfIo.h>
#   include	<geoUnits.h>

#   include	<appDebugon.h>
#   include	"docObject.h"
#   include	"docObjectProperties.h"

/************************************************************************/
/*									*/
/*  Read a metafile object from file.					*/
/*									*/
/************************************************************************/

int docReadEmfObject(		InsertedObject *	io,
				const MemoryBuffer *	filename )
    {
    EmfHeader			eh;
    int				rval= 0;

    MemoryBuffer		mb;

    SimpleInputStream *		sisIn= (SimpleInputStream *)0;
    SimpleOutputStream *	sosMem= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosMeta= (SimpleOutputStream *)0;

    PictureProperties *		pip= &(io->ioPictureProperties);
    const DocumentRectangle *	dr;

    utilInitMemoryBuffer( &mb );

    sisIn= sioInFileioOpen( filename );
    if  ( ! sisIn )
	{ XDEB(sisIn); rval= -1; goto ready;	}

    sosMem= sioOutMemoryOpen( &mb );
    if  ( ! sosMem )
	{ XDEB(sosMem); rval= -1; goto ready;	}

    sosMeta= sioOutHexOpen( sosMem );
    if  ( ! sosMeta )
	{ XDEB(sosMeta); rval= -1; goto ready;	}

    if  ( bmMetaReadEmfHeader( &eh, sisIn ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    dr= &(eh.ehFrame);

    if  ( dr->drX1 > dr->drX0 )
	{ pip->pipTwipsWide= ( TWIPS_PER_M* ( dr->drX1- dr->drX0 ) )/ 100000;	}
    else{ pip->pipTwipsWide= ( TWIPS_PER_M* ( dr->drX0- dr->drX1 ) )/ 100000;	}

    if  ( dr->drY1 > dr->drY0 )
	{ pip->pipTwipsHigh= ( TWIPS_PER_M* ( dr->drY1- dr->drY0 ) )/ 100000;	}
    else{ pip->pipTwipsHigh= ( TWIPS_PER_M* ( dr->drY0- dr->drY1 ) )/ 100000;	}

    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    dr= &(eh.ehBounds);
    if  ( dr->drX1 > dr->drX0 )
	{ pip->pip_xWinExt= dr->drX1- dr->drX0;	}
    else{ pip->pip_xWinExt= dr->drX0- dr->drX1;	}

    if  ( dr->drY1 > dr->drY0 )
	{ pip->pip_yWinExt= dr->drY1- dr->drY0;	}
    else{ pip->pip_yWinExt= dr->drY0- dr->drY1;	}

    if  ( bmMetaWriteEmfHeader( &eh, sosMeta ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    for (;;)
	{
	unsigned char		buf[4096];
	int			done;

	done= sioInReadBytes( sisIn, buf, 4096 );
	if  ( done < 1 )
	    { break;	}

	if  ( sioOutWriteBytes( sosMeta, buf, done ) != done )
	    { LDEB(done); rval= -1; goto ready;	}
	}

     sioOutClose( sosMeta ); sosMeta= (SimpleOutputStream *)0; /*flush,steal*/
     sioOutClose( sosMem ); sosMem= (SimpleOutputStream *)0; /*flush,steal*/

    io->ioObjectData= mb; utilInitMemoryBuffer( &mb ); /* steal */
    io->ioKind= DOCokPICTEMFBLIP;
    pip->pipType= DOCokPICTEMFBLIP;
    pip->pipMapMode= 8; /*MM_ANISOTROPIC*/

  ready:

    utilCleanMemoryBuffer( &mb );

    if  ( sosMeta )
	{ sioOutClose( sosMeta );	}
    if  ( sosMem )
	{ sioOutClose( sosMem );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

