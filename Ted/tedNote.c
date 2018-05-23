/************************************************************************/
/*									*/
/*  Ted: Field,Bookmark,Hyperlink related editing functionality.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedEdit.h"
#   include	"tedSelect.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docEditImpl.h>
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a footnote in the document.					*/
/*									*/
/*  1)  Change the selection to a reference to a note.			*/
/*  2)	Insert a note immediately after the reference.			*/
/*  3)	Preliminary layout of the footnote.				*/
/*  4)	Layout of the node that receives the footnote. (Usually redoes	*/
/*	the layout of the footnote)					*/
/*  5)	Select the end of the freshly inserted footnote.		*/
/*  6)	Finish the edit operation with the cursor at the end of the new	*/
/*	note. That is after the number and the space after it.		*/
/*  7)	Notes only have a single location: Make the formatter happy and	*/
/*	tell that it has been selected where it belongs.		*/
/*									*/
/************************************************************************/

int tedDocInsertNote(	EditDocument *		ed,
			int			noteTreeType,
			int			traced )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsInside;
    DocumentSelection		dsAround;

    const unsigned int		fieldUpdMask= FIELDdoCHFTN;

    DocumentNote *		dn;
    DocumentField *		dfNote;
    struct BufferItem *		bodySectNode;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpEnd;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    int				headPart;
    int				tailPart;

    int				fieldKind= DOCfkCHFTN;

    DocumentSelection		dsTraced;

    FieldInstructions		fi;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fi );

    if  ( docStartFieldInstructions( &fi, "-CHFTN", 6 ) )
	{ LDEB(6); rval= -1; goto ready;	}

    utilInitTextAttribute( &taSet );
    utilPropMaskClear( &taSetMask );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( sd.sdInTreeType != DOCinBODY )
	{ LDEB(sd.sdInTreeType); rval= -1; goto ready;	}

    if  ( tedEditStartReplace( &dsTraced, &teo, EDITcmdINS_NOTE, DOClevSPAN, 0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bodySectNode= docGetSectNode( eo->eoHeadDp.dpNode );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); rval= -1; goto ready;	}

    /*  1  */
    if  ( tedDocReplaceSelectionWithField( &teo, &dfNote,
					    &headPart, &tailPart,
					    &dsInside, &dsAround,
					    &fi, fieldKind,
					    &taSetMask, &taSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  2  */
    if  ( docEditMakeNote( &dn, bd, dfNote,
				    bodySectNode, noteTreeType, fieldKind ) )
	{ LDEB(1); rval= -1; goto ready;	}
    utilIndexSetAdd( &(eo->eoNoteFieldsAdded), dfNote->dfFieldNumber );

    /*  4  */
    if  ( tedLayoutNodeOfField( &teo, &dsAround, fieldUpdMask ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  5  */
    if  ( docTailPosition( &dpEnd, dn->dnDocumentTree.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  6  */
    eo->eoTree= &(dn->dnDocumentTree);
    docGetSelectionScope( &(eo->eoSelectionScope), dpEnd.dpNode );

    docSetIBarRange( &(eo->eoAffectedRange), &dpEnd );
    docSetIBarRange( &(eo->eoSelectedRange), &dpEnd );
    tedEditFinishSelectionTail( &teo );

    tedFinishEditOperation( &teo );

  ready:

    docCleanFieldInstructions( &fi );
    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the kind of note for the current selection.			*/
/*									*/
/************************************************************************/

int tedChangeNoteImpl(		EditOperation *		eo,
				DocumentField *		dfNote,
				DocumentNote *		dn,
				int			selInNote,
				const PropertyMask *	npSetMask,
				const NoteProperties *	npSet )
    {
    PropertyMask		npDoneMask;

    utilPropMaskClear( &npDoneMask );

    if  ( docUpdNoteProperties( &npDoneMask, &(dn->dnNoteProperties),
							npSetMask, npSet ) )
	{ LDEB(1); return -1;	}

    if  ( PROPmaskISSET( &npDoneMask, NOTEpropTREE_TYPE ) )
	{
	if  ( docCheckSeparatorItemForNoteType( eo->eoDocument,
					    dn->dnNoteProperties.npTreeType ) )
	    { LDEB(DOCinFTNSEP); return -1;	}

	dn->dnNoteNumber= 0;
	docInvalidateTreeLayout( &(dn->dnDocumentTree) );

	if  ( ! dn->dnDocumentTree.dtRoot )
	    { XDEB(dn->dnDocumentTree.dtRoot); return -1;	}

	docSetTreeTypeOfNode( dn->dnDocumentTree.dtRoot,
					    dn->dnNoteProperties.npTreeType );

	if  ( selInNote )
	    {
	    eo->eoSelectionScope.ssTreeType= dn->dnNoteProperties.npTreeType;
	    }
	}

    eo->eoFieldUpdate |= FIELDdoCHFTN;
    utilIndexSetAdd( &(eo->eoNoteFieldsAdded), dfNote->dfFieldNumber );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the kind of note for the current selection.			*/
/*									*/
/************************************************************************/

int tedDocChangeCurrentNote(	EditDocument *		ed,
				const PropertyMask *	npSetMask,
				const NoteProperties *	npSet,
				int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentField *		dfNote= (DocumentField *)0;
    DocumentNote *		dn= (DocumentNote *)0;
    int				selInNote= 0;

    const int			fullWidth= 0;

    PropertyMask		npDoneMask;

    utilPropMaskClear( &npDoneMask );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    dfNote= docEditOperationGetSelectedNote( &dn, &selInNote, eo );
    if  ( ! dfNote )
	{ XDEB(dfNote); rval= -1; goto ready;	}

    if  ( tedEditStartStep( &teo, EDITcmdUPD_NOTE ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceOldNoteProperties( eo,
				    npSetMask, &(dn->dnNoteProperties) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docRtfTraceNewNoteProperties( eo, npSetMask, npSet ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedChangeNoteImpl( eo, dfNote, dn, selInNote, npSetMask, npSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/

static DocumentField * tedGetSelectedNote(	DocumentNote **	pDn,
						int *		pSelInNote,
						EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return docGetSelectedNote( pDn, pSelInNote, bd, &ds );
    }

/************************************************************************/

static int tedGotoNoteRef(	EditDocument *		ed,
				const DocumentField *	dfNote )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsRef;
    const int			lastLine= 0;

    if  ( docSelectionForEditPositionsInTree( &dsRef, &(bd->bdBody),
		    &(dfNote->dfHeadPosition), &(dfNote->dfTailPosition) ) )
	{ LDEB(1); return -1;	}

    tedSetSelection( ed, &dsRef, lastLine, (int *)0, (int *)0 );

    return 0;
    }

static int tedGotoNoteDef(	EditDocument *		ed,
				const DocumentNote *	dn )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentPosition		dpNote;
    const int			lastLine= 0;

    if  ( ! dn->dnDocumentTree.dtRoot					||
	  docHeadPosition( &dpNote, dn->dnDocumentTree.dtRoot )	)
	{ XDEB(dn->dnDocumentTree.dtRoot); return -1;	}

    docAvoidParaHeadField( &dpNote, (int *)0, bd );

    tedSetSelectedPosition( ed, &dpNote, lastLine, &scrolledX, &scrolledY );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Jump to the foot/end note thet is selected in the body of the	*/
/*  document.								*/
/*									*/
/************************************************************************/

void tedDocGotoNoteDef(		EditDocument *	ed )
    {
    DocumentNote *		dn= (DocumentNote *)0;
    DocumentField *		dfNote= (DocumentField *)0;
    int				selInNote= 0;

    dfNote= tedGetSelectedNote( &dn, &selInNote, ed );
    if  ( ! dfNote || selInNote )
	{ XDEB(dfNote); return;	}

    if  ( tedGotoNoteDef( ed, dn ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Jump to the foot/end note reference in the body of the document to	*/
/*  the note text that is selected.					*/
/*									*/
/************************************************************************/

void tedDocGotoNoteRef(		EditDocument *	ed )
    {
    DocumentField *		dfNote= (DocumentField *)0;
    DocumentNote *		dn= (DocumentNote *)0;
    int				selInNote= 0;

    dfNote= tedGetSelectedNote( &dn, &selInNote, ed );
    if  ( ! dfNote || ! selInNote )
	{ XLDEB(dfNote,selInNote); return;	}

    if  ( tedGotoNoteRef( ed, dfNote ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Navigate over the notes in the document.				*/
/*									*/
/************************************************************************/

static void tedStepNote(		EditDocument *	ed,
					int		direction )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentField *		dfNote= (DocumentField *)0;
    DocumentNote *		dn= (DocumentNote *)0;
    int				selInNote= 0;

    dfNote= tedGetSelectedNote( &dn, &selInNote, ed );
    if  ( ! dfNote )
	{ XDEB(dfNote); return;	}

    if  ( direction > 0 )
	{
	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 );
	if  ( ! dfNote )
	    { XDEB(dfNote); return;	}
	}
    else{
	dfNote= docGetPrevNoteInDocument( &dn, bd, dfNote, -1 );
	if  ( ! dfNote )
	    { XDEB(dfNote); return;	}
	}

    if  ( selInNote )
	{
	if  ( tedGotoNoteDef( ed, dn ) )
	    { LDEB(1);	}
	}
    else{
	if  ( tedGotoNoteRef( ed, dfNote ) )
	    { LDEB(1);	}
	}

    return;
    }

void tedDocNextNote(		EditDocument *	ed )
    {
    const int	direction=  1;

    tedStepNote( ed, direction );
    }

void tedDocPrevNote(		EditDocument *	ed )
    {
    const int	direction= -1;

    tedStepNote( ed, direction );
    }
