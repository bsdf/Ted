/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<utilRanges.h>
#   include	"docFieldInstructions.h"
#   include	"docDocumentField.h"

#   include	"docTocField.h"
#   include	"docListDepth.h"
#   include	<utilMemoryBufferPrintf.h>

void docInitTocField(	TocField *	tf )
    {
    tf->tfType= TOCtypeTOC;

    utilInitMemoryBuffer( &(tf->tfIdentifierName) );
    utilInitMemoryBuffer( &(tf->tfMarkName) );
    utilInitMemoryBuffer( &(tf->tfSeparatorBytes) );
    utilInitMemoryBuffer( &(tf->tfSeqName) );
    utilInitMemoryBuffer( &(tf->tfSeqSeparatorBytes) );
    utilInitMemoryBuffer( &(tf->tfStyleNamesBytes) );

    tf->tfIncludeSeqLabelAndNumber= 0;

    tf->tfUseTcEntries= 0;
    tf->tfEntryLevel0= 0;
    tf->tfEntryLevel1= PPoutlineDEEPEST;

    tf->tfUseStyleNames= 0;

    tf->tfUseStyleLevels= 0;
    tf->tfStyleLevel0= 0;
    tf->tfStyleLevel1= PPoutlineDEEPEST;

    tf->tfUseOutlineLevels= 0;

    tf->tfHyperlinks= 0;
    tf->tfWebNoNumber= 0;
    tf->tfPreserveTabs= 0;
    tf->tfPreserveNewlines= 0;

    tf->tfNLevel0= PPoutlineBODYTEXT;
    tf->tfNLevel1= PPoutlineBODYTEXT;

    tf->tfStyleNameLevels= (StyleNameLevel *)0;
    tf->tfStyleNameLevelCount= 0;
    }

void docCleanTocField(	TocField *	tf )
    {
    int		i;

    utilCleanMemoryBuffer( &(tf->tfIdentifierName) );
    utilCleanMemoryBuffer( &(tf->tfMarkName) );
    utilCleanMemoryBuffer( &(tf->tfSeparatorBytes) );
    utilCleanMemoryBuffer( &(tf->tfSeqName) );
    utilCleanMemoryBuffer( &(tf->tfSeqSeparatorBytes) );
    utilCleanMemoryBuffer( &(tf->tfStyleNamesBytes) );

    for ( i= 0; i < tf->tfStyleNameLevelCount; i++ )
	{
	if  ( tf->tfStyleNameLevels[i].snlStyleName )
	    { free( tf->tfStyleNameLevels[i].snlStyleName );	}
	}
    if  ( tf->tfStyleNameLevels )
	{ free( tf->tfStyleNameLevels );	}

    return;
    }

int docCopyTocField(		TocField *		to,
				const TocField *	from )
    {
    to->tfType= from->tfType;

    if  ( utilCopyMemoryBuffer( &(to->tfIdentifierName),
					    &(from->tfIdentifierName) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->tfMarkName), &(from->tfMarkName) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->tfSeparatorBytes),
					    &(from->tfSeparatorBytes) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->tfSeqName), &(from->tfSeqName) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->tfSeqSeparatorBytes),
					    &(from->tfSeqSeparatorBytes) ) )
	{ LDEB(1); return -1;	}
    if  ( utilCopyMemoryBuffer( &(to->tfStyleNamesBytes),
					    &(from->tfStyleNamesBytes) ) )
	{ LDEB(1); return -1;	}

    to->tfUseTcEntries= from->tfUseTcEntries;
    to->tfUseStyleNames= from->tfUseStyleNames;

    to->tfUseStyleLevels= from->tfUseStyleLevels;
    to->tfStyleLevel0= from->tfStyleLevel0;
    to->tfStyleLevel1= from->tfStyleLevel1;

    to->tfUseOutlineLevels= from->tfUseOutlineLevels;

    to->tfIncludeSeqLabelAndNumber= from->tfIncludeSeqLabelAndNumber;
    to->tfHyperlinks= from->tfHyperlinks;
    to->tfWebNoNumber= from->tfWebNoNumber;
    to->tfPreserveTabs= from->tfPreserveTabs;
    to->tfPreserveNewlines= from->tfPreserveNewlines;
    to->tfNLevel0= from->tfNLevel0;
    to->tfNLevel1= from->tfNLevel1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Split TOC field.							*/
/*									*/
/************************************************************************/

static int docTocFieldParseLevels(
			unsigned char *				pLevel0,
			unsigned char *				pLevel1,
			const InstructionsComponent *		ic )
    {
    int			rval;
    int			l0, l1;

    rval= utilParseRange( &l0, &l1, (const char *)ic->icBuffer.mbBytes );
    if  ( rval )
	{ SDEB((const char *)ic->icBuffer.mbBytes); }
    if  ( l0 < 0		||
	  l1 < 0		||
	  l0 > PPoutlineDEEPEST	||
	  l1 > PPoutlineDEEPEST	||
	  l1 < l0		)
	{ LLDEB(l0,l1); rval= -1;	}

    if  ( rval == 0 )
	{ *pLevel0= l0; *pLevel1= l1;	}

    return rval;
    }

/************************************************************************/

static int docTocFieldParseStyleNames(		TocField *	tf )
    {
    StyleNameLevel *	snl;
    const char *	from= utilMemoryBufferGetString( &(tf->tfStyleNamesBytes) );
    const char *	s;
    int			ns;
    int			level= 0;

    ns= 0;
    s= from;
    while( *s )
	{
	if  ( *s == ',' )
	    { ns++;	}
	s++;
	}
    ns++;
    if  ( ns % 2 == 1 )
	{ ns++;	}
    ns /= 2;
    tf->tfStyleNameLevels= (StyleNameLevel *)malloc(
					    ns* sizeof(StyleNameLevel) );
    if  ( ! tf->tfStyleNameLevels )
	{ LXDEB(ns,tf->tfStyleNameLevels); return -1;	}

    snl= tf->tfStyleNameLevels;
    tf->tfStyleNameLevelCount= 0;

    while( *from )
	{
	int		len;
	const char *	comma;

	comma= strchr( from, ',' );
	if  ( comma )
	    {
	    char *	past= (char *)comma+ 1;
	    int		l;

	    past= (char *)comma+ 1;
	    len= comma- from;

	    l= strtol( comma+ 1, &past, 10 );
	    if  ( past == comma+ 1 )
		{ SDEB(comma); level= PPoutlineBODYTEXT;	}
	    else{ level= l- 1;			}

	    comma= strchr( past, ',' );
	    }
	else{
	    level= PPoutlineBODYTEXT;
	    len= strlen( from );
	    }

	if  ( len == 0 || level < 0 || level > PPoutlineBODYTEXT )
	    { SLLDEB(from,len,level);	}
	else{
	    snl->snlStyleName= (char *)malloc( len+ 1 );
	    if  ( ! snl->snlStyleName )
		{ XDEB(snl->snlStyleName); return -1;	}
	    strncpy( snl->snlStyleName, from, len )[len]= '\0';
	    snl->snlLevel= level;
	    tf->tfStyleNameLevelCount++; snl++;
	    }

	if  ( ! comma )
	    { break;	}

	from= comma+ 1;
	}

    return 0;
    }

/************************************************************************/

int docFieldGetToc(	TocField *		tf,
			const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkTOC )
	{ return -1;	}

    docCleanTocField( tf );
    docInitTocField( tf );

    ic= df->dfInstructions.fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docFieldHasMergeformat( fi, comp ) )
	    { comp++; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'a' ) )
	    {
	    ic++, comp++;
	    tf->tfType= TOCtypeSEQ;
	    tf->tfIncludeSeqLabelAndNumber= 0;

	    if  ( utilCopyMemoryBuffer( &(tf->tfIdentifierName),
							&(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'b' ) )
	    {
	    ic++, comp++;
	    if  ( utilCopyMemoryBuffer( &(tf->tfMarkName), &(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}
	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'c' ) )
	    {
	    ic++, comp++;
	    tf->tfType= TOCtypeSEQ;
	    tf->tfIncludeSeqLabelAndNumber= 0;
	    if  ( utilCopyMemoryBuffer( &(tf->tfIdentifierName),
							&(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}
	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'c' ) )
	    { continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'f' ) )
	    {
	    tf->tfType= TOCtypeTOC;
	    tf->tfUseTcEntries= 1;

	    if  ( comp < fi->fiComponentCount- 1 && ! ic[1].icIsFlag )
		{
		ic++, comp++;

		if  ( utilCopyMemoryBuffer( &(tf->tfIdentifierName),
							    &(ic->icBuffer) ) )
		    { LDEB(1); return -1;	}
		continue;
		}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    {
	    tf->tfHyperlinks= 1;
	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'l' ) )
	    {
	    ic++, comp++;

	    tf->tfType= TOCtypeTOC;
	    tf->tfUseTcEntries= 1;
	    if  ( docTocFieldParseLevels( &(tf->tfEntryLevel0),
						&(tf->tfEntryLevel1), ic ) )
		{ LDEB(1); return -1;	}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'n' ) )
	    {
	    if  ( comp < fi->fiComponentCount- 1 && ! ic[1].icIsFlag )
		{
		ic++, comp++;

		if  ( docTocFieldParseLevels( &(tf->tfNLevel0),
						    &(tf->tfNLevel1), ic ) )
		    { LDEB(1); return -1;	}

		continue;
		}
	    else{
		tf->tfNLevel0= 0;
		tf->tfNLevel1= PPoutlineDEEPEST;
		}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'o' ) )
	    {
	    if  ( comp < fi->fiComponentCount- 1 && ! ic[1].icIsFlag )
		{
		ic++, comp++;

		tf->tfType= TOCtypeTOC;
		tf->tfUseStyleLevels= 1;

		if  ( docTocFieldParseLevels( &(tf->tfStyleLevel0),
						&(tf->tfStyleLevel1), ic ) )
		    { LDEB(1); return -1;	}

		continue;
		}
	    /* else ignore: That is what MS-Word does */

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'p' ) )
	    {
	    ic++, comp++;
	    if  ( utilCopyMemoryBuffer( &(tf->tfSeparatorBytes),
							&(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}
	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 's' ) )
	    {
	    ic++, comp++;
	    if  ( utilCopyMemoryBuffer( &(tf->tfSeqName), &(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}
	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'd' ) )
	    {
	    ic++, comp++;
	    if  ( utilCopyMemoryBuffer( &(tf->tfSeqSeparatorBytes),
							&(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}
	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 't' ) )
	    {
	    tf->tfType= TOCtypeTOC;
	    tf->tfUseStyleNames= 1;

	    if  ( comp < fi->fiComponentCount- 1 && ! ic[1].icIsFlag )
		{
		ic++, comp++;

		if  ( utilCopyMemoryBuffer( &(tf->tfStyleNamesBytes),
							&(ic->icBuffer) ) )
		    { LDEB(1); return -1;	}
		}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'u' ) )
	    {
	    tf->tfType= TOCtypeTOC;
	    tf->tfUseOutlineLevels= 1;
	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'w' ) )
	    {
	    tf->tfPreserveTabs= 1;
	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'x' ) )
	    {
	    tf->tfPreserveNewlines= 1;
	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'z' ) )
	    {
	    tf->tfWebNoNumber= 1;
	    continue;
	    }

	LSDEB(comp,utilMemoryBufferGetString(&(ic->icBuffer))); continue;
	}

    if  ( tf->tfUseStyleNames					&&
	  ! utilMemoryBufferIsEmpty( &(tf->tfStyleNamesBytes) ) )
	{
	if  ( docTocFieldParseStyleNames( tf ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the TOC field string.						*/
/*									*/
/*  There are two kinds of TOC:						*/
/*	-  A list of captions.						*/
/*	   =    With the name of the sequence and the number in the	*/
/*		sequence along with the caption text in the entry.	*/
/*	   =    Just repeating the the caption text in the entries.	*/
/*	-  A real table of contents.					*/
/*									*/
/************************************************************************/

static int docTocFieldAddLevels(	FieldInstructions *	fi,
					int			flag,
					int			level0,
					int			level1 )
    {
    int			rval= 0;
    MemoryBuffer	mbScratch;

    utilInitMemoryBuffer( &mbScratch );

    if  ( utilMemoryBufferPrintf( &mbScratch,
				"%d-%d", level0+ 1, level1+ 1 ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFieldInstructionsAddArgFlag( fi, flag, &mbScratch ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &mbScratch );

    return rval;
    }

int docTocFieldSetTocInstructions(	FieldInstructions *	fi,
					const TocField *	tf )
    {
    int		flag;

    if  ( docStartFieldInstructions( fi, "TOC", 3 ) )
	{ LDEB(4); return -1;	}

    switch( tf->tfType )
	{
	case TOCtypeSEQ:
	    if  ( tf->tfIncludeSeqLabelAndNumber )
		{ flag= 'a';		}
	    else{ flag= 'c';		}

	    if  ( docFieldInstructionsAddArgFlag( fi, flag,
						    &(tf->tfIdentifierName) ) )
		{ LDEB(1); return -1;	}
	    break;

	case TOCtypeTOC:
	    if  ( tf->tfUseStyleLevels )
		{
		if  ( docTocFieldAddLevels( fi, 'o',
				    tf->tfStyleLevel0, tf->tfStyleLevel1 ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( tf->tfUseStyleNames )
		{
		if  ( docFieldInstructionsAddArgFlagIfSet( fi, 't',
						&(tf->tfStyleNamesBytes) ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( tf->tfUseTcEntries )
		{
		if  ( docFieldInstructionsAddFlag( fi, 'f' ) )
		    { LDEB(1); return -1;	}

		if  ( ! utilMemoryBufferIsEmpty( &(tf->tfIdentifierName) ) &&
		      docFieldInstructionsAddComponent( fi,
					    &(tf->tfIdentifierName) )	)
		    { LDEB(1); return -1;	}

		if  ( tf->tfEntryLevel0 > 0 ||
					tf->tfEntryLevel1 < PPoutlineDEEPEST )
		    {
		    if  ( docTocFieldAddLevels( fi, 'l',
				    tf->tfEntryLevel0, tf->tfEntryLevel1 ) )
			{ LDEB(1); return -1;	}
		    }
		}

	    if  ( tf->tfUseOutlineLevels )
		{
		if  ( docFieldInstructionsAddFlag( fi, 'u' ) )
		    { LDEB(1); return -1;	}
		}

	    break;

	default:
	    LDEB(tf->tfType); return -1;
	}

    /*  b  */
    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 'b', &(tf->tfMarkName) ) )
	{ LDEB(1); return -1;	}

    /*  h  */
    if  ( tf->tfHyperlinks )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'h' ) )
	    { LDEB(1); return -1;	}
	}

    /*  n  */
    if  ( tf->tfNLevel0 < PPoutlineBODYTEXT )
	{
	if  ( tf->tfNLevel0 > 0 || tf->tfNLevel1 < PPoutlineDEEPEST )
	    {
	    if  ( docTocFieldAddLevels( fi, 'n',
					tf->tfNLevel0, tf->tfNLevel1 ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( docFieldInstructionsAddFlag( fi, 'n' ) )
		{ LDEB(1); return -1;	}
	    }
	}

    /*  p  */
    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 'p',
						    &(tf->tfSeparatorBytes) ) )
	{ LDEB(1); return -1;	}

    /*  s  */
    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 's', &(tf->tfSeqName) ) )
	{ LDEB(1); return -1;	}

    /*  d  */
    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 'd',
					    &(tf->tfSeqSeparatorBytes) ) )
	{ LDEB(1); return -1;	}

    /*  w  */
    if  ( tf->tfPreserveTabs )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'w' ) )
	    { LDEB(1); return -1;	}
	}

    /*  x  */
    if  ( tf->tfPreserveNewlines )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'x' ) )
	    { LDEB(1); return -1;	}
	}

    /*  z  */
    if  ( tf->tfWebNoNumber )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'z' ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }
