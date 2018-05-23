/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docFieldInstructions.h"

/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

static void docCleanInstructionsComponent(	InstructionsComponent *	ic )
    { utilCleanMemoryBuffer( &(ic->icBuffer) );	}

static void docInitInstructionsComponent(	InstructionsComponent *	ic )
    {
    utilInitMemoryBuffer( &(ic->icBuffer) );

    ic->icIsFlag= 0;
    ic->icIsQuoted= 0;
    }

static int docCopyInstructionsComponent( InstructionsComponent *	to,
					const InstructionsComponent *	from )
    {
    if  ( utilCopyMemoryBuffer( &(to->icBuffer), &(from->icBuffer) ) )
	{ LDEB(1); return -1;	}

    to->icIsFlag= from->icIsFlag;
    to->icIsQuoted= from->icIsQuoted;

    return 0;
    }

void docInitFieldInstructions(	FieldInstructions *	fi )
    {
    fi->fiComponents= (InstructionsComponent *)0;
    fi->fiComponentCount= 0;
    }

void docCleanFieldInstructions(	FieldInstructions *	fi )
    {
    if  ( fi->fiComponents )
	{
	int		i;

	for ( i= 0; i < fi->fiComponentCount; i++ )
	    {
	    docCleanInstructionsComponent( &(fi->fiComponents[i]) );
	    }

	free( fi->fiComponents );
	}

    return;
    }

int docAllocateComponents(	FieldInstructions *		fi,
				int				n )
    {
    InstructionsComponent *	fresh;

    if  ( n == 0 )
	{
	docCleanFieldInstructions( fi );
	fi->fiComponents= (InstructionsComponent *)0;
	fi->fiComponentCount= 0;
	return 0;
	}

    while( fi->fiComponentCount > n )
	{
	fi->fiComponentCount--;
	docCleanInstructionsComponent( fi->fiComponents+
					fi->fiComponentCount );
	}

    fresh= realloc( fi->fiComponents,
				    n* sizeof(InstructionsComponent) );
    if  ( ! fresh )
	{ LXDEB(n,fresh); return -1;	}
    fi->fiComponents= fresh;

    while( fi->fiComponentCount < n )
	{
	docInitInstructionsComponent( fi->fiComponents+
					fi->fiComponentCount++ );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy the properties of a field.					*/
/*									*/
/*  Only properties that have an application meaning are copied. The	*/
/*  set is determined by what can appaer in RTF.			*/
/*									*/
/************************************************************************/

int docCopyFieldInstructions(	FieldInstructions *		fiTo,
				const FieldInstructions *	fiFrom )
    {
    int			i;

    if  ( docAllocateComponents( fiTo,
					fiFrom->fiComponentCount ) )
	{ LDEB(fiFrom->fiComponentCount); return -1;	}

    for ( i= 0; i < fiTo->fiComponentCount; i++ )
	{
	if  ( docCopyInstructionsComponent(
				    fiTo->fiComponents+ i,
				    fiFrom->fiComponents+ i ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Split field instructions in a vector of arguments.			*/
/*									*/
/************************************************************************/

static int docSplitFieldInstructions(
				InstructionsComponent *		ic,
				const char *			bytes,
				int				count )
    {
    const char *	from= bytes;
    int			offset= 0;
    int			n= 0;

    for (;;)
	{
	int		size= 0;
	const char *	head= from;
	int		quoted= 0;
	int		flag= 0;

	int		unmatchedQuote= 0;

	while( offset < count && *from == ' ' )
	    { offset++; from++;	}

	if  ( offset >= count )
	    { break;	}

	if  ( from[0] == '\\'		&&
	      offset < count- 1		&&
	      from[1] != ' '		)
	    {
	    if  ( ic )
		{
		if  ( utilMemoryBufferSetBytes( &(ic->icBuffer),
					    (const unsigned char *)from, 2 ) )
		    { LDEB(1); return -1;	}

		ic->icIsFlag= 1;
		ic->icIsQuoted= 0;
		ic++;
		}

	    offset += 2;  from += 2; n++;
	    continue;
	    }

	if  ( from[0] == '"' )
	    {
	    offset++; from++;

	    if  ( from[0] == '"' )
		{ offset++; from++; continue; }

	    head= from;
	    quoted= 1;
	    flag= 0;
	    while( offset < count && *from != '"' )
		{ size++; offset++; from++;	}

	    if  ( offset >= count )
		{ SLLDEB(bytes,offset,count); unmatchedQuote= 1;	}
	    else{ offset++; from++;					}
	    }
	else{
	    head= from;
	    quoted= 0;
	    flag= *from == '\\';
	    while( offset < count && *from != ' ' )
		{ size++; offset++; from++;	}
	    }

	if  ( ic )
	    {
	    if  ( utilMemoryBufferSetBytes( &(ic->icBuffer),
					(const unsigned char *)head, size ) )
		{ LDEB(1); return -1;	}

	    if  ( unmatchedQuote					&&
		  utilMemoryBufferAppendString( &(ic->icBuffer), "\"" )	)
		{ LDEB(unmatchedQuote); return -1;	}

	    ic->icIsFlag= flag;
	    ic->icIsQuoted= quoted;
	    ic++;
	    }

	n++;
	}

    return n;
    }

int docSetFieldInstructions(	FieldInstructions *	fi,
				const char *		bytes,
				int			size )
    {
    int		n;

    n= docSplitFieldInstructions( (InstructionsComponent *)0, bytes, size );
    if  ( n < 0 )
	{ LDEB(n); return -1;	}

    if  ( docAllocateComponents( fi, n ) )
	{ LDEB(n); return -1;	}

    n= docSplitFieldInstructions( fi->fiComponents, bytes, size );
    if  ( n < 0 )
	{ LDEB(n); return -1;	}

    return 0;
    }

static int docFieldMustBeQuoted(	const InstructionsComponent *	ic )
    {
    const MemoryBuffer *	mb= &(ic->icBuffer);
    int				i;

    if  ( utilMemoryBufferIsEmpty( mb ) )
	{ return 1;	}
    if  ( mb->mbBytes[0] == '\\' )
	{ return 1;	}

    for ( i= 0; i < mb->mbSize; i++ )
	{
	if  ( mb->mbBytes[i] == ' ' )
	    { return 1;	}
	}

    return 0;
    }

static int docFieldSetComponentBuffer(	InstructionsComponent *	ic,
					const MemoryBuffer *	mb )
    {
    if  ( utilCopyMemoryBuffer( &(ic->icBuffer), mb ) )
	{ LDEB(1); return -1;	}

    ic->icIsFlag= 0;
    ic->icIsQuoted= docFieldMustBeQuoted( ic );

    return 0;
    }

int docFieldInstructionsAddFlag(	FieldInstructions *	fi,
					int			flag )
    {
    unsigned char		scratch[3];
    InstructionsComponent *	ic;

    if  ( docAllocateComponents( fi, fi->fiComponentCount+ 1 ) )
	{ LDEB(fi->fiComponentCount); return -1;	}

    scratch[0]= '\\';
    scratch[1]= flag;
    scratch[2]= '\0';

    ic= &(fi->fiComponents[fi->fiComponentCount- 1]);

    if  ( utilMemoryBufferSetBytes( &(ic->icBuffer), scratch, 2 ) )
	{ LDEB(2); return -1;	}

    ic->icIsFlag= 1;
    ic->icIsQuoted= 0;

    return 0;
    }

int docStartFieldInstructions(	FieldInstructions *	fi,
				const char *		kindName,
				int			kindSize )
    {
    if  ( docAllocateComponents( fi, 1 ) )
	{ LDEB(2); return -1;	}

    if  ( utilMemoryBufferSetBytes( &(fi->fiComponents[0].icBuffer),
				(const unsigned char *)kindName, kindSize ) )
	{ LDEB(kindSize); return -1;	}

    fi->fiComponents[0].icIsFlag= 0;
    fi->fiComponents[0].icIsQuoted= 0;

    return 0;
    }

int docFieldInstructionsAddArgFlagIfSet( FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	valueName )
    {
    if  ( ! utilMemoryBufferIsEmpty( valueName ) )
	{ return docFieldInstructionsAddArgFlag( fi, flag, valueName );	}
    else{ return 0;							}
    }

int docFieldInstructionsAddArgFlag(	FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	valueName )
    {
    unsigned char		scratch[3];
    InstructionsComponent *	ic;

    if  ( docAllocateComponents( fi, fi->fiComponentCount+ 2 ) )
	{ LDEB(fi->fiComponentCount); return -1;	}

    scratch[0]= '\\';
    scratch[1]= flag;
    scratch[2]= '\0';

    ic= &(fi->fiComponents[fi->fiComponentCount- 2]);

    if  ( utilMemoryBufferSetBytes( &(ic->icBuffer), scratch, 2 ) )
	{ LDEB(2); return -1;	}

    ic->icIsFlag= 1;
    ic->icIsQuoted= 0;

    ic++;

    if  ( docFieldSetComponentBuffer( ic, valueName ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docFieldInstructionsAddComponent(	FieldInstructions *	fi,
					const MemoryBuffer *	mb )
    {
    InstructionsComponent *	ic;

    if  ( docAllocateComponents( fi, fi->fiComponentCount+ 1 ) )
	{ LDEB(fi->fiComponentCount); return -1;	}

    ic= fi->fiComponents + fi->fiComponentCount- 1;

    if  ( docFieldSetComponentBuffer( ic, mb ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docComponentIsFlag(		const FieldInstructions *	fi,
				int				comp,
				int				flag )
    {
    const InstructionsComponent *	ic= fi->fiComponents+ comp;

    if  ( ! ic->icIsFlag )
	{ return 0;	}
    if  ( ic->icBuffer.mbSize != 2 )
	{ return 0;	}
    if  ( ic->icBuffer.mbBytes[0] != '\\' )
	{ return 0;	}
    if  ( ic->icBuffer.mbBytes[1] != flag )
	{ return 0;	}

    return 1;
    }

int docComponentIsArgFlag(	const FieldInstructions *	fi,
				int				comp,
				int				flag )
    {
    if  ( comp > fi->fiComponentCount- 2 )
	{ return 0;	}

    return docComponentIsFlag( fi, comp, flag );
    }

int docComponentEqualsWord(	const InstructionsComponent *	ic,
				const char *			word,
				int				len )
    {
    if  ( ic->icBuffer.mbSize != len )
	{ return 0;	}
    if  ( memcmp( ic->icBuffer.mbBytes, word, len ) )
	{ return 0;	}

    return 1;
    }

int docComponentEqualsWordNoCase( const InstructionsComponent *	ic,
				const char *			word,
				int				len )
    {
    int		i;

    if  ( ic->icBuffer.mbSize != len )
	{ return 0;	}

    for ( i= 0; i < len; i++ )
	{
	if  ( tolower( ic->icBuffer.mbBytes[i] ) != tolower( word[i] ) )
	    { return 0;	}
	}

    return 1;
    }
