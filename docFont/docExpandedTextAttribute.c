/************************************************************************/
/*									*/
/*  Manage expanded text attributes.					*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<utilPropMask.h>
#   include	<textAttribute.h>
#   include	<utilPalette.h>

#   include	"docExpandedTextAttribute.h"

/************************************************************************/
/*									*/
/*  Set text attributes to a default value.				*/
/*									*/
/************************************************************************/

void docInitExpandedTextAttribute(	ExpandedTextAttribute *	eta )
    {
    utilInitTextAttribute( &(eta->etaTextAttribute) );

    utilInitRGB8Color( &(eta->etaTextColor) );
    eta->etaTextColorExplicit= 0;

    eta->etaFontName= (char *)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Cleanup of an Expanded Text attribute.				*/
/*									*/
/************************************************************************/

void docCleanExpandedTextAttribute(	ExpandedTextAttribute *	eta )
    {
    if  ( eta->etaFontName )
	{ free( eta->etaFontName );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set the font name of the attribute.					*/
/*									*/
/************************************************************************/

int docExpandedTextAttributeSetFontName(
				ExpandedTextAttribute *		etaTo,
				int *				pChanged,
				const char *			fontName )
    {
    int		changed= 0;

    if  ( etaTo->etaFontName )
	{
	if  ( ! fontName				||
	      strcmp( etaTo->etaFontName, fontName )	)
	    {
	    free( etaTo->etaFontName );
	    etaTo->etaFontName= (char *)0;
	    changed= 1;
	    }
	}

    if  ( fontName )
	{
	if  ( ! etaTo->etaFontName )
	    {
	    etaTo->etaFontName= strdup( fontName );
	    if  ( ! etaTo->etaFontName )
		{ XDEB(etaTo->etaFontName); return -1;	}

	    changed= 1;
	    }
	}

    *pChanged= changed;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy an expanded text attribute.					*/
/*									*/
/************************************************************************/

int docCopyExpandedTextAttribute(
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom )
    {
    int		changed= 0;

    /* Font Name */
    if  ( docExpandedTextAttributeSetFontName( etaTo, &changed,
						    etaFrom->etaFontName ) )
	{ SDEB(etaFrom->etaFontName); return -1;	}

    /* Color */
    if  ( etaTo->etaTextColorExplicit )
	{
	if  ( ! etaFrom->etaTextColorExplicit )
	    {
	    etaTo->etaTextColorExplicit= 0;
	    }
	else{
	    if  ( bmRGB8ColorsDiffer( &(etaTo->etaTextColor),
					    &(etaFrom->etaTextColor) ) )
		{
		etaTo->etaTextColor= etaFrom->etaTextColor;
		}
	    }
	}
    else{
	if  ( etaFrom->etaTextColorExplicit )
	    {
	    etaTo->etaTextColorExplicit= 1;
	    etaTo->etaTextColor= etaFrom->etaTextColor;
	    }
	}

    /* Rest */
    etaTo->etaTextAttribute= etaFrom->etaTextAttribute;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set text attribute and remember difference mask.			*/
/*									*/
/************************************************************************/

static void utilUpdateTextAttributeX(	PropertyMask *		pDoneMask,
					TextAttribute *		taTo,
					const TextAttribute *	taFrom,
					const PropertyMask *	setMask )
    {
    PropertyMask	doneMask= *pDoneMask;
    int			prop;

    for ( prop= 0; prop < TAprop_COUNT; prop++ )
	{
	if  ( prop == TApropTEXT_COLOR )
	    { continue;	}
	if  ( prop == TApropFONT_NUMBER )
	    { continue;	}

	if  ( ! setMask || PROPmaskISSET( setMask, prop ) )
	    {
	    int	to= utilGetTextProperty( taTo, prop );
	    int	from= utilGetTextProperty( taFrom, prop );

	    if  ( to != from )
		{
		utilSetTextProperty( taTo, prop, from );
		PROPmaskADD( &doneMask, prop );
		}
	    }
	}

    *pDoneMask= doneMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Transfer the properties of one expanded text attribute to another.	*/
/*									*/
/************************************************************************/

int docUpdateExpandedTextAttribute(
				PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask )
    {
    PropertyMask		doneMask= *pDoneMask;
    TextAttribute *		taTo= &(etaTo->etaTextAttribute);
    const TextAttribute *	taFrom= &(etaFrom->etaTextAttribute);

    if  ( PROPmaskISSET( setMask, TApropFONT_NUMBER ) )
	{
	int		changed= 0;

	if  ( docExpandedTextAttributeSetFontName( etaTo, &changed,
						    etaFrom->etaFontName ) )
	    { SDEB(etaFrom->etaFontName); return -1;	}

	if  ( changed )
	    { PROPmaskADD( &doneMask, TApropFONT_NUMBER );	}

	/* Should be superfluous */
	etaTo->etaTextAttribute.taFontNumber= 
				    etaFrom->etaTextAttribute.taFontNumber;
	}

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR ) )
	{
	if  ( etaTo->etaTextColorExplicit )
	    {
	    if  ( ! etaFrom->etaTextColorExplicit )
		{
		etaTo->etaTextColorExplicit= 0;
		PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		}
	    else{
		if  ( bmRGB8ColorsDiffer( &(etaTo->etaTextColor),
						&(etaFrom->etaTextColor) ) )
		    {
		    etaTo->etaTextColor= etaFrom->etaTextColor;
		    PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		    }
		}
	    }
	else{
	    if  ( etaFrom->etaTextColorExplicit )
		{
		etaTo->etaTextColorExplicit= 1;
		etaTo->etaTextColor= etaFrom->etaTextColor;
		PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		}
	    }
	}

    utilUpdateTextAttributeX( &doneMask, taTo, taFrom, setMask );

    *pDoneMask= doneMask;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Conversion back and forth between Text attributes in indirect and	*/
/*  expanded form.							*/
/*									*/
/************************************************************************/

int docExpandTextAttribute(	PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const TextAttribute *		taFrom,
				const PropertyMask *		setMask,
				const DocumentFontList *	dfl,
				const ColorPalette *		cp )
    {
    PropertyMask		doneMask= *pDoneMask;
    TextAttribute *		taTo= &(etaTo->etaTextAttribute);

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR ) )
	{
	if  ( taFrom->taTextColorNumber == 0 )
	    {
	    if  ( etaTo->etaTextColorExplicit != 0 )
		{
		etaTo->etaTextColorExplicit= 0;
		utilInitRGB8Color( &(etaTo->etaTextColor) );
		PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		}
	    }
	else{
	    if  ( taFrom->taTextColorNumber < 0			||
		  taFrom->taTextColorNumber >= cp->cpColorCount	)
		{
		LLDEB(taFrom->taTextColorNumber,cp->cpColorCount);

		etaTo->etaTextColorExplicit= 0;
		utilInitRGB8Color( &(etaTo->etaTextColor) );
		}
	    else{
		if  ( ! etaTo->etaTextColorExplicit			||
		      bmRGB8ColorsDiffer( &(etaTo->etaTextColor),
			&(cp->cpColors[taFrom->taTextColorNumber]) )	)
		    {
		    etaTo->etaTextColorExplicit= 1;
		    etaTo->etaTextColor=
				    cp->cpColors[taFrom->taTextColorNumber];
		    PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		    }
		}
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONT_NUMBER ) )
	{
	const DocumentFont *	df;

	df= docFontListGetFontByNumber( dfl, taFrom->taFontNumber );
	if  ( ! df )
	    { LXDEB(taFrom->taFontNumber,df);	}
	else{
	    int		changed= 0;

	    if  ( docExpandedTextAttributeSetFontName( etaTo, &changed,
						    df->dfName ) )
		{ SDEB(df->dfName); return -1;	}

	    if  ( changed )
		{ PROPmaskADD( &doneMask, TApropFONT_NUMBER );	}
	    }

	/* Should be superfluous */
	etaTo->etaTextAttribute.taFontNumber= taFrom->taFontNumber;
	}

    utilUpdateTextAttributeX( &doneMask, taTo, taFrom, setMask );

    *pDoneMask= doneMask;
    return 0;
    }

void docIndirectTextAttribute(	PropertyMask *			pDoneMask,
				TextAttribute *			taTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask,
				DocumentFontList *		dfl,
				ColorPalette *			cp )
    {
    PropertyMask		doneMask;
    const TextAttribute *	taFrom= &(etaFrom->etaTextAttribute);

    utilPropMaskClear( &doneMask );

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR ) )
	{
	const int		avoidZero= 1;
	const int		maxColors= 256;
	int			color;

	if  ( etaFrom->etaTextColorExplicit )
	    {
	    color= utilPaletteInsertColor( cp,
			    avoidZero, maxColors, &(etaFrom->etaTextColor) );
	    if  ( color < 0 )
		{ LDEB(color); color= 0;	}
	    }
	else{ color= 0;		}

	if  ( taTo->taTextColorNumber != color )
	    {
	    taTo->taTextColorNumber= color;
	    PROPmaskADD( &doneMask, TApropTEXT_COLOR );
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONT_NUMBER ) )
	{
	if  ( ! etaFrom->etaFontName )
	    {
	    XDEB(etaFrom->etaFontName);
	    /* Should be superfluous */
	    taTo->taFontNumber= etaFrom->etaTextAttribute.taFontNumber;
	    }
	else{
	    int	fontNumber= docGetFontByName( dfl, etaFrom->etaFontName );

	    if  ( fontNumber < 0 )
		{
		SLDEB(etaFrom->etaFontName,fontNumber);
		/* Should be superfluous */
		taTo->taFontNumber= etaFrom->etaTextAttribute.taFontNumber;
		}
	    else{
		if  ( taTo->taFontNumber != fontNumber )
		    {
		    taTo->taFontNumber= fontNumber;
		    PROPmaskADD( &doneMask, TApropFONT_NUMBER );
		    }
		}
	    }
	}

    utilUpdateTextAttributeX( &doneMask, taTo, taFrom, setMask );

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &doneMask );	}

    return;
    }

