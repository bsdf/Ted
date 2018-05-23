/************************************************************************/
/*									*/
/*  Layout of a stack of contiguous table rows.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Layout of a stack of contiguous table rows.				*/
/*									*/
/*  The caller keeps track of the bottom position of the slice: It must	*/
/*  set the bottom position of the last row.				*/
/*									*/
/************************************************************************/

int docLayoutTableSlice(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				BufferItem *		parentNode,
				int			from,
				int			upto,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int			i= from;
    int			restartFrom= from;

    LayoutPosition	lpHere= *lpTop;
    LayoutPosition	lpRestartFrom= *lpTop;

    int			stayInThisColumn= 0;
    int			attempt= 0;

    while( i < upto )
	{
	int		found= FORMATstopREADY;
	BufferItem *	rowNode= parentNode->biChildren[i];

	LayoutPosition	lpRowTop= lpHere;

	docLayoutStartNodeLayout( rowNode, lj, &lpHere );

	if  ( docLayoutRowNode( &found, &lpHere, &lpHere,
			rowNode, bf, stayInThisColumn && attempt == 0, lj ) )
	    { LDEB(i); return -1;	}

	if  ( found == FORMATstopFRAME_FULL )
	    {
	    if  ( ! stayInThisColumn )
		{ LLDEB(found,stayInThisColumn);	}
	    else{
		attempt++;

		lpHere= lpRestartFrom;
		docLayoutToNextColumn( &lpHere, bf, parentNode, lj );

		i= restartFrom;
		continue;
		}
	    }

	if  ( i < upto- 1 )
	    {
	    docLayoutFinishNodeLayout( (int *)0, rowNode, lj, &lpHere );
	    }

	if  ( attempt == 0						&&
	      ( rowNode->biRowIsTableHeader	||
	        rowNode->biRow_Keepfollow )	)
	    {
	    if  ( ! stayInThisColumn )
		{
		lpRestartFrom= lpRowTop;
		stayInThisColumn= 1; restartFrom= i;
		}
	    }
	else{ stayInThisColumn= 0; restartFrom= i+ 1;	}

	i++;
	}

    *lpBelow= lpHere;
    return 0;
    }

