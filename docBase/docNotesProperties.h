/************************************************************************/
/*									*/
/*  Note related Properties.						*/
/*									*/
/************************************************************************/

#   include	<utilPropMask.h>

#   ifndef	DOC_NOTES_PROPERTIES_H
#   define	DOC_NOTES_PROPERTIES_H

#   define	DPfetFOOT_ONLY		0
#   define	DPfetEND_ONLY		1
#   define	DPfetFOOT_END_BOTH	2

/************************************************************************/
/*									*/
/*  Placement of notes.							*/
/*									*/
/*  Experimentation shows that if multiple sections in a document	*/
/*  are on the same page, that \sftntj placement places the notes at	*/
/*  the bottom of the section and that \sftnbj places them at the	*/
/*  bottom of the page.							*/
/*									*/
/************************************************************************/

typedef enum NotesJustification
    {
    FTNjustifyBELOW_TEXT= 0,	/*  [s][a]ftntj				*/
    FTNjustifyPAGE_BOTTOM,	/*  [s][a]ftnbj				*/

    FTNjustify_COUNT
    } NotesJustification;

typedef enum NotesPlacement
    {
	/**
	 * No explicit tag in RTF.
	 */
    FTNplacePAGE_END= 0,
	/**
	 *  [a]endnotes: No sect variant
	 *  The property is ignored for footnotes.
	 */
    FTNplaceSECT_END,
	/**
	 *  [a]enddoc: No sect variant
	 *  The property is ignored for footnotes.
	 */
    FTNplaceDOC_END,

    FTNplace_COUNT
    } NotesPlacement;

typedef enum NotesRestart
    {
    FTN_RST_CONTINUOUS= 0,
    FTN_RST_PER_SECTION,
    FTN_RST_PER_PAGE,

    FTN_RST__COUNT
    } NotesRestart;

typedef enum NotesNumberStyle
    {
    FTNstyleNAR= 0,
    FTNstyleNALC,
    FTNstyleNAUC,
    FTNstyleNRLC,
    FTNstyleNRUC,
    FTNstyleNCHI,

    FTNstyle_COUNT
    } NotesNumberStyle;

/**
 *  Notes properties. Never directly used.
 *  The document and the section have their own property numbers.
 */
typedef enum NotesProperty
    {
    NOTESpropSTARTNR= 0,
    NOTESpropJUSTIFICATION,
    NOTESpropPLACEMENT,
    NOTESpropRESTART,
    NOTESpropSTYLE,

    NOTESprop_COUNT
    } NotesProperty;

typedef enum FootEndNotesProperty
    {
    FEPpropFOOT_STARTNR= 0,
    FEPpropFOOT_JUSTIFICATION,
    FEPpropFOOT_PLACEMENT,
    FEPpropFOOT_RESTART,
    FEPpropFOOT_STYLE,

    FEPpropEND_STARTNR,
    FEPpropEND_JUSTIFICATION,
    FEPpropEND_PLACEMENT,
    FEPpropEND_RESTART,
    FEPpropEND_STYLE,

    FEPprop_COUNT
    } FootEndNotesProperty;

typedef struct NotesProperties
    {
    int			npStartNumber;
    unsigned char	npJustification;
    unsigned char	npPlacement;		/*  Ignored for sect	*/
    unsigned char	npRestart;
    unsigned char	npNumberStyle;
    } NotesProperties;

typedef struct FootEndNotesProperties
    {
    NotesProperties	fepFootnotesProps;
    NotesProperties	fepEndnotesProps;
    } FootEndNotesProperties;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docUpdFootEndNotesProperties(
		PropertyMask *			pDoneMask,
		FootEndNotesProperties *	fep,
		const PropertyMask *		setMask,
		const FootEndNotesProperties *	fepSet,
		const int			fepPropMap[FEPprop_COUNT] );

extern int docFootEndNotesPropertyDifference(
		PropertyMask *			pDifMask,
		const FootEndNotesProperties *	fep1,
		const PropertyMask *		cmpMask,
		const FootEndNotesProperties *	fep2,
		const int			fepPropMap[FEPprop_COUNT] );

extern int docSetNotesProperty(		NotesProperties *	np,
					int			prop,
					int			val );

extern int docGetNotesProperty(		const NotesProperties *	np,
					int			prop );

extern void docInitNotesProperties(		NotesProperties *	np );
extern void docInitFootEndNotesProperties(	FootEndNotesProperties * fep );

extern void docDefaultFootNotesProperties(	NotesProperties *	np );
extern void docDefaultEndNotesProperties(	NotesProperties *	np );
extern void docDefaultFootEndNotesProperties(	FootEndNotesProperties * fep );

extern const char * docNotesJustificationStr(	int		val );
extern const char * docNotesPlacementStr(	int		val );

#   endif	/*  DOC_NOTES_PROPERTIES_H  */
