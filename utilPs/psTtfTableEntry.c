#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"psTtfTableEntry.h"

#   include	<appDebugon.h>
#   include	<sioEndian.h>

void utilInitTrueTypeTableEntry(	TrueTypeTableEntry *	ttte )
    {
    ttte->ttteTag[0]= '\0';
    ttte->ttteChecksum= 0;
    ttte->ttteOffset= 0;
    utilInitMemoryBuffer( &(ttte->ttteMemoryBuffer) );

    ttte->ttteSaveOffset= 0;

    return;
    }

void utilCleanTrueTypeTableEntry(	TrueTypeTableEntry *	ttte )
    {
    utilCleanMemoryBuffer( &(ttte->ttteMemoryBuffer) );
    }


static int utilTrueTypeFontCompareTables(	const void *	vttte1,
						const void *	vttte2 )
    {
    const TrueTypeTableEntry *	ttte1= (const TrueTypeTableEntry *)vttte1;
    const TrueTypeTableEntry *	ttte2= (const TrueTypeTableEntry *)vttte2;

    if  ( ttte1->ttteOffset > ttte2->ttteOffset )
	{ return  1;	}
    if  ( ttte1->ttteOffset < ttte2->ttteOffset )
	{ return -1;	}

    return 0;
    }

TrueTypeTableEntry * psTtfReadTableEntries(
					SimpleInputStream *	sisTtf,
					int			tableCount,
					int			filePos )
    {
    TrueTypeTableEntry *	rval= (TrueTypeTableEntry *)0;
    TrueTypeTableEntry *	tables= (TrueTypeTableEntry *)0;
    TrueTypeTableEntry *	ttte;
    int				tab;

    tables= (TrueTypeTableEntry *)malloc( tableCount*
						sizeof(TrueTypeTableEntry) );
    if  ( ! tables )
	{ LXDEB(tableCount,tables); goto ready;	}

    ttte= tables;
    for ( tab= 0; tab < tableCount; ttte++, tab++ )
	{ utilInitTrueTypeTableEntry( ttte );	}

    ttte= tables;
    for ( tab= 0; tab < tableCount; ttte++, tab++ )
	{
	int		b;

	for ( b= 0; b < 4; b++ )
	    { ttte->ttteTag[b]= sioInGetByte( sisTtf );	}
	ttte->ttteTag[b]= '\0';
	filePos += 4;

	ttte->ttteChecksum= sioEndianGetBeUint32( sisTtf ); filePos += 4;
	ttte->ttteOffset= sioEndianGetBeUint32( sisTtf ); filePos += 4;
	ttte->ttteMemoryBuffer.mbSize= sioEndianGetBeUint32( sisTtf ); filePos += 4;
	}

    qsort( tables, tableCount, sizeof(TrueTypeTableEntry),
					    utilTrueTypeFontCompareTables );

    ttte= tables;
    for ( tab= 0; tab < tableCount; ttte++, tab++ )
	{
	/*
	appDebug( "%3d: \"%.4s\" @%-7lu: %6lu bytes (%lu)\n",
					    tab, ttte->ttteTag,
					    ttte->ttteOffset, ttte->ttteLength,
					    ttte->ttteChecksum );
	*/

	if  ( filePos > ttte->ttteOffset )
	    { LLDEB(filePos,ttte->ttteOffset); goto ready;	}

	while( filePos < ttte->ttteOffset )
	    {
	    int	c= sioInGetByte( sisTtf );
	    if  ( c == EOF )
		{ LLLDEB(filePos,ttte->ttteOffset,c); goto ready; }
	    filePos++;
	    }

	ttte->ttteMemoryBuffer.mbBytes= (unsigned char *)malloc( ttte->ttteMemoryBuffer.mbSize );
	if  ( ! ttte->ttteMemoryBuffer.mbBytes )
	    {
	    LXDEB(ttte->ttteMemoryBuffer.mbSize,ttte->ttteMemoryBuffer.mbBytes);
	    goto ready;
	    }
	if  ( sioInReadBytes( sisTtf, ttte->ttteMemoryBuffer.mbBytes,
					    ttte->ttteMemoryBuffer.mbSize ) !=
					    ttte->ttteMemoryBuffer.mbSize )
	    { LDEB(ttte->ttteMemoryBuffer.mbSize); goto ready;	}

	filePos += ttte->ttteMemoryBuffer.mbSize;
	}

    rval= tables; tables= (TrueTypeTableEntry *)0; /* steal */

  ready:

    if  ( tables )
	{
	ttte= tables;
	for ( tab= 0; tab < tableCount; ttte++, tab++ )
	    { utilCleanTrueTypeTableEntry( ttte );	}
	free( tables );
	}

    return rval;
    }
