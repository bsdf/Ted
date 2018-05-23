/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docStyle.h"
#   include	"docListDepth.h"

/************************************************************************/
/*									*/
/*  Style administration.						*/
/*									*/
/************************************************************************/

void docInitDocumentStyle(	DocumentStyle *	ds )
    {
    ds->dsStyleNumber= -1;
    ds->dsBasedOn= -1;
    ds->dsNext= -1;
    ds->dsLink= -1;
    ds->dsLevel= -1;

    ds->dsAdditive= 0;
    ds->dsAutoupd= 0;
    ds->dsHidden= 0;
    ds->dsLocked= 0;
    ds->dsSemiHidden= 0;
    ds->dsPersonal= 0;
    ds->dsBusy= 0;

    ds->dsOutlineLevel= PPoutlineBODYTEXT;

    docInitSectionProperties( &(ds->dsSectProps) );
    docInitRowProperties( &(ds->dsRowProps) );
    docInitCellProperties( &(ds->dsCellProps) );
    docInitParagraphProperties( &(ds->dsParaProps) );
    utilInitTextAttribute( &(ds->dsTextAttribute) );

    utilPropMaskClear( &(ds->dsSectMask ) );
    utilPropMaskClear( &(ds->dsRowMask ) );
    utilPropMaskClear( &(ds->dsCellMask ) );
    utilPropMaskClear( &(ds->dsParaMask ) );
    utilPropMaskClear( &(ds->dsTextMask ) );

    ds->dsName= (char *)0;

    return;
    }

void docCleanDocumentStyle(	DocumentStyle *	ds )
    {
    docCleanParagraphProperties( &(ds->dsParaProps) );
    docCleanCellProperties( &(ds->dsCellProps) );
    docCleanRowProperties( &(ds->dsRowProps) );
    docCleanSectionProperties( &(ds->dsSectProps) );

    if  ( ds->dsName )
	{ free( ds->dsName );	}

    return;
    }

int docCopyStyle(	DocumentStyle *			to,
			const DocumentStyle *		from,
			const DocumentAttributeMap *	dam )
    {
    int			rval= 0;
    DocumentStyle	ds;

    docInitDocumentStyle( &ds );
    ds= *from;

    docInitParagraphProperties( &(ds.dsParaProps) );
    docInitCellProperties( &(ds.dsCellProps) );
    docInitRowProperties( &(ds.dsRowProps) );
    docInitSectionProperties( &(ds.dsSectProps) );
    ds.dsName= (char *)0;

    if  ( docUpdParaProperties( (PropertyMask *)0, &(ds.dsParaProps),
		    &(from->dsParaMask), &(from->dsParaProps), dam ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docUpdCellProperties( (PropertyMask *)0, &(ds.dsCellProps),
			&(from->dsCellMask), &(from->dsCellProps), dam ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docUpdRowProperties( (PropertyMask *)0, &(ds.dsRowProps),
			&(from->dsRowMask), &(from->dsRowProps), dam ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docCopySectionProperties( &(ds.dsSectProps),
					&(from->dsSectProps ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( from->dsName )
	{
	ds.dsName= strdup( from->dsName );
	if  ( ! ds.dsName )
	    { XDEB(ds.dsName); LDEB(1); rval= -1; goto ready;	}
	}

    docCleanDocumentStyle( to ); *to= ds; /* steal */
    docInitDocumentStyle( &ds );

  ready:
    docCleanDocumentStyle( &ds );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set a style property.						*/
/*									*/
/************************************************************************/

int docSetStyleProperty(		DocumentStyle *		ds,
					int			prop,
					int			arg )
    {
    switch( prop )
	{
	case DSpropSTYLE_NUMBER:
	    ds->dsStyleNumber= arg;
	    break;

	case DSpropADDITIVE:
	    ds->dsAdditive= arg != 0;
	    break;
	case DSpropBASED_ON:
	    ds->dsBasedOn= arg;
	    break;
	case DSpropNEXT:
	    ds->dsNext= arg;
	    break;
	case DSpropLINK:
	    ds->dsLink= arg;
	    break;
	case DSpropAUTO_UPD:
	    ds->dsAutoupd= arg != 0;
	    break;
	case DSpropHIDDEN:
	    ds->dsHidden= arg != 0;
	    break;
	case DSpropLOCKED:
	    ds->dsLocked= arg != 0;
	    break;
	case DSpropSEMI_HIDDEN:
	    ds->dsSemiHidden= arg != 0;
	    break;
	case DSpropPERSONAL:
	    ds->dsPersonal= arg != 0;
	    break;

	case DSpropIGNORED:
	    break;

	default:
	    LDEB(prop);
	    return -1;
	}

    return 0;
    }
