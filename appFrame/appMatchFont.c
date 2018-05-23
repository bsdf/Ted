/************************************************************************/
/*									*/
/*  Basic font matching.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	"appMatchFont.h"
#   include	<psMatchFont.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find a PostScript font.						*/
/*									*/
/************************************************************************/

AfmFontInfo * appGetFontInfo(	const char *			familyName,
				int				styleInt,
				const IndexSet *		unicodesUsed,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl )
    {
    AfmFontInfo *	afi= (AfmFontInfo *)0;

#   ifdef USE_FONTCONFIG
    if  ( psfl->psflAvoidFontconfig )
	{
	afi= psGetPsFontInfoForAttribute( familyName, styleInt,
						    unicodesUsed, ta, psfl );
	if  ( ! afi )
	    { SXDEB(familyName,afi);	}
	}
    else{
	afi= appFcGetFontInfoForAttribute( familyName, styleInt,
						    unicodesUsed, ta, psfl );
	if  ( ! afi )
	    { SXDEB(familyName,afi);	}
	}
#   else
    afi= psGetPsFontInfoForAttribute( familyName, styleInt,
						    unicodesUsed, ta, psfl );
    if  ( ! afi )
	{ SXDEB(familyName,afi);	}
#   endif

    return afi;
    }

/************************************************************************/
/*									*/
/*  Find a PostScript font for a document font.				*/
/*									*/
/************************************************************************/

const AfmFontInfo * appGetFontInfoForAttribute(
				const IndexSet **		pUnicodesWanted,
				const TextAttribute *		ta,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl )
    {
    DocumentFont *		df;
    AfmFontInfo *		afi= (AfmFontInfo *)0;
    int				faceIndex;

    df= docFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{ LXDEB(ta->taFontNumber,df); return (AfmFontInfo *)0;	}

    faceIndex= FACE_INDEX(ta->taFontIsSlanted,ta->taFontIsBold);

    afi= df->dfPsFontInfo[faceIndex];
    if  ( ! afi )
	{
	afi= appGetFontInfo( df->dfName, df->dfStyleInt,
					    &(df->dfUnicodesUsed), ta, psfl );
	if  ( ! afi )
	    { XDEB(afi); return (AfmFontInfo *)0;	}

	df->dfPsFontInfo[faceIndex]= afi;
	}

    if  ( afi->afiMetricsDeferred		&&
	  appGetDeferredFontMetrics( afi )	)
	{
	SLDEB(afi->afiFullName,afi->afiMetricsDeferred);
	return (AfmFontInfo *)0;
	}

    if  ( pUnicodesWanted )
	{ *pUnicodesWanted= &(df->dfUnicodesUsed);	}

    return afi;
    }

int appGetDeferredFontMetricsForList(	PostScriptFontList *	psfl )
    {
    int		rval= 0;
    int		i;

    for ( i= 0; i < psfl->psflInfoCount; i++ )
	{
	if  ( appGetDeferredFontMetrics( psfl->psflInfos[i] ) )
	    { LDEB(i); rval= -1;	}
	}

    return rval;
    }

int appGetDeferredFontMetrics(		AfmFontInfo *		afi )
    {
#   ifdef USE_FONTCONFIG
    if  ( afi->afiMetricsDeferred				&&
	  utilMemoryBufferIsEmpty( &(afi->afiAfmFileName) )	)
	{
	if  ( appFcGetFontMetrics( afi ) )
	    { SDEB(afi->afiFullName); return -1;	}
	}
#   endif

    if  ( afi->afiMetricsDeferred				&&
	  ! utilMemoryBufferIsEmpty( &(afi->afiAfmFileName) )	)
	{
	if  ( psGetDeferredMetrics( afi ) )
	    { LDEB(afi->afiAfmFileName.mbSize); return -1;	}
	}

    if  ( afi->afiMetricsDeferred )
	{ SLDEB(afi->afiFullName,afi->afiMetricsDeferred); return -1; }

    return 0;
    }

