#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>
#   include	<uniUtf8.h>

#   include	"psTtfNameTable.h"
#   include	<utilEndian.h>
#   include	<sioEndian.h>

#   include	<appDebugon.h>

void psTtfInitTrueTypeNameRecord(	TrueTypeNameRecord *	ttnr )
    {
    ttnr->ttnrPlatformID= 0;
    ttnr->ttnrEncodingID= 0;
    ttnr->ttnrLanguageID= 0;
    ttnr->ttnrNameID= 0;
    ttnr->ttnrLength= 0;
    ttnr->ttnrOffset= 0;

    ttnr->ttnrSavedBytes= (unsigned char *)0;

    return;
    }

void psTtfCleanTrueTypeNameRecord(	TrueTypeNameRecord *	ttnr )
    {
    if  ( ttnr->ttnrSavedBytes )
	{ free( ttnr->ttnrSavedBytes );	}

    return;
    }

void psTtfInitTrueTypeNameTable(	TrueTypeNameTable *	ttnt )
    {
    ttnt->ttntFormat= 0;
    ttnt->ttntRecordCount= 0;
    ttnt->ttntOffset= 0;

    ttnt->ttntRecords= (TrueTypeNameRecord *)0;
    }

void psTtfCleanTrueTypeNameTable(	TrueTypeNameTable *	ttnt )
    {
    int		rec;

    for ( rec= 0; rec < ttnt->ttntRecordCount; rec++ )
	{ psTtfCleanTrueTypeNameRecord( &(ttnt->ttntRecords[rec]) );	}

    if  ( ttnt->ttntRecords )
	{ free( ttnt->ttntRecords );	}
    }

/************************************************************************/
/*									*/
/*  Extract the name table from a TrueType file.			*/
/*									*/
/************************************************************************/

int psTtfExtractNameRecords(	SimpleInputStream *		sisName,
				const TrueTypeTableEntry *	ttte,
				TrueTypeNameTable *		ttnt )
    {
    int				rval= 0;

    int				rec;
    TrueTypeNameRecord *	ttnr;

    int				recordCount;

    ttnt->ttntFormat= sioEndianGetBeUint16( sisName );
    recordCount= sioEndianGetBeUint16( sisName );
    ttnt->ttntOffset= sioEndianGetBeUint16( sisName );

    ttnt->ttntRecords= (TrueTypeNameRecord *)malloc( recordCount* sizeof(TrueTypeNameRecord) );
    if  ( ! ttnt->ttntRecords )
	{ LXDEB(recordCount,ttnt->ttntRecords); rval= -1; goto ready; }

    ttnr= ttnt->ttntRecords;
    for ( rec= 0; rec < recordCount; ttnr++, rec++ )
	{ psTtfInitTrueTypeNameRecord( ttnr );	}
    ttnt->ttntRecordCount= recordCount;

    ttnr= ttnt->ttntRecords;
    for ( rec= 0; rec < recordCount; ttnr++, rec++ )
	{
	const unsigned char *	from;

	ttnr->ttnrPlatformID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrEncodingID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrLanguageID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrNameID= sioEndianGetBeUint16( sisName );
	ttnr->ttnrLength= sioEndianGetBeUint16( sisName );
	ttnr->ttnrOffset= sioEndianGetBeUint16( sisName );

	from= ttte->ttteMemoryBuffer.mbBytes+ ttnt->ttntOffset+ ttnr->ttnrOffset;

	ttnr->ttnrSavedBytes= (unsigned char *)malloc( ttnr->ttnrLength+ 1 );
	if  ( ! ttnr->ttnrSavedBytes )
	    {
	    LXDEB(ttnr->ttnrLength,ttnr->ttnrSavedBytes);
	    rval= -1; goto ready;
	    }

	memcpy( ttnr->ttnrSavedBytes, from, ttnr->ttnrLength );
	ttnr->ttnrSavedBytes[ttnr->ttnrLength]= '\0';

	/* LLSDEB(rec,ttnr->ttnrLength,(char *)ttnr->ttnrSavedBytes); */
	}

  ready:

    return rval;
    }

static int utilTtfStringBytes( int p, int e )
    {
    if  ( p == 0 )		{ return 2;	}	/*  Unicode	*/
    if  ( p == 1 && e == 0 )	{ return 1;	}	/*  Mac Roman	*/
    if  ( p == 3 && e == 0 )	{ return 2;	}	/*  MS symbol	*/
    if  ( p == 3 && e == 1 )	{ return 2;	}

    if  ( p == 1 && e == 8 )	{ return 1;	}	/*  Mac symbol	*/

    LLDEB(p,e); return 1;
    }

static int utilTtfGetNameRecord( const TrueTypeNameTable * 	ttnt,
				int				nameID )
    {
    int				found= -1;
    int				rec;
    const TrueTypeNameRecord *	ttnr;
    /*
    const TrueTypeNameRecord *	ttnrFound= (const TrueTypeNameRecord *)0;
    */

    ttnr= ttnt->ttntRecords;
    for ( rec= 0; rec < ttnt->ttntRecordCount; ttnr++, rec++ )
	{
	if  ( ttnr->ttnrNameID != nameID )
	    { continue; }

	found= rec; /*ttnrFound= ttnr;*/ break;

	/*
	LDEB(ttnr->ttnrPlatformID);
	LDEB(ttnr->ttnrEncodingID);
	LDEB(ttnr->ttnrLanguageID);
	SDEB((char *)ttnr->ttnrSavedBytes);
	*/
	}

    return found;
    }

int psTtfGetName(	char **				pName,
			const TrueTypeNameTable *	ttnt,
			int				which )
    {
    int				recFound;
    char *			name;
    const TrueTypeNameRecord *	ttnr;

    recFound= utilTtfGetNameRecord( ttnt, which );
    if  ( recFound < 0 )
	{ /*LLDEB(which,recFound);*/ return -1;	}

    ttnr= ttnt->ttntRecords+ recFound;

    name= (char *)malloc( 3* ttnr->ttnrLength+ 1 ); /* Exagerated */
    if  ( ! name )
	{ LXDEB(ttnr->ttnrLength,name); return -1;	}

    if  ( utilTtfStringBytes( ttnr->ttnrPlatformID, ttnr->ttnrEncodingID )
									== 2 )
	{
	int		from;
	int		to= 0;

	for ( from= 0; from < ttnr->ttnrLength; from += 2 )
	    {
	    int		uni;
	    int		step;

	    uni= utilEndianExtractBeUint16( ttnr->ttnrSavedBytes+ from );
	    step= uniPutUtf8( name+ to, uni );
	    if  ( step < 1 )
		{ LLDEB(uni,step); return -1;	}

	    to += step;
	    }
	name[to]= '\0';
	}
    else{
	memcpy( name, ttnr->ttnrSavedBytes, ttnr->ttnrLength );
	name[ttnr->ttnrLength]= '\0';
	}

    if  ( *pName )
	{ free( *pName );	}

    *pName= name;

    while( *name )
	{
	if  ( *name == '\n' )
	    { *name= ' ';	}

	name++;
	}

    return 0;
    }

