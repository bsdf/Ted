/************************************************************************/
/*									*/
/*  Book keeping on TableRectangle's					*/
/*									*/
/*  The colspan and rowspan fields are managed in a halfhearted way:	*/
/*  You need the table to do a correct administration. So their value	*/
/*  is the responsibility of the caller.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a Table Rectangle.					*/
/*									*/
/************************************************************************/

void docInitTableRectangle(	TableRectangle *	tr )
    {
    tr->trCol0= -1;
    tr->trCol1= -1;
    tr->trCol11= -1;

    tr->trRow00= -1;
    tr->trRow0= -1;
    tr->trRow1= -1;
    tr->trRow11= -1;

    tr->trIsRowSlice= 0;
    tr->trIsColSlice= 0;
    tr->trIsTableSlice= 0;
    tr->trIsWholeTable= 0;
    tr->trIsSingleCell= 0;

    tr->trCellRowspan= 0;
    tr->trCellColspan= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Intersect two TableRectangles.					*/
/*									*/
/************************************************************************/

int docIntersectTableRectangle(	TableRectangle *		tr,
				const TableRectangle *		tr1,
				const TableRectangle *		tr2 )
    {
    TableRectangle	res= *tr1;

    if  ( tr1->trRow00 != tr2->trRow00 )
	{ LLDEB(tr1->trRow00,tr2->trRow00); return 0;	}
    if  ( tr1->trRow11 != tr2->trRow11 )
	{ LLDEB(tr1->trRow11,tr2->trRow11); return 0;	}
    if  ( tr1->trCol11 != tr2->trCol11 )
	{ LLDEB(tr1->trCol11,tr2->trCol11); return 0;	}

    if  ( tr1->trCol1 < tr2->trCol0 )
	{ return 0;	}

    if  ( tr2->trCol1 < tr1->trCol0 )
	{ return 0;	}

    if  ( tr1->trRow1 < tr2->trRow0 )
	{ return 0;	}

    if  ( tr2->trRow1 < tr1->trRow0 )
	{ return 0;	}

    if  ( ! tr )
	{ return 1;	}

    if  ( tr1->trCol0 < tr2->trCol0 )
	{ res.trCol0= tr2->trCol0;	}
    else{ res.trCol0= tr1->trCol0;	}

    if  ( tr1->trRow0 < tr2->trRow0 )
	{ res.trRow0= tr2->trRow0;	}
    else{ res.trRow0= tr1->trRow0;	}

    if  ( tr1->trCol1 < tr2->trCol1 )
	{ res.trCol1= tr1->trCol1;	}
    else{ res.trCol1= tr2->trCol1;	}

    if  ( tr1->trRow1 < tr2->trRow1 )
	{ res.trRow1= tr1->trRow1;	}
    else{ res.trRow1= tr2->trRow1;	}

    res.trCellRowspan= 0;
    res.trCellColspan= 0;

    *tr= res; return 1;
    }

void docExpandTableRectangleToWholeRows(	TableRectangle *	tr )
    {
    tr->trCol0= 0;
    tr->trCol1= tr->trCol11;

    if  ( tr->trRow0 != tr->trRow1	||
	  tr->trCol0 != tr->trCol1	)
	{
	tr->trCellRowspan= 0;
	tr->trCellColspan= 0;
	}

    return;
    }

void docExpandTableRectangleToWholeColumns(	TableRectangle *	tr )
    {
    tr->trRow0= tr->trRow00;
    tr->trRow1= tr->trRow11;

    if  ( tr->trRow0 != tr->trRow1	||
	  tr->trCol0 != tr->trCol1	)
	{
	tr->trCellRowspan= 0;
	tr->trCellColspan= 0;
	}

    return;
    }

