/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<uniUtf8.h>
#   include	<ucdGeneralCategory.h>

#   include	"tedEdit.h"
#   include	"tedDocument.h"
#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedToolFront.h"
#   include	<docRtfTrace.h>
#   include	<docTreeType.h>
#   include	<docField.h>
#   include	<docTextParticule.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Replace the selection in the document with new text.		*/
/*									*/
/*  b)  Replace the tail of the beginning paragraph with the new text.	*/
/*  c)  Merge the two paragraphs.					*/
/*  d)  Erase all paragraphs after the beginning of the selection	*/
/*	to, including the end.						*/
/*  e)  Update the paragraph buffer and the particule administration.	*/
/*									*/
/*  B)  The first particule of the line was split, also reformat the	*/
/*	previous line.							*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*  C)  - Recalculate the geometry from the start of the selection to	*/
/*	  the end of the paragraph.					*/
/*	- Redraw the affected part of the paragraph.			*/
/*  6)  Update the notion of current particule and current line.	*/
/*  7)  Redraw the I-Bar.						*/
/*									*/
/************************************************************************/

int tedEditReplaceSelection(	TedEditOperation *	teo,
				const char *		addedText,
				int			addedLength,
				int			textAttributeNr )
    {
    EditOperation *		eo= &(teo->teoEo);

    /*  b,c,d,e  */
    if  ( docReplaceSelection( eo, addedText, addedLength, textAttributeNr ) )
	{ LDEB(addedLength); return -1;	}

    return 0;
    }

int tedEditDeleteSelection(	TedEditOperation *	teo )
    {
    EditOperation *		eo= &(teo->teoEo);

    /*  b,c,d,e  */
    if  ( docDeleteSelection( eo ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Merge the selection into one paragraph.				*/
/*									*/
/************************************************************************/

int tedMergeParagraphsInSelection(	EditDocument *		ed,
					int			traced )
    {
    int				rval= 0;
    int				beforeEnd;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdMERGE_PARAS, DOClevSPAN, 0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    beforeEnd= docParaStrlen( eo->eoTailDp.dpNode )-
					eo->eoSelectedRange.erTail.epStroff;

    if  ( docMergeParagraphsInSelection( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    eo->eoSelectedRange.erTail.epParaNr= eo->eoSelectedRange.erHead.epParaNr;
    eo->eoSelectedRange.erTail.epStroff=
				docParaStrlen( eo->eoHeadDp.dpNode )- beforeEnd;
    eo->eoAffectedRange= eo->eoSelectedRange;

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposALL ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Implementation of replace functionality.				*/
/*									*/
/************************************************************************/

static int tedReplaceSelectionImpl(	TedEditOperation *	teo,
					const char *		word,
					int			len,
					int			typing,
					int			textAttrNr )
    {
    EditOperation *		eo= &(teo->teoEo);

    if  ( tedEditReplaceSelection( teo, word, len, textAttrNr ) )
	{ LDEB(1); return -1;	}

    if  ( typing == TYPING_BLANK || typing == TYPING_NONBLANK )
	{
	EditDocument *		ed= teo->teoEditDocument;
	TedDocument *		td= (TedDocument *)ed->edPrivateData;
	EditTrace *		et= &(td->tdEditTrace);

	eo->eoAffectedRange.erHead= et->etTypingOldRange.erHead;
	}

    /*  6,7  */
    tedEditFinishSelectionTail( teo );

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); return -1;	}
	}

    tedFinishEditOperation( teo );

    return 0;
    }

int tedDeleteSelectionImpl(	TedEditOperation *	teo )
    {
    EditDocument *		ed= teo->teoEditDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    return tedReplaceSelectionImpl( teo, (const char *)0, 0, TYPING_NO,
			td->tdSelectionDescription.sdTextAttributeNumber );
    }

/************************************************************************/

static int tedDetermineTypingClass(	int *			pHeadClass,
					const EditTrace *	et,
					const char *		str,
					int			length )
    {
    int			headClass= TYPING_NO;
    int			tailClass= TYPING_NO;

    int		done= 0;

    while( done < length )
	{
	unsigned short	uni;
	int		step;

	step= uniGetUtf8( &uni, str+ done );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	if  ( ucdIsZ( uni ) )
	    { tailClass= TYPING_BLANK;		}
	else{ tailClass= TYPING_NONBLANK;	}

	if  ( done == 0 )
	    { headClass= tailClass;		}

	done += step;
	}

    if  ( et->etTyping == TYPING_NO					||
	  ( et->etTyping == TYPING_BLANK && headClass == TYPING_NONBLANK ) )
	{ *pHeadClass= TYPING_START;	}
    else{ *pHeadClass= et->etTyping;	}

    return tailClass;
    }

/************************************************************************/
/*									*/
/*  Refuse to scribble in the 'bullet' of a note.			*/
/*									*/
/************************************************************************/

static int tedEditHoldsNoteHead(	const SelectionDescription *	sd,
					const EditOperation *		eo )
    {
    if  ( sd->sdInTreeType == DOCinFOOTNOTE		||
	  sd->sdInTreeType == DOCinENDNOTE		)
	{
	DocumentPosition	dp= eo->eoHeadDp;

	if  ( docPrevPosition( &dp ) )
	    { return 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the selection with something new.				*/
/*									*/
/*  1)  No selection... Just refuse.					*/
/*  2)  Replace nothing with nothing: Do nothing.			*/
/*	(Saves a lot of work, and avoids some combinations of special	*/
/*	cases)								*/
/*  3)  Refuse to scribble in the 'bullet' of a note.			*/
/*									*/
/************************************************************************/

int tedDocReplaceSelection(	EditDocument *		ed,
				int			command,
				const char *		str,
				int			length,
				int			traced )
    {
    int				rval =0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditTrace *			et= &(td->tdEditTrace);

    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			fullWidth= 0;

    /*  1  */
    if  ( ! tedHasSelection( ed ) )
	{ LDEB(1); rval= -1; goto ready; }

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    /*  2  */
    if  ( teo.teoEo.eoIBarSelectionOld && length == 0 )
	{ goto ready;	}

    /*  3  */
    if  ( tedEditHoldsNoteHead( &sd, &(teo.teoEo) ) )
	{ goto ready;	}

    if  ( traced )
	{
	DocumentSelection		dsTraced;

	if  ( tedEditStartReplace( &dsTraced, &teo, command, DOClevSPAN, 0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedReplaceSelectionImpl( &teo, str, length, TYPING_NO,
			td->tdSelectionDescription.sdTextAttributeNumber ) )
	{ LDEB(length); rval= -1; goto ready;	}

    et->etTyping= TYPING_NO;

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

int tedDocDeleteSelection(	EditDocument *		ed,
				int			command,
				int			traced )
    {
    return tedDocReplaceSelection( ed, command, (const char *)0, 0, traced );
    }

int tedDocReplaceSelectionTyping(	EditDocument *		ed,
					const char *		str,
					int			length )
    {
    int				rval =0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditTrace *			et= &(td->tdEditTrace);

    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			fullWidth= 0;

    int				headClass= et->etTyping;
    int				tailClass= TYPING_NO;

    /*  1  */
    if  ( ! tedHasSelection( ed ) )
	{ LDEB(1); rval= -1; goto ready; }

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, td->tdTraced );

    /*  3  */
    if  ( tedEditHoldsNoteHead( &sd, &(teo.teoEo) ) )
	{ goto ready;	}

    if  ( td->tdTraced )
	{
	tailClass= tedDetermineTypingClass( &headClass, et, str, length );

	if  ( tailClass < 0 )
	    { LDEB(tailClass); rval= -1; goto ready;	}
	}
    else{
	if  ( et->etTyping != TYPING_NO )
	    { CDEB(et->etTyping);	}
	}

    if  ( td->tdTraced )
	{
	if  ( headClass == TYPING_BLANK		||
	      headClass == TYPING_NONBLANK	)
	    {
	    if  ( teo.teoEditTrace					&&
		  docTraceExtendReplace( &(teo.teoEo), teo.teoEditTrace,
				    EDITcmdEXTEND_REPLACE, DOClevSPAN, 0 ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	else{
	    DocumentSelection		dsTraced;

	    if  ( tedEditStartReplace( &dsTraced, &teo,
					    EDITcmdREPLACE, DOClevSPAN, 0 ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( headClass == TYPING_START )
		{ docSetEditRange( &(et->etTypingOldRange), &dsTraced ); }
	    }
	}

    if  ( tedReplaceSelectionImpl( &teo, str, length, headClass,
			td->tdSelectionDescription.sdTextAttributeNumber ) )
	{ LDEB(length); rval= -1; goto ready;	}

    et->etTyping= tailClass;

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a string with known attributes (font)			*/
/*									*/
/************************************************************************/

void tedDocInsertStringWithAttribute(	EditDocument *		ed,
					const char *		word,
					int			len,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask,
					int			traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    PropertyMask		taDoneMask;

    TextAttribute		ta= td->tdSelectionDescription.sdTextAttribute;
    int				textAttrNr= td->tdSelectionDescription.sdTextAttributeNumber;

    DocumentSelection		dsTraced;

    const int			fullWidth= 0;

    /*  1  */
    if  ( ! tedHasSelection( ed ) )
	{ LDEB(1); goto ready; }

    utilUpdateTextAttribute( &taDoneMask, &ta, taSetMask, taSet );
    textAttrNr= docTextAttributeNumber( bd, &ta );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartReplace( &dsTraced, &teo,
					    EDITcmdREPLACE, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    if  ( tedReplaceSelectionImpl( &teo, word, len, TYPING_NO, textAttrNr ) )
	{ LDEB(len); goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a (line/page) break particule in the document.		*/
/*									*/
/*  Against flashing, a more subtle approach is used for TABs.		*/
/*  (redoLayout=0).							*/
/*									*/
/************************************************************************/

void tedEditInsertSpecialParticule(	EditDocument *	ed,
					int		kind,
					int		command,
					int		redoLayout,
					int		traced )
    {
    TextParticule *		tp;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsTraced;

    const int			fullWidth= 0;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartReplace( &dsTraced, &teo, command, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    if  ( tedEditDeleteSelection( &teo ) )
	{ goto ready;	}

    tp= docEditParaSpecialParticule( eo, kind );
    if  ( ! tp )
	{ XDEB(tp); goto ready;	}

    if  ( docCheckNoBreakAsLast( eo, eo->eoTailDp.dpNode ) )
	{ LDEB(1);	}

    /*  3,4,5  */
    if  ( redoLayout )
	{ docEditIncludeNodeInReformatRange( eo, eo->eoTailDp.dpNode );	}

    /*  6,7  */
    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:
    tedCleanEditOperation( &teo );

    return;
    }

int tedDocUpdField(	TedEditOperation *		teo,
			DocumentField *			dfTo,
			const FieldInstructions *	fi )
    {
    int			rval= 0;
    EditDocument *	ed= teo->teoEditDocument;
    EditOperation *	eo= &(teo->teoEo);

    DocumentSelection	dsAround;

    if  ( tedEditStartTypedStep( teo, EDITcmdUPD_FIELD, dfTo->dfKind ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceOldField( eo, dfTo ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docCopyFieldInstructions( &(dfTo->dfInstructions), fi ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedFormatFieldListChanged( ed->edApplication );

    if  ( dfTo->dfKind == DOCfkTOC			&&
	  tedRefreshTocField( &dsAround, teo, dfTo )	)
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishOldSelection( teo );

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceNewField( eo, dfTo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( teo );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Surround the current selection with a field.			*/
/*									*/
/*  This is a special case for Hyperlinks and Bookmarks that hold	*/
/*  editable text that is part of the document rather than content that	*/
/*  is calculated by the field itself.					*/
/*									*/
/************************************************************************/

int tedDocSetField(	TedEditOperation *		teo,
			const DocumentSelection *	ds,
			int				command,
			int				fieldKind,
			const FieldInstructions *	fi,
			const PropertyMask *		taSetMask,
			const TextAttribute *		taSet )
    {
    int			rval= 0;

    EditDocument *	ed= teo->teoEditDocument;
    EditOperation *	eo= &(teo->teoEo);

    DocumentField *	df;
    DocumentSelection	dsField;

    DocumentSelection	dsInside;
    DocumentSelection	dsAround;
    int			headPart;
    int			tailPart;

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}

    dsField= *ds;

    if  ( DOC_FieldKinds[fieldKind].fkiSingleParagraph )
	{
	int	beginMoved;
	int	endMoved;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved,
							    &dsField );
	eo->eoHeadDp= dsField.dsHead;
	eo->eoTailDp= dsField.dsTail;
	eo->eoLastDp= eo->eoTailDp;
	}

    if  ( tedEditStartStep( teo, command ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo->teoEditTrace && taSetMask )
	{
	DocumentSelection	dsTraced;

	if  ( docRtfTraceNewProperties( eo,
		    taSetMask, taSet,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docRtfTraceOldContents( &dsTraced, eo, DOClevSPAN, 0 ) )
	    { LDEB(1); goto ready;	}
	}

    /*  4  */
    if  ( docEditSurroundTextSelectionByField( eo, &df,
			&dsInside, &dsAround, &headPart, &tailPart,
			taSetMask, taSet, fieldKind, fi ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  5  */
    if  ( tedLayoutNodeOfField( teo, &dsAround, FIELDdoNOTHING ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedFormatFieldListChanged( ed->edApplication );

    tedEditFinishSelection( teo, &dsInside );

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceNewField( eo, df ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( teo );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Roll the rows in a table.						*/
/*									*/
/************************************************************************/

int tedEditShiftRowsInTable(	EditDocument *		ed,
				int			direction,
				int			traced )
    {
    int				rval= 0;
    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			move= 1;
    int				rowsdown= 0;
    int				command= -1;

    const int			fullWidth= 1;

    if  ( direction > 0 )
	{
	rowsdown=  1;
	command= EDITcmdSHIFT_ROWS_DOWN;
	}

    if  ( direction < 0 )
	{
	rowsdown= -1;
	command= EDITcmdSHIFT_ROWS_UP;
	}

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartStep( &teo, command ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedDocRollRowsInTableImpl( &teo, &(sd.sdTableRectangle),
							    move, rowsdown ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishOldSelection( &teo );

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

