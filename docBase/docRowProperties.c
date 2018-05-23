/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docRowProperties.h"
#   include	"docPropVal.h"

/************************************************************************/

static const int DocRowIntProperties[]=
{
    RPpropGAP_WIDTH,
    RPpropLEFT_INDENT,
    RPpropHEIGHT,
    RPpropALIGNMENT,
    RPpropIS_TABLE_HEADER,
    RPpropKEEP_ON_ONE_PAGE,
    RPprop_KEEPFOLLOW,
    RPpropAUTOFIT,
    RPpropRTOL,
    RPpropTRW_WIDTH,
    RPpropTRFTS_WIDTH,
    RPpropTRSPDL,
    RPpropTRSPDR,
    RPpropTRSPDT,
    RPpropTRSPDB,
    RPpropTRSPDFL,
    RPpropTRSPDFR,
    RPpropTRSPDFT,
    RPpropTRSPDFB,
    RPpropTRPADDL,
    RPpropTRPADDR,
    RPpropTRPADDT,
    RPpropTRPADDB,
    RPpropTRPADDFL,
    RPpropTRPADDFR,
    RPpropTRPADDFT,
    RPpropTRPADDFB,
    RPpropTRW_WIDTHB,
    RPpropTRW_WIDTHA,
    RPpropTRFTS_WIDTHB,
    RPpropTRFTS_WIDTHA,
};

static const int DocRowIntPropertyCount=
				sizeof(DocRowIntProperties)/sizeof(int);

static const int DocRowBorderProperties[]=
{
    RPpropTOP_BORDER,
    RPpropBOTTOM_BORDER,
    RPpropLEFT_BORDER,
    RPpropRIGHT_BORDER,
    RPpropHORIZ_BORDER,
    RPpropVERT_BORDER,
};

static const int DocRowBorderPropertyCount=
				sizeof(DocRowBorderProperties)/sizeof(int);

/************************************************************************/
/*									*/
/*  Initialise row properties.						*/
/*									*/
/************************************************************************/

void docCleanRowProperties(	RowProperties *	rp )
    {
    if  ( rp->rpCells )
	{ free( rp->rpCells );	}
    }

void docInitRowProperties(	RowProperties *	rp )
    {
    rp->rpCellCount= 0;
    rp->rpCells= (CellProperties *)0;

    rp->rpHalfGapWidthTwips= 0;
    rp->rpHeightTwips= 0;
    rp->rpLeftIndentTwips= 0;

    rp->rpIsTableHeader= 0;
    rp->rpKeepOnOnePage= 0;
    rp->rp_Keepfollow= 0;
    rp->rpAutofit= 0;
    rp->rpRToL= 0;

    rp->rpTopBorderNumber= 0;
    rp->rpBottomBorderNumber= 0;
    rp->rpLeftBorderNumber= 0;
    rp->rpRightBorderNumber= 0;
    rp->rpHorizontalBorderNumber= 0;
    rp->rpVerticalBorderNumber= 0;

    rp->rpShadingNumber= 0;
    rp->rpFrameNumber= 0;

    rp->rpAlignment= DOCthaLEFT;

    /**/
    rp->rpPreferredWidth= 0;
    rp->rpPreferredWidthUnit= TRautoNONE;

    /**/
    rp->rpLeftDefaultCellSpacing= 0;
    rp->rpRightDefaultCellSpacing= 0;
    rp->rpTopDefaultCellSpacing= 0;
    rp->rpBottomDefaultCellSpacing= 0;

    rp->rpLeftDefaultCellSpacingUnit= TRautoNONE;
    rp->rpRightDefaultCellSpacingUnit= TRautoNONE;
    rp->rpTopDefaultCellSpacingUnit= TRautoNONE;
    rp->rpBottomDefaultCellSpacingUnit= TRautoNONE;

    /**/
    rp->rpLeftCellPadding= 0;
    rp->rpRightCellPadding= 0;
    rp->rpTopCellPadding= 0;
    rp->rpBottomCellPadding= 0;

    rp->rpLeftCellPaddingUnit= TRautoNONE;
    rp->rpRightCellPaddingUnit= TRautoNONE;
    rp->rpTopCellPaddingUnit= TRautoNONE;
    rp->rpBottomCellPaddingUnit= TRautoNONE;

    /**/
    rp->rpCellWidthBefore= 0;
    rp->rpCellWidthAfter= 0;

    rp->rpCellWidthBeforeUnit= TRautoNONE;
    rp->rpCellWidthAfterUnit= TRautoNONE;

    /**/
    rp->rpRowNumber= -1;
    rp->rpRowBandNumber= -1;
    rp->rpRowStyle= -1;

    rp->rpIsLastRow= 0;
    rp->rpAuthor= -1;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the description of a column into those for a row.		*/
/*									*/
/*  If we are asked to shift the columns to the right, shift them by	*/
/*  the width of the new column. I.E. Its new right hand position minus	*/
/*  the left hand position of the first cell that it will push to the	*/
/*  right. This value is not influenced by the operation.		*/
/*									*/
/************************************************************************/

int docInsertRowColumn(	RowProperties *			rp,
			int				col,
			int				shiftTail,
			const CellProperties *		cp,
			const DocumentAttributeMap *	dam )
    {
    int			i;
    CellProperties *	fresh;
    int			shiftTailBy= 0;

    fresh= (CellProperties *)realloc( rp->rpCells,
			(rp->rpCellCount+ 1)* sizeof(CellProperties) );
    if  ( ! fresh )
	{ LXDEB(rp->rpCellCount,fresh); return -1;	}
    rp->rpCells= fresh;

    if  ( col < 0 )
	{ col= rp->rpCellCount;	}

    if  ( col < rp->rpCellCount && shiftTail )
	{
	shiftTailBy= cp->cpRightBoundaryTwips- docColumnLeft( rp, col );
	}

    for ( i= rp->rpCellCount; i > col; i-- )
	{
	docCopyCellProperties( fresh+ i, fresh+ i- 1, dam );
	fresh[i].cpRightBoundaryTwips += shiftTailBy;
	}

    docInitCellProperties( fresh+ col );
    docCopyCellProperties( fresh+ col, cp, dam );

    rp->rpCellCount++;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a column wider (or narrower).					*/
/*									*/
/************************************************************************/

int docRowPropertiesMakeColWider(	RowProperties *		rp,
					int			col,
					int			wider )
    {
    int			i;
    CellProperties *	cp;

    if  ( col < 0 || col >= rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount); return -1;	}

    cp= rp->rpCells+ col;
    for ( i= col; i < rp->rpCellCount; cp++, i++ )
	{ cp->cpRightBoundaryTwips += wider;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the width of a range of columns. Space might have been stolen	*/
/*  from a victim column that has become narrower. Also adjust its	*/
/*  width.								*/
/*									*/
/************************************************************************/

void docRowPropertiesSetWidth(		RowProperties *		rp,
					int			col0,
					int			col1,
					int			wide,
					int			victim,
					int			victimWide )
    {
    CellProperties *	cp;
    int			col;

    int			ox0= rp->rpLeftIndentTwips;
    int			nx0= rp->rpLeftIndentTwips;

    cp= rp->rpCells;
    for ( col= 0; col < col0; cp++, col++ )
	{
	int		ox1= cp->cpRightBoundaryTwips;
	int		w= cp->cpRightBoundaryTwips- ox0;

	if  ( col == victim )
	    { w= victimWide;	}

	ox0= ox1;
	nx0= cp->cpRightBoundaryTwips= nx0+ w;
	}

    for ( col= col0; col <= col1; cp++, col++ )
	{
	int		ox1= cp->cpRightBoundaryTwips;

	ox0= ox1;
	nx0= cp->cpRightBoundaryTwips= nx0+ wide;
	}

    for ( col= col1+ 1; col < rp->rpCellCount; cp++, col++ )
	{
	int		ox1= cp->cpRightBoundaryTwips;
	int		w= cp->cpRightBoundaryTwips- ox0;

	if  ( col == victim )
	    { w= victimWide;	}

	ox0= ox1;
	nx0= cp->cpRightBoundaryTwips= nx0+ w;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the width of a column.					*/
/*  Calculate the left hand side coordinate.				*/
/*  Calculate the right hand side coordinate.				*/
/*									*/
/************************************************************************/

int docColumnWidth(		const RowProperties *	rp,
				int			col )
    {
    int		left;

    if  ( col < 0 || col >= rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount); return -1;	}

    if  ( col == 0 )
	{ left= rp->rpLeftIndentTwips;				}
    else{ left= rp->rpCells[col-1].cpRightBoundaryTwips;	}

    return rp->rpCells[col].cpRightBoundaryTwips- left;
    }

int docColumnLeft(		const RowProperties *	rp,
				int			col )
    {
    if  ( col == 0 )
	{ return rp->rpLeftIndentTwips;			}
    else{
	if  ( col <= 0 || col > rp->rpCellCount )
	    { LLDEB(col,rp->rpCellCount); return -1;	}

	return rp->rpCells[col-1].cpRightBoundaryTwips;
	}
    }

int docColumnRight(		const RowProperties *	rp,
				int			col )
    {
    if  ( col < 0 || col >= rp->rpCellCount )
	{ LLDEB(col,rp->rpCellCount); return -1;	}

    return rp->rpCells[col].cpRightBoundaryTwips;
    }

int docCellRight(	int *			pColspan,
			const RowProperties *	rp,
			int			col )
    {
    int				colspan= 1;
    const CellProperties *	cp= rp->rpCells+ col;
    int				right= cp->cpRightBoundaryTwips;

    if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	{
	int		c;

	for ( c= col+ 1; c < rp->rpCellCount; c++ )
	    {
	    if  ( rp->rpCells[c].cpHorizontalMerge != CELLmergeFOLLOW )
		{ break;	}

	    colspan++;
	    right= rp->rpCells[c].cpRightBoundaryTwips;
	    }
	}

    *pColspan= colspan;
    return right;
    }

/************************************************************************/
/*									*/
/*  Delete a number of columns from row properties.			*/
/*									*/
/************************************************************************/

int docDeleteColumnsFromRow(	RowProperties *		rp,
				int			col0,
				int			count,
				int			shiftTail )
    {
    int		col1= col0+ count- 1;
    int		shiftBy= 0;
    int		col;

    if  ( col0+ count > rp->rpCellCount )
	{ LLLDEB(col0,count,rp->rpCellCount); return -1;	}

    if  ( shiftTail )
	{
	int	left= docColumnLeft( rp, col0 );

	shiftBy= rp->rpCells[col1].cpRightBoundaryTwips- left;
	}

    rp->rpCellCount -= count;

    for ( col= col0; col < rp->rpCellCount; col++ )
	{
	docCleanCellProperties( &(rp->rpCells[col]) );
	rp->rpCells[col]= rp->rpCells[col+ count];
	rp->rpCells[col].cpRightBoundaryTwips -= shiftBy;
	}

    return 0;
    }

/************************************************************************/

void docRowMaskToCellMask(	PropertyMask *		cellMask,
				const PropertyMask *	rowMask )
    {
    utilPropMaskClear( cellMask );

    if  ( PROPmaskISSET( rowMask, RPpropCELL_PROPS ) )
	{ utilPropMaskFill( cellMask, CLprop_COUNT );	}

    if  ( PROPmaskISSET( rowMask, RPpropCELL_LAYOUT ) )
	{ PROPmaskADD( cellMask, CLpropCELLX );	}
    else{ PROPmaskUNSET( cellMask, CLpropCELLX );	}

    return;
    }

void docRowMaskApplyCellMask(	PropertyMask *		rowMask,
				const PropertyMask *	cellMask )
    {
    if  ( cellMask && ! utilPropMaskIsEmpty( cellMask ) )
	{
	PropertyMask	cellMaskCopy= *cellMask;

	if  ( PROPmaskISSET( &cellMaskCopy, CLpropCELLX ) )
	    {
	    PROPmaskADD( rowMask, RPpropCELL_LAYOUT );
	    PROPmaskUNSET( &cellMaskCopy, CLpropCELLX );
	    }

	if  ( ! utilPropMaskIsEmpty( &cellMaskCopy ) )
	    { PROPmaskADD( rowMask, RPpropCELL_PROPS );	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Compare row properties.						*/
/*									*/
/************************************************************************/

void docRowPropertyDifference(	PropertyMask *			pRpDifPask,
				const RowProperties *		rp1,
				const PropertyMask *		rpCmpMask,
				const RowProperties *		rp2,
				const DocumentAttributeMap *	dam )
    {
    int				p;
    PropertyMask		rpDifMask;

    utilPropMaskClear( &rpDifMask );

    if  ( PROPmaskISSET( rpCmpMask, RPpropCELL_LAYOUT )	||
	  PROPmaskISSET( rpCmpMask, RPpropCELL_PROPS )	)
	{
	int			col;
	const CellProperties *	cp1;
	const CellProperties *	cp2;

	if  ( rp1->rpCellCount != rp2->rpCellCount )
	    {
	    PROPmaskADD( &rpDifMask, RPpropCELL_LAYOUT );
	    PROPmaskADD( &rpDifMask, RPpropCELL_PROPS );
	    }

	cp1= rp1->rpCells;
	cp2= rp2->rpCells;
	for ( col= 0; col < rp1->rpCellCount && col < rp2->rpCellCount; cp1++, cp2++, col++ )
	    {
	    PropertyMask	cpCmpMask;
	    PropertyMask	cpDifMask;

	    docRowMaskToCellMask( &cpCmpMask, rpCmpMask );
	    utilPropMaskClear( &cpDifMask );

	    docCellPropertyDifference( &cpDifMask, cp1, &cpCmpMask, cp2, dam );
	    docRowMaskApplyCellMask( &rpDifMask, &cpDifMask );
	    }
	}

    for ( p= 0; p < DocRowIntPropertyCount; p++ )
	{
	int	prop= DocRowIntProperties[p];

	if  ( PROPmaskISSET( rpCmpMask, prop ) )
	    {
	    int		val1= docGetRowProperty( rp1, prop );
	    int		val2= docGetRowProperty( rp2, prop );

	    if  ( val1 != val2 )
		{ PROPmaskADD( &rpDifMask, prop );	}
	    }
	}

    for ( p= 0; p < DocRowBorderPropertyCount; p++ )
	{
	int	prop= DocRowBorderProperties[p];

	if  ( PROPmaskISSET( rpCmpMask, prop ) )
	    {
	    int		val1= docGetRowProperty( rp1, prop );
	    int		val2= docGetRowProperty( rp2, prop );

	    if  ( val2 >= 0 && dam && dam->damBorderMap )
		{ val2= dam->damBorderMap[val2];	}

	    if  ( val1 != val2 )
		{ PROPmaskADD( &rpDifMask, prop );	}
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpCmpMask, RPpropSHADING ) )
	{
	int	fromNumber= rp2->rpShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( rp1->rpShadingNumber != fromNumber )
	    { PROPmaskADD( &rpDifMask, RPpropSHADING );	}
	}

    if  ( PROPmaskISSET( rpCmpMask, RPpropFRAME ) )
	{
	int	fromNumber= rp2->rpFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( rp1->rpFrameNumber != fromNumber )
	    { PROPmaskADD( &rpDifMask, RPpropFRAME );	}
	}

    *pRpDifPask= rpDifMask; return;
    }

/************************************************************************/
/*									*/
/*  Copy row properties. As this routine is not called very often,	*/
/*  just call the 'update' routine.					*/
/*									*/
/************************************************************************/

int docCopyRowProperties(	RowProperties *			rpTo,
				const RowProperties *		rpFrom,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		rpDoneMask;
    PropertyMask		rpSetMask;

    utilPropMaskClear( &rpDoneMask );
    utilPropMaskClear( &rpSetMask );

    utilPropMaskFill( &rpSetMask, RPprop_FULL_COUNT );

    if  ( docUpdRowProperties( &rpDoneMask, rpTo, &rpSetMask, rpFrom, dam ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  1)  Are the columns in two RowProperties 'the same' (Do they	*/
/*	align?)								*/
/*  2)  All internal borders equal?					*/
/*  3)  All column properties identical?				*/
/*									*/
/************************************************************************/

/*  1  */
int docApproximatelyAlignedColumns(	const RowProperties *	rp1,
					const RowProperties *	rp2 )
    {
    CellProperties *	cp1;
    CellProperties *	cp2;
    int			i;

    const int		D= 40;

    if  ( rp1->rpCellCount != rp2->rpCellCount )
	{ return 0;	}

    /* No!
    if  ( rp1->rpHalfGapWidthTwips != rp2->rpHalfGapWidthTwips )
	{ return 0;	}
    */

    if  ( rp1->rpLeftIndentTwips > rp2->rpLeftIndentTwips+ D	||
	  rp1->rpLeftIndentTwips < rp2->rpLeftIndentTwips- D	)
	{ return 0;	}

    cp1= rp1->rpCells;
    cp2= rp2->rpCells;
    for ( i= 0; i < rp1->rpCellCount; cp2++, cp1++, i++ )
	{
	if  ( cp1->cpRightBoundaryTwips > cp2->cpRightBoundaryTwips+ D	||
	      cp1->cpRightBoundaryTwips < cp2->cpRightBoundaryTwips- D	)
	    { return 0;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Change row properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdRowProperties(	PropertyMask *			pRpDonePask,
				RowProperties *			rpTo,
				const PropertyMask *		rpSetMask,
				const RowProperties *		rpFrom,
				const DocumentAttributeMap *	dam )
    {
    int			p;
    PropertyMask	rpDoneMask;

    int			updCellLayout;
    int			updCellProps;

    updCellLayout= PROPmaskISSET( rpSetMask, RPpropCELL_LAYOUT );
    updCellProps= PROPmaskISSET( rpSetMask, RPpropCELL_PROPS );

    utilPropMaskClear( &rpDoneMask );

    if  ( updCellLayout || updCellProps )
	{
	int			col;
	CellProperties *	cpTo;
	const CellProperties *	cpFrom;

	if  ( updCellLayout )
	    {
	    while( rpTo->rpCellCount > rpFrom->rpCellCount )
		{
		docCleanCellProperties( &(rpTo->rpCells[rpTo->rpCellCount-1]) );
		PROPmaskADD( &rpDoneMask, RPpropCELL_LAYOUT );
		if  ( updCellProps )
		    { PROPmaskADD( &rpDoneMask, RPpropCELL_PROPS );	}
		rpTo->rpCellCount--;
		}
	    }

	cpTo= rpTo->rpCells;
	cpFrom= rpFrom->rpCells;
	for ( col= 0;
	      col < rpTo->rpCellCount && col < rpFrom->rpCellCount;
	      cpTo++, cpFrom++, col++ )
	    {
	    PropertyMask	cpSetMask;
	    PropertyMask	cpDoneMask;

	    docRowMaskToCellMask( &cpSetMask, rpSetMask );
	    utilPropMaskClear( &cpDoneMask );

	    if  ( docUpdCellProperties( &cpDoneMask, cpTo,
						&cpSetMask, cpFrom, dam ) )
		{ LDEB(1); return -1;	}

	    docRowMaskApplyCellMask( &rpDoneMask, &cpDoneMask );
	    }

	if  ( updCellLayout )
	    {
	    while( rpTo->rpCellCount < rpFrom->rpCellCount )
		{
		const int	shiftTail= 0; /* at end: irrelevant */

		if  ( docInsertRowColumn( rpTo, rpTo->rpCellCount, shiftTail,
								cpFrom, dam ) )
		    { LDEB(rpTo->rpCellCount); return -1;	}

		PROPmaskADD( &rpDoneMask, RPpropCELL_LAYOUT );
		if  ( updCellProps )
		    { PROPmaskADD( &rpDoneMask, RPpropCELL_PROPS );	}
		cpFrom++;
		}
	    }
	}

    for ( p= 0; p < DocRowIntPropertyCount; p++ )
	{
	int	prop= DocRowIntProperties[p];

	if  ( PROPmaskISSET( rpSetMask, prop ) )
	    {
	    int		valt= docGetRowProperty( rpTo, prop );
	    int		valf= docGetRowProperty( rpFrom, prop );

	    if  ( valt != valf )
		{
		if  ( docSetRowProperty( rpTo, prop, valf ) )
		    { LLDEB(prop,valf); return -1;	}

		PROPmaskADD( &rpDoneMask, prop );
		}
	    }
	}

    for ( p= 0; p < DocRowBorderPropertyCount; p++ )
	{
	int	prop= DocRowBorderProperties[p];

	if  ( PROPmaskISSET( rpSetMask, prop ) )
	    {
	    int		valt= docGetRowProperty( rpTo, prop );
	    int		valf= docGetRowProperty( rpFrom, prop );

	    if  ( valf >= 0 && dam && dam->damBorderMap )
		{ valf= dam->damBorderMap[valf];	}

	    if  ( valt != valf )
		{
		if  ( docSetRowProperty( rpTo, prop, valf ) )
		    { LLDEB(prop,valf); return -1;	}

		PROPmaskADD( &rpDoneMask, prop );
		}
	    }
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropSHADING ) )
	{
	int	fromNumber= rpFrom->rpShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( rpTo->rpShadingNumber != fromNumber )
	    {
	    rpTo->rpShadingNumber= fromNumber;
	    PROPmaskADD( &rpDoneMask, RPpropSHADING );
	    }
	}

    if  ( PROPmaskISSET( rpSetMask, RPpropFRAME ) )
	{
	int	fromNumber= rpFrom->rpFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( rpTo->rpFrameNumber != fromNumber )
	    {
	    rpTo->rpFrameNumber= fromNumber;
	    PROPmaskADD( &rpDoneMask, RPpropFRAME );
	    }
	}

    if  ( pRpDonePask )
	{ utilPropMaskOr( pRpDonePask, pRpDonePask, &rpDoneMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a kind of 'default' row layout: Evenly distribute the		*/
/*  available width over the columns.					*/
/*									*/
/************************************************************************/

int docEqualWidthColumns(		RowProperties *		rp,
					int			columns,
					int			wide,
					int			fontHalfPoints )
    {
    int				rval= 0;
    int				col;

    CellProperties		cp;

    const int			shiftTail= 0; /* at end: irrelevant */

    docInitCellProperties( &cp );

    rp->rpHalfGapWidthTwips= 5* fontHalfPoints;
    rp->rpLeftIndentTwips=  -5* fontHalfPoints;

    for ( col= 0; col < columns; col++ )
	{
	cp.cpRightBoundaryTwips= ( ( col+ 1 )* wide )/ columns;

	if  ( docInsertRowColumn( rp, col, shiftTail, &cp,
					(const DocumentAttributeMap *)0 ) )
	    { LDEB(col); rval= -1; goto ready;	}
	}

  ready:
    docCleanCellProperties( &cp );

    return rval;
    }

int docSetRowProperty(		RowProperties *		rp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case RPpropGAP_WIDTH:
	    rp->rpHalfGapWidthTwips= arg;
	    break;

	case RPpropLEFT_INDENT:
	    rp->rpLeftIndentTwips= arg;
	    break;

	case RPpropHEIGHT:
	    rp->rpHeightTwips= arg;
	    break;

	case RPpropTOP_BORDER:
	    rp->rpTopBorderNumber= arg;
	    break;
	case RPpropBOTTOM_BORDER:
	    rp->rpBottomBorderNumber= arg;
	    break;
	case RPpropLEFT_BORDER:
	    rp->rpLeftBorderNumber= arg;
	    break;
	case RPpropRIGHT_BORDER:
	    rp->rpRightBorderNumber= arg;
	    break;
	case RPpropHORIZ_BORDER:
	    rp->rpHorizontalBorderNumber= arg;
	    break;
	case RPpropVERT_BORDER:
	    rp->rpVerticalBorderNumber= arg;
	    break;

	case RPpropSHADING:
	    rp->rpShadingNumber= arg;
	    break;
	case RPpropFRAME:
	    rp->rpFrameNumber= arg;
	    break;

	/**/
	case RPpropALIGNMENT:
	    rp->rpAlignment= arg;
	    break;

	case RPpropIS_TABLE_HEADER:
	    rp->rpIsTableHeader= ( arg != 0 );
	    break;
	case RPpropKEEP_ON_ONE_PAGE:
	    rp->rpKeepOnOnePage= ( arg != 0 );
	    break;
	case RPprop_KEEPFOLLOW:
	    rp->rp_Keepfollow= ( arg != 0 );
	    break;
	case RPpropAUTOFIT:
	    rp->rpAutofit= ( arg != 0 );
	    break;
	case RPpropRTOL:
	    rp->rpRToL= ( arg != 0 );
	    break;

	/**/
	case RPpropTRW_WIDTH:
	    rp->rpPreferredWidth= arg;
	    break;
	case RPpropTRFTS_WIDTH:
	    rp->rpPreferredWidthUnit= arg;
	    break;

	/**/
	case RPpropTRSPDL:
	    rp->rpLeftDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDR:
	    rp->rpRightDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDT:
	    rp->rpTopDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDB:
	    rp->rpBottomDefaultCellSpacing= arg;
	    break;

	case RPpropTRSPDFL:
	    rp->rpLeftDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFR:
	    rp->rpRightDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFT:
	    rp->rpTopDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFB:
	    rp->rpBottomDefaultCellSpacingUnit= arg;
	    break;

	/**/
	case RPpropTRPADDL:
	    rp->rpLeftCellPadding= arg;
	    break;
	case RPpropTRPADDR:
	    rp->rpRightCellPadding= arg;
	    break;
	case RPpropTRPADDT:
	    rp->rpTopCellPadding= arg;
	    break;
	case RPpropTRPADDB:
	    rp->rpBottomCellPadding= arg;
	    break;

	case RPpropTRPADDFL:
	    rp->rpLeftCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFR:
	    rp->rpRightCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFT:
	    rp->rpTopCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFB:
	    rp->rpBottomCellPaddingUnit= arg;
	    break;

	/**/
	case RPpropTRW_WIDTHB:
	    rp->rpCellWidthBefore= arg;
	    break;
	case RPpropTRW_WIDTHA:
	    rp->rpCellWidthAfter= arg;
	    break;

	case RPpropTRFTS_WIDTHB:
	    rp->rpCellWidthBeforeUnit= arg;
	    break;
	case RPpropTRFTS_WIDTHA:
	    rp->rpCellWidthAfterUnit= arg;
	    break;

	case RPpropAUTOFORMAT_BORDERS:
	    rp->rpAutoformatBorders= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_SHADING:
	    rp->rpAutoformatShading= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FONT:
	    rp->rpAutoformatFont= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_COLOR:
	    rp->rpAutoformatColor= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_APPLY_BEST_FIT:
	    rp->rpAutoformatApplyBestFit= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FIRST_ROW:
	    rp->rpAutoformatFirstRow= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_LAST_ROW:
	    rp->rpAutoformatLastRow= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FIRST_COLUMN:
	    rp->rpAutoformatFirstColumn= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_LAST_COLUMN:
	    rp->rpAutoformatLastColumn= ( arg != 0 );
	    break;

	case RPpropROW_NUMBER:
	    rp->rpRowNumber= arg;
	    break;
	case RPpropROW_BAND_NUMBER:
	    rp->rpRowBandNumber= arg;
	    break;
	case RPpropROW_STYLE:
	    rp->rpRowStyle= arg;
	    break;

	case RPpropIS_LAST_ROW:
	    rp->rpIsLastRow= ( arg != 0 );
	    break;

	case RPpropTRAUTH:
	    rp->rpAuthor= arg;
	    break;

	case RPpropCELL_LAYOUT:
	case RPpropCELL_PROPS:
	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetRowProperty(		const RowProperties *	rp,
				int			prop )
    {
    switch( prop )
	{
	case RPpropGAP_WIDTH:
	    return rp->rpHalfGapWidthTwips;
	    break;

	case RPpropLEFT_INDENT:
	    return rp->rpLeftIndentTwips;
	    break;

	case RPpropHEIGHT:
	    return rp->rpHeightTwips;
	    break;

	case RPpropTOP_BORDER:
	    return rp->rpTopBorderNumber;
	    break;
	case RPpropBOTTOM_BORDER:
	    return rp->rpBottomBorderNumber;
	    break;
	case RPpropLEFT_BORDER:
	    return rp->rpLeftBorderNumber;
	    break;
	case RPpropRIGHT_BORDER:
	    return rp->rpRightBorderNumber;
	    break;
	case RPpropHORIZ_BORDER:
	    return rp->rpHorizontalBorderNumber;
	    break;
	case RPpropVERT_BORDER:
	    return rp->rpVerticalBorderNumber;
	    break;

	case RPpropSHADING:
	    return rp->rpShadingNumber;
	    break;
	case RPpropFRAME:
	    return rp->rpFrameNumber;
	    break;

	/**/
	case RPpropALIGNMENT:
	    return rp->rpAlignment;
	    break;

	case RPpropIS_TABLE_HEADER:
	    return rp->rpIsTableHeader;
	    break;
	case RPpropKEEP_ON_ONE_PAGE:
	    return rp->rpKeepOnOnePage;
	    break;
	case RPprop_KEEPFOLLOW:
	    return rp->rp_Keepfollow;
	    break;
	case RPpropAUTOFIT:
	    return rp->rpAutofit;
	    break;
	case RPpropRTOL:
	    return rp->rpRToL;
	    break;

	/**/
	case RPpropTRW_WIDTH:
	    return rp->rpPreferredWidth;
	    break;
	case RPpropTRFTS_WIDTH:
	    return rp->rpPreferredWidthUnit;
	    break;

	/**/
	case RPpropTRSPDL:
	    return rp->rpLeftDefaultCellSpacing;
	    break;
	case RPpropTRSPDR:
	    return rp->rpRightDefaultCellSpacing;
	    break;
	case RPpropTRSPDT:
	    return rp->rpTopDefaultCellSpacing;
	    break;
	case RPpropTRSPDB:
	    return rp->rpBottomDefaultCellSpacing;
	    break;

	case RPpropTRSPDFL:
	    return rp->rpLeftDefaultCellSpacingUnit;
	    break;
	case RPpropTRSPDFR:
	    return rp->rpRightDefaultCellSpacingUnit;
	    break;
	case RPpropTRSPDFT:
	    return rp->rpTopDefaultCellSpacingUnit;
	    break;
	case RPpropTRSPDFB:
	    return rp->rpBottomDefaultCellSpacingUnit;
	    break;

	/**/
	case RPpropTRPADDL:
	    return rp->rpLeftCellPadding;
	    break;
	case RPpropTRPADDR:
	    return rp->rpRightCellPadding;
	    break;
	case RPpropTRPADDT:
	    return rp->rpTopCellPadding;
	    break;
	case RPpropTRPADDB:
	    return rp->rpBottomCellPadding;
	    break;

	case RPpropTRPADDFL:
	    return rp->rpLeftCellPaddingUnit;
	    break;
	case RPpropTRPADDFR:
	    return rp->rpRightCellPaddingUnit;
	    break;
	case RPpropTRPADDFT:
	    return rp->rpTopCellPaddingUnit;
	    break;
	case RPpropTRPADDFB:
	    return rp->rpBottomCellPaddingUnit;
	    break;

	/**/
	case RPpropTRW_WIDTHB:
	    return rp->rpCellWidthBefore;
	    break;
	case RPpropTRW_WIDTHA:
	    return rp->rpCellWidthAfter;
	    break;

	case RPpropTRFTS_WIDTHB:
	    return rp->rpCellWidthBeforeUnit;
	    break;
	case RPpropTRFTS_WIDTHA:
	    return rp->rpCellWidthAfterUnit;
	    break;

	case RPpropAUTOFORMAT_BORDERS:
	    return rp->rpAutoformatBorders;
	    break;
	case RPpropAUTOFORMAT_SHADING:
	    return rp->rpAutoformatShading;
	    break;
	case RPpropAUTOFORMAT_FONT:
	    return rp->rpAutoformatFont;
	    break;
	case RPpropAUTOFORMAT_COLOR:
	    return rp->rpAutoformatColor;
	    break;
	case RPpropAUTOFORMAT_APPLY_BEST_FIT:
	    return rp->rpAutoformatApplyBestFit;
	    break;
	case RPpropAUTOFORMAT_FIRST_ROW:
	    return rp->rpAutoformatFirstRow;
	    break;
	case RPpropAUTOFORMAT_LAST_ROW:
	    return rp->rpAutoformatLastRow;
	    break;
	case RPpropAUTOFORMAT_FIRST_COLUMN:
	    return rp->rpAutoformatFirstColumn;
	    break;
	case RPpropAUTOFORMAT_LAST_COLUMN:
	    return rp->rpAutoformatLastColumn;
	    break;

	case RPpropROW_NUMBER:
	    return rp->rpRowNumber;
	    break;
	case RPpropROW_BAND_NUMBER:
	    return rp->rpRowBandNumber;
	    break;
	case RPpropROW_STYLE:
	    return rp->rpRowStyle;
	    break;

	case RPpropIS_LAST_ROW:
	    return rp->rpIsLastRow;
	    break;

	case RPpropTRAUTH:
	    return rp->rpAuthor;
	    break;

	case RPpropCELL_LAYOUT:
	case RPpropCELL_PROPS:
	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }
