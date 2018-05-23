/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in UTF16 format 	*/
/*  while the program communicates in UTF8 format.			*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioUtf8_16.h"
#   include	"uniUtf8.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Free allocated resource.						*/
/*									*/
/************************************************************************/

static int sioUtfClose( void *	voidx )
    {
    free( voidx );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/************************************************************************/

typedef struct UtfInputStream
    {
    SimpleInputStream *		uisSisUtf16;
    int				uisSwab;
    int				uisEndian;
    int				uisTryBom;

    char			uisLeft[6+1];
    int				uisLeftCount;
    } UtfInputStream;

static int sioInUtf16ReadBytes(	void *		voiduis,
				unsigned char *	buffer,
				unsigned int	count )
    {
    UtfInputStream *		uis= (UtfInputStream *)voiduis;
    SimpleInputStream *		sisUtf16= uis->uisSisUtf16;
    int				done= 0;

    if  ( uis->uisTryBom )
	{
	int			b0= sioInGetByte( sisUtf16 );
	int			b1= sioInGetByte( sisUtf16 );
	unsigned short		one= 1;

	int			machineBE= ! *((unsigned char *)&one);

	if  ( b0 == EOF )
	    { return 0;		}
	if  ( b1 == EOF )
	    { XDEB(b1); return -1;	}

	if  ( b0 == 0xfe && b1 == 0xff )
	    {
	    uis->uisSwab= ! machineBE;

	    if  ( uis->uisEndian < 0 )
		{ LXXDEB(uis->uisEndian,b0,b1);	}
	    }
	else{
	    if  ( b0 == 0xff && b1 == 0xfe )
		{
		uis->uisSwab= machineBE;

		if  ( uis->uisEndian > 0 )
		    { LXXDEB(uis->uisEndian,b0,b1);	}
		}
	    else{
		int	step;

		if  ( uis->uisEndian == 0 )
		    { LXXDEB(uis->uisEndian,b0,b1); return -1;	}

		uis->uisSwab=	( uis->uisEndian > 0 && ! machineBE )	||
				( uis->uisEndian < 0 &&   machineBE )	;

		if  ( uis->uisSwab )
		    { step= uniPutUtf8( uis->uisLeft, 256* b1+ b0 );	}
		else{ step= uniPutUtf8( uis->uisLeft, 256* b0+ b1 );	}

		if  ( step < 1 )
		    { LDEB(step); return -1;	}
		uis->uisLeftCount= step;
		}
	    }

	uis->uisTryBom= 0;
	}

    if  ( uis->uisLeftCount > 0 )
	{
	if  ( count < uis->uisLeftCount )
	    {
	    memcpy( buffer, uis->uisLeft, count );
	    uis->uisLeftCount -= count;
	    memmove( uis->uisLeft, uis->uisLeft+ count, uis->uisLeftCount );
	    return count;
	    }
	else{
	    memcpy( buffer, uis->uisLeft, uis->uisLeftCount );
	    done= uis->uisLeftCount;
	    uis->uisLeftCount= 0;
	    }
	}

    while( done < count )
	{
	int		b0= sioInGetByte( sisUtf16 );
	int		b1= sioInGetByte( sisUtf16 );
	int		step;

	if  ( b0 == EOF )
	    { break;		}
	if  ( b1 == EOF )
	    { XDEB(b1); return -1;	}

	if  ( uis->uisSwab )
	    { step= uniPutUtf8( uis->uisLeft, 256* b1+ b0 );	}
	else{ step= uniPutUtf8( uis->uisLeft, 256* b0+ b1 );	}
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	if  ( done+ step <= count )
	    {
	    memcpy( buffer+ done, uis->uisLeft, step );
	    done += step;
	    continue;
	    }
	else{
	    int	left= done+ step- count;

	    memcpy( buffer+ done, uis->uisLeft, count- done );
	    memmove( uis->uisLeft, uis->uisLeft+ count- done, left );
	    uis->uisLeftCount= left;
	    done= count;
	    break;
	    }
	}

    return done;
    }

SimpleInputStream * sioInUtf8_16Open(
				    SimpleInputStream * 	sisUtf16,
				    int				endian )
    {
    UtfInputStream *		uis= (UtfInputStream *)malloc( sizeof(UtfInputStream) );
    SimpleInputStream *		sisUtf8;

    if  ( ! uis )
	{ XDEB(uis); return (SimpleInputStream *)0;	}

    uis->uisSisUtf16= sisUtf16;
    uis->uisSwab= 0;
    uis->uisEndian= endian;
    uis->uisTryBom= 1;
    memset( uis->uisLeft, 0, sizeof(uis->uisLeft) );
    uis->uisLeftCount= 0;

    sisUtf8= sioInOpen( (void *)uis, sioInUtf16ReadBytes, sioUtfClose );

    if  ( ! sisUtf8 )
	{ XDEB(sisUtf8); free( uis ); return (SimpleInputStream *)0; }

    return sisUtf8;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

typedef struct UtfOutputStream
    {
    SimpleOutputStream *	uosSosUtf16;
    int				uosBom;
    int				uosSwab;

    unsigned char		uosLeft[6+1];
    int				uosLeftCount;
    } UtfOutputStream;

static int sioOutUtf16Emit(		SimpleOutputStream *	sosUtf16,
					int			swab,
					const unsigned char *	buffer,
					int			count )
    {
    int		done= 0;

    while( done < count )
	{
	unsigned short	utf16;
	int		step= uniGetUtf8( &utf16, (char *)buffer+ done );

	if  ( step < 1 )
	    { break;	}

	if  ( swab )
	    {
	    if  ( sioOutPutByte( ( utf16 >> 0 ) & 0xff, sosUtf16 ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( ( utf16 >> 8 ) & 0xff, sosUtf16 ) < 0 )
		{ return -1;	}
	    }
	else{
	    if  ( sioOutPutByte( ( utf16 >> 8 ) & 0xff, sosUtf16 ) < 0 )
		{ return -1;	}
	    if  ( sioOutPutByte( ( utf16 >> 0 ) & 0xff, sosUtf16 ) < 0 )
		{ return -1;	}
	    }

	done += step;
	}

    return done;
    }

static int sioOutUtf16WriteBytes(	void *			voiduos,
					const unsigned char *	buffer,
					int			count )
    {
    UtfOutputStream *		uos= (UtfOutputStream *)voiduos;
    SimpleOutputStream *	sosUtf16= uos->uosSosUtf16;
    int				done= 0;
    int				step;

    if  ( uos->uosBom )
	{
	unsigned short	utf16= 0xfeff;

	if  ( uos->uosSwab )
	    {
	    if  ( sioOutPutByte( ( utf16 >> 0 ) & 0xff, sosUtf16 ) )
		{ return -1;	}
	    if  ( sioOutPutByte( ( utf16 >> 8 ) & 0xff, sosUtf16 ) )
		{ return -1;	}
	    }
	else{
	    if  ( sioOutPutByte( ( utf16 >> 8 ) & 0xff, sosUtf16 ) )
		{ return -1;	}
	    if  ( sioOutPutByte( ( utf16 >> 0 ) & 0xff, sosUtf16 ) )
		{ return -1;	}
	    }

	uos->uosBom= 0;
	}

    if  ( uos->uosLeftCount > 0 )
	{
	int	got= uos->uosLeftCount;
	int	cnt= 0;

	while( got < sizeof(uos->uosLeft)- 1 && cnt < count )
	    { uos->uosLeft[got++]= buffer[cnt++];	}
	uos->uosLeft[got]= '\0';

	step= sioOutUtf16Emit( sosUtf16, uos->uosSwab, uos->uosLeft, got );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}

	if  ( step < uos->uosLeftCount )
	    {
	    if  ( cnt < count )
		{ LLDEB(cnt,count); return -1;	}

	    uos->uosLeftCount= got;
	    uos->uosLeft[got]= '\0';
	    }
	else{
	    done= step- uos->uosLeftCount;
	    uos->uosLeftCount= 0;
	    }
	}

    step= sioOutUtf16Emit( sosUtf16, uos->uosSwab, buffer+ done, count- done );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    if  ( done < count )
	{
	int	left= count- done;

	if  ( left >= sizeof(uos->uosLeft) )
	    { LLDEB(left,sizeof(uos->uosLeft)); return -1;	}

	memcpy( uos->uosLeft, buffer+ done, left );
	uos->uosLeftCount= left;
	uos->uosLeft[left]= '\0';
	}

    return count;
    }

SimpleOutputStream * sioOutUtf8_16Open(
			    SimpleOutputStream *	sosUtf16,
			    int				endian,
			    int				bom )
    {
    UtfOutputStream *		uos= (UtfOutputStream *)malloc( sizeof(UtfOutputStream) );
    SimpleOutputStream *	sosUtf8;
    unsigned short		one= 1;

    if  ( ! uos )
	{ XDEB(uos); return (SimpleOutputStream *)0;	}

    uos->uosSosUtf16= sosUtf16;
    uos->uosBom= bom;
    uos->uosSwab=	( endian > 0 &&   *((unsigned char *)&one) )	||
			( endian < 0 && ! *((unsigned char *)&one) )	;
    memset( uos->uosLeft, 0, sizeof(uos->uosLeft) );
    uos->uosLeftCount= 0;

    sosUtf8= sioOutOpen( (void *)uos, sioOutUtf16WriteBytes, sioUtfClose );

    if  ( ! sosUtf8 )
	{ XDEB(sosUtf8); free( uos ); return (SimpleOutputStream *)0; }

    return sosUtf8;
    }

