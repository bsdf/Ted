#   include	"bitmapConfig.h"

#   include	"bmcolor.h"
#   include	<stdlib.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Color administration.						*/
/*									*/
/************************************************************************/

void bmSetCalculatedShifts(	ColorAllocator *	ca,
				unsigned long		redMask,
				unsigned long		greenMask,
				unsigned long		blueMask )
    {
    ca->caRedPixelShift= 0;
    while( ! ( redMask & 0x1 ) )
	{ ca->caRedPixelShift++; redMask= redMask >> 1;	}
    ca->caRedMask= redMask;
    ca->caRedApproxShift= 8;
    while( redMask & 0x1 )
	{ ca->caRedApproxShift--; redMask= redMask >> 1;	}

    ca->caGreenPixelShift= 0;
    while( ! ( greenMask & 0x1 ) )
	{ ca->caGreenPixelShift++; greenMask= greenMask >> 1;	}
    ca->caGreenMask= greenMask;
    ca->caGreenApproxShift= 8;
    while( greenMask & 0x1 )
	{ ca->caGreenApproxShift--; greenMask= greenMask >> 1;	}

    ca->caBluePixelShift= 0;
    while( ! ( blueMask & 0x1 ) )
	{ ca->caBluePixelShift++; blueMask= blueMask >> 1;	}
    ca->caBlueMask= blueMask;
    ca->caBlueApproxShift= 8;
    while( blueMask & 0x1 )
	{ ca->caBlueApproxShift--; blueMask= blueMask >> 1;	}

    return;
    }

void bmCleanColorAllocator(	ColorAllocator *	ca )
    {
    if  ( ca->caSystemCleanup )
	{ (*ca->caSystemCleanup)( ca ); }

    if  ( ca->caColors )
	{ free( ca->caColors ); ca->caColors= (AllocatorColor *)0;	}

    ca->caColorCount= 0;

    return;
    }

void bmInitColorAllocator(	ColorAllocator *	ca )
    {
    int		i;

    ca->caColorCount= 0;
    ca->caColors= (AllocatorColor *)0;

    for ( i= 0; i < 64; i++ )
	{ ca->ca222Colors[i].acAllocated= AC_UNALLOCATED;	}

    ca->caAllocationType= CA_UNKNOWN;
    ca->caSystemPrivate= (void *)0;
    ca->caSystemAllocator= (SystemAllocator)0;
    ca->caSystemCleanup= (SystemCleanup)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Find a color that is already allocated and that will do when	*/
/*  else helps.								*/
/*									*/
/************************************************************************/

int bmFindNearestColorRgb(	AllocatorColor *	acRet,
				const ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    int			d;
    int			col;

    for ( d= 0; d < 128; d++ )
	{
	const AllocatorColor *	ac;

	ac= ca->ca222Colors;
	for ( col= 0; col < 64; ac++, col++ )
	    {
	    if  ( ac->acAllocated == AC_UNALLOCATED )
		{ continue;	}

	    if  ( ac->acRed	> 256* ( r- d )		&&
		  ac->acRed	< 256* ( r+ d )		&&
		  ac->acGreen	> 256* ( g- d )		&&
		  ac->acGreen	< 256* ( g+ d )		&&
		  ac->acBlue	> 256* ( b- d )		&&
		  ac->acBlue	< 256* ( b+ d )		)
		{
		*acRet= *ac;
		acRet->acAllocated= AC_COPIED;
		return 0;
		}
	    }

	ac= ca->caColors;
	for ( col= 0; col < ca->caColorCount; ac++, col++ )
	    {
	    if  ( ac->acAllocated == AC_UNALLOCATED )
		{ continue;	}

	    if  ( ac->acRed	> 256* ( r- d )		&&
		  ac->acRed	< 256* ( r+ d )		&&
		  ac->acGreen	> 256* ( g- d )		&&
		  ac->acGreen	< 256* ( g+ d )		&&
		  ac->acBlue	> 256* ( b- d )		&&
		  ac->acBlue	< 256* ( b+ d )		)
		{
		*acRet= *ac;
		acRet->acAllocated= AC_COPIED;
		return 0;
		}
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Allocate the palette for palette type allocation.			*/
/*									*/
/************************************************************************/

int bmAllocateAllocatorColors(	ColorAllocator *	ca,
				int			count )
    {
    int			i;
    AllocatorColor *	fresh;

    fresh= (AllocatorColor *)realloc( ca->caColors,
					count* sizeof(AllocatorColor) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    ca->caColors= fresh;
    ca->caColorCount= count;

    for ( i= 0; i < count; i++ )
	{
	ca->caColors[i].acColorNumber= 0;
	ca->caColors[i].acColorPrivate= (void *)0;
	ca->caColors[i].acAllocated= AC_UNALLOCATED;
	}

    return 0;
    }

