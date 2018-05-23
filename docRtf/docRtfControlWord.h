/************************************************************************/
/*									*/
/*  Control words in Read/Write RTF files.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_CONTROL_WORD_H
#   define	DOC_RTF_CONTROL_WORD_H

/************************************************************************/
/*									*/
/*  Map control words to functions that handle them.			*/
/*									*/
/*  This structure will be gradually extended to also cover the		*/
/*  Microsoft WordProcessingML markup language.				*/
/*									*/
/************************************************************************/

struct RtfControlWord;
struct RtfReader;

typedef int (*RtfApplyWord)(	const struct RtfControlWord *	rcw,
				int				arg,
				struct RtfReader *		rr );

typedef int (*RtfCommitGroup)(	const struct RtfControlWord *	rcw,
				struct RtfReader *		rr );

typedef enum RtfControlType
    {
    RTCtypeANY= 0,
    RTCtypeTOGGLE,
    RTCtypeDEST,
    RTCtypeSYMBOL,
    RTCtypeVALUE,
    RTCtypeFLAG,
    RTCtypeENUM,

    RTCtype_COUNT
    } RtfControlType;

typedef struct RtfControlWord
    {
    const char *	rcwWord;
    int			rcwID;
    unsigned char	rcwType;
			/*
			 * Routine to apply the countrol word.
			 */
    RtfApplyWord	rcwApply;
			/*
			 * Integer value that is the implicit argument to 
			 * the control word.
			 */
    int			rcwEnumValue;
			/*
			 * Routine to prepare for this word. It is used 
			 * in combination with de detail words for borders.
			 */
    RtfApplyWord	rcwPrepare;
			/*
			 * Method to handle text inside a group for this 
			 * control word.
			 */
    RtfCommitGroup	rwcCommitGroup;

			/*
			 * A terminated array of control words that 
			 * follow the control word to give additional 
			 * details. (Used for borders)
			 */
    struct RtfControlWord *	rcwDetailWords;
    } RtfControlWord;

#   define	TEDszRTFCONTROL		32

/************************************************************************/
/*									*/
/*  Shortcut macros for defining control words.				*/
/*									*/
/************************************************************************/

#   define RTF_DEST_CO( s, id, ap, co ) \
	    { s, id, RTCtypeDEST, ap, 0, (RtfApplyWord)0, co }
#   define RTF_DEST_XX( s, id, ap ) \
	    { s, id, RTCtypeDEST, ap, 0, (RtfApplyWord)0, (RtfCommitGroup)0 }

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#    endif	/*  DOC_RTF_CONTROL_WORD_H	*/
