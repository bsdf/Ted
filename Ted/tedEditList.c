/************************************************************************/
/*									*/
/*  List related editing and formatting changes.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedDocFront.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docEditImpl.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the properties of the list where the selection starts.	*/
/*									*/
/************************************************************************/

int tedDocChangeCurrentList(	EditDocument *		ed,
				const DocumentList *	dlNew,
				int			traced )
    {
    int					rval= 0;

    struct ListOverride *		lo;
    DocumentList *			dl;

    SelectionGeometry			sg;
    SelectionDescription		sd;

    TedEditOperation			teo;
    EditOperation *			eo= &(teo.teoEo);

    const int				fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( ! sd.sdHasLists		||
	  sd.sdListOverride < 1	)
	{ LLDEB(sd.sdHasLists,sd.sdListOverride); return -1;	}

    if  ( docGetListOfParagraph( &lo, &dl, sd.sdListOverride, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    if  ( tedEditStartStep( &teo, EDITcmdUPD_LIST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceOldList( eo, dl ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docRtfTraceNewList( eo, dlNew ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEditChangeList( eo, dl, lo, dlNew ) )
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
/*									*/
/*  Ted: The 'Set' button on the list font tool was pushed.		*/
/*									*/
/************************************************************************/

int tedDocListFontToolSet(	EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet )
    {
    int				rval= 0;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= dp->dpFontList;

    DocumentList		dlNew;

    ListLevel *			ll;

    TextAttribute		taSet;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentList( &dlNew );

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! sd.sdHasLists	||
	  sd.sdListOverride < 1	||
	  sd.sdMultiList	||
	  sd.sdMultiLevel	)
	{ LLDEB(sd.sdHasLists,sd.sdListOverride); goto ready; }

    {
    struct ListOverride *	lo;
    DocumentList *		dlOld= (DocumentList *)0;

    if  ( docGetListOfParagraph( &lo, &dlOld, sd.sdListOverride, bd ) )
	{ LDEB(1); goto ready;	}

    if  ( docCopyDocumentListSameDocument( &dlNew, dlOld ) )
	{ LDEB(1); rval= -1; goto ready;	}
    }

    utilInitTextAttribute( &taSet );

    docIndirectTextAttribute( (PropertyMask *)0, &taSet, etaSet, taSetMask,
						dfl, dp->dpColorPalette );

    ll= &(dlNew.dlLevels[sd.sdListLevel]);
    docListLevelApplyTextAttribute( ll, taSetMask, &taSet );

    if  ( tedDocChangeCurrentList( ed, &dlNew, td->tdTraced ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentList( &dlNew );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a new list and set it as the list of the current selection.	*/
/*									*/
/************************************************************************/

int tedDocSetNewList(		EditDocument *		ed,
				int			traced )
    {
    int				rval= 0;
    PropertyMask		ppSetMask;
    PropertyMask		ppOldMask;
    ParagraphProperties		ppSet;
    int				ls;
    int				lsExample= -1;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentList *	dl= (const DocumentList *)0;

    PropertyMask		taSetMask;
    TextAttribute		taNew;

    SelectionGeometry		sg;
    SelectionDescription	sd;
    DocumentSelection		ds;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    const int			fullWidth= 1;

    DocumentSelection		dsTraced;

    utilPropMaskClear( &taSetMask );
    utilInitTextAttribute( &taNew );

    utilPropMaskClear( &ppSetMask );
    utilPropMaskClear( &ppOldMask );
    docInitParagraphProperties( &ppSet );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    ls= docNewList( &dl, (const struct ListOverride **)0, lsExample, bd,
							&taSetMask, &taNew );
    if  ( ls < 0 )
	{ LDEB(ls); rval= -1; goto ready;	}

    ppSet.ppListOverride= ls;

    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );
    ppOldMask= ppSetMask;
    PROPmaskADD( &ppOldMask, PPpropFIRST_INDENT );
    PROPmaskADD( &ppOldMask, PPpropLEFT_INDENT );

    if  ( tedEditStartStep( &teo, EDITcmdSET_NEW_LIST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceOldProperties( &dsTraced, eo, DOClevPARA,
		    (const PropertyMask *)0, &ppOldMask,
		    (const PropertyMask *)0, (const PropertyMask *)0,
		    (const PropertyMask *)0, (const PropertyMask *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( sd.sdHasLists		&&
	  ! sd.sdMultiList	&&
	  sd.sdListOverride > 0	)
	{ lsExample= sd.sdListOverride;	}

    utilPropMaskFill( &taSetMask, TAprop_COUNT );
    taNew= td->tdSelectionDescription.sdTextAttribute;

    PROPmaskUNSET( &taSetMask, TApropSUPERSUB );
    PROPmaskUNSET( &taSetMask, TApropSMALLCAPS );
    PROPmaskUNSET( &taSetMask, TApropCAPITALS );
    PROPmaskUNSET( &taSetMask, TApropSTRIKETHROUGH );
    PROPmaskUNSET( &taSetMask, TApropTEXT_COLOR );
    PROPmaskUNSET( &taSetMask, TApropTEXT_STYLE );

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewProperties( eo,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    &ppOldMask, &ppSet,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedEditChangeSelectionPropertiesImpl( &teo, &ds,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    &ppSetMask, &ppSet,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
	{ LDEB(ls); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    docCleanParagraphProperties( &ppSet );
    tedCleanEditOperation( &teo );

    return rval;
    }

