/************************************************************************/
/*									*/
/*  Manage notes.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docNodeTree.h"
#   include	"docField.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>

/************************************************************************/
/*									*/
/*  Manage Footnotes/Endnotes.						*/
/*									*/
/************************************************************************/

void docInitNote(		DocumentNote *		dn )
    {
    docInitDocumentTree( &(dn->dnDocumentTree) );

    dn->dnNoteNumber= -1;
    dn->dnReferringPage= -1;
    dn->dnReferringColumn= -1;

    docInitNoteProperties( &(dn->dnNoteProperties) );

    return;
    }

void docCleanNote(		BufferDocument *	bd,
				DocumentNote *		dn )
    {
    docCleanDocumentTree( bd, &(dn->dnDocumentTree) );
    docCleanNoteProperties( &(dn->dnNoteProperties) );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a Footnote/Endnote in a BufferItem.				*/
/*									*/
/*  1)  Look for a note or the place to insert a new one.		*/
/*	NOTE that positions with a negative paragraph number are always	*/
/*	skipped.							*/
/*	NOTE that when a position for a new note is found, its index	*/
/*	is that of its successor, that usually will be shifted away.	*/
/*  2)  When the previous position is an empty hole, use it in stead	*/
/*	of shifting the successors away.				*/
/*  3)  Make a guess about the note number. If the guess is wrong, it	*/
/*	fixed later. Fixing can involve a reformat, so making not too	*/
/*	bad a guess here does help.					*/
/*									*/
/************************************************************************/

static void docSetNote(		DocumentNote *		fresh,
				int			autoNumber )
    {
    docInitNote( fresh );

    /*  3  */
    if  ( autoNumber )
	{ fresh->dnNoteNumber= autoNumber;	}
    else{ fresh->dnNoteNumber= 0;		}

    fresh->dnNoteProperties.npAutoNumber= autoNumber;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a note or an annotation.					*/
/*									*/
/*  1)  More than one annotation can be attached to the same location.	*/
/*									*/
/************************************************************************/

int docInsertNote(			DocumentNote **		pDn,
					BufferDocument *	bd,
					DocumentField *		dfNote,
					int			autoNumber )
    {
    NotesList *		nl= &(bd->bdNotesList);
    DocumentNote *	fresh;

    int			i;

    fresh= (DocumentNote *)realloc( nl->nlNotes,
				( nl->nlNoteCount+ 1 )* sizeof(DocumentNote) );
    if  ( ! fresh )
	{ LXDEB(nl->nlNoteCount,fresh); return -1;	}
    nl->nlNotes= fresh;

    for ( i= 0; i < nl->nlNoteCount; i++ )
	{
	if  ( NOTE_IS_DELETED( &(fresh[i]) ) )
	    { break;	}
	}

    docSetNote( fresh+ i, autoNumber );
    
    if  ( i >= nl->nlNoteCount )
	{ nl->nlNoteCount++;	}

    dfNote->dfNoteIndex= i;
    *pDn= fresh+ i; return i;
    }

/************************************************************************/
/*									*/
/*  Return the first note on a page.					*/
/*									*/
/************************************************************************/

DocumentField * docGetFirstNoteFromColumn(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			column,
				int			treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	if  ( dn->dnReferringPage > page )
	    { break;	}

	if  ( dn->dnReferringPage == page )
	    {
	    if  ( dn->dnReferringColumn > column )
		{ break;	}

	    if  ( dn->dnReferringColumn == column )
		{ *pDn= dn; return dfNote;	}
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetFirstNoteInColumn(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			column,
				int			treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( dt->dtPageFormattedFor > page )
	    { break;	}

	if  ( dt->dtPageFormattedFor == page )
	    {
	    if  ( dt->dtColumnFormattedFor > column )
		{ break;	}

	    if  ( dt->dtColumnFormattedFor == column )
		{ *pDn= dn; return dfNote;	}
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetLastNoteInColumn(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			column,
				int			treeType )
    {
    DocumentField *	dfRet= (DocumentField *)0;
    DocumentNote *	dnRet= (DocumentNote *)0;

    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( dt->dtPageFormattedFor > page )
	    { break;	}

	if  ( dt->dtPageFormattedFor == page )
	    {
	    if  ( dt->dtColumnFormattedFor > column )
		{ break;	}

	    if  ( dt->dtColumnFormattedFor == column )
		{ dnRet= dn; dfRet= dfNote;	}
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    *pDn= dnRet;
    return dfRet;
    }

DocumentField * docGetFirstNoteOnPage(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( dt->dtPageFormattedFor > page )
	    { break;	}

	if  ( dt->dtPageFormattedFor == page )
	    { *pDn= dn; return dfNote;	}

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    return (DocumentField *)0;
    }

static DocumentField * docGetNextSectionNote(
				DocumentField *		df,
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			sect,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    while( df )
	{
	if  ( docFieldHasNote( df->dfKind ) )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn); 	}
	    else{
		if  ( treeType < 0					||
		      dn->dnNoteProperties.npTreeType == treeType	)
		    { *pDn= dn; return df;	}
		}
	    }

	df= docGetNextFieldInSection( rootFields, sect, df );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetFirstNoteOfSection(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			sect,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);
    DocumentField *		df;

    if  ( sect < 0 )
	{ return docGetFirstNoteOfDocument(  pDn, bd, treeType ); }

    df= docGetFirstFieldOfSection( rootFields, sect );
    return docGetNextSectionNote( df, pDn, bd, sect, treeType );
    }

DocumentField * docGetNextNoteInSection(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			sect,
				DocumentField *		df,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    if  ( sect < 0 )
	{ return docGetNextNoteInDocument(  pDn, bd, df, treeType ); }

    df= docGetNextFieldInSection( rootFields, sect, df );
    return docGetNextSectionNote( df, pDn, bd, sect, treeType );
    }

static DocumentField * docGetNextDocumentNote(
					DocumentField *		df,
					DocumentNote **		pDn,
					const BufferDocument *	bd,
					int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    while( df )
	{
	if  ( docFieldHasNote( df->dfKind ) )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn);	}
	    else{
		if  ( treeType < 0					||
		      dn->dnNoteProperties.npTreeType == treeType	)
		    {
		    if  ( pDn )
			{ *pDn= dn;	}
		    return df;
		    }
		}
	    }

	df= docGetNextField( rootFields, df );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetFirstNoteOfDocument(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);
    DocumentField *		df;

    df= docGetFirstField( rootFields );

    return docGetNextDocumentNote( df, pDn, bd, treeType );
    }

DocumentField * docGetNextNoteInDocument(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				DocumentField *		df,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    df= docGetNextField( rootFields, df );

    return docGetNextDocumentNote( df, pDn, bd, treeType );
    }

static DocumentField * docGetPrevDocumentNote(
					DocumentField *		df,
					DocumentNote **		pDn,
					const BufferDocument *	bd,
					int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    while( df )
	{
	if  ( docFieldHasNote( df->dfKind ) )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn); 	}
	    else{
		if  ( treeType < 0					||
		      dn->dnNoteProperties.npTreeType == treeType	)
		    {
		    if  ( pDn )
			{ *pDn= dn;	}
		    return df;
		    }
		}
	    }

	df= docGetPrevField( rootFields, df );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetLastNoteOfDocument(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);
    DocumentField *		df;

    df= docGetLastField( rootFields );

    return docGetPrevDocumentNote( df, pDn, bd, treeType );
    }

DocumentField * docGetPrevNoteInDocument(
				DocumentNote **		pDn,
				const BufferDocument *	bd,
				DocumentField *		df,
				int			treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    df= docGetPrevField( rootFields, df );

    return docGetPrevDocumentNote( df, pDn, bd, treeType );
    }

/************************************************************************/
/*									*/
/*  Get the note corresponding to a selection.				*/
/*  Always return the field that owns the note.				*/
/*									*/
/************************************************************************/

DocumentField * docGetSelectedNote(
				DocumentNote **			pDn,
				int *				pSelInNote,
				BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    DocumentField *		dfNote= (DocumentField *)0;
    DocumentNote *		dn= (DocumentNote *)0;
    int				selInNote= 0;

    /*  2  */
    if  ( ds->dsSelectionScope.ssTreeType == DOCinFOOTNOTE	||
	  ds->dsSelectionScope.ssTreeType == DOCinENDNOTE	)
	{
	BufferItem *		bi= ds->dsHead.dpNode;
	int			ownerNumber;

	bi= docGetSectNode( ds->dsHead.dpNode );
	if  ( ! bi )
	    { XDEB(bi); return (DocumentField *)0;		}

	ownerNumber= bi->biSectSelectionScope.ssOwnerNumber;
	dfNote= docGetFieldByNumber( &(bd->bdFieldList), ownerNumber );
	dn= docGetNoteOfField( dfNote, bd );
	if  ( ! dn )
	    { XDEB(dn); return (DocumentField *)0;	}

	selInNote= 1;
	}

    if  ( ds->dsSelectionScope.ssTreeType == DOCinBODY )
	{
	const int	lastOne= 0;

	if  ( ! docIsParaSelection( ds ) )
	    { return (DocumentField *)0;	}

	dfNote= docFindTypedFieldForPosition( bd, &(ds->dsHead),
							DOCfkCHFTN, lastOne );
	if  ( ! dfNote && docIsIBarSelection( ds ) )
	    {
	    DocumentPosition	dpPrev= ds->dsHead;

	    if  ( ! docPrevPosition( &dpPrev )		&&
		  dpPrev.dpNode == ds->dsHead.dpNode	)
		{
		dfNote= docFindTypedFieldForPosition( bd, &dpPrev,
							DOCfkCHFTN, lastOne );
		}
	    }

	if  ( ! dfNote )
	    { return (DocumentField *)0;	}
	else{
	    dn= docGetNoteOfField( dfNote, bd );
	    if  ( ! dn )
		{ XDEB(dn); return (DocumentField *)0;	}
	    }

	selInNote= 0;
	}

    if  ( ! dn || ! dfNote )
	{ /* XXDEB(dn,dfNote); */ return (DocumentField *)0;	}

    *pDn= dn;
    *pSelInNote= selInNote;
    return dfNote;
    }

/************************************************************************/

int docCheckSeparatorItemForNoteType(	BufferDocument *	bd,
					int			noteTreeType )
    {
    switch( noteTreeType )
	{
	case DOCinFOOTNOTE:
	    if  ( docCheckNoteSeparatorItem( bd, DOCinFTNSEP ) )
		{ LDEB(noteTreeType); return -1; }
	    break;

	case DOCinENDNOTE:
	    if  ( docCheckNoteSeparatorItem( bd, DOCinAFTNSEP ) )
		{ LDEB(noteTreeType); return -1; }
	    break;

	default:
	    LDEB(noteTreeType); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return a particular separator for an annotation by scope.		*/
/*									*/
/************************************************************************/

DocumentTree *	docDocumentNoteSeparator(	BufferDocument *	bd,
						int			treeType )
    {
    switch( treeType )
	{
	case DOCinFTNSEP:
	    return &(bd->bdEiFtnsep);

	case DOCinFTNSEPC:
	    return &(bd->bdEiFtnsepc);

	case DOCinFTNCN:
	    return &(bd->bdEiFtncn);

	case DOCinAFTNSEP:
	    return &(bd->bdEiAftnsep);

	case DOCinAFTNSEPC:
	    return &(bd->bdEiAftnsepc);

	case DOCinAFTNCN:
	    return &(bd->bdEiAftncn);

	default:
	    LDEB(treeType); return (DocumentTree *)0;
	}
    }

/************************************************************************/
/*									*/
/*  Verify that a certain kind of notes separator exists. Make it if	*/
/*  not.								*/
/*									*/
/*  1)  Correctly handeled by docTextAttributeNumber().			*/
/*									*/
/************************************************************************/

int docCheckNoteSeparatorItem(		BufferDocument *	bd,
					int			sepTreeType )
    {
    DocumentTree *	dt;
    int			particuleKind;

    switch( sepTreeType )
	{
	case DOCinFTNSEP:
	case DOCinAFTNSEP:
	    particuleKind= DOCkindCHFTNSEP;
	    break;

	case DOCinFTNSEPC:
	case DOCinAFTNSEPC:
	    particuleKind= DOCkindCHFTNSEPC;
	    break;

	default:
	    LDEB(sepTreeType);
	    particuleKind= DOCkindCHFTNSEP;
	    break;
	}

    dt= docDocumentNoteSeparator( bd, sepTreeType );
    if  ( ! dt )
	{ LXDEB(sepTreeType,dt); return -1;	}

    if  ( ! dt->dtRoot )
	{
	const int		ownerNumber= -1;
	int			txtAttrNr;

	BufferItem *		paraNode;
	TextAttribute		ta;

	utilInitTextAttribute( &ta );
	ta.taFontNumber= -1; /*  1  */
	ta.taFontSizeHalfPoints= 24;

	txtAttrNr= docTextAttributeNumber( bd, &ta );
	if  ( txtAttrNr < 0 )
	    { LDEB(txtAttrNr); return -1;	}

	paraNode= docMakeExternalParagraph( bd, dt, sepTreeType,
				bd->bdBody.dtRoot, ownerNumber, txtAttrNr );
	if  ( ! paraNode )
	    { XDEB(paraNode);	}
	else{
	    int		stroffShift= 0;
	    int		off= docParaStrlen( paraNode );

	    if  ( docParaStringReplace( &stroffShift, paraNode,
							off, off, "-", 1 ) )
		{ LDEB(1);	}
	    else{
		TextParticule *	tp= paraNode->biParaParticules;

		tp->tpKind= particuleKind;
		tp->tpStrlen= 1;
		}
	    }
	}

    return 0;
    }

/************************************************************************/

DocumentNote *	docGetNoteOfField(	const DocumentField *	dfNote,
					const BufferDocument *	bd )
    {
    const NotesList *	nl= &(bd->bdNotesList);

    if  ( ! docFieldHasNote( dfNote->dfKind ) )
	{
	LSDEB(dfNote->dfKind,docFieldKindStr(dfNote->dfKind));
	return (DocumentNote *)0;
	}

    if  ( dfNote->dfSelectionScope.ssTreeType == DOCinBODY )
	{
	if  ( dfNote->dfNoteIndex < 0			||
	      dfNote->dfNoteIndex >= nl->nlNoteCount	)
	    {
	    LLLDEB(dfNote->dfFieldNumber,dfNote->dfNoteIndex,nl->nlNoteCount);
	    return (DocumentNote *)0;
	    }

	return nl->nlNotes+ dfNote->dfNoteIndex;
	}

    if  ( dfNote->dfSelectionScope.ssTreeType == DOCinFOOTNOTE		||
	  dfNote->dfSelectionScope.ssTreeType == DOCinENDNOTE		||
	  dfNote->dfSelectionScope.ssTreeType == DOCinANNOTATION	)
	{
	DocumentField *	dfOwner;

	dfOwner= docGetFieldByNumber( &(bd->bdFieldList),
				dfNote->dfSelectionScope.ssOwnerNumber );

	if  ( dfOwner->dfNoteIndex < 0			||
	      dfOwner->dfNoteIndex >= nl->nlNoteCount	)
	    {
	    LLLDEB(dfOwner->dfFieldNumber,dfOwner->dfNoteIndex,nl->nlNoteCount);
	    return (DocumentNote *)0;
	    }

	return nl->nlNotes+ dfOwner->dfNoteIndex;
	}

    LDEB(dfNote->dfSelectionScope.ssTreeType);
    return (DocumentNote *)0;
    }

