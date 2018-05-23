/************************************************************************/
/*									*/
/*  Font Chooser.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>
#   include	<limits.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<geoString.h>
#   include	<utilPropMask.h>
#   include	<textAttributeUtil.h>

#   include	"appFrame.h"
#   include	"guiToolUtil.h"
#   include	"appMatchFont.h"

#   include	"appFontTool.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"
#   include	"drawDrawingSurfacePrivate.h"
#   include	"guiTextUtil.h"

/************************************************************************/
/*									*/
/*  Value lists that can be used when no particular font is selected.	*/
/*									*/
/************************************************************************/

static int EditFontToolSizesHalfPoints[]=
			    {
			    2* 8,
			    2* 9,
			    2* 10,
			    2* 11,
			    2* 12,
			    2* 14,
			    2* 16,
			    2* 18,
			    2* 24,
			    2* 36,
			    2* 48,
			    2* 64
			    };

static const int EditFontToolSizeCount=
		sizeof(EditFontToolSizesHalfPoints)/sizeof(int);

/************************************************************************/
/*									*/
/*  Format the size of the font in the Text box below the list.		*/
/*									*/
/************************************************************************/

static void appFontToolFormatSize(	char *		scratch,
					int		halfPoints )
    {
    if  ( halfPoints % 2 )
	{ sprintf( scratch, "%d.5", halfPoints/ 2 );	}
    else{ sprintf( scratch, "%d", halfPoints/ 2 );	}
    }

/************************************************************************/
/*									*/
/*  Find an existing face of the font.					*/
/*									*/
/************************************************************************/

static int appFontToolFindExistingFace(	const ExpandedTextAttribute *	etaC,
					const DocumentFont *		df )
    {
    int			i;
    /*
    TextAttribute *	taC= &(etaC->etaTextAttribute);
    */

    for ( i= 0; i < FONTface_COUNT; i++ )
	{
	if  ( df->dfPsFontInfo[i] )
	    {
	    /*
	    taC->taFontIsBold= FACE_BOLD( i );
	    taC->taFontIsSlanted= FACE_SLANTED( i );
	    */
	    return i;
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Redraw the sample of the font chooser.				*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appFontRedraw, w, voidafc, exposeEvent )
    {
    AppFontChooser *			afc= (AppFontChooser *)voidafc;
    DrawingSurface			ds= afc->afcDrawingSurface;

    DocumentRectangle			drClip;

    int					x;
    int					y;
    int					wide;
    int					high;

    DocumentRectangle			drWidget;
    DocumentRectangle			drText;

    int					l= strlen( afc->afcChoiceText );

    guiCollectExposures( &drClip, afc->afcSampleDrawing, exposeEvent );

    drawSetClipRect( ds, &drClip );

    drawSetForegroundColorWhite( ds );
    drawFillRectangle( ds, &drClip );

    if  ( afc->afcScreenFont < 0 )
	{ /*SLDEB(afc->afcChoiceText,afc->afcScreenFont);*/ return;	}

    drawSetForegroundColorBlack( ds );

    guiDrawGetSizeOfWidget( &wide, &high, w );
    drWidget.drX0= 0;
    drWidget.drX1= wide- 1;
    drWidget.drY0= 0;
    drWidget.drY1= high- 1;

    x= y= 0;
    drawGetTextExtents( &drText, ds, x, y,
			afc->afcScreenFont, afc->afcChoiceText, l );

    y= ( drWidget.drY1+ drWidget.drY0 )/ 2- ( drText.drY1+ drText.drY0+ 1 )/2;
    x= ( drWidget.drX1+ drWidget.drX0 )/ 2- ( drText.drX1+ drText.drX0+ 1 )/2;

    drawString( ds, x, y, afc->afcScreenFont, afc->afcChoiceText, l );

    drawNoClipping( ds );

    return;
    }

/************************************************************************/
/*									*/
/*  Show the current font size.						*/
/*									*/
/************************************************************************/

static void appFontShowSizeInList(
				AppFontChooser *	afc,
				int			fontSizeHalfPoints )
    {
    int		pos;
    int		sizeChosen= -1;

    afc->afcInProgrammaticChange++;

    for ( pos= 0; pos < EditFontToolSizeCount; pos++ )
	{
	if  ( fontSizeHalfPoints > 0				&&
	      EditFontToolSizesHalfPoints[pos] ==
					fontSizeHalfPoints	)
	    { sizeChosen= pos; break;	}
	}

    if  ( sizeChosen >= 0 )
	{ appGuiSelectPositionInListWidget( afc->afcSizeList, sizeChosen ); }
    else{ appGuiRemoveSelectionFromListWidget( afc->afcSizeList );	    }

    afc->afcInProgrammaticChange--;

    return;
    }

static void appFontShowSizeInText(
				AppFontChooser *	afc,
				int			fontSizeHalfPoints )
    {
    char	scratch[80];

    if  ( fontSizeHalfPoints >= 0 )
	{
	appFontToolFormatSize( scratch, fontSizeHalfPoints );
	appStringToTextWidget( afc->afcSizeText, scratch );
	}
    else{
	appStringToTextWidget( afc->afcSizeText, "" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the size of the currently selected font.			*/
/*									*/
/************************************************************************/

static int appFontToolGetSize(	int *			pFontSizeHalfPoints,
				int			anyway,
				AppFontChooser *	afc )
    {
    ExpandedTextAttribute *	eta= &(afc->afcTextAttributeChosen);
    TextAttribute *		ta= &(eta->etaTextAttribute);
    int				sizeHalfPoints= ta->taFontSizeHalfPoints;

    char *		val;
    char *		s;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    int			isEmpty;

    if  ( afc->afcSizeText )
	{
	s= val= appGetStringFromTextWidget( afc->afcSizeText );
	while( isspace( *s ) )
	    { s++;	}
	isEmpty= ! *s;

	appFreeStringFromTextWidget( val );
	}
    else{ isEmpty= 1;	}

    if  ( isEmpty )
	{ sizeHalfPoints= -1;	}
    else{
	int		changed= 0;
	int		sizeTwips= sizeHalfPoints;

	if  ( sizeTwips > 0 )
	    { sizeTwips *= 10;	}

	if  ( appGetLengthFromTextWidget( afc->afcSizeText,
				&sizeTwips, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    {
	    if  ( anyway )
		{ sizeHalfPoints= -1;	}
	    else{ return -1;			}
	    }
	else{
	    char	scratch[80];

	    sizeHalfPoints= ( sizeTwips+ 5 )/ 10;

	    appFontToolFormatSize( scratch, sizeHalfPoints );

	    val= appGetStringFromTextWidget( afc->afcSizeText );

	    if  ( strcmp( val, scratch ) )
		{ appStringToTextWidget( afc->afcSizeText, scratch );	}

	    appFreeStringFromTextWidget( val );
	    }
	}

    *pFontSizeHalfPoints= sizeHalfPoints;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the current baseline shift.					*/
/*									*/
/************************************************************************/

static int appFontToolGetBaseline(
				int *			pBaselineHalfPoints,
				int *			pEmpty,
				int			anyway,
				AppFontChooser *	afc )
    {
    ExpandedTextAttribute *	eta= &(afc->afcTextAttributeChosen);
    TextAttribute *		ta= &(eta->etaTextAttribute);
    int				baselineHalfPoints= ta->taBaselineShiftHalfPoints;

    char *		val;
    char *		s;

    const int		minValue= INT_MIN;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    if  ( afc->afcBaselineText )
	{
	s= val= appGetStringFromTextWidget( afc->afcBaselineText );
	while( isspace( *s ) )
	    { s++;	}
	*pEmpty= ! *s;

	appFreeStringFromTextWidget( val );
	}
    else{ *pEmpty= 1;	}

    if  ( *pEmpty )
	{ baselineHalfPoints= -1;	}
    else{
	int		changed= 0;
	int		baselineTwips= baselineHalfPoints;

	if  ( baselineTwips > 0 )
	    { baselineTwips *= 10;	}

	if  ( appGetLengthFromTextWidget( afc->afcBaselineText,
				&baselineTwips, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    {
	    if  ( anyway )
		{ baselineHalfPoints= -1;	}
	    else{ return -1;			}
	    }
	else{
	    char	scratch[80];

	    if  ( baselineTwips < 0 )
		{ baselineHalfPoints= ( baselineTwips- 5 )/ 10;	}
	    else{ baselineHalfPoints= ( baselineTwips+ 5 )/ 10;	}

	    appFontToolFormatSize( scratch, baselineHalfPoints );

	    val= appGetStringFromTextWidget( afc->afcBaselineText );

	    if  ( strcmp( val, scratch ) )
		{ appStringToTextWidget( afc->afcBaselineText, scratch ); }

	    appFreeStringFromTextWidget( val );
	    }
	}

    *pBaselineHalfPoints= baselineHalfPoints;
    return 0;
    }

static int appFontGetCurrent(	int *			pFamily,
				int *			pFace,
				int *			pFontSizeHalfPoints,
				int			anyway,
				AppFontChooser *	afc )
    {
    ExpandedTextAttribute *		etaC= &(afc->afcTextAttributeChosen);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);

    DocumentFontList *			dfl= &(afc->afcDocumentFontList);

    int					fontChosen= -1;
    int					faceChosen= afc->afcFaceChosen;

    int					sizeHalfPoints;

    if  ( appFontToolGetSize( &sizeHalfPoints, anyway, afc ) )
	{ return -1;	}

    if  ( PROPmaskISSET( chosenMask, TApropFONT_NUMBER )	&&
	  afc->afcFontSortIndexChosen >= 0			)
	{
	fontChosen= utilDocumentFontListGetArrayIndex( dfl,
						afc->afcFontSortIndexChosen );
	}

    if  ( fontChosen < 0 && anyway )
	{ fontChosen= 0; }
    if  ( faceChosen < 0 && anyway )
	{ faceChosen= 0; }
    if  ( sizeHalfPoints < 0 && anyway )
	{ sizeHalfPoints= 24; }

    if  ( anyway && fontChosen >= 0 )
	{
	DocumentFont *	df;

	df= docFontListGetFontByNumber( dfl, fontChosen );

	if  ( df && ( faceChosen < 0 || ! df->dfPsFontInfo[faceChosen] ) )
	    { faceChosen= appFontToolFindExistingFace( etaC, df );	}
	}

    if  ( pFamily )
	{ *pFamily= fontChosen;	}
    if  ( pFace )
	{ *pFace= faceChosen;	}
    if  ( pFontSizeHalfPoints )
	{ *pFontSizeHalfPoints= sizeHalfPoints;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Format the current font name.					*/
/*									*/
/************************************************************************/

static void appFontFormatCurrent(	char *			target,
					AppFontChooser *	afc,
					const char *		familyName,
					int			face,
					int			sizeHalfPoints )
    {
    const char *	familyText= "*";
    char		faceName[40];
    char		sizeStr[40];

    faceName[0]= '*';
    faceName[1]= '\0';

    sizeStr[0]= '*';
    sizeStr[1]= '\0';

    if  ( familyName )
	{ familyText= familyName;	}

    if  ( face >= 0		&&
	  face < FONTface_COUNT &&
	  strlen( (char *)afc->afcResources.aftrFaces[face] ) <
							sizeof(faceName) )
	{ strcpy( faceName, afc->afcResources.aftrFaces[face] ); }

    if  ( sizeHalfPoints > 0 )
	{
	if  ( sizeHalfPoints % 2 )
	    { sprintf( sizeStr, "%d.5", sizeHalfPoints/ 2 );	}
	else{ sprintf( sizeStr, "%d", sizeHalfPoints/ 2 );	}
	}

    sprintf( target, "%s,%s,%s", familyText, faceName, sizeStr );

    return;
    }

/************************************************************************/
/*									*/
/*  Reflect the currently selected font.				*/
/*									*/
/************************************************************************/

static void appFontShowExampleOfCurrent(	AppFontChooser *	afc )
    {
    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    DrawingSurface			ds= afc->afcDrawingSurface;
    DocumentFontList *			dfl= &(afc->afcDocumentFontList);
    int					pixelSize;

    const PostScriptFontList *		psfl= afc->afcPostScriptFontList;
    const AfmFontInfo *			afi;
    const IndexSet *			unicodesWanted;

    int					fontSizeHalfPoints= -1;

    int					family= -1;
    int					face= -1;

    int			screenFont;

    TextAttribute	ta;
    PropertyMask	taSetMask;
    PropertyMask	taDoneMask;

    utilPropMaskClear( &taSetMask );
    utilInitTextAttribute( &ta );

    PROPmaskADD( &taSetMask, TApropFONT_NUMBER );
    PROPmaskADD( &taSetMask, TApropFONTSIZE );
    PROPmaskADD( &taSetMask, TApropFONTBOLD );
    PROPmaskADD( &taSetMask, TApropFONTSLANTED );

    utilPropMaskAnd( &taSetMask, &taSetMask, &(afc->afcChosenMask) );

    docIndirectTextAttribute( &taDoneMask, &ta, etaC, &taSetMask, dfl,
							(ColorPalette *)0 );

    afc->afcScreenFont= -1;

    appFontGetCurrent( &family, &face, &fontSizeHalfPoints, 0, afc );

    appFontFormatCurrent( afc->afcChoiceText, afc, etaC->etaFontName,
						face, fontSizeHalfPoints );
    appFontGetCurrent( &family, &face, &fontSizeHalfPoints, 1, afc );
    if  ( ta.taFontNumber < 0 )
	{ ta.taFontNumber= family; }
    if  ( ta.taFontNumber < 0 )
	{ ta.taFontNumber= 0; }
    if  ( ta.taFontSizeHalfPoints <= 0 )
	{ ta.taFontSizeHalfPoints= fontSizeHalfPoints; }

    afi= appGetFontInfoForAttribute( &unicodesWanted, &ta, dfl, psfl );
    if  ( afi )
	{ appGuiChangeLabelText( afc->afcPsName, afi->afiFontName );	}
    else{ appGuiChangeLabelText( afc->afcPsName, "<?>" );		}

    pixelSize= textGetPixelSize( afc->afcPixelsPerTwip, &ta );
    screenFont= drawOpenScreenFont( ds, afi, pixelSize, unicodesWanted );
    if  ( screenFont < 0 )
	{ LDEB(screenFont);	}
    else{
	afc->afcScreenFont= screenFont;

	if  ( drawFontImplementationName( afc->afcScreenChoiceText,
					    sizeof(afc->afcScreenChoiceText),
					    ds, afc->afcScreenFont ) )
	    { appGuiChangeLabelText( afc->afcScreenName, "<?>" );	}
	else{
	    appGuiChangeLabelText( afc->afcScreenName,
						afc->afcScreenChoiceText );
	    }
	}

    guiExposeDrawingWidget( afc->afcSampleDrawing );

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

/************************************************************************/
/*									*/
/*  A font family has been chosen, or set.				*/
/*  Try to find a face that matches the current attributes.		*/
/*									*/
/*  NOTE: The family list is set by the caller.				*/
/*									*/
/************************************************************************/

static int appFontToolGetFaceNumber(	AppFontChooser *	afc )
    {
    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    const TextAttribute *		taC= &(etaC->etaTextAttribute);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);

    if  ( PROPmaskISSET( chosenMask, TApropFONTBOLD )		&&
	  PROPmaskISSET( chosenMask, TApropFONTSLANTED )	)
	{ return FACE_INDEX(taC->taFontIsSlanted,taC->taFontIsBold); }

    return -1;
    }

static void appFontReflectFamily(	AppFontChooser *	afc )
    {
    int					face;
    int					set;

    appGuiEmptyListWidget( afc->afcFaceList );

    for ( face= 0; face < FONTface_COUNT; face++ )
	{
	afc->afcFaceMapFwd[face]= -1;
	afc->afcFaceMapBck[face]= -1;
	}

    set= 0;
    for ( face= 0; face < FONTface_COUNT; face++ )
	{
	appGuiAddValueToListWidget( afc->afcFaceList, -1,
					afc->afcResources.aftrFaces[face] );

	afc->afcFaceMapFwd[set]= face;
	afc->afcFaceMapBck[face]= set;
	set++;
	}

    afc->afcFaceChosen= appFontToolGetFaceNumber( afc );

    if  ( afc->afcFaceChosen >= 0 )
	{
	appGuiSelectPositionInListWidget( afc->afcFaceList,
				    afc->afcFaceMapBck[afc->afcFaceChosen] );
	}
    else{
	appGuiRemoveSelectionFromListWidget( afc->afcFaceList );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Current properties have just been set: adapt to them.		*/
/*									*/
/*  1)  If a font family is set, look for it in the list of families.	*/
/*  2)	Position the list accordingly.					*/
/*  3)	Set the encoding menu.						*/
/*  4)	Select the correct font face in the list widget.		*/
/*  5)	Display the font size.						*/
/*  7)	Set underline toggle.						*/
/*  8)	Set strikethrough toggle.					*/
/*  9)	Set the superscript toggle.					*/
/*  10)	Set the subscript toggle.					*/
/*  11)	Set the smallcaps toggle.					*/
/*  12)	Set the capitals toggle.					*/
/*  15)	Show an example of the current font.				*/
/*									*/
/************************************************************************/

static void appFontReflectProperties(	AppFontChooser *	afc )
    {
    int					fontIndex;
    int					face;

    const ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    const TextAttribute *		taC= &(etaC->etaTextAttribute);
    const PropertyMask *		chosenMask= &(afc->afcChosenMask);

    DocumentFontList *			dfl= &(afc->afcDocumentFontList);
    int					set;

    /*  1  */
    fontIndex= -1;

    if  ( PROPmaskISSET( chosenMask, TApropFONT_NUMBER )	&&
	  etaC->etaFontName					)
	{
	int fontNumber= docGetFontByName( dfl, etaC->etaFontName );

	if  ( fontNumber >= 0 )
	    { fontIndex= utilDocumentFontListGetSortIndex( dfl, fontNumber ); }
	}

    /*  2  */
    if  ( afc->afcFontSortIndexChosen != fontIndex )
	{
	afc->afcFontSortIndexChosen= fontIndex;

	if  ( afc->afcFontSortIndexChosen >= 0 )
	    {
	    appGuiSelectPositionInListWidget( afc->afcFamilyList,
					afc->afcFontSortIndexChosen );
	    }
	else{
	    appGuiRemoveSelectionFromListWidget( afc->afcFamilyList );
	    }

	appFontReflectFamily( afc );
	}

    /*  4  */
    face= appFontToolGetFaceNumber( afc );

    if  ( afc->afcFaceChosen != face )
	{
	afc->afcFaceChosen= face;

	if  ( afc->afcFaceChosen >= 0 )
	    {
	    appGuiSelectPositionInListWidget( afc->afcFaceList,
				    afc->afcFaceMapBck[afc->afcFaceChosen] );
	    }
	else{ appGuiRemoveSelectionFromListWidget( afc->afcFaceList );	}
	}

    /*  5  */
    {
    int		fontSizeHalfPoints= -1;

    if  ( PROPmaskISSET( chosenMask, TApropFONTSIZE ) )
	{ fontSizeHalfPoints= taC->taFontSizeHalfPoints;	}

    appFontShowSizeInList( afc, fontSizeHalfPoints );
    appFontShowSizeInText( afc, fontSizeHalfPoints );
    }

    {
    if  ( PROPmaskISSET( chosenMask, TApropBASELINE_SHIFT ) )
	{
	char	scratch[80];

	appFontToolFormatSize( scratch, taC->taBaselineShiftHalfPoints );

	appStringToTextWidget( afc->afcBaselineText, scratch );
	}
    else{
	appStringToTextWidget( afc->afcBaselineText, "" );
	}
    }

    /*  7  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropTEXTUNDERLINED ) )
	{ set= taC->taTextIsUnderlined;	}
    appGuiSetToggleState( afc->afcUnderlinedToggle, set );

    /*  8  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSTRIKETHROUGH ) )
	{ set= taC->taHasStrikethrough;	}
    appGuiSetToggleState( afc->afcStrikethroughToggle, set );

    /*  9  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB ) )
	{ set= taC->taSuperSub == TEXTvaSUPERSCRIPT;	}
    appGuiSetToggleState( afc->afcSuperscriptToggle, set );

    /*  10  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB ) )
	{ set= taC->taSuperSub == TEXTvaSUBSCRIPT;	}
    appGuiSetToggleState( afc->afcSubscriptToggle, set );

    /*  11  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropSMALLCAPS ) )
	{ set= taC->taSmallCaps;	}
    appGuiSetToggleState( afc->afcSmallcapsToggle, set );

    /*  12  */
    set= 0;
    if  ( PROPmaskISSET( &(afc->afcChosenMask), TApropCAPITALS ) )
	{ set= taC->taCapitals;	}
    appGuiSetToggleState( afc->afcCapitalsToggle, set );

    /*  15  */
    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Underline' toggle was activated.				*/
/*  The 'Strikethrough' toggle was activated.				*/
/*  The 'Superscript' toggle was activated.				*/
/*  The 'Subscript' toggle was activated.				*/
/*  The 'Smallcaps' toggle was activated.				*/
/*  The 'Capitals' toggle was activated.				*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appFontUnderlinedToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropTEXTUNDERLINED ) )
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropTEXTUNDERLINED );
    taC->taTextIsUnderlined= ( set != 0 );

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontStrikethroughToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSTRIKETHROUGH )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSTRIKETHROUGH );
    taC->taHasStrikethrough= ( set != 0 );

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontSuperscriptToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB ) )
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSUPERSUB );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( taC->taSuperSub == TEXTvaSUBSCRIPT )
	    { resetOther= 1;	}

	taC->taSuperSub= TEXTvaSUPERSCRIPT;

	if  ( resetOther )
	    { appGuiSetToggleState( afc->afcSubscriptToggle, 0 );	}
	}
    else{
	if  ( taC->taSuperSub == TEXTvaSUPERSCRIPT )
	    { taC->taSuperSub= TEXTvaREGULAR;	}
	}

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontSubscriptToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSUPERSUB )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSUPERSUB );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( taC->taSuperSub == TEXTvaSUPERSCRIPT )
	    { resetOther= 1;	}

	taC->taSuperSub= TEXTvaSUBSCRIPT;

	if  ( resetOther )
	    { appGuiSetToggleState( afc->afcSubscriptToggle, 0 );	}
	}
    else{
	if  ( taC->taSuperSub == TEXTvaSUBSCRIPT )
	    { taC->taSuperSub= TEXTvaREGULAR;	}
	}

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontSmallcapsToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropSMALLCAPS )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropSMALLCAPS );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( taC->taCapitals )
	    { resetOther= 1;	}

	taC->taSmallCaps= 1;

	if  ( resetOther )
	    {
	    taC->taCapitals= 0;
	    appGuiSetToggleState( afc->afcCapitalsToggle, taC->taCapitals );
	    PROPmaskADD( &(afc->afcChosenMask), TApropCAPITALS );
	    }
	}
    else{
	taC->taSmallCaps= 0;
	}

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFontCapitalsToggled, w, voidafc, voidtbcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( ! set								&&
	  ! PROPmaskISSET( &(afc->afcChosenMask), TApropCAPITALS )	)
	{ return;	}

    PROPmaskADD( &(afc->afcChosenMask), TApropCAPITALS );

    if  ( set )
	{
	int		resetOther= 0;

	if  ( taC->taSmallCaps )
	    { resetOther= 1;	}

	taC->taCapitals= 1;

	if  ( resetOther )
	    {
	    taC->taSmallCaps= 0;
	    appGuiSetToggleState( afc->afcSmallcapsToggle, taC->taSmallCaps );
	    PROPmaskADD( &(afc->afcChosenMask), TApropSMALLCAPS );
	    }
	}
    else{
	taC->taCapitals= 0;
	}

    guiEnableWidget( afc->afcApplyButton,
			    ! utilPropMaskIsEmpty( &(afc->afcChosenMask) ) );

    return;
    }

/************************************************************************/
/*									*/
/*  A font family has been chosen					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appFontFamilyChosen, w, voidafc, voidlcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    DocumentFontList *		dfl= &(afc->afcDocumentFontList);

    int				fontIndex;
    int				changed= 0;

    const DocumentFont *	df;

    if  ( afc->afcInProgrammaticChange > 0 )
	{ return;	}

    fontIndex= appGuiGetPositionFromListCallback( w, voidlcs );
    if  ( fontIndex < 0 || fontIndex >= dfl->dflFontCount )
	{ LLDEB(fontIndex,dfl->dflFontCount); return;	}

    df= utilDocumentFontListGetFontBySortIndex( dfl, fontIndex );
    if  ( ! df )
	{ XDEB(df); return;	}

    if  ( docExpandedTextAttributeSetFontName( etaC, &changed, df->dfName ) )
	{ SDEB(df->dfName); return;	}

    if  ( ! PROPmaskISSET( &(afc->afcChosenMask), TApropFONT_NUMBER ) )
	{ changed= 1;	}

    afc->afcFontSortIndexChosen= fontIndex;
    PROPmaskADD( &(afc->afcChosenMask), TApropFONT_NUMBER );

    if  ( afc->afcFaceChosen >= 0			&&
	  ! df->dfPsFontInfo[afc->afcFaceChosen]	)
	{
	int	faceChosen;

	faceChosen= appFontToolFindExistingFace( etaC, df );
	if  ( faceChosen >= 0 )
	    { afc->afcFaceChosen= faceChosen;	}
	}

    if  ( changed )
	{
	afc->afcInProgrammaticChange++;

	appFontReflectFamily( afc );
	appFontShowExampleOfCurrent( afc );

	afc->afcInProgrammaticChange--;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A font face has been chosen						*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appFontFaceChosen, w, voidafc, voidlcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);
    PropertyMask *		chosenMask= &(afc->afcChosenMask);

    int				i;

    if  ( afc->afcInProgrammaticChange > 0 )
	{ return;	}

    i= appGuiGetPositionFromListCallback( w, voidlcs );
    if  ( i == afc->afcFaceChosen )
	{ return;	}

    if  ( i < 0 || i >= FONTface_COUNT )
	{ LLDEB(i,FONTface_COUNT); return;	}

    afc->afcFaceChosen= afc->afcFaceMapFwd[i];

    taC->taFontIsBold= FACE_BOLD( afc->afcFaceChosen );
    taC->taFontIsSlanted= FACE_SLANTED( afc->afcFaceChosen );

    PROPmaskADD( chosenMask, TApropFONTBOLD );
    PROPmaskADD( chosenMask, TApropFONTSLANTED );

    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  A font size has been chosen in the list widget.			*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appFontSizeChosen, w, voidafc, voidlcs )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);

    int				i;

    if  ( afc->afcInProgrammaticChange > 0 )
	{ return;	}

    i= appGuiGetPositionFromListCallback( w, voidlcs );
    if  ( i < 0 || i >= EditFontToolSizeCount )
	{ LLDEB(i,EditFontToolSizeCount); return; }

    taC->taFontSizeHalfPoints= EditFontToolSizesHalfPoints[i];
    PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );

    appFontShowSizeInText( afc, taC->taFontSizeHalfPoints );

    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  The user typed 'Enter' in the text box with the font size.		*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( appFontToolSizeChanged, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);

    const int			anyway= 0;
    int				sizeHalfPoints;

    if  ( appFontToolGetSize( &sizeHalfPoints, anyway, afc ) )
	{ return;	}

    if  ( sizeHalfPoints > 0 )
	{
	taC->taFontSizeHalfPoints= sizeHalfPoints;
	PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );
	}
    else{
	PROPmaskUNSET( &(afc->afcChosenMask), TApropFONTSIZE );
	}

    appFontShowExampleOfCurrent( afc );

    return;
    }

/************************************************************************/
/*									*/
/*  The contents of the font size text box changed: Select the new	*/
/*  value in the list.							*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appFontToolSizeTyped, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);

    int				fontSizeHalfPoints= -1;
    int				fontSizeTwips= -1;

    char *			s;

    s= appGetStringFromTextWidget( afc->afcSizeText );

    if  ( ! geoLengthFromString( s, UNITtyPOINTS, &fontSizeTwips ) )
	{
	fontSizeHalfPoints= ( fontSizeTwips+ 5 )/ 10;
	PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );
	}
    else{
	PROPmaskUNSET( &(afc->afcChosenMask), TApropFONTSIZE );
	}

    appFreeStringFromTextWidget( s );

    if  ( taC->taFontSizeHalfPoints != fontSizeHalfPoints )
	{ appFontShowSizeInList( afc, fontSizeHalfPoints );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Cleanup of the Font Chooser.					*/
/*									*/
/************************************************************************/

void appFontChooserCleanPage( AppFontChooser *	afc )
    {
    docCleanExpandedTextAttribute( &(afc->afcTextAttributeSet) );
    docCleanExpandedTextAttribute( &(afc->afcTextAttributeChosen) );

    docCleanFontList( &(afc->afcDocumentFontList) );

    if  ( afc->afcDrawingSurface )
	{ drawFreeDrawingSurface( afc->afcDrawingSurface );	}

    return;
    }

/************************************************************************/
/*									*/
/*  make the font chooser form with three listboxes.			*/
/*									*/
/************************************************************************/

static void appFontMakeChooseForm(	APP_WIDGET			parent,
					const AppFontToolResources *	aftr,
					AppFontChooser *		afc )
    {
    const int		heightResizable= 1;

    const int		familyPosition= 0;
    const int		familyColspan= 5;
    const int		facePosition= familyPosition+ familyColspan;
    const int		faceColspan= 3;
    const int		sizePosition= facePosition+ faceColspan;
    const int		sizeColspan= 2;
    const int		colcount= sizePosition+ sizeColspan;

    const int		listHeight= 6;

    afc->afcChooseRow= appMakeRowInColumn( parent, colcount, heightResizable );

    /********************************************************************/
    /*									*/
    /*  Parents with listbox and label:					*/
    /*									*/
    /********************************************************************/

    appMakeColumnInRow( &(afc->afcFamilyColumn), afc->afcChooseRow,
				familyPosition, familyColspan );

    appMakeLabelInColumn( &(afc->afcFamilyLabel),
				afc->afcFamilyColumn, aftr->aftrFamily );

    appGuiMakeListInColumn( &(afc->afcFamilyList),
		afc->afcFamilyColumn, listHeight,
		appFontFamilyChosen, (APP_BUTTON_CALLBACK_T)0, (void *)afc );

    /***/

    appMakeColumnInRow( &(afc->afcFaceColumn), afc->afcChooseRow,
						facePosition, faceColspan );

    appMakeLabelInColumn( &(afc->afcFaceLabel),
					afc->afcFaceColumn, aftr->aftrFace );

    appGuiMakeListInColumn( &(afc->afcFaceList), afc->afcFaceColumn, listHeight,
		appFontFaceChosen, (APP_BUTTON_CALLBACK_T)0, (void *)afc );

    {
    const int		textColumns= 4;
    const int		textEnabled= 1;

    appMakeColumnInRow( &(afc->afcSizeColumn), afc->afcChooseRow,
						sizePosition, sizeColspan );

    appMakeLabelInColumn( &(afc->afcSizeLabel), afc->afcSizeColumn,
							    aftr->aftrSize );

    appGuiMakeListInColumn( &(afc->afcSizeList), afc->afcSizeColumn, 0,
		appFontSizeChosen, (APP_BUTTON_CALLBACK_T)0, (void *)afc );

    appMakeTextInColumn( &(afc->afcSizeText), afc->afcSizeColumn,
						textColumns, textEnabled );

    appGuiSetGotValueCallbackForText( afc->afcSizeText,
					appFontToolSizeChanged, (void *)afc );

    appGuiSetTypingCallbackForText( afc->afcSizeText,
					appFontToolSizeTyped, (void *)afc );
    }

    return;
    }

/************************************************************************/
/*									*/
/*  make the label to preview the font.					*/
/*									*/
/************************************************************************/

static void appFontMakePreviewDrawing(	APP_WIDGET		parent,
					AppFontChooser *	afc,
					int			twipsSize )
    {
    const int		wide= -1;
    int			high= 1.15* ( afc->afcPixelsPerTwip* twipsSize )+ 0.5;
    const int		heightResizable= 0;

    appGuiMakeDrawingAreaInColumn( &(afc->afcSampleDrawing), parent,
		    wide, high, heightResizable, appFontRedraw, (void *)afc );

    
    appMakeLabelInColumn( &(afc->afcScreenName), parent, "?" );
    appMakeLabelInColumn( &(afc->afcPsName), parent, "?" );

    return;
    }

/************************************************************************/
/*									*/
/*  The set button of a font tool has been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appFontSetPushed, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);
    TextAttribute *		taC= &(etaC->etaTextAttribute);

    int				sizeHalfPoints;
    int				baselineHalfPoints;
    int				empty= 1;
    const int			anyway= 1;

    if  ( ! afc->afcSetFont )
	{ XDEB(afc->afcSetFont); return;	}

    /**/
    if  ( appFontToolGetSize( &sizeHalfPoints, anyway, afc ) )
	{ return;	}

    if  ( sizeHalfPoints < 0 )
	{ PROPmaskUNSET( &(afc->afcChosenMask), TApropFONTSIZE );	}
    else{
	PROPmaskADD( &(afc->afcChosenMask), TApropFONTSIZE );
	taC->taFontSizeHalfPoints= sizeHalfPoints;
	}

    /**/
    if  ( appFontToolGetBaseline( &baselineHalfPoints, &empty, anyway, afc ) )
	{ return;	}

    if  ( empty )
	{ PROPmaskUNSET( &(afc->afcChosenMask), TApropBASELINE_SHIFT ); }
    else{
	taC->taBaselineShiftHalfPoints= baselineHalfPoints;
	PROPmaskADD( &(afc->afcChosenMask), TApropBASELINE_SHIFT );
	}

    /**/
    (*afc->afcSetFont)( afc->afcApplication, &(afc->afcChosenMask), etaC );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of a font tool has been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appFontRevertPushed, w, voidafc )
    {
    AppFontChooser *		afc= (AppFontChooser *)voidafc;

    docCopyExpandedTextAttribute( &(afc->afcTextAttributeChosen),
					    &(afc->afcTextAttributeSet) );

    afc->afcChosenMask= afc->afcSetMask;

    afc->afcInProgrammaticChange++;
    appFontReflectProperties( afc );
    afc->afcInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  Reflect/Remember the current font of the application.		*/
/*									*/
/*  1)  Adapt the old fashioned internals to new style interface.	*/
/*									*/
/************************************************************************/

static int appFontToolSetCurrentFont(
				AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const ExpandedTextAttribute *	etaNew )
    {
    ExpandedTextAttribute *	etaS= &(afc->afcTextAttributeSet);
    ExpandedTextAttribute *	etaC= &(afc->afcTextAttributeChosen);

    PropertyMask		doneMask;

    docCleanExpandedTextAttribute( etaS );
    docInitExpandedTextAttribute( etaS );

    afc->afcFontSortIndexChosen= -1;
    afc->afcFaceChosen= -1;

    utilPropMaskClear( &doneMask );
    docUpdateExpandedTextAttribute( &doneMask, etaS, etaNew, newMask );
    docCopyExpandedTextAttribute( etaC, etaS );

    afc->afcSetMask= *newMask;
    afc->afcChosenMask= afc->afcSetMask;

    afc->afcInProgrammaticChange++;
    appFontReflectProperties( afc );
    afc->afcInProgrammaticChange--;

    return 0;
    }

int appFontToolShowCurrentFont(	AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const TextAttribute *		taNew,
				unsigned int			documentId,
				int				canChange,
				const DocumentFontList *	dfl,
				const ColorPalette *		cp )
    {
    int				rval= 0;
    DocumentFontList *		dflTo= &(afc->afcDocumentFontList);

    PropertyMask		doneMask;
    ExpandedTextAttribute	eta;

    docInitExpandedTextAttribute( &eta );
    utilPropMaskClear( &doneMask );

    afc->afcInProgrammaticChange++;

    if  ( afc->afcCurrentDocumentId != documentId )
	{
	int				i;

	if  ( docCopyFontList( dflTo, dfl ) )
	    { LDEB(1); rval= -1; goto ready; }

	appGuiRemoveSelectionFromListWidget( afc->afcFamilyList );
	appGuiEmptyListWidget( afc->afcFamilyList );

	for ( i= 0; i < dflTo->dflFontCount; i++ )
	    {
	    const DocumentFont *	df;

	    df= utilDocumentFontListGetFontBySortIndex( dflTo, i );
	    if  ( ! df )
		{ XDEB(df); continue;	}

	    appGuiAddValueToListWidget( afc->afcFamilyList, -1, df->dfName );
	    }

	afc->afcFontSortIndexChosen= -1;
	afc->afcFaceChosen= -1;

	afc->afcCurrentDocumentId= documentId;
	}

    docExpandTextAttribute( &doneMask, &eta, taNew, newMask, dflTo, cp );

    if  ( appFontToolSetCurrentFont( afc, newMask, &eta ) )
	{ LDEB(1);	}

    afc->afcCanChange= canChange;

    guiEnableWidget( afc->afcChooseRow, afc->afcCanChange );
    guiEnableWidget( afc->afcToggleRow1, afc->afcCanChange );
    guiEnableWidget( afc->afcToggleRow2, afc->afcCanChange );
    guiEnableWidget( afc->afcApplyRow, afc->afcCanChange );
    guiEnableWidget( afc->afcBaselineRow, afc->afcCanChange );
    guiEnableText( afc->afcBaselineText, afc->afcCanChange );
    guiEnableText( afc->afcSizeText, afc->afcCanChange );

  ready:

    docCleanExpandedTextAttribute( &eta );

    afc->afcInProgrammaticChange--;

    return rval;
    }

/************************************************************************/
/*									*/
/*  make a fonts tool.							*/
/*									*/
/************************************************************************/

void appFontToolFillPage(	AppFontChooser *		afc,
				const AppFontToolResources *	aftr,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    int				i;
    int				pos;
    APP_WIDGET			sep;

    EditApplication *		ea= afc->afcApplication;

    afc->afcPixelsPerTwip= 0;
    afc->afcDrawingSurface= (DrawingSurface)0;

    afc->afcPostScriptFontList= &(ea->eaPostScriptFontList);

    afc->afcInProgrammaticChange= 0;
    afc->afcFamilyList= (APP_WIDGET)0;
    afc->afcFaceList= (APP_WIDGET)0;
    afc->afcSizeList= (APP_WIDGET)0;
    afc->afcSizeText= (APP_WIDGET)0;

    afc->afcScreenFont= -1;
    afc->afcChoiceText[0]= '\0';
    afc->afcScreenChoiceText[0]= '\0';

    afc->afcSetFont= (FontChooserSetFont)0;
    afc->afcSubjectPage= subjectPage;
    afc->afcResources= *aftr;

    is->isPrivate= (void *)afc;

    for ( i= 0; i < FONTface_COUNT; i++ )
	{
	afc->afcFaceMapFwd[i]= -1;
	afc->afcFaceMapBck[i]= -1;
	}

    afc->afcPixelsPerTwip= ea->eaMagnification* ea->eaPixelsPerTwip;

    {
    const int			textColumns= 10;
    const int			twipsSize= 20* 48;

    appFontMakePreviewDrawing( pageWidget, afc, twipsSize );

    appGuiInsertSeparatorInColumn( &sep, pageWidget );

    appFontMakeChooseForm( pageWidget, aftr, afc );

    appGuiInsertSeparatorInColumn( &sep, pageWidget );

    guiToolMake3ToggleRow( &(afc->afcToggleRow1), pageWidget,
					&(afc->afcUnderlinedToggle),
					&(afc->afcSuperscriptToggle),
					&(afc->afcSmallcapsToggle),

					aftr->aftrTextUnderlined,
					aftr->aftrSuperscript,
					aftr->aftrSmallcaps,

					appFontUnderlinedToggled,
					appFontSuperscriptToggled,
					appFontSmallcapsToggled,
					(void *)afc );

    guiToolMake3ToggleRow( &(afc->afcToggleRow2), pageWidget,
					&(afc->afcStrikethroughToggle),
					&(afc->afcSubscriptToggle),
					&(afc->afcCapitalsToggle),

					aftr->aftrTextStrikethrough,
					aftr->aftrSubscript,
					aftr->aftrCapitals,

					appFontStrikethroughToggled,
					appFontSubscriptToggled,
					appFontCapitalsToggled,
					(void *)afc );

    guiToolMakeLabelAndTextRow( &(afc->afcBaselineRow),
					&(afc->afcBaselineLabel),
					&(afc->afcBaselineText), pageWidget,
					aftr->aftrBaseline, textColumns, 1 );

    /**/

    appGuiInsertSeparatorInColumn( &sep, pageWidget );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
			&(is->isRevertButton), &(is->isApplyButton),
			isr->isrRevert, isr->isrApplyToSubject,
			appFontRevertPushed, appFontSetPushed, afc );

    afc->afcApplyRow= is->isApplyRow;
    afc->afcRevertButton= is->isRevertButton;
    afc->afcApplyButton= is->isApplyButton;
    }

    docInitExpandedTextAttribute( &(afc->afcTextAttributeSet) );
    docInitExpandedTextAttribute( &(afc->afcTextAttributeChosen) );

    docInitFontList( &(afc->afcDocumentFontList) );
    afc->afcCurrentDocumentId= 0;

    utilPropMaskClear( &(afc->afcChosenMask) );
    utilPropMaskClear( &(afc->afcSetMask) );

    afc->afcFontSortIndexChosen= -2;
    afc->afcFaceChosen= -2;

    /**/
    appGuiEmptyListWidget( afc->afcSizeList );

    for ( pos= 0; pos < EditFontToolSizeCount; pos++ )
	{
	char	scratch[80];

	appFontToolFormatSize( scratch, EditFontToolSizesHalfPoints[pos] );

	appGuiAddValueToListWidget( afc->afcSizeList, -1, scratch );
	}
    /**/

    return;
    }

/************************************************************************/
/*									*/
/*  Final stages in making the font tool.				*/
/*									*/
/************************************************************************/

void appFontToolFillChoosers(	AppFontChooser *		afc,
				const AppFontToolResources *	aftr )
    {
    return;
    }

void appFontToolFinishPage(	AppFontChooser *		afc,
				const AppFontToolResources *	aftr )
    {
    afc->afcDrawingSurface= guiDrawingSurfaceForNativeWidget(
			    afc->afcSampleDrawing,
			    afc->afcPostScriptFontList->psflAvoidFontconfig );

    return;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_FontToolResourceTable[]=
    {
    APP_RESOURCE( "fontToolFamily",
		offsetof(AppFontToolResources,aftrFamily),
		"Family" ),
    APP_RESOURCE( "fontToolFace",
		offsetof(AppFontToolResources,aftrFace),
		"Typeface" ),
    APP_RESOURCE( "fontToolSize",
		offsetof(AppFontToolResources,aftrSize),
		"Size" ),

    APP_RESOURCE( "fontToolRegular",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceREGULAR]),
		"Regular" ),
    APP_RESOURCE( "fontToolBold",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceBOLD]),
		"Bold" ),
    APP_RESOURCE( "fontToolSlanted",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceSLANTED]),
		"Italic" ),
    APP_RESOURCE( "fontToolBoldSlanted",
		offsetof(AppFontToolResources,aftrFaces[FONTfaceBOLD_SLANTED]),
		"Bold Italic" ),

    APP_RESOURCE( "fontToolTextUnderlined",
		offsetof(AppFontToolResources,aftrTextUnderlined),
		"Underlined" ),
    APP_RESOURCE( "fontToolTextStrikethrough",
		offsetof(AppFontToolResources,aftrTextStrikethrough),
		"Strikethrough" ),

    APP_RESOURCE( "fontToolSuperscript",
		offsetof(AppFontToolResources,aftrSuperscript),
		"Superscript" ),
    APP_RESOURCE( "fontToolSubscript",
		offsetof(AppFontToolResources,aftrSubscript),
		"Subscript" ),

    APP_RESOURCE( "fontToolSmallcaps",
		offsetof(AppFontToolResources,aftrSmallcaps),
		"Small caps" ),
    APP_RESOURCE( "fontToolCapitals",
		offsetof(AppFontToolResources,aftrCapitals),
		"All caps" ),

    APP_RESOURCE( "fontToolBaseline",
		offsetof(AppFontToolResources,aftrBaseline),
		"Raise" ),
    };

void appFontToolGetResourceTable(	EditApplication *		ea,
					AppFontToolResources *		aftr )
    {
    static int	gotResources= 0;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)aftr,
				APP_FontToolResourceTable,
				sizeof(APP_FontToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
