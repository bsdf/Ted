/************************************************************************/
/*									*/
/*  Manage field hierarchy.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docDocumentField.h"

/************************************************************************/
/*									*/
/*  Insert a child field into a field or into the root of a document	*/
/*  tree.								*/
/*									*/
/************************************************************************/

static int docInsertFieldAt(		DocumentField *		parent,
					ChildFields *		cf,
					DocumentField *		df,
					int			pos )
    {
    DocumentField **		fresh;
    int				i;

    fresh= (DocumentField **)realloc( cf->cfChildren,
			( cf->cfChildCount+ 1 )* sizeof(DocumentField *) );
    if  ( ! fresh )
	{ LXDEB(cf->cfChildCount,fresh); return -1;	}
    cf->cfChildren= fresh;

    for ( i= cf->cfChildCount; i > pos; i-- )
	{
	cf->cfChildren[i]= cf->cfChildren[i-1];
	cf->cfChildren[i]->dfNumberInParent= i;
	}
    cf->cfChildren[pos]= df;
    cf->cfChildren[pos]->dfNumberInParent= pos;
    df->dfParent= parent;

    cf->cfChildCount++;

    return 0;
    }

int docInsertChildField(		DocumentField *		parent,
					ChildFields *		cf,
					DocumentField *		df )
    {
    int				pos;

    pos= cf->cfChildCount- 1;
    while( pos >= 0 && docCompareEditPositions(
				&(df->dfHeadPosition),
				&(cf->cfChildren[pos]->dfTailPosition) ) < 0 )
	{ pos--;	}
    pos++;

    return docInsertFieldAt( parent, cf, df, pos );
    }

/************************************************************************/
/*									*/
/*  Delete a field from its parent: Replace it with its children.	*/
/*									*/
/************************************************************************/

int docDeleteChildField(	ChildFields *		cf,
				DocumentField *		df )
    {
    int		pos= df->dfNumberInParent;

    if  ( df->dfNumberInParent < 0			||
	  df->dfNumberInParent >= cf->cfChildCount	)
	{
	LLLDEB(df->dfFieldNumber,df->dfNumberInParent,cf->cfChildCount);
	return -1;
	}

    if  ( df->dfChildFields.cfChildCount >= 1 )
	{
	int		i;
	int		step= df->dfChildFields.cfChildCount- 1;

	if  ( step > 0 )
	    {
	    DocumentField **	fresh;
	    int			newCount;

	    newCount= cf->cfChildCount+ step;

	    fresh= (DocumentField **)realloc( cf->cfChildren,
					newCount* sizeof(DocumentField *) );
	    if  ( ! fresh )
		{ LXDEB(newCount,fresh); return -1;	}
	    cf->cfChildren= fresh;

	    for ( i= cf->cfChildCount+ step- 1; i > pos+ step; i-- )
		{
		cf->cfChildren[i]= cf->cfChildren[i-step];
		cf->cfChildren[i]->dfNumberInParent= i;
		}
	    }

	for ( i= 0; i <= step; i++ ) /* <= !! */
	    {
	    cf->cfChildren[pos+ i]= df->dfChildFields.cfChildren[i];
	    cf->cfChildren[pos+ i]->dfNumberInParent= pos+ i;
	    cf->cfChildren[pos+ i]->dfParent= df->dfParent;
	    }

	cf->cfChildCount += step;
	}
    else{
	int			i;

	cf->cfChildCount--;

	for ( i= pos; i < cf->cfChildCount; i++ )
	    {
	    cf->cfChildren[i]= cf->cfChildren[i+1];
	    cf->cfChildren[i]->dfNumberInParent= i;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the innermost field for a position in the document tree.	*/
/*  If at a certain position there is more than one field, the first	*/
/*  one is returned.							*/
/*									*/
/*  This function looks complicated, but is not: Start in the root	*/
/*  fields of the document tree. As soon as you find a field that holds	*/
/*  the position remember the field and look for a child that also	*/
/*  holds the position. Eventually that returns the innermost field	*/
/*  that contains the position.						*/
/*									*/
/************************************************************************/

DocumentField * docFindChildField(	const ChildFields *	cf,
					const EditPosition *	ep,
					int			lastOne )
    {
    DocumentField *	dfFound= (DocumentField *)0;

    for (;;)
	{
	const int	count= cf->cfChildCount;
	int		i;

	if  ( lastOne )
	    {
	    for ( i= count- 1; i >= 0; i-- )
		{
		DocumentField *	df= cf->cfChildren[i];

		if  ( docEditPositionInField( df, ep ) )
		    {
		    dfFound= df;
		    cf= &(dfFound->dfChildFields);
		    break;
		    }
		}

	    if  ( i < 0 )
		{ break;	}
	    }
	else{
	    for ( i= 0; i < count; i++ )
		{
		DocumentField *	df= cf->cfChildren[i];

		if  ( docEditPositionInField( df, ep ) )
		    {
		    dfFound= df;
		    cf= &(dfFound->dfChildFields);
		    break;
		    }
		}

	    if  ( i >= count )
		{ break;	}
	    }
	}

    return dfFound;
    }

DocumentField * docFindTypedChildField(	const ChildFields *	cf,
					const EditPosition *	ep,
					int			kind )
    {
    const int		lastOne= 1;
    DocumentField *	df= docFindChildField( cf, ep, lastOne );

    while( df && df->dfKind != kind )
	{ df= df->dfParent;	}

    return df;
    }

/************************************************************************/
/*									*/
/*  Find a field with a certain kind inside a range.			*/
/*									*/
/************************************************************************/

DocumentField * docFindFieldInRange(	const EditRange *	er,
					const ChildFields *	cf,
					int			lastOne,
					int			kind )
    {
    int		pos;

    if  ( lastOne )
	{
	for ( pos= cf->cfChildCount- 1; pos >= 0; pos-- )
	    {
	    DocumentField *		dfC= cf->cfChildren[pos];
	    const EditPosition *	epHead= &(dfC->dfHeadPosition);
	    const EditPosition *	epTail= &(dfC->dfTailPosition);
	    DocumentField *		df;

	    if  ( docCompareEditPositions( epHead, &(er->erTail) ) > 0 )
		{ continue;	}
	    if  ( docCompareEditPositions( epTail, &(er->erHead) ) < 0 )
		{ break;	}

	    if  ( docCompareEditPositions( epHead, &(er->erHead) ) >= 0	&&
		  docCompareEditPositions( epTail, &(er->erTail) ) <= 0	&&
		  ( dfC->dfKind == kind || kind < 0 )			)
		{ return dfC;	}

	    df= docFindFieldInRange( er, &(dfC->dfChildFields), lastOne, kind );
	    if  ( df )
		{ return df;	}
	    }
	}
    else{
	for ( pos= 0; pos < cf->cfChildCount; pos++ )
	    {
	    DocumentField *		dfC= cf->cfChildren[pos];
	    const EditPosition *	epHead= &(dfC->dfHeadPosition);
	    const EditPosition *	epTail= &(dfC->dfTailPosition);
	    DocumentField *		df;

	    if  ( docCompareEditPositions( epTail, &(er->erHead) ) < 0 )
		{ continue;	}
	    if  ( docCompareEditPositions( epHead, &(er->erTail) ) > 0 )
		{ break;	}

	    if  ( docCompareEditPositions( epHead, &(er->erHead) ) >= 0	&&
		  docCompareEditPositions( epTail, &(er->erTail) ) <= 0	&&
		  ( dfC->dfKind == kind || kind < 0 )			)
		{ return dfC;	}

	    df= docFindFieldInRange( er, &(dfC->dfChildFields), lastOne, kind );
	    if  ( df )
		{ return df;	}
	    }
	}

    return (DocumentField *)0;
    }

/************************************************************************/
/*									*/
/*  Remember the document position of the end of a field.		*/
/*									*/
/************************************************************************/

void docSetFieldTail(		DocumentField *		dfPa,
				const EditPosition *	epTail )
    {
    dfPa->dfTailPosition= *epTail;

    if  ( dfPa->dfChildFields.cfChildCount > 0 )
	{
	DocumentField *	dfCh;

	dfCh= dfPa->dfChildFields.cfChildren[
					dfPa->dfChildFields.cfChildCount- 1];

	if  ( docCompareEditPositions( &(dfCh->dfTailPosition),
						&(dfPa->dfTailPosition) ) >= 0 )
	    { LDEB(1);	}
	}

    return;
    }

/************************************************************************/
/*  									*/
/*  Keep track of the hierarchy of fields. Do some elementary checks	*/
/*									*/
/*  NOTE: The stacked construction of fields makes that children are	*/
/*	added to the parent before the end position of the parent is	*/
/*	set: Do not verify the end position.				*/
/*  									*/
/************************************************************************/

int docAddChildToField(			DocumentField *		dfCh,
					DocumentField *		dfPa )
    {
    if  ( ! docSelectionSameScope( &(dfCh->dfSelectionScope),
					&(dfPa->dfSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( docCompareEditPositions( &(dfCh->dfHeadPosition),
					&(dfPa->dfHeadPosition) ) < 0 )
	{ LDEB(1); return -1;	}
    /* NO!
    if  ( docCompareEditPositions( &(dfCh->dfTailPosition),
					&(dfPa->dfTailPosition) ) > 0 )
	{ LDEB(1); return -1;	}
    */

    if  ( docInsertChildField( dfPa, &(dfPa->dfChildFields), dfCh ) )
	{ LDEB(1); return -1;	}
    dfCh->dfParent= dfPa;

    return 0;
    }

int docInsertFieldInTree(		ChildFields *		cf,
					DocumentField *		df )
    {
    DocumentField *	parent= (DocumentField *)0;

    if  ( df->dfChildFields.cfChildCount > 0 )
	{ LDEB(df->dfChildFields.cfChildCount); return -1;	}

    for (;;)
	{
	int		i0;
	int		i1;
	int		i;
	int		pos;
	int		step;

	/* Find the last field that is completely before the insert	*/
	i0= 0;
	while( i0 < cf->cfChildCount )
	    {
	    DocumentField *	df0= cf->cfChildren[i0];

	    if  ( docCompareEditPositions( &(df->dfTailPosition),
					    &(df0->dfHeadPosition) ) <= 0 )
		{ break;	}

	    i0++;
	    }
	i0--;

	/* Find the first field that is completely after the insert	*/
	i1= cf->cfChildCount- 1;
	while( i1 >= 0 )
	    {
	    DocumentField *	df1= cf->cfChildren[i1];

	    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					    &(df1->dfTailPosition) ) >= 0 )
		{ break;	}

	    i1--;
	    }
	i1++;

	/* between fields */
	if  ( i1 == i0+ 1 )
	    {
	    return docInsertFieldAt( parent, cf, df, i1 );
	    }

	/* inside a field: continue there */
	if  ( i0 == i1 )
	    {
	    DocumentField *	df1= cf->cfChildren[i1];

	    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					&(df1->dfHeadPosition) ) >= 0	&&
		  docCompareEditPositions( &(df->dfTailPosition),
					&(df1->dfTailPosition) ) <= 0	)
		{
		cf= &(df1->dfChildFields);
		parent= df1;
		continue;
		}
	    }

	/* Surround the fields found with the insert */
	if  ( i1 <= i0 )
	    {
	    pos= 0;
	    for ( i= i1; i <= i0; i++ )
		{
		if  ( docInsertFieldAt( df, &(df->dfChildFields),
						cf->cfChildren[i], pos++ ) )
		    { LDEB(pos); return -1;	}
		}
	    step= i0- i1+ 1;
	    cf->cfChildCount -= step;
	    i= i1;
	    while( i < cf->cfChildCount )
		{ cf->cfChildren[i]= cf->cfChildren[i+ step]; i++; }

	    return docInsertFieldAt( parent, cf, df, i1 );
	    }

	return docInsertFieldAt( parent, cf, df, i1 );
	}
    }

/************************************************************************/

static int docFieldPath(	DocumentField ***	pPath,
				DocumentField *		dfTo )
    {
    DocumentField *	df;
    int			deep= 0;
    DocumentField **	path= (DocumentField **)0;

    df= dfTo;
    while( df )
	{ deep++; df= df->dfParent;	}

    if  ( deep > 0 )
	{
	int		d= deep;

	path= (DocumentField **)malloc( deep* sizeof(DocumentField *) );
	if  ( ! path )
	    { LXDEB(deep,path); return -1;	}

	df= dfTo;
	while( df )
	    { path[--d]= df; df= df->dfParent;	}
	}

    *pPath= path;
    return deep;
    }

DocumentField *	docFieldGetCommonParent(	DocumentField *	dfHead,
						DocumentField *	dfTail )
    {
    DocumentField *	rval= (DocumentField *)0;
    DocumentField **	pathHead= (DocumentField **)0;
    DocumentField **	pathTail= (DocumentField **)0;

    int			deepHead;
    int			deepTail;
    int			deep;

    deepHead= docFieldPath( &pathHead, dfHead );
    deepTail= docFieldPath( &pathTail, dfTail );
    if  ( deepHead < 0 || deepTail < 0 )
	{ LLDEB(deepHead,deepTail); goto ready;	}

    for ( deep= 0; deep < deepHead && deep < deepTail; deep++ )
	{
	DocumentField *	df;

	if  ( pathHead[deep] != pathTail[deep] )
	    { break;	}

	df= pathHead[deep];
	if  ( df == dfHead )
	    { break;	}
	if  ( df == dfTail )
	    { break;	}

	rval= df;
	}

  ready:

    if  ( pathHead )
	{ free( pathHead );	}
    if  ( pathTail )
	{ free( pathTail );	}

    return rval;
    }

