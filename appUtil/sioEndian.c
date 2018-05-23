#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"sioEndian.h"
#   include	"utilEndian.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

int sioEndianPutBeInt32(	long			l,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    if  ( l < 0 )
	{
	NEGBYTES32(b3,b2,b1,b0,l)
	}
    else{
	POSBYTES32(b3,b2,b1,b0,l)
	}

    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b2, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b3, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutLeInt32(	long			l,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    if  ( l < 0 )
	{
	NEGBYTES32(b3,b2,b1,b0,l)
	}
    else{
	POSBYTES32(b3,b2,b1,b0,l)
	}

    if  ( sioOutPutByte( b3, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b2, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutBeInt16(	int			i,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    if  ( i < 0 )
	{
	NEGBYTES16(b1,b0,i)
	}
    else{
	POSBYTES16(b1,b0,i)
	}

    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutLeInt16(	int			i,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    if  ( i < 0 )
	{
	NEGBYTES16(b1,b0,i)
	}
    else{
	POSBYTES16(b1,b0,i)
	}

    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutBeUint32(	unsigned long		ul,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    POSBYTES32(b3,b2,b1,b0,ul)

    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b2, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b3, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutLeUint32(	unsigned long		ul,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1,b2,b3;

    POSBYTES32(b3,b2,b1,b0,ul)

    if  ( sioOutPutByte( b3, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b2, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutBeUint16(	unsigned int		ui,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    POSBYTES16(b1,b0,ui)

    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int sioEndianPutLeUint16(	unsigned int		ui,
				SimpleOutputStream *	sos )
    {
    unsigned char	b0,b1;

    POSBYTES16(b1,b0,ui)

    if  ( sioOutPutByte( b1, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( b0, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/************************************************************************/

int sioEndianGetLeInt16(	SimpleInputStream *	sis )
    {
    int			i;

    unsigned char	b0,b1;

    b1= sioInGetByte( sis );
    b0= sioInGetByte( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE16(b0,b1,i,int)
	}
    else{
	POSVALUE16(b0,b1,i,int)
	}

    return i;
    }

int sioEndianGetBeInt16(	SimpleInputStream *	sis )
    {
    int			i;

    unsigned char	b0,b1;

    b0= sioInGetByte( sis );
    b1= sioInGetByte( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE16(b0,b1,i,int)
	}
    else{
	POSVALUE16(b0,b1,i,int)
	}

    return i;
    }

unsigned int sioEndianGetLeUint16(	SimpleInputStream *	sis )
    {
    unsigned int	i;

    unsigned char	b0,b1;

    b1= sioInGetByte( sis );
    b0= sioInGetByte( sis );

    POSVALUE16(b0,b1,i,unsigned int)

    return i;
    }

unsigned int sioEndianGetBeUint16(	SimpleInputStream *	sis )
    {
    unsigned int	i;

    unsigned char	b0,b1;

    b0= sioInGetByte( sis );
    b1= sioInGetByte( sis );

    POSVALUE16(b0,b1,i,unsigned int)

    return i;
    }

long sioEndianGetLeInt32(	SimpleInputStream *	sis )
    {
    long		l;

    unsigned char	b0,b1,b2,b3;

    b3= sioInGetByte( sis );
    b2= sioInGetByte( sis );
    b1= sioInGetByte( sis );
    b0= sioInGetByte( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE32(b0,b1,b2,b3,l,long)
	}
    else{
	POSVALUE32(b0,b1,b2,b3,l,long)
	}

    return l;
    }

long sioEndianGetBeInt32(	SimpleInputStream *	sis )
    {
    long		l;

    unsigned char	b0,b1,b2,b3;

    b0= sioInGetByte( sis );
    b1= sioInGetByte( sis );
    b2= sioInGetByte( sis );
    b3= sioInGetByte( sis );

    if  ( b0 > 127 )
	{
	NEGVALUE32(b0,b1,b2,b3,l,long)
	}
    else{
	POSVALUE32(b0,b1,b2,b3,l,long)
	}

    return l;
    }

unsigned long sioEndianGetLeUint32(	SimpleInputStream *	sis )
    {
    unsigned long		l;

    unsigned char	b0,b1,b2,b3;

    b3= sioInGetByte( sis );
    b2= sioInGetByte( sis );
    b1= sioInGetByte( sis );
    b0= sioInGetByte( sis );

    POSVALUE32(b0,b1,b2,b3,l,unsigned long)

    return l;
    }

unsigned long sioEndianGetBeUint32(	SimpleInputStream *	sis )
    {
    unsigned long		l;

    unsigned char	b0,b1,b2,b3;

    b0= sioInGetByte( sis );
    b1= sioInGetByte( sis );
    b2= sioInGetByte( sis );
    b3= sioInGetByte( sis );

    POSVALUE32(b0,b1,b2,b3,l,unsigned long)

    return l;
    }

int sioEndianPutLeFloat(	float			f,
				SimpleOutputStream *	sos )
    {
    /* fishy */
    if  ( sizeof(float) == sizeof(unsigned long) )
	{
	unsigned long	ul;
	memcpy( (void *)&ul, (void *)&f, sizeof(float) );
	return sioEndianPutLeUint32( ul, sos );
	}

    /* fishy */
    if  ( sizeof(float) == sizeof(unsigned int) )
	{
	unsigned int	ui;
	memcpy( (void *)&ui, (void *)&f, sizeof(float) );
	return sioEndianPutLeUint32( ui, sos );
	}

    LLLDEB(sizeof(float),sizeof(unsigned long),sizeof(unsigned int));
    return -1;
    }

int sioEndianPutBeFloat(	float			f,
				SimpleOutputStream *	sos )
    {
    /* fishy */
    if  ( sizeof(float) == sizeof(unsigned long) )
	{
	unsigned long	ul;
	memcpy( (void *)&ul, (void *)&f, sizeof(float) );
	return sioEndianPutBeUint32( ul, sos );
	}

    /* fishy */
    if  ( sizeof(float) == sizeof(unsigned int) )
	{
	unsigned int	ui;
	memcpy( (void *)&ui, (void *)&f, sizeof(float) );
	return sioEndianPutBeUint32( ui, sos );
	}

    LLLDEB(sizeof(float),sizeof(unsigned long),sizeof(unsigned int));
    return -1;
    }

float sioEndianGetLeFloat(	SimpleInputStream *	sis )
    {
    unsigned long	ul;
    float		f;

    ul= sioEndianGetLeUint32( sis );

    /* fishy */
    if  ( sizeof(float) ==  sizeof(unsigned long) )
	{
	memcpy( (void *)&f, (void *)&ul, sizeof(float) );
	return f;
	}

    /* fishy */
    if  ( sizeof(float) ==  sizeof(unsigned int) )
	{
	unsigned int ui= ul;

	memcpy( (void *)&f, (void *)&ui, sizeof(float) );
	return f;
	}

    LLLDEB(sizeof(float),sizeof(unsigned long),sizeof(unsigned int));
    return 0;
    }

float sioEndianGetBeFloat(	SimpleInputStream *	sis )
    {
    unsigned long	ul;
    float		f;

    if  ( sizeof(float) != sizeof(unsigned long) )
	{ LLDEB(sizeof(float),sizeof(unsigned long));	}

    ul= sioEndianGetBeUint32( sis );

    /* fishy */
    if  ( sizeof(float) ==  sizeof(unsigned long) )
	{
	memcpy( (void *)&f, (void *)&ul, sizeof(float) );
	return f;
	}

    /* fishy */
    if  ( sizeof(float) ==  sizeof(unsigned int) )
	{
	unsigned int ui= ul;

	memcpy( (void *)&f, (void *)&ui, sizeof(float) );
	return f;
	}

    LLLDEB(sizeof(float),sizeof(unsigned long),sizeof(unsigned int));
    return 0;
    }
