/************************************************************************/
/*									*/
/*  Format changes to paragraphs.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docEdit.h"
#   include	<docParaParticules.h>
#   include	"docEditSetProperties.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the attributes of the selection.				*/
/*									*/
/*  A)  Exclude properties that are derived from a list from the task.	*/
/*  1)  Adjust the start line for recalculating the layout of the first	*/
/*	paragraph in the selection.					*/
/*  2)  If necessary, split the first particule to change the text	*/
/*	attributes of the second half.					*/
/*  3)  For all paragraphs in the selection upto the last one..		*/
/*  4)  Change the text attributes.					*/
/*									*/
/************************************************************************/

static int docEditChangeParaAndTextProperties(
				EditOperation *			eo,
				PropertyMask *			pPpDoneMask,
				PropertyMask *			pTaDoneMask,
				BufferItem *			paraNode,
				int				partFrom,
				int				partUpto,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet )
    {
    BufferDocument *		bd= eo->eoDocument;

    PropertyMask		ppDoneMask;
    PropertyMask		taDoneMask;

    int				textChanged= 0;

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}

    utilPropMaskClear( &ppDoneMask );
    utilPropMaskClear( &taDoneMask );

    /*  A  */
    if  ( paraNode->biParaListOverride > 0 )
	{
	DocumentField *		dfHead= (DocumentField *)0;
	DocumentSelection	dsInsideHead;
	DocumentSelection	dsAroundHead;
	int			bulletPartBegin= -1;
	int			bulletPartEnd= -1;

	if  ( docDelimitParaHeadField( &dfHead,
			    &dsInsideHead, &dsAroundHead,
			    &bulletPartBegin, &bulletPartEnd, paraNode, bd ) )
	    { LDEB(1);	}

	if  ( partFrom <= bulletPartEnd )
	    { partFrom= bulletPartEnd+ 1;	}
	}

    if  ( taSetMask )
	{
	if  ( docChangeParticuleAttributes( &textChanged, &taDoneMask, bd,
						paraNode, partFrom, partUpto,
						taSet, taSetMask ) )
	    { LLDEB(partFrom,partUpto); return -1;	}
	}

    if  ( ppSetMask )
	{
	if  ( docEditUpdParaProperties( eo, &ppDoneMask, paraNode,
					ppSetMask, ppSet,
					(const DocumentAttributeMap *)0 ) )
	    { LDEB(1); return -1;	}
	}

    if  ( pTaDoneMask )
	{ *pTaDoneMask= taDoneMask;	}
    if  ( pPpDoneMask )
	{ *pPpDoneMask= ppDoneMask;	}

    return 0;
    }

/************************************************************************/

static int docFormatHeadParticule( BufferItem *			paraNode,
				int *				pSplit,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    int				part;
    TextParticule *		tp;
    int				split= 0;

    PropertyMask		pmSplit;

    if  ( docFindParticuleOfPosition( &part, (int *)0,
						&(ds->dsHead), PARAfindLAST ) )
	{ LDEB(ds->dsHead.dpStroff); return -1;	}

    /*  1  */
    tp= paraNode->biParaParticules+ part;

    /*  2  */
    utilPropMaskClear( &pmSplit );
    if  ( taSetMask && (unsigned)ds->dsHead.dpStroff > tp->tpStroff )
	{
	TextAttribute	ta;

	docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

	utilAttributeDifference( &pmSplit, &ta, taSetMask, taSet );
	}

    if  ( ! utilPropMaskIsEmpty( &pmSplit ) )
	{
	int		stroff= ds->dsHead.dpStroff;

	if  ( (unsigned)stroff < tp->tpStroff+ tp->tpStrlen )
	    {
	    if  ( docSplitTextParticule( (TextParticule **)0,
				(TextParticule **)0, paraNode, part, stroff ) )
		{ LDEB(part); return -1;	}

	    split= 1; part++;
	    }
	else{ /* tp++; */ part++; }
	}

    *pSplit= split;
    return part;
    }

static int docFormatTailParticule( BufferItem *			paraNode,
				int *				pSplit,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    int				part1;
    TextParticule *		tp;

    PropertyMask		pmSplit;
    int				split= 0;

    if  ( docFindParticuleOfPosition( &part1, (int *)0,
					    &(ds->dsTail), PARAfindFIRST ) )
	{ LDEB(ds->dsTail.dpStroff); return -1;	}

    tp= paraNode->biParaParticules+ part1;

    utilPropMaskClear( &pmSplit );
    if  ( taSetMask && part1 < paraNode->biParaParticuleCount )
	{
	TextAttribute	ta;

	docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

	utilAttributeDifference( &pmSplit, &ta, taSetMask, taSet );
	}

    if  ( ! utilPropMaskIsEmpty( &pmSplit ) )
	{
	int	strend= tp->tpStroff+ tp->tpStrlen;

	if  ( ds->dsTail.dpStroff < strend )
	    {
	    if  ( docSplitTextParticule( (TextParticule **)0,
					(TextParticule **)0,
					paraNode, part1, ds->dsTail.dpStroff ) )
		{ LDEB(1); return -1;	}

	    split= 1;
	    }

	part1++;
	}

    *pSplit= split;
    return part1;
    }

/************************************************************************/

int docEditChangeParaProperties( SetProperties *		setProps,
				const DocumentSelection *	ds,
				BufferItem *			paraNode,
				const TextAttribute *		taSet,
				const ParagraphProperties *	ppSet )
    {
    EditOperation *	eo= setProps->spEditOperation;
    BufferDocument *	bd= eo->eoDocument;

    int			part= 0;
    int			part1= paraNode->biParaParticuleCount;
    int			split;

    PropertyMask	paraTaDoneMask;
    PropertyMask	paraPpDoneMask;

    if  ( paraNode == ds->dsHead.dpNode )
	{
	part= docFormatHeadParticule( paraNode, &split,
				    setProps->sp_taSetMask, taSet, bd, ds );
	if  ( part < 0 )
	    { LDEB(part); return -1;	}
	}

    if  ( paraNode == ds->dsTail.dpNode )
	{
	part1= docFormatTailParticule( paraNode, &split,
			setProps->sp_taSetMask, taSet, bd, ds );
	if  ( part1 < 0 )
	    { LDEB(part1); return -1;	}
	}

    /*  4  */
    if  ( docEditChangeParaAndTextProperties( eo,
				&paraPpDoneMask, &paraTaDoneMask, paraNode,
				part, part1,
				setProps->sp_taSetMask, taSet,
				setProps->sp_ppSetMask, ppSet ) )
	{ LDEB(1);	}

    if  ( ! utilPropMaskIsEmpty( &paraPpDoneMask )	||
	  ! utilPropMaskIsEmpty( &paraTaDoneMask )	)
	{
	if  ( setProps->spRedoParaLayout )
	    { docEditIncludeNodeInReformatRange( eo, paraNode );	}

	if  ( setProps->sp_ppDoneMask )
	    {
	    utilPropMaskOr( setProps->sp_ppDoneMask,
				setProps->sp_ppDoneMask, &paraPpDoneMask );
	    }

	if  ( setProps->sp_taDoneMask )
	    {
	    utilPropMaskOr( setProps->sp_taDoneMask,
				setProps->sp_taDoneMask, &paraTaDoneMask );
	    }
	}

    return 0;
    }

