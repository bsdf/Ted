/************************************************************************/
/*									*/
/*  Manage tab stops.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<geoGrid.h>
#   include	"docTabStop.h"
#   include	"docTabStopList.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a tab stop.						*/
/*									*/
/************************************************************************/

void docInitTabStop(	TabStop *	ts )
    {
    ts->tsTwips= 0;
    ts->tsPixels= 0;
    ts->tsAlignment= DOCtaLEFT;
    ts->tsLeader= DOCtlNONE;
    ts->tsFromStyleOrList= 0;
    }

int docTabStopSetProperty(	TabStop *		ts,
				int			prop,
				int			val )
    {
    switch( prop )
	{
	case TABpropX:
	    ts->tsTwips= val;
	    return 0;

	case TABpropALIGN:
	    ts->tsAlignment= val;
	    return 0;

	case TABpropLEADER:
	    ts->tsLeader= val;
	    return 0;

	case TABpropFROM_STYLE:
	    ts->tsFromStyleOrList= val != 0;
	    return 0;

	default:
	    LDEB(prop); return -1;
	}
    }

int docTabStopGetProperty(	const TabStop *		ts,
				int			prop )
    {
    switch( prop )
	{
	case TABpropX:
	    return ts->tsTwips;

	case TABpropALIGN:
	    return ts->tsAlignment;

	case TABpropLEADER:
	    return ts->tsLeader;

	case TABpropFROM_STYLE:
	    return ts->tsFromStyleOrList;

	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Initialise a tab stop List.						*/
/*									*/
/************************************************************************/

void docInitTabStopList(	TabStopList *	tsl )
    {
    tsl->tslTabStopCount= 0;
    tsl->tslTabStops= (TabStop *)0;

    return;
    }

void docCleanTabStopList(	TabStopList *	tsl )
    {
    if  ( tsl->tslTabStops )
	{ free( tsl->tslTabStops );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a tab stop into the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

static int docAddTabToList(	TabStopList *		tsl )
    {
    TabStop *		ts;
    int			newSize;

    if  ( tsl->tslTabStopCount % 10 )
	{ newSize= tsl->tslTabStopCount+  1; }
    else{ newSize= tsl->tslTabStopCount+ 11; }

    newSize *= sizeof(TabStop);

    ts= (TabStop *)realloc( tsl->tslTabStops, newSize );
    if  ( ! ts )
	{ LXDEB(newSize,ts); return -1; }
    tsl->tslTabStops= ts;

    return 0;
    }

int docAddTabToListTwips(	TabStopList *		tsl,
				const TabStop *		tsNew )
    {
    TabStop *		ts;
    int			i;

    if  ( docAddTabToList( tsl ) )
	{ LDEB(tsl->tslTabStopCount); return -1;	}

    i= tsl->tslTabStopCount; ts= tsl->tslTabStops+ tsl->tslTabStopCount;
    while( i > 0 && ts[-1].tsTwips > tsNew->tsTwips )
	{ ts[0]= ts[-1]; i--; ts--;	}

    *ts= *tsNew;
    tsl->tslTabStopCount++;

    return i;
    }

int docAddTabToListPixels(	TabStopList *		tsl,
				const TabStop *		tsNew )
    {
    TabStop *		ts;
    int			i;

    if  ( docAddTabToList( tsl ) )
	{ LDEB(tsl->tslTabStopCount); return -1;	}

    i= tsl->tslTabStopCount; ts= tsl->tslTabStops+ tsl->tslTabStopCount;
    while( i > 0 && ts[-1].tsPixels > tsNew->tsPixels )
	{ ts[0]= ts[-1]; i--; ts--;	}

    *ts= *tsNew;
    tsl->tslTabStopCount++;

    return i;
    }

/************************************************************************/
/*									*/
/*  Remove a tab stop from the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

void docDeleteTabFromList(	TabStopList *		tsl,
				int			n )
    {
    TabStop *		ts;

    if  ( n < 0 || n >= tsl->tslTabStopCount )
	{ LLDEB(n,tsl->tslTabStopCount); return;	}

    tsl->tslTabStopCount--;

    ts= tsl->tslTabStops+ n;
    while( n < tsl->tslTabStopCount )
	{ ts[0]= ts[1]; ts++; n++;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Copy the list of tabs of one paragraph to another.			*/
/*									*/
/************************************************************************/

int docCopyTabStopList(		TabStopList *		to,
				const TabStopList *	from )
    {
    TabStop *	fresh= (TabStop *)0;

    if  ( from->tslTabStopCount > 0 )
	{
	int	i;

	fresh= (TabStop *)malloc( from->tslTabStopCount* sizeof(TabStop) );
	if  ( ! fresh )
	    { LXDEB(from->tslTabStopCount,fresh); return -1;	}

	for ( i= 0; i < from->tslTabStopCount; i++ )
	    { fresh[i]= from->tslTabStops[i];	}
	}

    if  ( to->tslTabStops )
	{ free( to->tslTabStops );	}
    to->tslTabStops= fresh;
    to->tslTabStopCount= from->tslTabStopCount;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the position of the next tab.					*/
/*									*/
/*  1)  The automatic tab indent implied by the hanging indent.		*/
/*									*/
/************************************************************************/

int docNextTabStop(	TabStop *			pTs,
			int *				pX,
			int *				pTab,
			const TabStopList *		tsl,
			int				x0Geometry,
			int				x0TextLines,
			int				tabInterval,
			int				xPosition )
    {
    int				tab;
    const TabStop *		ts= tsl->tslTabStops;

    if  ( tsl->tslTabStopCount == 0 )
	{
	/*  1  */
	if  ( xPosition < x0TextLines )
	    { *pX= x0TextLines; *pTab= -1; return 0; }

	if  ( tabInterval == 0 )
	    {
	    LLDEB(tsl->tslTabStopCount,tabInterval);
	    *pX= xPosition; *pTab= -1; return 0;
	    }
	else{
	    *pTab= -1;

	    xPosition -= x0Geometry;
	    *pX= ( xPosition+ 1 )/ tabInterval;
	    *pX *= tabInterval;
	    *pX += tabInterval;
	    *pX += x0Geometry;
	    return 0;
	    }
	}
    else{
	for ( tab= 0; tab < tsl->tslTabStopCount; ts++, tab++ )
	    {
	    if  ( ts->tsTwips > xPosition- x0Geometry )
		{ break;	}
	    }

	if  ( tab >= tsl->tslTabStopCount )
	    {
	    /*  1  */
	    if  ( xPosition < x0TextLines )
		{ *pX= x0TextLines; *pTab= -1; return 0; }

	    if  ( tabInterval > 0 )
		{
		xPosition -= x0Geometry;
		*pX= ( xPosition+ 1 )/ tabInterval;
		*pX *= tabInterval;
		*pX += tabInterval;
		*pX += x0Geometry;
		*pTab= -1;

		return 0;
		}
	    else{ LDEB(tabInterval); return 1;	}
	    }

	/*  1  */
	if  ( xPosition < x0TextLines					&&
	      ( ts->tsTwips > x0TextLines			||
		( tab == 0 && ts->tsAlignment != DOCtaLEFT )	)	)
	    { *pX= x0TextLines; *pTab= -1; return 0; }

	*pTs= *ts;
	*pTab= tab;
	*pX= ts->tsTwips+ x0Geometry;
	return 0;
	}
    }

/************************************************************************/
/*									*/
/*  A new series of tab stops is returned from the ruler. It is the	*/
/*  result of some user action on the current ruler. Merge the changes	*/
/*  into the current ruler.						*/
/*									*/
/*  In all practical situaltions, exactly one tab is moved, OR exactly	*/
/*  one tab is added OR exactly one tab is deleted. This routine is	*/
/*  written to handle any set of changes, but it tries to change as few	*/
/*  positions in twips as possible: Pixels are less precise and		*/
/*  documents would change all the time because tabs have moved a few	*/
/*  invisible twips.							*/
/*									*/
/************************************************************************/

int docRulerMergeTabs(		TabStopList *		tslOld,
				int			x0GeometryPixels,
				double			xfac,
				const TabStopList *	tslNew )
    {
    int			rval= 0;
    TabStop *		fresh= (TabStop *)0;

    if  ( tslNew->tslTabStopCount > 0 )
	{
	int		o= 0;
	int		n= 0;

	fresh= (TabStop *)malloc( tslNew->tslTabStopCount* sizeof(TabStop) );
	if  ( ! fresh )
	    { LXDEB(tslNew->tslTabStopCount,fresh); rval= -1; goto ready; }

	while( n < tslNew->tslTabStopCount )
	    {
	    /*
	     *  Copy tab stops from old with unchanged pixel values.
	     */
	    while( n < tslNew->tslTabStopCount		&&
		   o < tslOld->tslTabStopCount		)
		{
		int	nPixels= tslNew->tslTabStops[n].tsPixels;
		int	oTwips= tslOld->tslTabStops[o].tsTwips;
		int	oPixels= x0GeometryPixels+ COORDtoGRID( xfac, oTwips );

		if  ( oPixels != nPixels )
		    { break;	}

		fresh[n]= tslOld->tslTabStops[o];
		fresh[n].tsTwips= tslOld->tslTabStops[o].tsTwips;
		fresh[n].tsAlignment= tslOld->tslTabStops[o].tsAlignment;
		fresh[n].tsLeader= tslOld->tslTabStops[o].tsLeader;
		fresh[n].tsFromStyleOrList=
				    tslOld->tslTabStops[o].tsFromStyleOrList;
		fresh[n].tsPixels= nPixels;
		n++; o++;
		}

	    /*
	     *  Skip tab stops from old with lower pixel values.
	     */
	    while( n < tslNew->tslTabStopCount		&&
		   o < tslOld->tslTabStopCount		)
		{
		int	nPixels= tslNew->tslTabStops[n].tsPixels;
		int	oTwips= tslOld->tslTabStops[o].tsTwips;
		int	oPixels= x0GeometryPixels+ COORDtoGRID( xfac, oTwips );

		if  ( oPixels >= nPixels )
		    { break;	}

		o++;
		}

	    /*
	     *  Copy tab stops from new with lower pixel values.
	     *  (Or at tail)
	     */
	    while( n < tslNew->tslTabStopCount )
		{
		int	nPixels= tslNew->tslTabStops[n].tsPixels;

		if  ( o < tslOld->tslTabStopCount )
		    {
		    int	oTwips= tslOld->tslTabStops[o].tsTwips;
		    int	oPixels= x0GeometryPixels+ COORDtoGRID( xfac, oTwips );

		    if  ( oPixels <= nPixels )
			{ break;	}
		    }

		fresh[n]= tslNew->tslTabStops[n];
		fresh[n].tsTwips= GRIDtoCOORD(
		    tslNew->tslTabStops[n].tsPixels- x0GeometryPixels, xfac );
		n++;
		}
	    }
	}

    /*  steal by swapping */
    {
    TabStop * sw= tslOld->tslTabStops;
		  tslOld->tslTabStops= fresh;
		                       fresh= sw;
    tslOld->tslTabStopCount= tslNew->tslTabStopCount;
    }

  ready:

    if  ( fresh )
	{ free( fresh );	}

    return rval;
    }

const char * docTabAlignmentStr(	int	alignment )
    {
    static char	scratch[20];

    switch( alignment )
	{
	case DOCtaLEFT:		return "LEFT";
	case DOCtaRIGHT:	return "RIGHT";
	case DOCtaCENTER:	return "CENTER";
	case DOCtaDECIMAL:	return "DECIMAL";

	default:
	    sprintf( scratch, "%d", alignment );
	    return scratch;
	}
    }

const char * docTabLeaderStr(	int	leader )
    {
    static char	scratch[20];

    switch( leader )
	{
	case DOCtlNONE:		return "NONE";
	case DOCtlDOTS:		return "DOTS";
	case DOCtlUNDERLINE:	return "UNDERLINE";
	case DOCtlHYPH:		return "HYPH";
	case DOCtlTHICK:	return "THICK";
	case DOCtlEQUAL:	return "EQUAL";

	default:
	    sprintf( scratch, "%d", leader );
	    return scratch;
	}
    }

