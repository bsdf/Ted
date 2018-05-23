/************************************************************************/
/*									*/
/*  Ted: Changing font and text attributes.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"
#   include	"tedSelect.h"
#   include	<docExpandedTextAttribute.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position or selection.		*/
/*									*/
/************************************************************************/

static int tedGetDocumentAttributes(	EditDocument *		ed,
					PropertyMask *		pUpdMask,
					TextAttribute *		pTaNew )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! tedHasSelection( ed ) )
	{ return 1;	}

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return 1;	}

    *pUpdMask= sd.sdTextAttributeMask;
    *pTaNew= sd.sdTextAttribute;

    return 0;
    }

/************************************************************************/
/*									*/
/*  1)  Adapt fonts tool &c to the current values.			*/
/*  2)  Adapt fonts tool &c to the current selection.			*/
/*									*/
/************************************************************************/

/*  1  */
static void tedAdaptFontIndicatorsToValues(
					EditDocument *		ed,
					const PropertyMask *	taSetMask,
					TextAttribute *		taSet )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    appGuiSetToggleItemState( td->tdFontBoldOption,
			    PROPmaskISSET( taSetMask, TApropFONTBOLD ) &&
			    taSet->taFontIsBold );

    appGuiSetToggleItemState( td->tdFontItalicOption,
			    PROPmaskISSET( taSetMask, TApropFONTSLANTED ) &&
			    taSet->taFontIsSlanted );

    appGuiSetToggleItemState( td->tdFontUnderlinedOption,
			    PROPmaskISSET( taSetMask, TApropTEXTUNDERLINED ) &&
			    taSet->taTextIsUnderlined );

    appGuiSetToggleItemState( td->tdFontSuperscriptOption,
			    PROPmaskISSET( taSetMask, TApropSUPERSUB ) &&
			    taSet->taSuperSub == TEXTvaSUPERSCRIPT );

    appGuiSetToggleItemState( td->tdFontSubscriptOption,
			    PROPmaskISSET( taSetMask, TApropSUPERSUB ) &&
			    taSet->taSuperSub == TEXTvaSUBSCRIPT );

    return;
    }

/*  2  */
void tedAdaptFontIndicatorsToSelection(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    PropertyMask		taCurrMask;
    TextAttribute		taCurr;

    utilPropMaskClear( &taCurrMask );
    utilInitTextAttribute( &taCurr );

    if  ( tedGetDocumentAttributes( ed, &taCurrMask, &taCurr ) )
	{ goto ready;	}

    td->tdInProgrammaticChange++;

    tedAdaptFontIndicatorsToValues( ed, &taCurrMask, &taCurr );

    td->tdInProgrammaticChange--;

  ready:

    return;
    }


/************************************************************************/
/*									*/
/*  One of the text attribute options has been pushed.			*/
/*									*/
/************************************************************************/

void tedToggleTextAttribute(	EditDocument *	ed,
				int		set,
				int		prop )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    TextAttribute	taSet;
    PropertyMask	taSetMask;

    utilPropMaskClear( &taSetMask );
    utilInitTextAttribute( &taSet );

    if  ( td->tdInProgrammaticChange )
	{ return;	}

    switch( prop )
	{
	case TApropFONTBOLD:
	    taSet.taFontIsBold= ( set != 0 );
	    PROPmaskADD( &taSetMask, prop );
	    break;

	case TApropFONTSLANTED:
	    taSet.taFontIsSlanted= ( set != 0 );
	    PROPmaskADD( &taSetMask, prop );
	    break;

	case TApropTEXTUNDERLINED:
	    taSet.taTextIsUnderlined= ( set != 0 );
	    PROPmaskADD( &taSetMask, prop );
	    break;

	default:
	    LDEB(prop);
	    return;
	}

    if  ( ! utilPropMaskIsEmpty( &taSetMask ) )
	{
	tedDocChangeTextAttribute( ed, &taSetMask, &taSet, td->tdTraced );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change the base line of the current selection.			*/
/*									*/
/************************************************************************/

void tedSetFontSupersub(		EditDocument *		ed,
					int			supersub )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    PropertyMask	taSetMask;
    TextAttribute	taSet;

    if  ( td->tdInProgrammaticChange )
	{ return;	}

    utilPropMaskClear( &taSetMask );
    PROPmaskADD( &taSetMask, TApropSUPERSUB );

    utilInitTextAttribute( &taSet );
    taSet.taSuperSub= supersub;

    tedDocChangeTextAttribute( ed, &taSetMask, &taSet, td->tdTraced );

    return;
    }

