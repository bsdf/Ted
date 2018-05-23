/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docField.h"
#   include	<docTreeType.h>
#   include	<docTextParticule.h>
#   include	<docBookmarkField.h>
#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	"docNodeTree.h"

/************************************************************************/
/*									*/
/*  Find the inner most field for a particular position.		*/
/*									*/
/************************************************************************/

DocumentField * docFindFieldForPosition(
				BufferDocument *		bd,
				const DocumentPosition *	dp )
    {
    EditPosition	ep;
    DocumentTree *	dt;
    BufferItem *	bodySectBi;
    const int		lastOne= 0;

    docSetEditPosition( &ep, dp );

    if  ( docGetTreeOfNode( &dt, &bodySectBi, bd, dp->dpNode ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return docFindChildField( &(dt->dtRootFields), &ep, lastOne );
    }

/************************************************************************/
/*									*/
/*  Find a field of a certain type that holds a position.		*/
/*									*/
/************************************************************************/

DocumentField * docFindTypedFieldForPosition(
				BufferDocument *		bd,
				const DocumentPosition *	dp,
				int				kind,
				int				lastOne )
    {
    EditPosition	ep;
    DocumentTree *	dt;
    DocumentField *	dfInner;

    docSetEditPosition( &ep, dp );

    if  ( docGetTreeOfNode( &dt, (BufferItem **)0, bd, dp->dpNode ) )
	{ LDEB(1); return (DocumentField *)0;	}

    dfInner= docFindChildField( &(dt->dtRootFields), &ep, lastOne );

    while( dfInner && docEditPositionInField( dfInner, &ep ) )
	{
	DocumentField *	df= dfInner;

	while( df )
	    {
	    if  ( df->dfKind == kind || kind == -1 )
		{ return df;	}

	    df= df->dfParent;
	    }

	dfInner= docGetNextField( &(dt->dtRootFields), dfInner );
	}

    return (DocumentField *)0;
    }

DocumentField * docFindTypedFieldInSelection(
			    struct BufferDocument *		bd,
			    const struct DocumentSelection *	ds,
			    int					kind,
			    int					lastOne )
    {
    EditRange		er;
    DocumentTree *	dt;

    docSetEditRange( &er, ds );

    if  ( docGetRootOfSelectionScope( &dt, (BufferItem **)0,
					    bd, &(ds->dsSelectionScope) ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return docFindFieldInRange( &er, &(dt->dtRootFields), lastOne, kind );
    }

/************************************************************************/
/*									*/
/*  Suggest a bookmark name that matches the text in a selection.	*/
/*									*/
/************************************************************************/

int docSuggestNewBookmarkName(	MemoryBuffer *			markName,
				const BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    int			rval= 0;
    const BufferItem *	paraBi= ds->dsHead.dpNode;
    int			stroff= ds->dsHead.dpStroff;

    if  ( paraBi )
	{
	const char *	s= (const char *)docParaString( paraBi, stroff );
	int		stroffTail;

	if  ( paraBi == ds->dsTail.dpNode )
	    { stroffTail= ds->dsTail.dpStroff;	}
	else{ stroffTail= docParaStrlen( paraBi );	}

	if  ( docBookmarkFromText( markName, s, stroffTail- stroff ) )
	    { LDEB(1); return -1;	}

	if  ( docMakeBookmarkUnique( bd, markName ) )
	    { LDEB(1); return -1;	}

	rval= 1;
	}

    return rval;
    }

/************************************************************************/

int docMakeBookmarkUnique(	const BufferDocument *	bd,
				MemoryBuffer *		markName )
    {
    DocumentField *	df;
    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    if  ( docFindBookmarkField( &df, dfl, markName ) >= 0 )
	{
	int		i;
	char		scratch[6+1];
	int		offset= docBookmarkSuffixIndex( markName, 6 );

	for ( i= 0; i < 1000000; i++ )
	    {
	    sprintf( scratch, "%06d", i );
	    utilMemoryBufferReplaceBytes( markName,
						offset, markName->mbSize,
						(unsigned char *)scratch, 6 );

	    if  ( docFindBookmarkField( &df, dfl, markName ) < 0 )
		{ return 0;	}
	    }

	LDEB(i); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Derive field kind from field instructions.				*/
/*									*/
/************************************************************************/

int docFieldKindFromInstructions(	const DocumentField *	df )
    {
    const FieldInstructions *	fi= &(df->dfInstructions);

    char *			s;
    char *			p;
    int				n;
    int				i;

    const FieldKindInformation *	fki;

    if  ( fi->fiComponentCount < 1 )
	{ LDEB(fi->fiComponentCount); return -1;	}

    s= (char *) fi->fiComponents[0].icBuffer.mbBytes;
    n= fi->fiComponents[0].icBuffer.mbSize;
    /* Cope with OpenOffice that inserts backslashes before the instructions */
    while( n > 0 && *s == '\\' )
	{ n--; s++;	}

    i= 0; p= s;
    while( i < n && isalpha( *p ) )
	{
	if  ( islower( *p ) )
	    { *p= toupper( *p );	}
	i++; p++;
	}

    if  ( DOC_FieldKindCount != DOCfk_COUNT )
	{ LLDEB(DOC_FieldKindCount,DOCfk_COUNT); return -1;	}

    /* LSDEB(p-s,(char *)s); */

    fki= DOC_FieldKinds;
    for ( i= 0; i < DOC_FieldKindCount; fki++, i++ )
	{
	if  ( ! fki->fkiIsFieldInRtf )
	    { continue;	}

	if  ( ! strncmp( s, fki->fkiLabel, p- s )	&&
	      ! fki->fkiLabel[p- s]			)
	    { return i;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Shift fields in an edit operation.					*/
/*  NOTE that we depend on the paragraph number only for shifting the	*/
/*  section number: Only the body has more than one section and there	*/
/*  is a colose relationship in that case.				*/
/*									*/
/*  1)	For (BODY) fields that span sections, the section number is	*/
/*	that of the head position of the field.				*/
/*									*/
/************************************************************************/

static void docShiftFieldStart(		DocumentField *		df,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    /*  earlier paragraph  */
    if  ( df->dfHeadPosition.epParaNr < paraFrom )
	{ return;	}

    /*  later paragraph  */
    if  ( df->dfHeadPosition.epParaNr > paraFrom )
	{
	df->dfHeadPosition.epParaNr += paraShift;
	/*  1  */
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += sectShift;	}

	return;
	}

    /* Same paragraph */
    if  ( df->dfHeadPosition.epStroff < stroffFrom )
	{ return;	}
    if  ( df->dfHeadPosition.epStroff > stroffFrom )
	{
	df->dfHeadPosition.epParaNr += paraShift;
	df->dfHeadPosition.epStroff += stroffShift;
	/*  1  */
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += sectShift;	}

	return;
	}

    return;
    }

static void docShiftFieldEnd(		DocumentField *		df,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    /*  earlier paragraph  */
    if  ( df->dfTailPosition.epParaNr < paraFrom )
	{ return;	}

    /*  later paragraph  */
    if  ( df->dfTailPosition.epParaNr > paraFrom )
	{
	df->dfTailPosition.epParaNr += paraShift;
	/*  1
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += sectShift;	}
	*/

	return;
	}

    /* Same paragraph */
    if  ( df->dfTailPosition.epStroff < stroffFrom )
	{ return;	}
    if  ( df->dfTailPosition.epStroff > stroffFrom )
	{
	df->dfTailPosition.epParaNr += paraShift;
	df->dfTailPosition.epStroff += stroffShift;
	/*  1
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += sectShift;	}
	*/

	return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Shift fields.							*/
/*									*/
/************************************************************************/

static void docShiftChildFieldReferences(
					const ChildFields *	cf,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    int			l;
    int			r;
    int			m;

    DocumentField *	df;

    if  ( cf->cfChildCount == 0 )
	{ return;	}

    l= 0; r= cf->cfChildCount; m= ( l+ r )/ 2;
    while( l < m )
	{
	df= cf->cfChildren[m];

	if  ( df->dfTailPosition.epParaNr < paraFrom )
	    { l= m;	}
	else{ r= m;	}

	m= ( l+ r )/ 2;
	}

    df= cf->cfChildren[m];
    if  ( df->dfTailPosition.epParaNr < paraFrom )
	{ m++;	}

    for ( m= m; m < cf->cfChildCount; m++ )
	{
	DocumentField *	dfc= cf->cfChildren[m];

	if  ( dfc->dfFieldNumber < 0 )
	    { LDEB(dfc->dfFieldNumber); continue;	}

	if  ( dfc->dfSelectionScope.ssTreeType == DOCinBODY		&&
	      dfc->dfSelectionScope.ssOwnerSectNr >= 0			&&
	      dfc->dfSelectionScope.ssOwnerSectNr > sectFrom		)
	    { dfc->dfSelectionScope.ssOwnerSectNr += sectShift;		}

	docShiftFieldStart( dfc, sectFrom, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );
	docShiftFieldEnd( dfc, sectFrom, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );

	if  ( dfc->dfChildFields.cfChildCount > 0 )
	    {
	    docShiftChildFieldReferences( &(dfc->dfChildFields),
					sectFrom, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );
	    }
	}

    return;
    }

int docShiftFieldReferences(		DocumentTree *		dt,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    /*
    appDebug( "docShiftFieldReferences( %s,"
		" para:%d+%d, stroff:%d+%d )\n",
	    docTreeTypeStr( dt->dtRoot?dt->dtRoot->biTreeType:-1 ),
	    paraFrom, paraShift, stroffFrom, stroffShift );
    */

    /*  1  */
    if  ( paraFrom < 1 )
	{ LDEB(paraFrom); return -1;	}

    docShiftChildFieldReferences( &(dt->dtRootFields),
					sectFrom, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete all fields that completely fall inside a range in the	*/
/*  document.								*/
/*									*/
/************************************************************************/

static void docDeleteFieldChildren(	int *			pUpdateFlags,
					BufferDocument *	bd,
					DocumentField *		dfp )
    {
    if  ( dfp->dfKind == DOCfkLISTTEXT )
	{ (*pUpdateFlags) |= FIELDdoLISTTEXT;	}
    if  ( dfp->dfKind == DOCfkSEQ )
	{ (*pUpdateFlags) |= FIELDdoSEQ;	}
    if  ( docFieldHasNote( dfp->dfKind ) && dfp->dfNoteIndex >= 0 )
	{ (*pUpdateFlags) |= FIELDdoCHFTN;	}

    docDeleteChildFields( pUpdateFlags, bd, &(dfp->dfChildFields) );
    return;
    }

void docDeleteChildFields(	int *			pUpdateFlags,
				BufferDocument *	bd,
				ChildFields *		cf )
    {
    int		i;

    for ( i= 0; i < cf->cfChildCount; i++ )
	{
	DocumentField *	dfc= cf->cfChildren[i];

	docDeleteFieldChildren( pUpdateFlags, bd, dfc );
	docDeleteFieldFromDocument( bd, dfc );
	}

    docCleanChildFields( cf );
    docInitChildFields( cf );
    }

/************************************************************************/
/*									*/
/*  Delete a field from a document and remove all references.		*/
/*									*/
/*  NOTE The shortcut that optimizes a lot of superfluous calls.	*/
/*									*/
/************************************************************************/

int docDeleteFieldFromParent(	DocumentTree *			dt,
				DocumentField *			df )
    {
    ChildFields *	cf;

    if  ( df->dfParent )
	{ cf= &(df->dfParent->dfChildFields);	}
    else{ cf= &(dt->dtRootFields);		}

    if  ( df->dfChildFields.cfChildCount == 0	&&
	  cf->cfChildCount == 1			)
	{ cf->cfChildCount--;	}
    else{
	if  ( docDeleteChildField( cf, df ) )
	    {
	    SDEB(docTreeTypeStr(df->dfSelectionScope.ssTreeType));
	    XDEB(df->dfParent);
	    return -1;
	    }
	}

    df->dfNumberInParent= -1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete all fields that overlap a range.				*/
/*									*/
/************************************************************************/

int docDeleteFieldRange(	int *			pUpdateFlags,
				BufferDocument *	bd,
				const EditRange *	er,
				ChildFields *		rootFields,
				DocumentField *		dfLeft,
				DocumentField *		dfRight )
    {
    int			rval= 0;
    int			i;

    int			count= rootFields->cfChildCount;
    DocumentField **	fields= (DocumentField **)0;

    if  ( rootFields->cfChildCount == 0 )
	{ goto ready;	}

    fields= (DocumentField **)malloc( count* sizeof(DocumentField *) );
    if  ( ! fields )
	{ LXDEB(count,fields); rval= -1; goto ready;	}

    for ( i= 0; i < count; i++ )
	{ fields[i]= rootFields->cfChildren[i];	}

    /*  1  */
    for ( i= 0; i < count; i++ )
	{
	DocumentField *		df= fields[i];
	int			bcm;
	int			ecm;

	if  ( docCompareEditPositions( &(df->dfTailPosition),
						    &(er->erHead) ) <= 0 )
	    { continue;	}

	if  ( docCompareEditPositions( &(df->dfHeadPosition),
						    &(er->erTail) ) >= 0 )
	    { break;	}

	bcm= docCompareEditPositions( &(df->dfHeadPosition), &(er->erHead) );
	ecm= docCompareEditPositions( &(df->dfTailPosition), &(er->erTail) );

	if  ( bcm < 0 || ecm > 0 )
	    {
	    docDeleteFieldRange( pUpdateFlags, bd, er,
						    &(df->dfChildFields),
						    dfLeft, dfRight );
	    }
	else{
	    if  ( df->dfKind == DOCfkLISTTEXT )
		{ (*pUpdateFlags) |= FIELDdoLISTTEXT;	}
	    if  ( df->dfKind == DOCfkSEQ )
		{ (*pUpdateFlags) |= FIELDdoSEQ;	}
	    if  ( docFieldHasNote( df->dfKind ) && df->dfNoteIndex >= 0 )
		{ (*pUpdateFlags) |= FIELDdoCHFTN;	}

	    docDeleteFieldChildren( pUpdateFlags, bd, df );
	    docDeleteChildField( rootFields, df );
	    docDeleteFieldFromDocument( bd, df );
	    }
	}

  ready:

    if  ( fields )
	{ free( fields );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert the special field at the head of a numbered paragraph.	*/
/*  (buller), or of a foot/endnote.					*/
/*									*/
/*  0)  Insert a text particule at the head of the paragraph as a	*/
/*	temporary field value.						*/
/*  1)  Allocate a field.						*/
/*  2)  Insert start particule.						*/
/*  3)  Insert end particule.						*/
/*  4)  Make sure there is at least one particule after the field.	*/
/*									*/
/************************************************************************/

int docInsertParaHeadField(	DocumentField **	pDfHead,
				DocumentSelection *	dsInsideHead,
				DocumentSelection *	dsAroundHead,
				int *			pHeadPart,
				int *			pTailPart,
				BufferItem *		paraBi,
				BufferDocument *	bd,
				DocumentTree *		dt,
				int			fieldKind,
				int			textAttrNr )
    {
    DocumentField *		df;
    TextParticule *		tpText;
    int				stroffShift= 0;
    int				head= 0;
    const int			stroffHead= 0;
    int				wasEmpty= ( docParaStrlen( paraBi ) == 0 );
    DocumentFieldList *		dfl= &(bd->bdFieldList);

    DocumentSelection		dsField;

    while( head+ 1 < paraBi->biParaParticuleCount			&&
	   paraBi->biParaParticules[head+ 1].tpStroff == stroffHead	&&
	   paraBi->biParaParticules[head+ 1].tpKind == DOCkindFIELDHEAD &&
	   docGetFieldKindByNumber( dfl,
	     paraBi->biParaParticules[head+ 1].tpObjectNumber==DOCfkBOOKMARK ) )
	{ head++;	}

    /*  4  */
    if  ( docParaStringReplace( &stroffShift, paraBi,
			    stroffHead, stroffHead, (const char *)"?", 1 ) )
	{ LDEB(docParaStrlen(paraBi)); return -1; }

    if  ( paraBi->biParaParticuleCount > 0 && wasEmpty )
	{
	tpText= paraBi->biParaParticules;
	if  ( tpText->tpKind != DOCkindSPAN )
	    { SDEB(docKindStr(tpText->tpKind)); return -1;	}

	tpText->tpStrlen= 1;

	if  ( docShiftParticuleOffsets( bd, paraBi, head+ 1,
				paraBi->biParaParticuleCount, stroffShift ) )
	    { LDEB(stroffShift); }
	}
    else{
	tpText= docInsertTextParticule( paraBi, head,
			    stroffHead, 1, DOCkindSPAN, textAttrNr );
	if  ( ! tpText )
	    { LXDEB(paraBi->biParaParticuleCount,tpText); return -1; }

	if  ( docShiftParticuleOffsets( bd, paraBi, head+ 1,
				paraBi->biParaParticuleCount, stroffShift ) )
	    { LDEB(stroffShift); }
	}

    docSetParaSelection( &dsField, paraBi, 1, stroffHead, stroffShift );

    /*  2,3  */
    df= docMakeField( bd, dt, &dsField, head, head+ 1, textAttrNr, textAttrNr );
    if  ( ! df )
	{ XDEB(df); return -1;	}

    df->dfKind= fieldKind;

    /*  4  */
    if  ( paraBi->biParaParticuleCount == head+ 3 )
	{
	tpText= docInsertTextParticule( paraBi, head+ 3,
					docParaStrlen( paraBi ), 0,
					DOCkindSPAN, textAttrNr );
	if  ( ! tpText )
	    { LXDEB(paraBi->biParaParticuleCount,tpText); return -1; }
	}

    if  ( docDelimitFieldInDoc( dsInsideHead, dsAroundHead,
					    pHeadPart, pTailPart, bd, df ) )
	{ LDEB(1); return -1; }

    *pDfHead= df;
    return 0;
    }

DocumentField * docMakeField(	BufferDocument *		bd,
				DocumentTree *			dt,
				const DocumentSelection *	dsInput,
				int				part0,
				int				part1,
				int				attr0,
				int				attr1 )
    {
    int			paraNr0= docNumberOfParagraph( dsInput->dsHead.dpNode );
    int			paraNr1;
    const BufferItem *	sectBi0;

    DocumentField *	rval= (DocumentField *)0;
    DocumentField *	df;
    int			singleParagraph= 0;

    int			stroff0;
    int			stroff1;

    df= docClaimField( &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); goto ready;	}
    df->dfKind= DOCfkUNKNOWN;

    sectBi0= docGetSectNode( dsInput->dsHead.dpNode );
    if  ( ! sectBi0 )
	{ XDEB(sectBi0); goto ready;	}

    if  ( dsInput->dsTail.dpNode == dsInput->dsHead.dpNode )
	{ singleParagraph= 1; paraNr1= paraNr0;	}
    else{
	const BufferItem *	sectBi1;

	singleParagraph= 0;
	paraNr1= docNumberOfParagraph( dsInput->dsTail.dpNode );

	sectBi1= docGetSectNode( dsInput->dsTail.dpNode );
	if  ( ! sectBi1 )
	    { XDEB(sectBi1); goto ready;	}

	if  ( sectBi1 != sectBi0 )
	    { XXDEB(sectBi0,sectBi1); goto ready;	}
	}

    {
    TextParticule *	tp;

    tp= docMakeSpecialParticule( dsInput->dsTail.dpNode, part1,
		    dsInput->dsTail.dpStroff, DOCkindFIELDTAIL, attr1 );
    if  ( ! tp )
	{ XDEB(tp); goto ready;	}
    tp->tpObjectNumber= df->dfFieldNumber;
    stroff1= tp->tpStroff;

    docShiftParticuleOffsets( bd, dsInput->dsTail.dpNode, part1+ 1,
		dsInput->dsTail.dpNode->biParaParticuleCount, tp->tpStrlen );
    }

    {
    TextParticule *	tp;

    tp= docMakeSpecialParticule( dsInput->dsHead.dpNode, part0,
		    dsInput->dsHead.dpStroff, DOCkindFIELDHEAD, attr0 );
    if  ( ! tp )
	{ XDEB(tp); goto ready;	}
    tp->tpObjectNumber= df->dfFieldNumber;
    stroff0= tp->tpStroff;

    docShiftParticuleOffsets( bd, dsInput->dsHead.dpNode, part0+ 1,
		dsInput->dsHead.dpNode->biParaParticuleCount, tp->tpStrlen );

    if  ( singleParagraph )
	{ stroff1 += tp->tpStrlen;	}
    }

    df->dfSelectionScope= sectBi0->biSectSelectionScope;
    df->dfHeadPosition.epParaNr= paraNr0;
    df->dfHeadPosition.epStroff= stroff0;
    df->dfTailPosition.epParaNr= paraNr1;
    df->dfTailPosition.epStroff= stroff1;

    if  ( docInsertFieldInTree( &(dt->dtRootFields), df ) )
	{ LDEB(1); goto ready;	}

    rval= df; df= (DocumentField *)0; /* steal */

  ready:

    if  ( df )
	{ docDeleteFieldFromList( &(bd->bdFieldList), df );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the special field at the head of a paragraph in a list.	*/
/*									*/
/************************************************************************/

int docParaHeadFieldKind(	const BufferItem *	paraBi,
				const BufferDocument *	bd )
    {
    int		fieldKind= -1;

    if  ( paraBi->biTreeType == DOCinFOOTNOTE	||
	  paraBi->biTreeType == DOCinENDNOTE	)
	{
	BufferItem *		bi= docGetSectNode( paraBi->biParent );
	if  ( ! bi )
	    { XDEB(bi); return -1;		}
	else{
	    DocumentPosition	dp;

	    if  ( docHeadPosition( &dp, bi ) )
		{ LDEB(1); return -1;	}

	    if  ( dp.dpNode == paraBi )
		{ fieldKind= DOCfkCHFTN;	}
	    }
	}

    if  ( paraBi->biParaListOverride > 0 )
	{ fieldKind= DOCfkLISTTEXT;	}

    return fieldKind;
    }

