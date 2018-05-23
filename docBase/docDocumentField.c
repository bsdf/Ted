/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docDocumentField.h"

/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

void docCleanDocumentField(	DocumentField *		df )
    {
    utilCleanMemoryBuffer( &(df->dfData) );
    docCleanFieldInstructions( &(df->dfInstructions) );
    docCleanChildFields( &(df->dfChildFields) );

    return;
    }

/************************************************************************/
/*									*/
/*  Free a field: Only called when the field list is cleaned as a	*/
/*  whole.								*/
/*									*/
/************************************************************************/

void docInitChildFields(	ChildFields *	cf )
    {
    cf->cfChildren= (DocumentField **)0;
    cf->cfChildCount= 0;
    }

void docCleanChildFields(	ChildFields *	cf )
    {
    if  ( cf->cfChildren )
	{ free( cf->cfChildren );	}
    }

void docInitDocumentField(	DocumentField *	df )
    {
    df->dfKind= DOCfkUNKNOWN;

    utilInitMemoryBuffer( &df->dfData );
    docInitFieldInstructions( &(df->dfInstructions) );

    docInitSelectionScope( &(df->dfSelectionScope) );
    docInitEditPosition( &(df->dfHeadPosition) );
    docInitEditPosition( &(df->dfTailPosition) );

    df->dfFirstColumn= -1;
    df->dfLastColumn= -1;

    df->dfDirty= 0;
    df->dfEdited= 0;
    df->dfLocked= 0;
    df->dfPrivate= 0;

    docInitChildFields( &(df->dfChildFields) );
    df->dfParent= (DocumentField *)0;
    df->dfNumberInParent= 0;

    df->dfFieldNumber= -1;
    df->dfNoteIndex= -1;

    df->dfPage= -1;
    }

/************************************************************************/
/*									*/
/*  Copy the properties of a field.					*/
/*									*/
/*  Only properties that have an application meaning are copied. The	*/
/*  set is determined by what can appaer in RTF.			*/
/*									*/
/************************************************************************/

int docCopyFieldProperties(	DocumentField *		dfTo,
				const DocumentField *	dfFrom )
    {
    if  ( docCopyFieldInstructions( &(dfTo->dfInstructions),
						&(dfFrom->dfInstructions) ) )
	{ LDEB(1); return -1;	}

    if  ( utilCopyMemoryBuffer( &(dfTo->dfData), &(dfFrom->dfData) ) )
	{ LDEB(1); return -1;	}

    dfTo->dfKind= dfFrom->dfKind;

    dfTo->dfFirstColumn= dfFrom->dfFirstColumn;
    dfTo->dfLastColumn= dfFrom->dfLastColumn;

    dfTo->dfDirty= dfFrom->dfDirty;
    dfTo->dfEdited= dfFrom->dfEdited;
    dfTo->dfLocked= dfFrom->dfLocked;
    dfTo->dfPrivate= dfFrom->dfPrivate;

    return 0;
    }

int docSetFieldInst(	DocumentField *		df,
			const char *		bytes,
			int			size )
    {
    return docSetFieldInstructions( &(df->dfInstructions), bytes, size );
    }

int docAddToFieldData(	DocumentField *		df,
			const char *		bytes,
			int			size )
    {
    return utilMemoryBufferAppendBytes( &(df->dfData),
					(const unsigned char *)bytes, size );
    }

int docSetFieldProperty(		DocumentField *		df,
					int			prop,
					int			val )
    {
    switch( prop )
	{
	case FPpropFIRST_COLUMN:
	    df->dfFirstColumn= val;
	    return 0;

	case FPpropLAST_COLUMN:
	    df->dfLastColumn= val;
	    return 0;

	case FPpropDIRTY:
	    df->dfDirty= ( val != 0 );
	    return 0;

	case FPpropEDITED:
	    df->dfEdited= ( val != 0 );
	    return 0;

	case FPpropLOCKED:
	    df->dfLocked= ( val != 0 );
	    return 0;

	case FPpropPRIVATE:
	    df->dfPrivate= ( val != 0 );
	    return 0;

	default:
	    LDEB(prop);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Navigation over the field tree in document order.			*/
/*									*/
/************************************************************************/

DocumentField * docGetFirstField(	const ChildFields *	rootFields )
    { return docGetNextField( rootFields, (DocumentField *)0 );	}

DocumentField * docGetLastField(	const ChildFields *	rootFields )
    { return docGetPrevField( rootFields, (DocumentField *)0 );	}

DocumentField * docGetNextField(	const ChildFields *	rootFields,
					DocumentField *		df )
    {
    const ChildFields *	parentFields;

    if  ( ! df )
	{
	if  ( rootFields->cfChildCount == 0 )
	    { return (DocumentField *)0;	}
	else{ return rootFields->cfChildren[0];	}
	}

    if  ( df->dfChildFields.cfChildCount > 0 )
	{ return df->dfChildFields.cfChildren[0];	}

    while( df->dfParent )
	{
	parentFields= &(df->dfParent->dfChildFields);

	if  ( df->dfNumberInParent < parentFields->cfChildCount- 1 )
	    { return parentFields->cfChildren[df->dfNumberInParent+ 1];	}

	df= df->dfParent;
	}

    parentFields= rootFields;
    if  ( df->dfNumberInParent >= parentFields->cfChildCount- 1 )
	{ return (DocumentField *)0;					}
    else{ return parentFields->cfChildren[df->dfNumberInParent+ 1];	}
    }

DocumentField * docGetPrevField(	const ChildFields *	rootFields,
					DocumentField *		df )
    {
    const ChildFields *	parentFields;

    if  ( ! df )
	{
	if  ( rootFields->cfChildCount == 0 )
	    { return (DocumentField *)0;				}
	else{ df= rootFields->cfChildren[rootFields->cfChildCount- 1];	}
	}
    else{
	if  ( df->dfNumberInParent == 0 )
	    { return df->dfParent;	}

	if  ( df->dfParent )
	    { parentFields= &(df->dfParent->dfChildFields);		}
	else{ parentFields= rootFields;				}

	df= parentFields->cfChildren[df->dfNumberInParent- 1];
	}

    while( df->dfChildFields.cfChildCount > 0 )
	{ df= df->dfChildFields.cfChildren[df->dfChildFields.cfChildCount- 1]; }

    return df;
    }

/************************************************************************/
/*									*/
/*  Return the first field in a section.				*/
/*									*/
/*  NOTE: That fields do not span sections.				*/
/*									*/
/************************************************************************/

DocumentField * docGetFirstFieldOfSection(
					const ChildFields *	rootFields,
					int			sect )
    {
    int		pos;

    for ( pos= 0; pos < rootFields->cfChildCount; pos++ )
	{
	DocumentField *	df= rootFields->cfChildren[pos];

	if  ( df->dfSelectionScope.ssSectNr == sect )
	    { return df;	}
	if  ( df->dfSelectionScope.ssSectNr >  sect )
	    { break;	}
	}

    return (DocumentField *)0;
    }

DocumentField * docGetNextFieldInSection(
					const ChildFields *	rootFields,
					int			sect,
					DocumentField *		df )
    {
    df= docGetNextField( rootFields, df );

    if  ( ! df )
	{ return df;	}

    if  ( df && df->dfSelectionScope.ssSectNr == sect )
	{ return df;	}

    return (DocumentField *)0;
    }
