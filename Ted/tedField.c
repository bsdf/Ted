/************************************************************************/
/*									*/
/*  Ted: Field,Bookmark,Hyperlink related editing functionality.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedSelect.h"
#   include	<docRtfTrace.h>
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	"tedToolFront.h"
#   include	<docField.h>
#   include	<docRecalculateFields.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine the area covered by a field or a bookmark.		*/
/*									*/
/************************************************************************/

static void tedIncludeFieldInRedraw(
				TedEditOperation *		teo,
				const DocumentSelection *	dsField,
				int				headPart,
				int				tailPart )
    {
    EditOperation *		eo= &(teo->teoEo);

    SelectionGeometry		sgNew;

    const int			lastLine= 1;

    tedSelectionGeometry( &sgNew, dsField, eo->eoBodySectNode, lastLine,
						    &(teo->teoLayoutContext) );

    tedIncludeRectangleInChange( teo, &(sgNew.sgRectangle) );

    return;
    }

/************************************************************************/
/*									*/
/*  Convert the current selection to a field or a bookmark.		*/
/*									*/
/************************************************************************/

int tedLayoutNodeOfField(	TedEditOperation *	teo,
				DocumentSelection *	dsAround,
				unsigned int		whenMask )
    {
    const LayoutContext *	lc= &(teo->teoLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    struct BufferItem *		bi;
    DocumentTree *		ei;
    struct BufferItem *		bodySectNode;

    bi= docGetSelectionRoot( &ei, &bodySectNode, bd, dsAround );
    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    if  ( whenMask )
	{
	RecalculateFields		rf;

	docInitRecalculateFields( &rf );

	rf.rfDocument= bd;
	rf.rfTree= ei;
	rf.rfSelectedTree= ei;
	rf.rfCloseObject= lc->lcCloseObject;
	rf.rfUpdateFlags= whenMask;
	rf.rfFieldsUpdated= 0;

	if  ( docRecalculateTextLevelFields( &rf, bi ) )
	    { XDEB(whenMask);	}
	}

    docEditIncludeNodeInReformatRange( &(teo->teoEo), bi );

    return 0;
    }

/************************************************************************/

int tedFlattenFieldImpl(	TedEditOperation *		teo,
				DocumentSelection *		dsExInside,
				const DocumentSelection *	dsAroundField,
				int				headPart,
				int				tailPart,
				DocumentField *			df )
    {
    EditOperation *		eo= &(teo->teoEo);

    /*  1  */
    tedIncludeFieldInRedraw( teo, dsAroundField, headPart, tailPart );

    /*  0,2  */
    docDeleteField( dsExInside, eo,
		    dsAroundField->dsHead.dpNode,
		    dsAroundField->dsTail.dpNode,
		    headPart, tailPart, df );

    docEditIncludeNodeInReformatRange( eo, dsAroundField->dsHead.dpNode );
    docEditIncludeNodeInReformatRange( eo, dsAroundField->dsTail.dpNode );

    tedFormatFieldListChanged( teo->teoEditDocument->edApplication );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove the particules that delimit a a field (link, bookmark etc)	*/
/*  and adjust the selection to changed particule numbers.		*/
/*									*/
/*  0)  First clean the field.						*/
/*									*/
/*  1)  Determine the rectangle that is to be redrawn.			*/
/*  2)  Delete the particules from the paragraph.			*/
/*  3)  Adjust the selection.						*/
/*  4)  Redraw.								*/
/*									*/
/************************************************************************/

static int tedFlattenField(	EditDocument *			ed,
				const DocumentSelection *	dsAroundField,
				int				headPart,
				int				tailPart,
				DocumentField *			df,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				int				traced )
    {
    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    DocumentSelection		dsExInside;

    const int			fullWidth= 0;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}

    if  ( tedEditStartTypedStep( &teo, EDITcmdDEL_FIELD, df->dfKind ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( taSetMask )
	{
	DocumentSelection	dsTraced;

	eo->eoHeadDp= dsAroundField->dsHead;
	eo->eoTailDp= dsAroundField->dsTail;
	eo->eoLastDp= eo->eoTailDp;

	if  ( teo.teoEditTrace )
	    {
	    if  ( docRtfTraceNewProperties( eo,
		    taSetMask, taSet,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    docRtfTraceFieldKind( eo, df->dfKind );

	    if  ( docRtfTraceOldContents( &dsTraced, eo, DOClevSPAN, 0 ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
				eo->eoDocument, eo->eoHeadDp.dpNode,
				headPart+ 1, tailPart, taSet, taSetMask ) )
	    { LDEB(1);	}
	}
    else{
	if  ( docRtfTraceOldField( eo, df ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedFlattenFieldImpl( &teo, &dsExInside,
				    dsAroundField, headPart, tailPart, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3,4  */
#   if 0
    if  ( tedEditFinishSelection( &teo, &dsExInside ) )
	{ LDEB(1);	}
#   else
    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}
#   endif

    docSetEditRange( &(eo->eoAffectedRange), &dsExInside );

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
/*									*/
/*  Replace the current selection of the document with a field.		*/
/*									*/
/*  2)  Replace the current selection with something arbitrary.		*/
/*  3)  Make the footnote number field.					*/
/*  4)  Finish by surrounding the replacement with the field.		*/
/*  5)  Final bookkeeping and redrawing.				*/
/*									*/
/************************************************************************/

int tedDocReplaceSelectionWithField(
				TedEditOperation *		teo,
				DocumentField **		pDf,
				int *				pHeadPart,
				int *				pTailPart,
				DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				const FieldInstructions *	fi,
				int				fieldKind,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    EditOperation *		eo= &(teo->teoEo);
    EditDocument *		ed= teo->teoEditDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentField *		df;

    /*  2  */
    if  ( tedEditReplaceSelection( teo, (const char *)"?", 1,
			td->tdSelectionDescription.sdTextAttributeNumber ) )
	{ LDEB(1); return -1;	}

    eo->eoHeadDp.dpStroff--; /* FRAUD: Before the ? */

    /*  3,4  */
    if  ( docEditSurroundTextSelectionByField( eo, &df,
				dsInside, dsAround, pHeadPart, pTailPart,
				taSetMask, taSet,
				fieldKind, fi ) )
	{ LDEB(1); return -1;	}

    df->dfKind= fieldKind;
    *pDf= df;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a page number in the document.				*/
/*									*/
/*  1)  Force the header or footer that will receive the page number	*/
/*	to be reformatted the next time that it is drawn.		*/
/*  2)  Replace the selection with a PAGE field. As the replecement is	*/
/*	a page number that is recalculated every time the		*/
/*	header/footer is drawn, no field calculation is required.	*/
/*									*/
/************************************************************************/

int tedInsertPageNumber(	EditDocument *	ed,
				int		traced )
    {
    int				rval= 0;

    DocumentSelection		dsInside;
    DocumentSelection		dsAround;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    SelectionGeometry		sg;
    SelectionDescription	sd;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    int				headPart;
    int				tailPart;

    DocumentField *		df;

    DocumentSelection		dsTraced;

    FieldInstructions		fi;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fi );

    if  ( docStartFieldInstructions( &fi, "PAGE", 4 ) )
	{ LDEB(4); rval= -1; goto ready;	}

    utilInitTextAttribute( &taSet );
    utilPropMaskClear( &taSetMask );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartReplace( &dsTraced, &teo,
					    EDITcmdREPLACE, DOClevSPAN, 0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  1  */
    docInvalidateTreeLayout( eo->eoTree );

    /*  2  */
    if  ( tedDocReplaceSelectionWithField( &teo, &df,
					&headPart, &tailPart,
					&dsInside, &dsAround,
					&fi, DOCfkPAGE,
					&taSetMask, &taSet ) )
	{ LDEB(4); rval= -1; goto ready;	}

    /*  5  */
    if  ( tedLayoutNodeOfField( &teo, &dsAround, FIELDdoPAGE_NUMBER ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishSelection( &teo, &dsAround );

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposALL ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:

    docCleanFieldInstructions( &fi );
    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Remove the bookmark that holds the head of the selection.		*/
/*									*/
/*  The contents of the bookmark are retained: Only the field is	*/
/*  removed.								*/
/*									*/
/************************************************************************/

int tedDocFlattenTypedField(	EditDocument *			ed,
				int				fieldType,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				int				traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentField *		df;
    DocumentSelection		dsInsideField;
    DocumentSelection		dsAroundField;
    int				headPart;
    int				tailPart;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed );

    df= docFindTypedFieldForPosition( bd, &(ds.dsHead), fieldType, 0 );
    if  ( ! df )
	{ XDEB(df); return -1;	}
    if  ( docDelimitFieldInDoc( &dsInsideField, &dsAroundField,
					    &headPart, &tailPart, bd, df ) )
	{ LDEB(1); return -1; }

    tedFlattenField( ed, &dsAroundField, headPart, tailPart, df,
						    taSetMask, taSet, traced );

    return 0;
    }

