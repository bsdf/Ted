/************************************************************************/
/*									*/
/*  Tabs in a ruler.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TAB_STOP_H
#   define	DOC_TAB_STOP_H

typedef enum TabAlignment
    {
    DOCtaLEFT= 0,
    DOCtaRIGHT,
    DOCtaCENTER,
    DOCtaDECIMAL,

    DOCta_COUNT
    } TabAlignment;

typedef enum TabLeader
    {
    DOCtlNONE= 0,

    DOCtlDOTS,
    DOCtlUNDERLINE,
    DOCtlHYPH,

    DOCtlTHICK,
    DOCtlEQUAL,

    DOCtl_COUNT
    } TabLeader;

typedef struct TabStop
    {
			/**
			 *  Position in twips
			 */
    int			tsTwips;
			/**
			 *  Alignment: A DOCta* value
			 */
    unsigned char	tsAlignment;
			/**
			 *  Tab leader: a DOCtl* value
			 */
    unsigned char	tsLeader;
			/**
			 *  True if the tab comes from a style or a
			 *  list.
			 */
    unsigned char	tsFromStyleOrList;
			/**
			 *  Position in pixels. A derived property.
			 */
    short int		tsPixels;
    } TabStop;

typedef enum TabProperty
    {
    TABpropX= 0,
    TABpropALIGN,
    TABpropLEADER,
    TABpropFROM_STYLE,

    TABprop_COUNT
    } TabProperty;

# define docEqualTabStop( a, b ) ( \
		(a)->tsTwips == (b)->tsTwips				&& \
		(a)->tsAlignment == (b)->tsAlignment			&& \
		(a)->tsLeader == (b)->tsLeader				&& \
		(a)->tsFromStyleOrList == (b)->tsFromStyleOrList	)


/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTabStop(	TabStop *		ts );

extern int docTabStopGetProperty(	const TabStop *		ts,
					int			prop );

extern int docTabStopSetProperty(	TabStop *		ts,
					int			prop,
					int			val );

extern const char * docTabAlignmentStr(	int	alignment );
extern const char * docTabLeaderStr(	int	leader );

#   endif	/*  DOC_TAB_STOP_H	*/
