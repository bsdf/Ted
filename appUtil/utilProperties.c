/************************************************************************/
/*									*/
/*  Read a file that can follows the format of an X11 resource file	*/
/*  ir a java properties file into a tree.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"sioFileio.h"
#   include	"utilTree.h"
#   include	"utilProperties.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a file with properties.					*/
/*									*/
/*  1)  Open file.							*/
/*  2)  For all lines in the file.					*/
/*  3)  Skip to first non blank.					*/
/*  4)  Lines with '#' or '!' are comments.				*/
/*  5)  Collect name.							*/
/*  6)  Collect value.							*/
/*  7)  Value size ignoring trailing white space.			*/
/*  8)  Save value and store on tree.					*/
/*									*/
/************************************************************************/

int utilPropertiesReadFile(		void *			tree,
					const MemoryBuffer *	filename )
    {
    int			rval= 0;
    SimpleInputStream *	sis= (SimpleInputStream *)0;
    int			c;

    unsigned char *	name=(unsigned char *)0;
    unsigned char *	value=(unsigned char *)0;
    int			nameSize= 0;
    int			valueSize= 0;

    /*  1  */
    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    /*  2  */
    for (;;)
	{
	int		ns= 0;
	int		vs= 0;
	int		vp= 0;

	char *		savedValue= (char *)0;
	void *		prevValue= (char *)0;

	/*  3  */
	c= sioInGetByte( sis );
	while( isspace( c ) )
	    { c= sioInGetByte( sis ); }
	if  ( c == EOF )
	    { break;	}

	/*  4  */
	if  ( c == '!' || c == '#' )
	    {
	    c= sioInGetByte( sis );
	    while( c != '\n' )
		{
		if  ( c == EOF )
		    { CDEB(c); goto ready;	}
		if  ( c == '\r' )
		    {
		    c= sioInGetByte( sis );
		    if  ( c != '\n' )
			{ CDEB(c); goto ready;	}
		    break;
		    }

		c= sioInGetByte( sis );
		}

	    continue;
	    }

	if  ( name )
	    { name[0]= '\0';	}
	if  ( value )
	    { value[0]= '\0';	}

	/*  5  */
	for (;;)
	    {
	    if  ( c == ':' || c == '=' || isspace( c ) )
		{ break;	}
	    if  ( c == '\r' || c == '\n' || c == EOF )
		{ CDEB(c); rval= -1; goto ready;	}

	    if  ( ns >= nameSize )
		{
		unsigned char *		fresh;

		fresh= (unsigned char *)realloc( name, nameSize+ 30+ 1 );
		if  ( ! fresh )
		    { LXDEB(nameSize,fresh); rval= -1; goto ready;	}
		name= fresh;
		nameSize += 30;
		}

	    name[ns++]= c;
	    name[ns  ]= '\0';
	    c= sioInGetByte( sis );
	    }

	while( isspace( c ) )
	    { c= sioInGetByte( sis );	}
	if  ( c != ':' && c != '=' )
	    { CDEB(c); rval= -1; goto ready;	}
	if  ( ns == 0 )
	    { LSDEB(ns,(char *)name); rval= -1; goto ready; }

	/*  6  */
	c= sioInGetByte( sis );
	while( isspace( c ) )
	    { c= sioInGetByte( sis );	}

	for (;;)
	    {
	    if  ( vs >= valueSize )
		{
		unsigned char *		fresh;

		fresh= (unsigned char *)realloc( value, valueSize+ 50+ 1 );
		if  ( ! fresh )
		    { LXDEB(valueSize,fresh); rval= -1; goto ready;	}
		value= fresh;
		valueSize += 50;
		value[vs]= '\0';
		}

	    if  ( c == '\r' || c == '\n' || c == EOF )
		{ break;	}

	    value[vs++]= c;
	    value[vs  ]= '\0';

	    /*  7  */
	    if  ( ! isspace( c ) )
		{ vp= vs;	}

	    c= sioInGetByte( sis );
	    }

	if  ( c == '\r' )
	    { c= sioInGetByte( sis );	}
	if  ( c != '\n' )
	    { CDEB(c); rval= -1; goto ready;	}

	/*  7  */
	value[vp]= '\0';

	/*  8  */
	savedValue= strdup( (char *)value );
	if  ( ! savedValue )
	    { XDEB(savedValue); rval= -1; goto ready;	}

	if  ( utilTreeStoreValue( tree, &prevValue, (const char **)0,
						(char *)name, savedValue ) )
	    {
	    free( savedValue );
	    SSDEB((char *)name,(char *)value); rval= -1; goto ready;
	    }

	if  ( prevValue )
	    { free( prevValue );	}
	}

  ready:

    if  ( value )
	{ free( value );	}
    if  ( name )
	{ free( name );	}

    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }
