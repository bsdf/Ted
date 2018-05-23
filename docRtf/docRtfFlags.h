/************************************************************************/
/*									*/
/*  Read/Write RTF files:						*/
/*  Special flags to influence the behavior of readers and writers.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_FLAGS_H
#   define	DOC_RTF_FLAGS_H

					/********************************/
					/* Read/Write flags		*/
					/********************************/
					/**
					 * The RTF file is in UTF8. This is 
					 * for internal purposes.
					 */
#   define	RTFflagUNENCODED	0x001

					/********************************/
					/* Write flags			*/
					/********************************/
#   define	RTFflagNO_BOOKMARKS	0x004
#   define	RTFflagNO_MERGEFIELDS	0x008
#   define	RTFflagCLOSE_SELECTION	0x010
#   define	RTFflagSAVE_SOMETHING	0x020
#   define	RTFflagOMIT_TEXT	0x040
#   define	RTFflagOMIT_PARAS	0x080
					/**
					 *  Emit the row and the cell for 
					 *  selections that are constrained to 
					 *  a single table cell.
					 */
#   define	RTFflagEMIT_CELL	0x100

					/********************************/
					/* Read flags			*/
					/********************************/
					/**
					 * Be a little lenient while reading 
					 * RTF. E.G: Do not fail on unmatched 
					 * braces.
					 */
#   define	RTFflagLENIENT		0x200

#    endif	/*  DOC_RTF_FLAGS_H	*/
