/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*  Individual list levels.						*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilBase26.h>
#   include	<utilRoman.h>
#   include	<uniUtf8.h>

#   include	<appDebugon.h>

#   include	"docListLevel.h"
#   include	"docParaNumber.h"

void docInitDocumentListLevel(	ListLevel *	ll )
    {
    ll->llStartAt= 1;
    ll->llNumberStyle= 0;
    ll->llJustification= DOCllaLEFT;
    ll->llFollow= DOCllfTAB;
    ll->llPrevToDigits= 0;
    ll->llNoRestart= 0;
    ll->llPictureNumber= -1;
    ll->llFontBias= 0;

    utilInitMemoryBuffer( &(ll->llFormatString) );
    ll->llLevelNumbers= (LevelNumber *)0;
    ll->llLevelNumberCount= 0;
    ll->llTemplateID= -1;

    ll->llFromOld= 0;			/*  O  */
    ll->llUsePrevText= 0;		/*  O  */
    ll->llUsePrevSpace= 0;		/*  O  */
    ll->llIndent= 0;			/*  O  */
    ll->llSpace= 0;			/*  O  */

    ll->llTabStopListNumber= 0;
    ll->llLeftIndentTwips= 0;
    ll->llFirstIndentTwips= 0;
    utilPropMaskClear( &(ll->llParaPropertyMask) );

    utilInitTextAttribute( &(ll->llTextAttribute) );
    utilPropMaskClear( &(ll->llTextAttributeMask) );
    }

void docCleanDocumentListLevel(	ListLevel *	ll )
    {
    utilCleanMemoryBuffer( &(ll->llFormatString) );

    if  ( ll->llLevelNumbers )
	{ free( ll->llLevelNumbers ); }
    }

int docListLevelApplyTextAttribute(	ListLevel *		ll,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet )
    {
    utilUpdateTextAttribute( (PropertyMask *)0, &(ll->llTextAttribute),
							taSetMask, taSet );
    utilPropMaskOr( &(ll->llTextAttributeMask),
				    &(ll->llTextAttributeMask), taSetMask );
    return 0;
    }

int docCopyDocumentListLevel(	ListLevel *		to,
				const ListLevel *	from,
				int			copyIds,
				const int *		fontMap,
				const int *		colorMap,
				const int *		rulerMap )
    {
    int			rval= 0;

    LevelNumber *	levelNumbers= (LevelNumber *)0;

    if  ( utilCopyMemoryBuffer( &(to->llFormatString),
						&(from->llFormatString) ) )
	{ LDEB(1); return -1;	}

    if  ( from->llLevelNumbers )
	{
	int		i;
	int		count= from->llLevelNumberCount+ 1; /* NOTE +1 */

	levelNumbers= (LevelNumber *)malloc( count* sizeof(LevelNumber) );
	if  ( ! levelNumbers )
	    { LPDEB(count,levelNumbers); rval= -1; goto ready;	}

	for ( i= 0; i < count; i++ )
	    { levelNumbers[i]= from->llLevelNumbers[i];	}
	}

    if  ( to->llLevelNumbers )
	{ free( to->llLevelNumbers ); }

    to->llStartAt= from->llStartAt;
    to->llNumberStyle= from->llNumberStyle;
    to->llJustification= from->llJustification;
    to->llFollow= from->llFollow;
    to->llPrevToDigits= from->llPrevToDigits;
    to->llNoRestart= from->llNoRestart;
    to->llFontBias= from->llFontBias;
    to->llPictureNumber= from->llPictureNumber;

    if  ( copyIds )
	{ to->llTemplateID= from->llTemplateID;	}

    to->llFromOld= from->llFromOld;
    to->llUsePrevText= from->llUsePrevText;
    to->llUsePrevSpace= from->llUsePrevSpace;
    to->llIndent= from->llIndent;
    to->llSpace= from->llSpace;
    to->llTabStopListNumber= from->llTabStopListNumber;
    to->llLeftIndentTwips= from->llLeftIndentTwips;
    to->llFirstIndentTwips= from->llFirstIndentTwips;
    to->llParaPropertyMask= from->llParaPropertyMask;
    to->llTextAttribute= from->llTextAttribute;
    to->llTextAttributeMask= from->llTextAttributeMask;

    if  ( fontMap							&&
	  PROPmaskISSET( &(to->llTextAttributeMask), TApropFONT_NUMBER ) )
	{
	to->llTextAttribute.taFontNumber=
				fontMap[from->llTextAttribute.taFontNumber];
	}
    if  ( colorMap							&&
	  PROPmaskISSET( &(to->llTextAttributeMask), TApropFONT_NUMBER ) &&
	  from->llTextAttribute.taTextColorNumber > 0			)
	{
	to->llTextAttribute.taTextColorNumber=
			    colorMap[from->llTextAttribute.taTextColorNumber];
	}
    if  ( rulerMap							&&
	  PROPmaskISSET( &(to->llParaPropertyMask), PPpropTAB_STOPS )	&&
	  from->llTabStopListNumber > 0			)
	{
	to->llTabStopListNumber= rulerMap[from->llTabStopListNumber];
	}

    /* steal */
    to->llLevelNumbers= levelNumbers; levelNumbers= (LevelNumber *)0;
    to->llLevelNumberCount= from->llLevelNumberCount;

  ready:

    if  ( levelNumbers )
	{ free( levelNumbers );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set the relevant style attributes in a list level.			*/
/*									*/
/*  1)  Paragraph Properties.						*/
/*  2)  Text Attributes.						*/
/*									*/
/************************************************************************/

int docListLevelSetStyle(	ListLevel *			ll,
				const PropertyMask *		paraMask,
				const ParagraphProperties *	pp,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    /*  1  */
    utilPropMaskClear( &(ll->llParaPropertyMask) );

    if  ( PROPmaskISSET( paraMask, PPpropTAB_STOPS ) )
	{
	ll->llTabStopListNumber= pp->ppTabStopListNumber;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropTAB_STOPS );
	}

    if  ( PROPmaskISSET( paraMask, PPpropLEFT_INDENT ) )
	{
	ll->llLeftIndentTwips= pp->ppLeftIndentTwips;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropLEFT_INDENT );
	}

    if  ( PROPmaskISSET( paraMask, PPpropFIRST_INDENT ) )
	{
	ll->llFirstIndentTwips= pp->ppFirstIndentTwips;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropFIRST_INDENT );
	}

    /*  2  */
    {
    PropertyMask	doneMask;

    utilPropMaskClear( &doneMask );

    ll->llTextAttributeMask= *taSetMask;
    utilUpdateTextAttribute( &doneMask, &(ll->llTextAttribute),
							    taSetMask, taSet );
    }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the constant text that precedes field 'idx' in the listtext	*/
/*  format. (Or for idx == ll->llNumberSizeBytes, return the trailing	*/
/*  piece.)								*/
/*									*/
/************************************************************************/

int docListLevelGetField(	int *			pConstOff,
				int *			pConstLen,
				int *			pConstChars,
				int *			pLevel,
				const ListLevel *	ll,
				int			field )
    {
    int		constHead= 0;
    int		constTail= 0;
    int		level= -1;

    if  ( field < 0 || field > ll->llLevelNumberCount )
	{ LLDEB(field,ll->llLevelNumberCount); return -1;	}

    if  ( field > 0 )
	{ constHead= ll->llLevelNumbers[field-1].lnOffsetBytes;	}

    constTail= ll->llLevelNumbers[field].lnOffsetBytes;

    if  ( field < ll->llLevelNumberCount )
	{ level= ll->llLevelNumbers[field].lnLevel;	}

    *pConstOff= constHead;
    *pConstLen= constTail- constHead;
    *pConstChars= ll->llLevelNumbers[field].lnConstChars;
    *pLevel= level;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the (constant) text before a component in the level text of a	*/
/*  list. (Or for idx == ll->llNumberSizeBytes, return the trailing	*/
/*  piece.								*/
/*									*/
/************************************************************************/

int docListLevelGetText(	char *			to,
				int			maxsize,
				const ListLevel *	ll,
				int			field )
    {
    int			constOff;
    int			constLen;
    int			constChars;
    int			level;

    if  ( docListLevelGetField( &constOff, &constLen, &constChars,
							&level, ll, field ) )
	{ LDEB(field); return -1;	}
    if  ( constLen > maxsize )
	{ LLDEB(constLen,maxsize); return -1;	}

    memcpy( to, ll->llFormatString.mbBytes+ constOff, constLen );
    to[constLen]= '\0';

    return constLen;
    }

/************************************************************************/
/*									*/
/*  Update a component in the level text of a list.			*/
/*									*/
/*  idx is the index in ll->llNumberIndices. The text before the	*/
/*  number is changed. The trailing text after the last number is	*/
/*  updated if idx == ll->llNumberSizeBytes.				*/
/*									*/
/*  1)  Determine stretch in ll->llTextFormat				*/
/*  2)  More memory needed?						*/
/*  3)  Insert new value.						*/
/*  4)  Shift indirection via ll->llNumberIndices.			*/
/*  5)  Remember size in ll->llTextFormat.				*/
/*									*/
/************************************************************************/

int docListLevelSetText(	int *			pChanged,
				ListLevel *		ll,
				const char *		text,
				int			field )
    {
    int			constOff;
    int			nbytes;
    int			ochars;
    int			nchars;
    int			obytes;
    int			level;

    const char *	from;
    int			i;

    if  ( ! ll->llLevelNumbers )
	{ PDEB(ll->llLevelNumbers); return -1;	}

    nbytes= 0; nchars= 0;
    from= text;
    while( *from )
	{
	unsigned short	symbol;
	int		step;

	step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LDEB(step); break;	}

	nchars++; nbytes += step; from += step;
	}

    /*  1  */
    if  ( docListLevelGetField( &constOff, &obytes, &ochars, &level,
								ll, field ) )
	{ LDEB(field); return -1;	}

    if  ( nbytes == obytes						&&
	  ! memcmp( ll->llFormatString.mbBytes+ constOff, text, obytes ) )
	{ *pChanged= 0; return 0;	}

    /*  2,3  */
    if  ( utilMemoryBufferReplaceBytes( &(ll->llFormatString),
					constOff, constOff+ obytes,
					(const unsigned char *)text, nbytes ) )
	{ LLDEB(obytes,nbytes); return -1;	}

    ll->llLevelNumbers[field].lnConstChars= nchars;

    /*  4  */
    for ( i= field; i <= ll->llLevelNumberCount; i++ ) /* NOTE: <= */
	{ ll->llLevelNumbers[i].lnOffsetBytes += nbytes- obytes;	}

    *pChanged= 1; return 0;
    }

int docListLevelSetNumber(	int *			pChanged,
				ListLevel *		ll,
				int			level,
				int			field )
    {
    if  ( field < 0 || field > ll->llLevelNumberCount )
	{ LLDEB(field,ll->llLevelNumberCount); return -1;	}

    if  ( field < ll->llLevelNumberCount		&&
    	  ll->llLevelNumbers[field].lnLevel == level	)
	{ pChanged= 0; return 0;	}

    if  ( field < ll->llLevelNumberCount )
	{ ll->llLevelNumbers[field].lnLevel= level;	}
    else{
	if  ( docListLevelInsertNumber( ll, level, field ) )
	    { LDEB(field); return -1;	}
	}

    *pChanged= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a reference to a level number in the format string of a list	*/
/*  level.								*/
/*									*/
/************************************************************************/

int docListLevelInsertNumber(	ListLevel *		ll,
				int			level,
				int			field )
    {
    LevelNumber *	fresh;
    int			count= ll->llLevelNumberCount+ 2; /* NOTE: +2 */
    int			i;

    /*  1  */
    if  ( field < 0 || field > ll->llLevelNumberCount )
	{ LLDEB(field,ll->llLevelNumberCount); return -1;	}

    /*  2  */
    fresh= (LevelNumber *)realloc( ll->llLevelNumbers, count* sizeof(LevelNumber) );
    if  ( ! fresh )
	{ LPDEB(count,fresh); return -1;	}

    ll->llLevelNumbers= fresh;
    ll->llLevelNumberCount++;

    for ( i= ll->llLevelNumberCount; i > field; i-- )
	{ ll->llLevelNumbers[i]= ll->llLevelNumbers[i-1];	}

    /* ll->llLevelNumbers[field].lnOffsetBytes:= unchanged */
    ll->llLevelNumbers[field].lnConstChars= 0;
    ll->llLevelNumbers[field].lnLevel= level;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete a reference to a level number from the format string of a	*/
/*  list level.								*/
/*									*/
/************************************************************************/

int docListLevelDeleteNumber(	ListLevel *		ll,
				int			field )
    {
    int			i;

    /*  1  */
    if  ( field < 0 || field >= ll->llLevelNumberCount )
	{ LLDEB(field,ll->llLevelNumberCount); return -1;	}

    for ( i= field; i < ll->llLevelNumberCount; i++ )
	{ ll->llLevelNumbers[i]= ll->llLevelNumbers[i+1];	}

    ll->llLevelNumberCount--;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Format the value of a 'listtext' field.				*/
/*									*/
/*  I.E. the text of the bullet or paragraph number at the beginning	*/
/*	of a paragraph that gets one because it has a list reference.	*/
/*									*/
/************************************************************************/

int docListLevelFormatLevelNumber(	char *	target,
					int	maxsize,
					int	val,
					int	format )
    {
    int		step= 0;

    if  ( maxsize < 1 )
	{ LDEB(maxsize); return -1;	}

    switch( format )
	{
	case DOCpnDEC:
	    if  ( maxsize < 11 )
		{ LDEB(maxsize); return -1;	}
	    sprintf( target, "%d", val );
	    step= strlen( target );
	    break;

	case DOCpnUCRM:
	    if  ( utilRomanString( target, maxsize, val, 1 ) )
		{ LDEB(val); return -1;	}
	    step= strlen( target );
	    break;

	case DOCpnLCRM:
	    if  ( utilRomanString( target, maxsize, val, 0 ) )
		{ LDEB(val); return -1;	}
	    step= strlen( target );
	    break;

	case DOCpnUCLTR:
	    if  ( val == 0 )
		{ strcpy( target, "+" ); }
	    else{
		if  ( utilBase26String( target, maxsize, val, 1 ) )
		    { LDEB(val); return -1;	}
		}
	    step= strlen( target );
	    break;

	case DOCpnLCLTR:
	    if  ( val == 0 )
		{ strcpy( target, "+" ); }
	    else{
		if  ( utilBase26String( target, maxsize, val, 0 ) )
		    { LDEB(val); return -1;	}
		}
	    step= strlen( target );
	    break;

	case 22:
	    if  ( maxsize < 11 )
		{ LDEB(maxsize); return -1;	}
	    sprintf( target, "%02d", val );
	    step= strlen( target );
	    break;

	case 23:
	    step= uniPutUtf8( target, 0xb7 );
	    target[step]= '\0';
	    break;

	case 255:
	    break;

	default:
	    LLDEB(format,val);
	    if  ( maxsize < 13 )
		{ LDEB(maxsize); return -1;	}
	    sprintf( target, "(%d)", val );
	    step= strlen( target );
	    break;
	}

    return step;
    }

/************************************************************************/
/*									*/
/*  Format a paragraph number: A listtext field.			*/
/*									*/
/************************************************************************/

static int docListLevelFormatConst(	int *			pConstLen,
					int *			pLevel,
					char *			to,
					int			maxbytes,
					const ListLevel *	ll,
					int			field )
    {
    int			constOff;
    int			constLen;
    int			constChars;
    int			level;

    if  ( docListLevelGetField( &constOff, &constLen, &constChars, &level,
								ll, field ) )
	{ LDEB(field); return -1;	}
    if  ( constLen > maxbytes )
	{ LLDEB(constLen,maxbytes); return -1;	}

    if  ( constLen > 0 )
	{ memcpy( to, ll->llFormatString.mbBytes+ constOff, constLen );	}

    to[constLen]= '\0';

    *pConstLen= constLen;
    *pLevel= level;

    return 0;
    }

int docListLevelFormatParagraphNumber(	char *			to,
					int			maxbytes,
					int *			byteOffsets,
					int			maxoffsets,
					int			ilvl,
					const int *		numberPath,
					const int *		startPath,
					const int *		formatPath,
					const ListLevel *	ll )
    {
    int			bytesDone= 0;
    int			field;

    int			constLen;
    int			level;

    if  ( ! ll->llLevelNumbers )
	{ PDEB(ll->llLevelNumbers); return -1; }

    for ( field= 0; field < ll->llLevelNumberCount; field++ )
	{
	int		varLen;

	level= -1;
	if  ( docListLevelFormatConst( &constLen, &level,
					to, maxbytes- bytesDone, ll, field ) )
	    { LDEB(field); return -1;	}

	if  ( 2* field+ 0 < maxoffsets )
	    { byteOffsets[2* field+ 0]= bytesDone;	}
	if  ( constLen > 0 )
	    { to += constLen; bytesDone += constLen;	}

	if  ( level > ilvl )
	    { LLDEB(ilvl,level); return -1;	}

	if  ( 2* field+ 1 < maxoffsets )
	    { byteOffsets[2* field+ 1]= bytesDone;	}

	varLen= docListLevelFormatLevelNumber( to, maxbytes- bytesDone,
				startPath[level]+ numberPath[level],
				formatPath[level] );
	if  ( varLen < 0 )
	    { LLDEB(level,varLen); return -1;	}

	to += varLen; bytesDone += varLen;
	}

    level= -1;
    if  ( docListLevelFormatConst( &constLen, &level,
					to, maxbytes- bytesDone, ll, field ) )
	{ LDEB(field); return -1;	}

    if  ( 2* field+ 0 < maxoffsets )
	{ byteOffsets[2* field+ 0]= bytesDone;	}
    if  ( constLen > 0 )
	{ to += constLen; bytesDone += constLen;	}

    if  ( 2* field+ 1 < maxoffsets )
	{ byteOffsets[2* field+ 1]= bytesDone;	}

    to[0]= '\0';
    return bytesDone;
    }

int docDefaultListLevel(	ListLevel *		llTo,
				int			level,
				int			deftab,
				int			levelTemplateID,
				const PropertyMask *	taSetMask,
				const TextAttribute *	taSet )
    {
    int			rval= 0;

    unsigned char *	formatString= (unsigned char *)0;
    LevelNumber *	levelNumbers= (LevelNumber *)0;

    int			i;

    docCleanDocumentListLevel( llTo );
    docInitDocumentListLevel( llTo );

    /*  1  */
    llTo->llTemplateID= levelTemplateID;

    formatString= (unsigned char *)malloc( level+ 1 );
    if  ( ! formatString )
	{ LPDEB(level,formatString); rval= -1; goto ready;	}

    levelNumbers= (LevelNumber *)malloc(
			    (level+ 2)* sizeof(LevelNumber) ); /* NOTE: +2 */
    if  ( ! levelNumbers )
	{ LPDEB(level,levelNumbers); rval= -1; goto ready;	}

    memset( formatString, '.', level );
    formatString[level]= '\0';

    for ( i= 0; i <= level; i++ )
	{
	levelNumbers[i].lnOffsetBytes= i;
	levelNumbers[i].lnLevel= i;
	levelNumbers[i].lnConstChars= ( i > 0 );
	}
    levelNumbers[i].lnOffsetBytes= i- 1;
    levelNumbers[i].lnLevel= -1;
    levelNumbers[i].lnConstChars= 0;

    if  ( utilMemoryBufferSetBytes( &(llTo->llFormatString), formatString, level ) )
	{ LDEB(level); rval= -1; goto ready;	}

    llTo->llLevelNumbers= levelNumbers;
    levelNumbers= (LevelNumber *)0; /* steal */
    llTo->llLevelNumberCount= level+ 1;

    llTo->llTextAttribute= *taSet;
    llTo->llTextAttributeMask= *taSetMask;

    llTo->llFirstIndentTwips= -deftab/ 2;
    llTo->llLeftIndentTwips= ( level+ 1 )* deftab;
    PROPmaskADD( &(llTo->llParaPropertyMask), PPpropFIRST_INDENT );
    PROPmaskADD( &(llTo->llParaPropertyMask), PPpropLEFT_INDENT );

  ready:

    if  ( formatString )
	{ free( formatString );		}
    if  ( levelNumbers )
	{ free( levelNumbers );	}

    return rval;
    }

int docListLevelToRtfStrings(	MemoryBuffer *		mbtext,
				MemoryBuffer *		mbnumbers,
				const ListLevel *	ll )
    {
    unsigned char		uc;
    int				chars= 1;
    int				field;

    int				constOff;
    int				constLen;
    int				constChars;
    int				level;

    const unsigned char *	formatString= ll->llFormatString.mbBytes;

    utilEmptyMemoryBuffer( mbtext );
    utilEmptyMemoryBuffer( mbnumbers );

    if  ( ! ll->llLevelNumbers )
	{ PDEB(ll->llLevelNumbers); return 0; }

    uc= 0;
    if  ( utilMemoryBufferAppendBytes( mbtext, &uc, 1 ) )
	{ LDEB(1); return -1;	}

    for ( field= 0; field < ll->llLevelNumberCount; field++ )
	{
	if  ( docListLevelGetField( &constOff, &constLen, &constChars, &level,
								ll, field ) )
	    { LDEB(field); return -1;	}

	if  ( utilMemoryBufferAppendBytes( mbtext,
					formatString+ constOff, constLen ) )
	    { LDEB(constLen); return -1;	}

	chars += constChars;

	uc= chars;
	if  ( utilMemoryBufferAppendBytes( mbnumbers, &uc, 1 ) )
	    { LDEB(1); return -1;	}

	uc= level;
	if  ( utilMemoryBufferAppendBytes( mbtext, &uc, 1 ) )
	    { LDEB(1); return -1;	}

	chars++;
	}

    if  ( docListLevelGetField( &constOff, &constLen, &constChars, &level,
								ll, field ) )
	{ LDEB(field); return -1;	}

    if  ( utilMemoryBufferAppendBytes( mbtext, formatString+ constOff, constLen ) )
	{ LDEB(constLen); return -1;	}

    chars += constChars;
    mbtext->mbBytes[0]= chars- 1;

    return 0;
    }

static int docListLevelConstFromRtfStrings(
					const char *	from,
					int		constChars )
    {
    int		c;
    int		constLen= 0;

    for ( c= 0; c < constChars; c++ )
	{
	unsigned short	symbol;
	int		step;

	step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { CLDEB(*from,step); return -1;	}

	constLen += step; from += step;
	}

    return constLen;
    }

int docListLevelFromRtfStrings(	ListLevel *		ll,
				const MemoryBuffer *	mbtext,
				const MemoryBuffer *	mbnumbers )
    {
    int				rval= 0;

    unsigned char *		formatString= (unsigned char *)0;
    LevelNumber *		levelNumbers= (LevelNumber *)0;

    const unsigned char *	fbytes;
    int				fsize= 0;
    const unsigned char *	lbytes;
    int				lsize= 0;

    int				field;
    int				offsetChars= 1; /* in rtf */
    int				constChars;
    int				constOff= 0;
    int				constLen= 0;

    const char *		 from;

    fbytes= utilMemoryBufferGetBytes( &fsize, mbtext );
    lbytes= utilMemoryBufferGetBytes( &lsize, mbnumbers );

    formatString= (unsigned char *)malloc( fsize );
    if  ( ! formatString )
	{ LPDEB(fsize,formatString); rval= -1; goto ready;	}
    formatString[0]= '\0';

    levelNumbers= (LevelNumber *)malloc( (lsize+ 1)*sizeof(LevelNumber) );
    if  ( ! levelNumbers )
	{ LPDEB(lsize,levelNumbers); rval= -1; goto ready;	}

    from= (char *)fbytes+ 1;

    for ( field= 0; field < lsize; field++ )
	{
	int	fieldOffsetChars= lbytes[field]; /* in rtf */

	constChars= fieldOffsetChars- offsetChars;
	constLen= docListLevelConstFromRtfStrings( from, constChars );

	if  ( constLen < 0 )
	    { LLDEB(field,constLen); rval= -1; goto ready;	}

	memcpy( formatString+ constOff, from, constLen );
	from += constLen;

	levelNumbers[field].lnOffsetBytes= constOff+ constLen;
	levelNumbers[field].lnConstChars= constChars;
	levelNumbers[field].lnLevel= *(from++);

	offsetChars= fieldOffsetChars+ 1;
	constOff += constLen;
	}

    constChars= fbytes[0]+ 1- offsetChars;
    constLen= docListLevelConstFromRtfStrings( from, constChars );
    if  ( constLen < 0 )
	{ LDEB(constLen); rval= -1; goto ready;	}

    memcpy( formatString+ constOff, from, constLen );

    levelNumbers[field].lnOffsetBytes= constOff+ constLen;
    levelNumbers[field].lnConstChars= constChars;
    levelNumbers[field].lnLevel= -1;

    constOff += constLen;
    formatString[constOff]= '\0';

    if  ( utilMemoryBufferSetBytes( &(ll->llFormatString), formatString, constOff ) )
	{ LDEB(constOff); rval= -1; goto ready;	}

    ll->llLevelNumbers= levelNumbers;
    levelNumbers= (LevelNumber *)0; /* steal */
    ll->llLevelNumberCount= lsize;

#   if 0
    SDEB(ll->llFormatString);
    for ( field= 0; field <= ll->llLevelNumberCount; field++ )
	{
	appDebug( "%d: ob= %2d, cg= %2d, lev= %2d\n", field,
			ll->llLevelNumbers[field].lnOffsetBytes,
			ll->llLevelNumbers[field].lnConstChars,
			ll->llLevelNumbers[field].lnLevel );
	}
    docListDocumentListLevel(-1,ll);
#   endif

  ready:

    if  ( formatString )
	{ free( formatString );	}
    if  ( levelNumbers )
	{ free( levelNumbers );	}

    return rval;
    }

int docSetListLevelProperty(	ListLevel *		ll,
				int			prop,
				int			val )
    {
    switch( prop )
	{
	case LLpropSTARTAT:
	    ll->llStartAt= val;
	    break;

	case LLpropSTYLE:
	    ll->llNumberStyle= val;
	    break;

	case LLpropJUSTIFY:
	    ll->llJustification= val;
	    break;

	case LLpropFOLLOW:
	    ll->llFollow= val;
	    break;

	case LLpropLEGAL:
	    ll->llPrevToDigits= val != 0;
	    break;

	case LLpropNORESTART:
	    ll->llNoRestart= val != 0;
	    break;

	case LLpropPICTURE:
	    ll->llPictureNumber= val;
	    break;

	case LLpropFBIAS:
	    ll->llFontBias= val != 0;
	    break;

	/* LLpropTEXT */

	case LLpropTEMPLATEID:
	    ll->llTemplateID= val;
	    break;

	/* LLpropNUMBERS */

	case LLpropOLD:
	    ll->llFromOld= val != 0;
	    break;

	case LLpropPREV:
	    ll->llUsePrevText= val != 0;
	    break;

	case LLpropPREVSPACE:
	    ll->llUsePrevSpace= val != 0;
	    break;

	case LLpropINDENT:
	    ll->llIndent= val;
	    break;

	case LLpropSPACE:
	    ll->llSpace= val;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }
