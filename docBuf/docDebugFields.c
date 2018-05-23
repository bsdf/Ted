#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docTreeNode.h"
#   include	"docDebug.h"
#   include	<docTreeType.h>
#   include	<docTextParticule.h>
#   include	<docDocumentNote.h>

#   include	<appDebugon.h>

void docListFieldParticule(	int			indent,
				const char *		label,
				int			n,
				const BufferItem *	bi,
				const TextParticule *	tp,
				const DocumentField *	df )
    {
    appDebug( "%*s%s %3d: [S %4d..%4d] %s",
		    indent, "", label, n,
		    tp->tpStroff, tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ) );

    if  ( tp->tpStrlen > 0 )
	{
	appDebug( " \"%.*s\"",
		    (int)tp->tpStrlen,
		    docParaString( bi, tp->tpStroff ) );
	}

    if  ( tp->tpKind == DOCkindOBJECT		||
	  tp->tpKind == DOCkindFIELDHEAD	||
	  tp->tpKind == DOCkindFIELDTAIL	||
	  tp->tpObjectNumber >= 0		)
	{ appDebug( " OBNR=%d", tp->tpObjectNumber );	}

    if  ( df )
	{ appDebug( " FIELD:%s", docFieldKindStr( df->dfKind ) );	}

    appDebug( "\n" );

    return;
    }

static int docCheckFieldHeadParticule(	int			stroff,
					int			part,
					int			paraNr,
					const BufferItem *	paraNode,
					const DocumentField *	df )
    {
    int				wrong= 0;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    if  ( tp->tpKind != DOCkindFIELDHEAD )
	{ SLLDEB("####",tp->tpKind,DOCkindFIELDHEAD); wrong= 1;	}

    if  ( df->dfFieldNumber != tp->tpObjectNumber )
	{
	LLDEB(df->dfFieldNumber,tp->tpObjectNumber);
	docListFieldParticule( 4, "#FN#", part, paraNode, tp, df );
	wrong= 1;
	}

    if  ( df->dfHeadPosition.epParaNr != paraNr )
	{
	LLDEB(df->dfHeadPosition.epParaNr,paraNr);
	docListFieldParticule( 4, "#PA#", part, paraNode, tp, df );
	wrong= 1;
	}
    if  ( df->dfHeadPosition.epStroff != tp->tpStroff )
	{
	LLDEB(df->dfHeadPosition.epStroff,tp->tpStroff);
	docListFieldParticule( 4, "#ST#", part, paraNode, tp, df );
	wrong= 1;
	}

    return wrong;
    }

static int docCheckFieldTailParticule(	int			stroff,
					int			part,
					int			paraNr,
					const BufferItem *	paraNode,
					const DocumentField *	df )
    {
    int				wrong= 0;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    if  ( tp->tpKind != DOCkindFIELDTAIL )
	{ SLLDEB("####",tp->tpKind,DOCkindFIELDTAIL); wrong= 1;	}

    if  ( df->dfFieldNumber != tp->tpObjectNumber )
	{
	LLDEB(df->dfFieldNumber,tp->tpObjectNumber);
	docListFieldParticule( 4, "#FN#", part, paraNode, tp, df );
	wrong= 1;
	}

    if  ( df->dfTailPosition.epParaNr != paraNr )
	{
	LLDEB(df->dfTailPosition.epParaNr,paraNr);
	docListFieldParticule( 4, "#PA#", part, paraNode, tp, df );
	wrong= 1;
	}
    if  ( df->dfTailPosition.epStroff != tp->tpStroff )
	{
	LLDEB(df->dfTailPosition.epStroff,tp->tpStroff);
	docListFieldParticule( 4, "#ST#", part, paraNode, tp, df );
	wrong= 1;
	}

    return wrong;
    }

void docCheckFieldOffsets(	const BufferDocument *	bd,
				const BufferItem *	bi )
    {
    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    switch( bi->biLevel )
	{
	int			i;
	int			part;
	const TextParticule *	tp;
	int			paraNr;
	int			wrong;

	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ docCheckFieldOffsets( bd, bi->biChildren[i] );	}
	    break;

	case DOClevPARA:
	    paraNr= docNumberOfParagraph( bi );
	    tp=  bi->biParaParticules;
	    wrong= 0;
	    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
		{
		const DocumentField *	df;

		if  ( tp->tpKind != DOCkindFIELDTAIL	&&
		      tp->tpKind != DOCkindFIELDHEAD	)
		    { continue;	}

		df= docGetFieldByNumber( dfl, tp->tpObjectNumber );
		if  ( ! df )
		    { SLXDEB("####",tp->tpObjectNumber,df); continue;	}

		if  ( tp->tpKind == DOCkindFIELDHEAD )
		    {
		    if  ( docCheckFieldHeadParticule( tp->tpStroff,
						    part, paraNr, bi, df ) )
			{ wrong= 1;	}
		    }
		if  ( tp->tpKind == DOCkindFIELDTAIL )
		    {
		    if  ( docCheckFieldTailParticule( tp->tpStroff,
						    part, paraNr, bi, df ) )
			{ wrong= 1;	}
		    }
		}
	    if  ( wrong )
		{ docListNode(0,bi,0);	}
	    break;

	case DOClevOUT:
	default:
	    break;
	}

    return;
    }

typedef struct FieldPosition
    {
    EditPosition	fpPosition;
    int			fpPage;
    int			fpColumn;
    } FieldPosition;

static void docCheckNoteOfField(	int			indent,
					FieldPosition *		fp,
					const DocumentField *	df,
					const BufferDocument *	bd )
    {
    const DocumentNote *	dn= docGetNoteOfField( df, bd );

    if  ( ! dn )
	{ SXDEB("##",dn); return;	}

    if  ( dn->dnReferringPage < fp->fpPage			||
	  ( dn->dnReferringPage == fp->fpPage	&&
	    dn->dnReferringColumn < fp->fpColumn	)	)
	{
	appDebug( "%*s## NOTE %d:%d PG/COL BEFORE PREVIOUS %d:%d\n",
		indent+ 15, "",
		dn->dnReferringPage, dn->dnReferringColumn,
		fp->fpPage, fp->fpColumn );
	}

    if  ( ! dn->dnDocumentTree.dtRoot )
	{ SXDEB("##",dn->dnDocumentTree.dtRoot);	}
    else{
	/*
	const int	noteIndent= 4;
	*/

	const SelectionScope *	ssRoot= &(dn->dnDocumentTree.dtRoot->biSectSelectionScope);

	if  ( ssRoot->ssSectNr != 0 )
	    { SLDEB("##",ssRoot->ssSectNr); }
	if  ( ssRoot->ssOwnerNumber != df->dfFieldNumber )
	    { SLLDEB("##",ssRoot->ssOwnerNumber,df->dfFieldNumber); }

	/*
	docListNode(noteIndent,dn->dnDocumentTree.dtRoot,0);
	*/

	fp->fpPage= dn->dnReferringPage;
	fp->fpColumn= dn->dnReferringColumn;
	}

    return;
    }

static void docCheckFieldPosition(
				int			indent,
				FieldPosition *		fp,
				const DocumentField *	df,
				const BufferDocument *	bd )
    {
    const NotesList *		nl= &(bd->bdNotesList);
    const char *		endString= "}";
    int				fix= 12- indent;

    if  ( fix < 0 )
	{ fix= 0;	}

    if  ( df->dfChildFields.cfChildCount > 0 )
	{ endString= " "; }

    appDebug( "%*s {%*s%4d:%-4d .. %4d:%-4d %s #%d:%s\n",
		indent, "",
		fix, "",
		df->dfHeadPosition.epParaNr,
		df->dfHeadPosition.epStroff,
		df->dfTailPosition.epParaNr,
		df->dfTailPosition.epStroff,
		endString,
		df->dfFieldNumber, docFieldKindStr( df->dfKind ) );

    /*
    docListFieldInstructions( indent+ 8, &(df->dfInstructions ) );
    */

    if  ( docCompareEditPositions( &(fp->fpPosition),
					    &(df->dfHeadPosition) ) > 0 )
	{ appDebug( "%*s## START BEFORE PREV or PARENT\n", indent+ 15, "" ); }

    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					&(df->dfTailPosition) ) > 0 )
	{ appDebug( "%*s## START AFTER END\n", indent+ 15, "" ); }

    if  ( df->dfSelectionScope.ssTreeType == DOCinBODY	&&
	  docFieldHasNote( df->dfKind )			&&
	  nl						)
	{
	if  ( df->dfNoteIndex < 0 || df->dfNoteIndex >= nl->nlNoteCount )
	    {
	    appDebug( "%*s## Note %d/%d\n", indent+ 15, "",
				df->dfNoteIndex, nl->nlNoteCount );
	    }
	else{ docCheckNoteOfField( indent, fp, df, bd );	}
	}

    {
    DocumentTree  *	dt;
    BufferItem *	bodySectNode= (BufferItem *)0;

    if  ( docGetRootOfSelectionScope( &dt, &bodySectNode,
			(BufferDocument *)bd, &(df->dfSelectionScope) )	||
	  ! bodySectNode						)
	{
	SXDEB("##",bodySectNode);
	SSDEB("##",docTreeTypeStr(df->dfSelectionScope.ssTreeType));
	SLDEB("##",df->dfSelectionScope.ssSectNr);
	SLDEB("##",df->dfSelectionScope.ssOwnerSectNr);
	SLDEB("##",df->dfSelectionScope.ssOwnerNumber);
	}
    }

    return;
    }

static void docCheckFieldTree(		int			indent,
					const BufferDocument *	bd,
					const DocumentTree *	dt,
					FieldPosition *		fp,
					const DocumentField *	parent,
					const ChildFields *	cf )
    {
    int			i;

    for ( i= 0; i < cf->cfChildCount; i++ )
	{
	const DocumentField *	df= cf->cfChildren[i];

	docCheckFieldPosition( indent, fp, df, bd );

	if  ( df->dfParent != parent )
	    { appDebug( "%*s## WRONG PARENT\n", indent+ 15, "" ); }
	if  ( df->dfNumberInParent != i )
	    {
	    appDebug( "%*s## WRONG INDEX %d@%d\n", indent+ 15, "",
						df->dfNumberInParent, i );
	    }

	if  ( i > 0 && df == cf->cfChildren[i-1] )
	    { appDebug( "%*s## FIELD APPEARS TWICE\n", indent+ 15, "" ); }

	fp->fpPosition= df->dfHeadPosition;

	if  ( df->dfChildFields.cfChildCount > 0 )
	    {
	    int			fix= 25- indent;

	    if  ( fix < 0 )
		{ fix= 0;	}

	    docCheckFieldTree( indent+ 1, bd, dt,
					fp, df, &(df->dfChildFields) );

	    if  ( docCompareEditPositions( &(fp->fpPosition),
					    &(df->dfTailPosition ) ) > 0 )
		{ appDebug( "%*s## CHILD PAST END\n", indent+ 15, "" ); }

	    appDebug( "%*s }%*s%4d:%-4d   #%d:%s\n",
			indent, "",
			fix, "",
			df->dfTailPosition.epParaNr,
			df->dfTailPosition.epStroff,
			df->dfFieldNumber, docFieldKindStr( df->dfKind ) );

	    if  ( dt )
		{
		DocumentSelection	dsInside;
		DocumentSelection	dsAround;
		int			part0;
		int			part1;

		if  ( docDelimitFieldInTree( &dsInside, &dsAround,
						&part0, &part1, bd, dt, df ) )
		    {
		    appDebug( "### '%s' Field not found\n",
					    docFieldKindStr( df->dfKind ) );
		    LLDEB(df->dfHeadPosition.epParaNr,df->dfHeadPosition.epStroff);
		    LLDEB(df->dfTailPosition.epParaNr,df->dfTailPosition.epStroff);
		    }
		else{
		    docCheckFieldHeadParticule(
				dsAround.dsHead.dpStroff,
				part0, df->dfHeadPosition.epParaNr,
				dsAround.dsHead.dpNode, df );

		    docCheckFieldTailParticule(
				dsInside.dsTail.dpStroff,
				part1, df->dfTailPosition.epParaNr,
				dsInside.dsTail.dpNode, df );
		    }
		}
	    }

	fp->fpPosition= df->dfTailPosition;
	}
    }

void docListFieldTree(	const BufferDocument *	bd,
			const DocumentTree *	dt )
    {
    const ChildFields *	cf= &(dt->dtRootFields);
    DocumentField *	parent= (DocumentField *)0;

    if  ( cf->cfChildCount > 0 )
	{
	FieldPosition	fp;

	fp.fpPosition= cf->cfChildren[0]->dfHeadPosition;
	fp.fpPage= -1;
	fp.fpColumn= 0;

	docCheckFieldTree( 0, bd, dt, &fp, parent, cf );
	}

    return;
    }

void docListFields(	const ChildFields *	cf,
			const BufferDocument *	bd )
    {
    DocumentField *	df;

    df= docGetFirstField( cf );
    if  ( df )
	{
	FieldPosition	fp;

	fp.fpPosition= df->dfHeadPosition;
	fp.fpPage= -1;
	fp.fpColumn= 0;

	while( df )
	    {
	    const int		indent= 0;
	    DocumentField *	odf= df;

	    docCheckFieldPosition( indent, &fp, df, bd );

	    df= docGetNextField( cf, df );
	    if  ( df == odf )
		{ XXDEB(df,odf); /*abort();*/	}
	    }
	}
    }

static void docScanNotes(	const BufferDocument *	bd,
				int			treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    appDebug("-- %s:\n", docTreeTypeStr(treeType) );
    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    if  ( dfNote )
	{
	FieldPosition	fp;

	fp.fpPosition= dfNote->dfHeadPosition;
	fp.fpPage= -1;
	fp.fpColumn= 0;

	while( dfNote )
	    {
	    const int		indent= 0;

	    docCheckFieldPosition( indent, &fp, dfNote, bd );

	    dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	    }
	}
    }

void docScanNotesOfDocument(	const BufferDocument *	bd )
    {
    docScanNotes( bd, DOCinFOOTNOTE );
    docScanNotes( bd, DOCinENDNOTE );
    }

void docListField(		int			indent,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);

    appDebug( "%*s %4d:%-4d .. %4d:%-4d #%d:%s %d instructions\n",
		indent, "",
		df->dfHeadPosition.epParaNr,
		df->dfHeadPosition.epStroff,
		df->dfTailPosition.epParaNr,
		df->dfTailPosition.epStroff,
		df->dfFieldNumber, docFieldKindStr( df->dfKind ),
		fi->fiComponentCount );

    docListFieldInstructions( indent+ 4, fi );

    return;
    }
