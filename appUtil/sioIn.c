/************************************************************************/
/*									*/
/*  Simplification of stdio: Input.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioGeneral.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Input...								*/
/*									*/
/************************************************************************/

int sioInFillBuffer(	SimpleInputStream *	sis )
    {
    int		got;

    if  ( sis->sisN > 0 )
	{ sis->sisN--; return *(sis->sisP++);	}

    got= (*sis->sisReadBytes)( sis->sisPrivate, sis->sisBuffer, SIOsizBUF );

    if  ( got <= 0 )
	{ return EOF;	}

    sis->sisBytesRead += got;
    sis->sisN= got;
    sis->sisP= sis->sisBuffer;

    sis->sisN--; return *(sis->sisP++);
    }

int sioInUngetLastRead(	SimpleInputStream *	sis )
    {
    if  ( sis->sisP == sis->sisBuffer )
	{ LLDEB(sis->sisN,SIOsizBUF); return -1;	}

    sis->sisN++; sis->sisP--;

    return 0;
    }

int sioInClose(	SimpleInputStream *		sis )
    {
    int		ret;

    ret= (*sis->sisClose)( sis->sisPrivate );

    free( sis );

    return ret;
    }

long sioInGetBytesRead(			SimpleInputStream *	sis )
    { return sis->sisBytesRead- sis->sisN;	}

SimpleInputStream * sioInOpen(	void *			specific,
				SIOinREADBYTES		readBytes,
				SIOinCLOSE		closeIt )
    {
    SimpleInputStream *	sis;

    sis= (SimpleInputStream *)malloc( sizeof(SimpleInputStream) );
    if  ( ! sis )
	{ XDEB(sis); return sis;	}

    sis->sisBytesRead= 0;
    sis->sisN= 0;
    sis->sisP= sis->sisBuffer;
    sis->sisPrivate= specific;

    sis->sisReadBytes= readBytes;
    sis->sisClose= closeIt;

    return sis;
    }

char * sioInGetString(	char *			s,
			int			size,
			SimpleInputStream *	sis )
    {
    int		c;
    char *	to= s;

    if  ( size == 0 )
	{ LDEB(size); return (char *)0;		}
    if  ( size == 1 )
	{ LDEB(size); *s= '\0'; return s;	}

    c= sioInGetByte( sis );
    if  ( c == EOF )
	{ return (char *)0;	}

    while( size-- > 1 )
	{
	*(to++)= c;

	if  ( c == '\n' )
	    { break;	}

	c= sioInGetByte( sis );
	if  ( c == EOF )
	    { break;	}
	}

    *to= '\0';

    return s;
    }

int sioInReadBytes(	SimpleInputStream *	sis,
			unsigned char *		buf,
			int			count )
    {
    int		done= 0;

    while( done < count )
	{
	int	todo;
	int	c;

	c= sioInGetByte( sis );

	if  ( c == EOF )
	    { break;	}

	*(buf++)= c;
	done++;

	todo= count- done;
	if  ( todo > sis->sisN )
	    { todo=  sis->sisN;	}

	memcpy( buf, sis->sisP, todo );

	buf += todo; sis->sisP += todo; sis->sisN -= todo;
	done += todo;
	}

    return done;
    }

