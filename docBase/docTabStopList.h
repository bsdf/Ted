/************************************************************************/
/*									*/
/*  Tabs in a ruler.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TAB_STOP_LIST_H
#   define	DOC_TAB_STOP_LIST_H

#   include	"docTabStop.h"

typedef struct TabStopList
    {
    TabStop *	tslTabStops;
    short int	tslTabStopCount;
    } TabStopList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void docInitTabStopList(	TabStopList *		tsl );
void docCleanTabStopList(	TabStopList *		tsl );

extern int docAddTabToListTwips(	TabStopList *		tsl,
					const TabStop *		tsNew );

extern int docAddTabToListPixels(	TabStopList *		tsl,
					const TabStop *		tsNew );

extern void docDeleteTabFromList(	TabStopList *		tsl,
					int			n );

extern int docCopyTabStopList(	TabStopList *			to,
				const TabStopList *		from );

extern int docNextTabStop(	TabStop *			pTs,
				int *				pX,
				int *				pTab,
				const TabStopList *		tsl,
				int				x0Geometry,
				int				x0TextLines,
				int				tabInterval,
				int				xPosition );


extern int docRulerMergeTabs(	TabStopList *		tslTo,
				int			x0GeometryPixels,
				double			xfac,
				const TabStopList *	tslFrom );

#   endif	/*  DOC_TAB_STOP_LIST_H	*/
