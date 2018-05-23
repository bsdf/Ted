/************************************************************************/
/*									*/
/*  A Symbol picker.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<ucdBlock.h>
#   include	<uniUtf8.h>
#   include	<utilMemoryBuffer.h>
#   include	<textAttributeUtil.h>

#   include	"appMatchFont.h"
#   include	"appSymbolPicker.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"
#   include	"guiToolUtil.h"

#   include	<appDebugon.h>

#   define	FONT_SIZE	10
#   define	MIN_CELLS_WIDE	8
#   define	MIN_CELLS_HIGH	6

/************************************************************************/
/*									*/
/*  Geometry calculations.						*/
/*									*/
/************************************************************************/

static void appSymbolPickerGetDimensions(
				int *			pPixelsWide,
				int *			pPixelsHigh,
				SymbolPicker *		sp,
				int			cellsWide,
				int			cellsHigh )
    {
    *pPixelsWide= cellsWide* ( sp->spCellSizePixels+ 1 )+ 1;
    *pPixelsHigh= cellsHigh* ( sp->spCellSizePixels+ 1 )+ 1;

    *pPixelsHigh += 2* ( sp->spCellSizePixels+ 1 );

    return;
    }

static void appSymbolPickerCalculateGridSize(
				SymbolPicker *		sp,
				int			pixelsWide,
				int			pixelsHigh )
    {
    pixelsHigh -= 2* ( sp->spCellSizePixels+ 1 );

    sp->spCellsWide= ( pixelsWide- 1 )/ ( sp->spCellSizePixels+ 1 );
    sp->spCellsHigh= ( pixelsHigh- 1 )/ ( sp->spCellSizePixels+ 1 );

    return;
    }

static void appSymbolRectangle(		DocumentRectangle *	drSym,
					const SymbolPicker *	sp,
					int			row,
					int			col )
    {
    drSym->drX0= col* ( sp->spCellSizePixels+ 1 )+ 1;
    drSym->drX1= drSym->drX0+ sp->spCellSizePixels- 1;

    drSym->drY0= row* ( sp->spCellSizePixels+ 1 )+ 1;
    drSym->drY1= drSym->drY0+ sp->spCellSizePixels- 1;

    return;
    }

static void appSymbolStringRectangle(	DocumentRectangle *	drString,
					SymbolPicker *		sp )
    {
    int		pixelsWide;
    int		pixelsHigh;

    guiDrawGetSizeOfWidget( &pixelsWide, &pixelsHigh, sp->spSymbolDrawing );

    if  ( sp->spCellsWide == 0 || sp->spCellsHigh == 0 )
	{
	appSymbolPickerCalculateGridSize( sp, pixelsWide, pixelsHigh );
	}

    drString->drX0= 0;
    drString->drX1= pixelsWide- 1;
    drString->drY0= ( sp->spCellsHigh* ( sp->spCellSizePixels+ 1 ) )+ 1;
    drString->drY1= pixelsHigh- 1;

    return;
    }

static void appSymbolGetStringOrigin(
			    int *			pX,
			    int *			pY,
			    const DocumentRectangle *	drString,
			    const SymbolPicker *	sp )
    {
    *pX= drString->drX0+ sp->spCellSizePixels/ 2;
    *pY= ( drString->drY0+ drString->drY1+ sp->spCellSizePixels )/ 2;
    return;
    }

/************************************************************************/
/*									*/
/*  Translation back and forth between cell and symbol.			*/
/*									*/
/************************************************************************/

static int appSymbolGetSymbol(		SymbolPicker *		sp,
					int			row,
					int			col )
    {
    const IndexSet *	is;

    int			sym;
    int			i;
    int			upto= row* sp->spCellsWide+ col+ 1;

    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); return -1; }

    is= &(sp->spFontInfo->afiUnicodesProvided);
    sym= sp->spSymbolOffset- 1;

    for ( i= 0; i < upto; i++ )
	{
	sym= utilIndexSetGetNext( is, sym );
	if  ( sym < 0 )
	    { return -1;	}
	}

    return sym;
    }

static int appSymbolGetRowCol(		int *			pRow,
					int *			pCol,
					SymbolPicker *		sp,
					int			symbol )
    {
    const IndexSet *	is;
    int			step;
    int			from;

    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); *pRow= *pCol= 0; return 1; }

    is= &(sp->spFontInfo->afiUnicodesProvided);
    step= 0;
    from= sp->spSymbolOffset- 1;
    while( step < sp->spCellsWide* sp->spCellsHigh )
	{
	from= utilIndexSetGetNext( is, from );
	if  ( from < 0 )
	    { break;	}

	if  ( from == symbol )
	    {
	    *pRow= step/ sp->spCellsWide;
	    *pCol= step% sp->spCellsWide;

	    return 0;
	    }

	step++;
	}

    *pRow= *pCol= 0; return 1;
    }

/************************************************************************/

static int appSymbolPickerNextPageOffset(	SymbolPicker *	sp )
    {
    const IndexSet *	is;
    int			sym;
    int			step;

    int			block= ucdGetBlock( sp->spSymbolOffset );

    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); return -1;	}

    is= &(sp->spFontInfo->afiUnicodesProvided);

    sym= sp->spSymbolOffset;
    for ( step= 0; step < sp->spCellsHigh* sp->spCellsWide; step++ )
	{
	sym= utilIndexSetGetNext( is, sym );
	if  ( sym < 0 )
	    { break;	}
	if  ( sym > UCD_Blocks[block].ubLast )
	    { break;	}
	}

    return sym;
    }

static int appSymbolPickerPrevPageOffset(	SymbolPicker *	sp )
    {
    const IndexSet *	is;
    int			sym;
    int			step;
    int			block;

    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); return -1;	}

    is= &(sp->spFontInfo->afiUnicodesProvided);

    sym= utilIndexSetGetPrev( is, sp->spSymbolOffset );
    if  ( sym < 0 )
	{ return -1;	}
    block= ucdGetBlock( sym );

    for ( step= 0; step < sp->spCellsHigh* sp->spCellsWide; step++ )
	{
	if  ( sym < UCD_Blocks[block].ubFirst )
	    { break;	}

	sym= utilIndexSetGetPrev( is, sym );
	if  ( sym < 0 )
	    { break;	}
	}

    if  ( sym >= 0 || block == 0 )
	{
	sym= utilIndexSetGetNext( is, sym );
	if  ( sym < 0 )
	    { LDEB(sym); return -1;	}
	}

    return sym;
    }

/************************************************************************/

static int appSymbolSetBlockOffset(	SymbolPicker *	sp,
					int		sym )
    {
    sp->spSymbolOffset= sym;
    guiExposeDrawingWidget( sp->spSymbolDrawing );

    guiEnableWidget( sp->spNextPageButton,
			    appSymbolPickerNextPageOffset( sp ) >= 0 );
    guiEnableWidget( sp->spPrevPageButton,
			    appSymbolPickerPrevPageOffset( sp ) >= 0 );

    return ucdGetBlock( sp->spSymbolOffset );
    }

static APP_BUTTON_CALLBACK_H( appSymbolPickerNextPage, w, voidsp )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;
    int			sym;
    int			block;

    sym= appSymbolPickerNextPageOffset( sp );
    if  ( sym < 0 )
	{ LDEB(sym); return;	}

    block= appSymbolSetBlockOffset( sp, sym );
    appSetOptionmenu( &(sp->spBlockOptionmenu), block );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolPickerPrevPage, w, voidsp )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;
    int			sym;
    int			block;

    sym= appSymbolPickerPrevPageOffset( sp );
    if  ( sym < 0 )
	{ LDEB(sym); return;	}

    block= appSymbolSetBlockOffset( sp, sym );
    appSetOptionmenu( &(sp->spBlockOptionmenu), block );

    return;
    }

/************************************************************************/

static void appSymbolPickerClear(	SymbolPicker *	sp )
    {
    int		sym;
    int		row;
    int		col;

    sym= sp->spSymbolSelected;
    if  ( sym >= 0 && ! appSymbolGetRowCol( &row, &col, sp, sym ) )
	{
	DocumentRectangle	drSym;

	appSymbolRectangle( &drSym, sp, row, col );
	guiExposeDrawingWidgetRectangle( sp->spSymbolDrawing, &drSym );
	}

    utilCleanMemoryBuffer( &(sp->spCollectedString) );
    utilInitMemoryBuffer( &(sp->spCollectedString) );
    sp->spPreviousSize= 0;

    sp->spSymbolSelected= -1;

    guiEnableWidget( sp->spClearButton, 0 );
    guiEnableWidget( sp->spInsertButton, 0 );
    }

/************************************************************************/
/*									*/
/*  Adapt the symbol Picker to the current font.			*/
/*									*/
/*  9)  Also remember that no symbol has been selected yet, and that	*/
/*	for that reason, no symbol can be inserted.			*/
/*									*/
/************************************************************************/

static int appSymbolAdaptDrawingToFont(		SymbolPicker *	sp )
    {
    DrawingSurface		ds= sp->spDrawingSurface;
    DocumentFontList *		dfl= &(sp->spDocumentFontList);
    TextAttribute *		ta= &(sp->spTextAttribute);
    int				pixelSize;
    int				screenFont;
    const IndexSet *		unicodesWanted;

    const PostScriptFontList *	psfl= sp->spPostScriptFontList;

    if  ( sp->spFontFamilyChosen < 0 )
	{ LDEB(sp->spFontFamilyChosen); return -1;	}

    if  ( sp->spScreenFont >= 0 )
	{ sp->spScreenFont= -1;	}

    sp->spFontInfo= appGetFontInfoForAttribute( &unicodesWanted,
							    ta, dfl, psfl );
    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); return -1;	}

    pixelSize= textGetPixelSize( sp->spPixelsPerTwip, ta );
    screenFont= drawOpenScreenFont( ds, sp->spFontInfo,
						pixelSize, unicodesWanted );
    if  ( screenFont < 0 )
	{ LDEB(screenFont);	}
    else{
	sp->spScreenFont= screenFont;
	}

    guiExposeDrawingWidget( sp->spSymbolDrawing );

    sp->spSymbolSelected= -1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find out what unicode blocks apply for the current font.		*/
/*									*/
/************************************************************************/

static void appSymbolAdaptBlockMenuToFont(	SymbolPicker *	sp )
    {
    const IndexSet *	is;
    int			block;
    int			sym;

    int			fontBlock= -1;

    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); return;	}

    is= &(sp->spFontInfo->afiUnicodesProvided);

    block= 0; sym= -1;
    while( block < sp->spUnicodeBlockOptCount )
	{
	sym= utilIndexSetGetNext( is, sym );
	if  ( sym < 0 )
	    { break;	}

	while( block < sp->spUnicodeBlockOptCount	&&
	       UCD_Blocks[block].ubLast < sym		)
	    {
	    guiEnableWidget( sp->spUnicodeBlockOpts[block], 0 );
	    appGuiSetWidgetVisible( sp->spUnicodeBlockOpts[block], 0 );
	    block++;
	    }

	if  ( block >= sp->spUnicodeBlockOptCount )
	    { break;	}

	guiEnableWidget( sp->spUnicodeBlockOpts[block], 1 );
	appGuiSetWidgetVisible( sp->spUnicodeBlockOpts[block], 1 );
	sym= UCD_Blocks[block].ubLast;

	if  ( UCD_Blocks[block].ubFirst <= sp->spSymbolOffset )
	    { fontBlock= block;	}
	else{
	    if  ( fontBlock < 0 )
		{ fontBlock= block;	}
	    }

	block++;
	}

    while( block < sp->spUnicodeBlockOptCount )
	{
	guiEnableWidget( sp->spUnicodeBlockOpts[block], 0 );
	appGuiSetWidgetVisible( sp->spUnicodeBlockOpts[block], 0 );
	block++;
	}

    appSetOptionmenu( &(sp->spBlockOptionmenu), fontBlock );
    if  ( fontBlock >= 0 )
	{
	if  ( sp->spSymbolOffset < UCD_Blocks[fontBlock].ubFirst	||
	      sp->spSymbolOffset > UCD_Blocks[fontBlock].ubLast	)
	    { appSymbolSetBlockOffset( sp, UCD_Blocks[fontBlock].ubFirst ); }
	else{
	    /* To enable/disable buttons */
	    appSymbolSetBlockOffset( sp, sp->spSymbolOffset );
	    }
	}

    appOptionmenuRefreshWidth( &(sp->spBlockOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  A font family has been selected: Adapt the rest of the picker to it.*/
/*									*/
/************************************************************************/

static int appSymbolAdaptToFamily(  SymbolPicker *	sp )
    {
    if  ( sp->spFontFamilyChosen < 0 )
	{ LDEB(sp->spFontFamilyChosen); return -1;	}

    if  ( appSymbolAdaptDrawingToFont( sp ) )
	{ LDEB(sp->spFontFamilyChosen); return -1;	}

    appSymbolPickerClear( sp );

    appSymbolAdaptBlockMenuToFont( sp );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Force the string to be redrawn.					*/
/*									*/
/************************************************************************/

static void appSymbolPickerExposeString(	SymbolPicker *	sp )
    {
    DocumentRectangle		drExpose;

    appSymbolStringRectangle( &drExpose, sp );
    guiExposeDrawingWidgetRectangle( sp->spSymbolDrawing, &drExpose );

    return;
    }

/************************************************************************/
/*									*/
/*  A symbol picker must be destroyed.					*/
/*									*/
/************************************************************************/

void appCleanSymbolPicker(	SymbolPicker *	sp )
    {
    docCleanFontList( &(sp->spDocumentFontList) );

    if  ( sp->spDrawingSurface )
	{ drawFreeDrawingSurface( sp->spDrawingSurface );	}

    if  ( sp->spFontFamilyOpts )
	{ free( sp->spFontFamilyOpts );	}
    if  ( sp->spUnicodeBlockOpts )
	{ free( sp->spUnicodeBlockOpts );	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Insert' button has been pushed.					*/
/*									*/
/************************************************************************/

static void appSymbolPickerInsert(	SymbolPicker *	sp )
    {
    PropertyMask		setMask;

    const unsigned char *	bytes;
    int				size= 0;

    utilPropMaskClear( &setMask );

    if  ( sp->spSymbolSelected < 0 || ! sp->spInsert )
	{ LXDEB(sp->spSymbolSelected,sp->spInsert); return;	}

    if  ( sp->spFontFamilyChosen >= 0 )
	{ PROPmaskADD( &setMask, TApropFONT_NUMBER );	}

    bytes= utilMemoryBufferGetBytes( &size, &(sp->spCollectedString) );
    if  ( ! bytes || size == 0 )
	{ LXDEB(size,bytes);	}
    else{
	(*sp->spInsert)( sp->spTarget, (char *)bytes, size,
					&(sp->spTextAttribute), &setMask );
	}

    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolInsertPushed, w, voidsp )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;

    appSymbolPickerInsert( sp );
    appSymbolPickerClear( sp );
    appSymbolPickerExposeString( sp );
    
    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolClearPushed, w, voidsp )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;

    appSymbolPickerClear( sp );
    appSymbolPickerExposeString( sp );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt to resize event.						*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appSymbolPickerConfigure, w, voidsp, event )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;
    /*
    int			wide;
    int			high;

    if  ( guiDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}
    */

    sp->spCellsWide= 0;
    sp->spCellsHigh= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the symbol part of the symbol picker.			*/
/*									*/
/************************************************************************/

static void appSymbolDrawSymbol(	SymbolPicker *		sp,
					int			sym,
					int			x0,
					int			y0 )
    {
    DrawingSurface		ds= sp->spDrawingSurface;

    int				x= 0;
    int				y= 0;

    DocumentRectangle		drCell;
    DocumentRectangle		drText;

    drCell.drX0= x0;
    drCell.drX1= x0+ sp->spCellSizePixels- 1;
    drCell.drY0= y0;
    drCell.drY1= y0+ sp->spCellSizePixels- 1;

    x= y= 0;
    drawGetSymbolExtents( &drText, ds, x, y, sp->spScreenFont, sym );

    y= ( drCell.drY1+ drCell.drY0 )/ 2- ( drText.drY1+ drText.drY0+ 1 )/2;
    x= ( drCell.drX1+ drCell.drX0 )/ 2- ( drText.drX1+ drText.drX0+ 1 )/2;

    drawSymbol( ds, x, y, sp->spScreenFont, sym );

    return;
    }

static void appSymbolRedrawSymbols(	SymbolPicker *			sp,
					const DocumentRectangle *	drClip )
    {
    const IndexSet *	is;
    int			sym;

    int			row;
    int			col;

    if  ( ! sp->spFontInfo )
	{ XDEB(sp->spFontInfo); return; }

    is= &(sp->spFontInfo->afiUnicodesProvided);
    sym= sp->spSymbolOffset- 1;

    for ( row= 0; row < sp->spCellsHigh; row++ )
	{
	for ( col= 0; col < sp->spCellsWide; col++ )
	    {
	    DocumentRectangle	drSym;
	    DocumentRectangle	drIgn;

	    appSymbolRectangle( &drSym, sp, row, col );

	    sym= utilIndexSetGetNext( is, sym );
	    if  ( sym < 0 )
		{ break;	}

	    if  ( ! geoIntersectRectangle( &drIgn, &drSym, drClip )	||
		  sym == sp->spSymbolSelected				)
		{ continue;	}

	    appSymbolDrawSymbol( sp, sym, drSym.drX0, drSym.drY0 );
	    }

	if  ( sym < 0 )
	    { break;	}
	}

    return;
    }

static void appSymbolRedrawGrid(	SymbolPicker *			sp,
					const DocumentRectangle *	drClip )
    {
    DrawingSurface			ds= sp->spDrawingSurface;

    DocumentRectangle			drSym;
    DocumentRectangle			drIgn;

    int					col;
    int					row;
    int					sym;

    int					rowSelected= -1;
    int					colSelected= -1;

    RGB8Color				disabledColor;

    DocumentRectangle			drLine;

    disabledColor.rgb8Red= 127;
    disabledColor.rgb8Green= 127;
    disabledColor.rgb8Blue= 127;
    disabledColor.rgb8Alpha= 255;

    sym= sp->spSymbolSelected;
    if  ( sym >= 0 && ! appSymbolGetRowCol( &row, &col, sp, sym ) )
	{
	rowSelected= row;
	colSelected= col;
	}

    if  ( sp->spEnabled )
	{ drawSetForegroundColorBlack( ds );		}
    else{ drawSetForegroundColor( ds, &disabledColor );	}

    drLine.drY0= 0;
    drLine.drY1= sp->spCellsHigh* ( sp->spCellSizePixels+ 1 );

    for ( col= 0; col <= sp->spCellsWide; col++ )
	{
	drLine.drX0= drLine.drX1= col* ( sp->spCellSizePixels+ 1 );
	drawFillRectangle( ds, &drLine );
	}

    drLine.drX0= 0;
    drLine.drX1= sp->spCellsWide* ( sp->spCellSizePixels+ 1 );

    for ( row= 0; row <= sp->spCellsHigh; row++ )
	{
	drLine.drY0= drLine.drY1= row* ( sp->spCellSizePixels+ 1 );
	drawFillRectangle( ds, &drLine );
	}

    drawSetForegroundColorWhite( ds );

    for ( row= 0; row < sp->spCellsHigh; row++ )
	{
	for ( col= 0; col < sp->spCellsWide; col++ )
	    {
	    appSymbolRectangle( &drSym, sp, row, col );

	    if  ( ! geoIntersectRectangle( &drSym, &drSym, drClip )	||
		  ( row == rowSelected && col == colSelected )		)
		{ continue;	}

	    drawFillRectangle( ds, &drSym );
	    }
	}

    if  ( sp->spEnabled )
	{ drawSetForegroundColorBlack( ds );		}
    else{ drawSetForegroundColor( ds, &disabledColor );	}

    appSymbolRedrawSymbols( sp, drClip );

    sym= sp->spSymbolSelected;
    if  ( sym >= 0 && rowSelected >= 0 && colSelected >= 0 )
	{
	appSymbolRectangle( &drSym, sp, rowSelected, colSelected );

	if  ( geoIntersectRectangle( &drIgn, &drSym, drClip ) )
	    {
	    if  ( sp->spEnabled )
		{ drawSetForegroundColorBlack( ds );		}
	    else{ drawSetForegroundColor( ds, &disabledColor );	}

	    drawFillRectangle( ds, &drSym );

	    drawSetForegroundColorWhite( ds );

	    appSymbolDrawSymbol( sp, sym, drSym.drX0, drSym.drY0 );
	    }
	}

    appSymbolStringRectangle( &drSym, sp );
    if  ( geoIntersectRectangle( &drIgn, &drSym, drClip ) )
	{
	const unsigned char *	bytes;
	int			size;

	bytes= utilMemoryBufferGetBytes( &size, &(sp->spCollectedString) );
	if  ( bytes && size > 0 )
	    {
	    int		x, y;

	    if  ( sp->spEnabled )
		{ drawSetForegroundColorBlack( ds );		}
	    else{ drawSetForegroundColor( ds, &disabledColor );	}

	    appSymbolGetStringOrigin( &x, &y, &drSym, sp );

	    drawString( ds, x, y, sp->spScreenFont, (char *)bytes, size );
	    }
	}

    return;
    }

static APP_EVENT_HANDLER_H( appSymbolRedraw, w, voidsp, exposeEvent )
    {
    SymbolPicker *			sp= (SymbolPicker *)voidsp;
    DrawingSurface			ds= sp->spDrawingSurface;

    DocumentRectangle			drClip;

    if  ( sp->spCellsWide == 0 || sp->spCellsHigh == 0 )
	{
	int		pixelsWide;
	int		pixelsHigh;

	guiDrawGetSizeOfWidget( &pixelsWide, &pixelsHigh, w );

	appSymbolPickerCalculateGridSize( sp, pixelsWide, pixelsHigh );
	}

    if  ( sp->spScreenFont < 0 )
	{ LDEB(sp->spScreenFont); return;	}

    guiCollectExposures( &drClip, sp->spSymbolDrawing, exposeEvent );

    drawSetClipRect( ds, &drClip );

    drawSetForegroundColor( ds, &(sp->spBackgroundColor) );
    drawFillRectangle( ds, &drClip );
    appSymbolRedrawGrid( sp, &drClip );

    drawNoClipping( ds );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appSymbolMousePress, w, voidsp, downEvent )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;

    int			x;
    int			y;
    int			row;
    int			col;
    int			sym;
    int			sameAsPrevious= 0;

    int			oldRow= -1;
    int			oldCol= -1;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    DocumentRectangle	drExpose;

    if  ( ! sp->spEnabled )
	{ return;	}

    if  ( sp->spSymbolSelected >= 0 )
	{ appSymbolGetRowCol( &oldRow, &oldCol, sp, sp->spSymbolSelected ); }

    if  ( guiGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 )
	{ return;	}

    row= y/ ( sp->spCellSizePixels+ 1 );
    col= x/ ( sp->spCellSizePixels+ 1 );
    sym= appSymbolGetSymbol( sp, row, col );

    if  ( sym >= 0 )
	{
	char	scratch[10];
	int	step;

	sameAsPrevious= sp->spSymbolSelected == sym;
	sp->spSymbolSelected= sym;

	if  ( seq == 1 || ! sameAsPrevious )
	    {
	    step= uniPutUtf8( scratch, sp->spSymbolSelected );
	    if  ( step < 1 )
		{ LDEB(step);	}
	    else{
		sp->spPreviousSize= sp->spCollectedString.mbSize;

		if  ( utilMemoryBufferAppendBytes( &(sp->spCollectedString),
					    (unsigned char *)scratch, step ) )
		    { LDEB(step);	}

		guiEnableWidget( sp->spClearButton, sp->spEnabled );
		guiEnableWidget( sp->spInsertButton, sp->spEnabled );
		}
	    }
	}

    if  ( oldRow >= 0 && oldCol >= 0 )
	{
	appSymbolRectangle( &drExpose, sp, oldRow, oldCol );
	guiExposeDrawingWidgetRectangle( sp->spSymbolDrawing, &drExpose );
	}

    appSymbolRectangle( &drExpose, sp, row, col );
    guiExposeDrawingWidgetRectangle( sp->spSymbolDrawing, &drExpose );

    appSymbolPickerExposeString( sp );

    if  ( sameAsPrevious && seq == 2 )
	{
#	ifdef USE_GTK
	/* Silly GTK sends 3 events for a double click and we can only
	 * know in the 3rd one that the second one was superfluous
	 */
	utilMemoryBufferSetSize( &(sp->spCollectedString), sp->spPreviousSize );
#	endif

	appSymbolPickerInsert( sp );
	appSymbolPickerClear( sp );
	appSymbolPickerExposeString( sp );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  A font family was selected by the user.				*/
/*  2)  A unicode block was selected by the user.			*/
/*									*/
/************************************************************************/

/*  1  */
static void appSymbolFontFamilyChosen(	int		sortIndex,
					void *		voidsp )
    {
    SymbolPicker *	sp= (SymbolPicker *)voidsp;
    DocumentFontList *	dfl= &(sp->spDocumentFontList);
    TextAttribute *	ta= &(sp->spTextAttribute);

    int			changed= 0;
    int			fontNumber;

    if  ( sortIndex < 0 || sortIndex >= dfl->dflFontCount )
	{ LLDEB(sortIndex,dfl->dflFontCount); return;	}

    fontNumber= utilDocumentFontListGetArrayIndex( dfl, sortIndex );

    if  ( ta->taFontNumber != fontNumber )
	{ ta->taFontNumber= fontNumber; changed= 1;	}

    if  ( changed )
	{
	sp->spFontFamilyChosen= fontNumber;

	if  ( appSymbolAdaptToFamily( sp ) )
	    { LDEB(fontNumber);	}
	}

    return;
    }

/*  1  */
static void appSymbolUnicodeBlockChosen(	int		block,
						void *		voidsp )
    {
    SymbolPicker *		sp= (SymbolPicker *)voidsp;

    if  ( block < 0 || block >= sp->spUnicodeBlockOptCount )
	{ LLDEB(block,sp->spUnicodeBlockOptCount); return;	}

    if  ( sp->spSymbolOffset != UCD_Blocks[block].ubFirst )
	{ appSymbolSetBlockOffset( sp, UCD_Blocks[block].ubFirst );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the font/encoding part of the symbol picker.			*/
/*									*/
/************************************************************************/

static void appSymbolMakeFontPart(	APP_WIDGET			parent,
					const SymbolPickerResources *	spr,
					SymbolPicker *			sp )
    {
    appMakeColumnFrameInColumn( &(sp->spFontFrame), &(sp->spFontPaned),
							parent, spr->sprFont );

    appMakeOptionmenuInColumn( &(sp->spFontOptionmenu),
					    sp->spFontPaned,
					    appSymbolFontFamilyChosen, sp );
    appMakeOptionmenuInColumn( &(sp->spBlockOptionmenu),
					    sp->spFontPaned,
					    appSymbolUnicodeBlockChosen, sp );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the symbol part of the symbol picker.				*/
/*									*/
/************************************************************************/

static void appSymbolMakeSymbolPart(
				APP_WIDGET		parent,
				SymbolPicker *		sp )
    {
    int				wide;
    int				high;

    const int			twipsSize= 20* FONT_SIZE;
    const int			heightResizable= 1;

    sp->spCellSizePixels= 1.5* ( sp->spPixelsPerTwip* twipsSize )+ 0.5;

    appSymbolPickerGetDimensions( &wide, &high, sp,
					MIN_CELLS_WIDE, MIN_CELLS_HIGH );

    appGuiMakeDrawingAreaInColumn( &(sp->spSymbolDrawing), parent,
		wide, high, heightResizable, appSymbolRedraw, (void *)sp );

    guiDrawSetButtonPressHandler( sp->spSymbolDrawing,
				    appSymbolMousePress, (void *)sp );

    guiDrawSetConfigureHandler( sp->spSymbolDrawing,
				    appSymbolPickerConfigure, (void *)sp );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the list of unicode blocks.					*/
/*									*/
/************************************************************************/

static int appSymbolFillBlockMenu(	SymbolPicker *		sp )
    {
    int		block;

    sp->spUnicodeBlockOpts= malloc( UCD_BlockCount* sizeof(APP_WIDGET) );
    if  ( ! sp->spUnicodeBlockOpts )
	{ LXDEB(UCD_BlockCount,sp->spUnicodeBlockOpts); return -1;	}

    for ( block= 0; block < UCD_BlockCount; block++ )
	{ sp->spUnicodeBlockOpts[block]= (APP_WIDGET)0;	}

    for ( block= 0; block < UCD_BlockCount; block++ )
	{
	if  ( UCD_Blocks[block].ubLast > 0xffff )
	    { break;	}

	sp->spUnicodeBlockOpts[block]= appAddItemToOptionmenu(
				&(sp->spBlockOptionmenu), 
				UCD_Blocks[block].ubDescription );
	}

    sp->spUnicodeBlockOptCount= block;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the list of font families.					*/
/*									*/
/************************************************************************/

/*  1  */
static void appSymbolFillFontMenu(
				int *				pEnabled,
				const SymbolPickerResources *	spr,
				SymbolPicker *		sp )
    {
    int			fam;
    DocumentFontList *	dfl= &(sp->spDocumentFontList);
    int			fontsAdded= 0;

    appEmptyOptionmenu( &(sp->spFontOptionmenu) );

    if  ( sp->spFontFamilyOptCount < dfl->dflFontCount+ 1 )
	{
	APP_WIDGET *	opts;

	opts= realloc( sp->spFontFamilyOpts,
				(dfl->dflFontCount+ 1)* sizeof(APP_WIDGET) );
	if  ( ! opts )
	    { XDEB(opts); return;	}

	sp->spFontFamilyOpts= opts;
	sp->spFontFamilyOptCount= dfl->dflFontCount;
	}

    for ( fam= 0; fam < dfl->dflFontCount; fam++ )
	{
	const DocumentFont *	df;

	df= utilDocumentFontListGetFontBySortIndex( dfl, fam );
	if  ( ! df )
	    { continue;	}

	sp->spFontFamilyOpts[fam]= appAddItemToOptionmenu(
				    &(sp->spFontOptionmenu), df->dfName );
	fontsAdded++;
	}

    if  ( fontsAdded == 0 )
	{
	sp->spFontFamilyOpts[0]= appAddItemToOptionmenu(
				&(sp->spFontOptionmenu), spr->sprNone );
	}

    *pEnabled= fontsAdded;

    sp->spFontFamilyChosen= 0;
    sp->spFontInfo= (const AfmFontInfo *)0;
    appSetOptionmenu( &(sp->spFontOptionmenu), 0 );

    appOptionmenuRefreshWidth( &(sp->spFontOptionmenu) );

    return;
    }


/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void appInitSymbolPicker(	SymbolPicker *	sp )
    {
    sp->spApplication= (EditApplication *)0;
    sp->spInspector= (AppInspector *)0;

    sp->spCurrentDocumentId= -1;

    sp->spResources= (SymbolPickerResources *)0;

    appInitOptionmenu( &(sp->spFontOptionmenu) );
    appInitOptionmenu( &(sp->spBlockOptionmenu) );

    sp->spFontFrame= (APP_WIDGET)0;
    sp->spFontPaned= (APP_WIDGET)0;
    sp->spSymbolDrawing= (APP_WIDGET)0;

    sp->spNextPageButton= (APP_WIDGET)0;
    sp->spPrevPageButton= (APP_WIDGET)0;
    sp->spClearButton= (APP_WIDGET)0;
    sp->spInsertButton= (APP_WIDGET)0;

    sp->spInsert= (SymbolPickerInsert)0;
    sp->spTarget= (void *)0;

    sp->spSymbolSelected= -1;
    sp->spSymbolOffset= 0;

    docInitFontList( &(sp->spDocumentFontList) );
    utilInitTextAttribute( &(sp->spTextAttribute) );
    sp->spFontFamilyChosen= -1;
    sp->spFontInfo= (AfmFontInfo *)0;

    sp->spPixelsPerTwip= 0;
    sp->spDrawingSurface= (DrawingSurface)0;
    sp->spScreenFont= -1;
    sp->spCellSizePixels= 0;
    sp->spCellsWide= 0;
    sp->spCellsHigh= 0;

    sp->spFontFamilyOpts= (APP_WIDGET *)0;
    sp->spFontFamilyOptCount= 0;
    sp->spUnicodeBlockOpts= (APP_WIDGET *)0;
    sp->spUnicodeBlockOptCount= 0;

    utilInitMemoryBuffer( &(sp->spCollectedString) );
    sp->spPreviousSize= 0;
    }

/************************************************************************/
/*									*/
/*  Make a Symbol Picker Tool.						*/
/*									*/
/************************************************************************/

void appFillSymbolPicker(	SymbolPicker *			sp,
				const SymbolPickerResources *	spr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    EditApplication *		ea= sp->spApplication;

    sp->spResources= spr;
    sp->spPostScriptFontList= &(ea->eaPostScriptFontList);
    sp->spEnabled= 1;

    utilInitTextAttribute( &(sp->spTextAttribute) );
    appSymbolMakeFontPart( pageWidget, spr, sp );

    sp->spPixelsPerTwip= ea->eaMagnification* ea->eaPixelsPerTwip;

    appSymbolMakeSymbolPart( pageWidget, sp );

    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
			    &(is->isPrevButton), &(is->isNextButton),
			    isr->isrPrevButtonText, isr->isrNextButtonText,
			    appSymbolPickerPrevPage, appSymbolPickerNextPage,
			    (void *)sp );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
			    &(is->isRevertButton), &(is->isApplyButton),
			    isr->isrRevert, isr->isrApplyToSubject,
			    appSymbolClearPushed, appSymbolInsertPushed,
			    (void *)sp );

    sp->spNextPageButton= is->isNextButton;
    sp->spPrevPageButton= is->isPrevButton;
    sp->spClearButton= is->isRevertButton;
    sp->spInsertButton= is->isApplyButton;

    guiEnableWidget( sp->spClearButton, 0 );
    guiEnableWidget( sp->spInsertButton, 0 );

    return;
    }

void appSymbolPickerFillChoosers(	SymbolPicker *			sp,
					const SymbolPickerResources *	spr )
    {
    int		enabled= 1;

    appSymbolFillFontMenu( &enabled, spr, sp );
    appSymbolFillBlockMenu( sp );

    return;
    }

void appFinishSymbolPicker(		SymbolPicker *			sp,
					const SymbolPickerResources *	spr )
    {
    appOptionmenuRefreshWidth( &(sp->spFontOptionmenu) );
    appOptionmenuRefreshWidth( &(sp->spBlockOptionmenu) );

    sp->spDrawingSurface= guiDrawingSurfaceForNativeWidget(
			    sp->spSymbolDrawing,
			    sp->spPostScriptFontList->psflAvoidFontconfig );

    guiGetBackgroundColor( &(sp->spBackgroundColor), sp->spSymbolDrawing );
    }

/************************************************************************/
/*									*/
/*  Adapt the symbol Picker to the current font family.			*/
/*									*/
/************************************************************************/

int appAdaptSymbolPickerToFontFamily(
				void *				voidsp,
				unsigned int			documentId,
				const DocumentFontList *	dflFrom,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    SymbolPicker *		sp= (SymbolPicker *)voidsp;
    DocumentFontList *		dfl= &(sp->spDocumentFontList);
    int				rval= 0;
    int				familyIndex= sp->spFontFamilyChosen;

    PropertyMask		doneMask;

    if  ( sp->spCurrentDocumentId != documentId )
	{
	int		enabled= 1;

	if  ( docCopyFontList( dfl, dflFrom ) )
	    { LDEB(1); return -1; }

	appSymbolFillFontMenu( &enabled, sp->spResources, sp );

	sp->spCurrentDocumentId= documentId;
	familyIndex= -1;
	}

    utilPropMaskClear( &doneMask );
    utilUpdateTextAttribute( &doneMask, &(sp->spTextAttribute),
							    taSetMask, taSet );

    sp->spTextAttribute.taFontSizeHalfPoints= 2* FONT_SIZE;
    sp->spTextAttribute.taFontIsBold= 0;
    sp->spTextAttribute.taFontIsSlanted= 0;

    if  ( sp->spTextAttribute.taFontNumber >= 0 )
	{
	const DocumentFont *	df= (const DocumentFont *)0;

	df= docFontListGetFontByNumber( dfl,
				    sp->spTextAttribute.taFontNumber );
	if  ( df )
	    { sp->spFontFamilyChosen= df->dfDocFontNumber;	}
	}
    else{
	sp->spTextAttribute.taFontNumber= 0;
	}

    if  ( sp->spFontFamilyChosen < 0 )
	{
	sp->spFontFamilyChosen= 0; 
	sp->spTextAttribute.taFontNumber= 0;
	}

    if  ( sp->spFontFamilyChosen != familyIndex )
	{
	int	sortIndex;
	if  ( appSymbolAdaptToFamily( sp ) )
	    { LDEB(sp->spFontFamilyChosen); rval= -1;	}

	sortIndex= utilDocumentFontListGetSortIndex( dfl,
					    sp->spFontFamilyChosen );

	appSetOptionmenu( &(sp->spFontOptionmenu), sortIndex );
	}

    return rval;
    }

void appEnableSymbolPicker(	SymbolPicker *		sp,
				int			enabled )
    {
    if  ( sp->spEnabled != enabled )
	{
	sp->spEnabled= enabled;
	guiExposeDrawingWidget( sp->spSymbolDrawing );

	guiEnableWidget( sp->spClearButton, sp->spEnabled );
	guiEnableWidget( sp->spInsertButton, sp->spEnabled );

	appGuiEnableOptionmenu( &(sp->spFontOptionmenu), sp->spEnabled );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Resources for the Symbol Picker.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_SymbolSubjectResourceTable[]=
    {
    APP_RESOURCE( "symbolPickerSymbol",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Symbol" ),
    APP_RESOURCE( "symbolPickerInsert",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Insert" ),
    APP_RESOURCE( "symbolPickerClear",
		offsetof(InspectorSubjectResources,isrRevert),
		"Clear" ),
    APP_RESOURCE( "symbolPickerNextPage",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Forward" ),
    APP_RESOURCE( "symbolPickerPrevPage",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Backward" ),
    };

static AppConfigurableResource APP_SymbolPickerResourceTable[]=
    {
    APP_RESOURCE( "symbolPickerFont",
		offsetof(SymbolPickerResources,sprFont),
		"Font" ),
    APP_RESOURCE( "symbolPickerBlock",
		offsetof(SymbolPickerResources,sprBlock),
		"Block" ),
    APP_RESOURCE( "symbolPickerNone",
		offsetof(SymbolPickerResources,sprNone),
		"None" ),
    };

void appSymbolPickerGetResourceTable(	EditApplication *		ea,
					SymbolPickerResources *		spr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)spr,
					APP_SymbolPickerResourceTable,
					sizeof(APP_SymbolPickerResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_SymbolSubjectResourceTable,
				sizeof(APP_SymbolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
