/************************************************************************/
/*									*/
/*  Positions in a document. To prevent crashes, all references are	*/
/*  to the paragraph number, rather than the paragraph object.		*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_POSITION_H
#   define		DOC_EDIT_POSITION_H

typedef enum SelectionPosition
    {
	    /**
	     * After the edit action, the selection is at the head of 
	     * the modified region.
	     */
    SELposHEAD= 0,
	    /**
	     * After the edit action, the selection is at the head of 
	     * the modified region. But as we split the paragraph, 
	     * the position shows up as the position after the split.
	     */
    SELposNEXT,
	    /**
	     * After the edit action, the modified region is selected.
	     */
    SELposALL,
	    /**
	     * After the edit action, the selection is at the tail of 
	     * the modified region.
	     */
    SELposTAIL,
	    /**
	     * After the edit action, the selection is before the 
	     * deleted region.
	     */
    SELposBEFORE,
	    /**
	     * After the edit action, the selection is after the 
	     * deleted region.
	     */
    SELposAFTER,

    SELpos_COUNT
    } SelectionPosition;

typedef struct EditPosition
    {
    int		epParaNr;
    int		epStroff;
    } EditPosition;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitEditPosition(		EditPosition *	ep );

extern void docAdjustEditPositionOffsetB(	EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		stroffShift );

extern void docAdjustEditPositionOffsetE(	EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		stroffShift );

extern void docAdjustEditPositionB(		EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		paraShift,
						int		stroffShift );

extern void docAdjustEditPositionE(		EditPosition *	ep,
						int		paraNr,
						int		stroff,
						int		paraShift,
						int		stroffShift );

extern int docCompareEditPositions(		const EditPosition *	ep1,
						const EditPosition *	ep2 );

#   endif	/*	DOC_EDIT_POSITION_H	*/
