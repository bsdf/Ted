/************************************************************************/
/*									*/
/*  UCD General Categories.						*/
/*									*/
/************************************************************************/

/**
 * To build the masks. Do not directly refer to these values.
 */
typedef enum UCDGeneralCategoryGroup
    {
    UCDgc_L= 1,		/*  Letter	*/
    UCDgc_M,		/*  Mark	*/
    UCDgc_N,		/*  Number	*/
    UCDgc_P,		/*  Punctuation	*/
    UCDgc_S,		/*  Symbol	*/
    UCDgc_Z,		/*  Separator	*/
    UCDgc_C,		/*  Other	*/

    UCDgc__COUNT
    } UCDGeneralCategoryGroup;

# define	UCDgc_X_SHIFT	5
# define	UCDgc_X_MASK	0xe0
# define	UCDgc_XX_MASK	0xf0

typedef enum UCDGeneralCategory
    {
	    /**********
	     * Offset for the letters. Do not directly refer to this value.
	     * Note that the order is different from that in the Unicode 
	     * standard to reserve a bit for the LC mask.
	     */
    UCDgc_L_MASK= ( UCDgc_L << UCDgc_X_SHIFT ),
	    /**
	     * Modifier_Letter: a modifier letter
	     */
    UCDgc_Lm,
	    /**
	     * Other_Letter: other letters, including syllables and ideographs
	     */
    UCDgc_Lo,
	    /**********
	     * Offset for the cased letters. Do not directly refer to
	     * this value.
	     */
    UCDgc_LC_MASK= ( ( UCDgc_L << UCDgc_X_SHIFT ) | 0x10 ),
	    /**
	     * Uppercase_Letter: an uppercase letter
	     */
    UCDgc_Lu,
	    /**
	     * Lowercase_Letter: a lowercase letter
	     */
    UCDgc_Ll,
	    /**
	     * Titlecase_Letter:
	     * a digraphic character, with first part uppercase
	     */
    UCDgc_Lt,

	    /**********
	     * Offset for the marks. Do not directly refer to this value.
	     */
    UCDgc_M_MASK= ( UCDgc_M << UCDgc_X_SHIFT ),
	    /**
	     * Nonspacing_Mark: a nonspacing combining mark (zero advance width)
	     */
    UCDgc_Mn,
	    /**
	     * Spacing_Mark: a spacing combining mark (positive advance width)
	     */
    UCDgc_Mc,
	    /**
	     * Enclosing_Mark: an enclosing combining mark
	     */
    UCDgc_Me,

	    /**********
	     * Offset for the numbers. Do not directly refer to this value.
	     */
    UCDgc_N_MASK= ( UCDgc_N << UCDgc_X_SHIFT ),
	    /**
	     * Decimal_Number: a decimal digit
	     */
    UCDgc_Nd,
	    /**
	     * Letter_Number: a letterlike numeric character
	     */
    UCDgc_Nl,
	    /**
	     * Other_Number: a numeric character of other type
	     */
    UCDgc_No,

	    /**********
	     * Offset for the punctuation. Do not directly refer to this value.
	     */
    UCDgc_P_MASK= ( UCDgc_P << UCDgc_X_SHIFT ),
	    /**
	     * Connector_Punctuation: a connecting punctuation mark, like a tie
	     */
    UCDgc_Pc,
	    /**
	     * Dash_Punctuation: a dash or hyphen punctuation mark
	     */
    UCDgc_Pd,
	    /**
	     * Open_Punctuation: an opening punctuation mark (of a pair)
	     */
    UCDgc_Ps,
	    /**
	     * Close_Punctuation: a closing punctuation mark (of a pair)
	     */
    UCDgc_Pe,
	    /**
	     * Initial_Punctuation: an initial quotation mark
	     */
    UCDgc_Pi,
	    /**
	     * Final_Punctuation: a final quotation mark
	     */
    UCDgc_Pf,
	    /**
	     * Other_Punctuation: a punctuation mark of other type
	     */
    UCDgc_Po,

	    /**********
	     * Offset for the signs. Do not directly refer to this value.
	     */
    UCDgc_S_MASK= ( UCDgc_S << UCDgc_X_SHIFT ),
	    /**
	     * Math_Symbol: a symbol of mathematical use
	     */
    UCDgc_Sm,
	    /**
	     * Currency_Symbol: a currency sign
	     */
    UCDgc_Sc,
	    /**
	     * Modifier_Symbol: a non-letterlike modifier symbol
	     */
    UCDgc_Sk,
	    /**
	     * Other_Symbol: a symbol of other type
	     */
    UCDgc_So,

	    /**********
	     * Offset for the spaces. Do not directly refer to this value.
	     */
    UCDgc_Z_MASK= ( UCDgc_Z << UCDgc_X_SHIFT ),
	    /**
	     * Space_Separator: a space character (of various non-zero widths)
	     */
    UCDgc_Zs,
	    /**
	     * Line_Separator: U+2028 LINE SEPARATOR only
	     */
    UCDgc_Zl,
	    /**
	     * Paragraph_Separator: U+2029 PARAGRAPH SEPARATOR only
	     */
    UCDgc_Zp,

	    /**********
	     * Offset for the other categories. Do not directly refer to
	     * this value.
	     */
    UCDgc_C_MASK= ( UCDgc_C << UCDgc_X_SHIFT ),
	    /**
	     * Control: a C0 or C1 control code
	     */
    UCDgc_Cc,
	    /**
	     * Format: a format control character
	     */
    UCDgc_Cf,
	    /**
	     * Surrogate: a surrogate code point
	     */
    UCDgc_Cs,
	    /**
	     * Private_Use: a private-use character
	     */
    UCDgc_Co,
	    /**
	     * Unassigned: a reserved unassigned code point or a noncharacter
	     */
    UCDgc_Cn
    } UCDGeneralCategory;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern int ucdGeneralCategory( int sym );
extern const char * ucdGeneralCategoryStr( int sym );

/************************************************************************/
/*									*/
/*  Probing macros							*/
/*									*/
/************************************************************************/

# define ucdIsL(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_L_MASK )
# define ucdIsLm(c)	( ucdGeneralCategory((c)) == UCDgc_Lm )
# define ucdIsLo(c)	( ucdGeneralCategory((c)) == UCDgc_Lo )
# define ucdIsLC(c)	( ( ucdGeneralCategory((c)) & UCDgc_XX_MASK ) == UCDgc_LC_MASK )
# define ucdIsLu(c)	( ucdGeneralCategory((c)) == UCDgc_Lu )
# define ucdIsLl(c)	( ucdGeneralCategory((c)) == UCDgc_Ll )
# define ucdIsLt(c)	( ucdGeneralCategory((c)) == UCDgc_Lt )

# define ucdIsM(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_M_MASK )
# define ucdIsMn(c)	( ucdGeneralCategory((c)) == UCDgc_Mn )
# define ucdIsMc(c)	( ucdGeneralCategory((c)) == UCDgc_Mc )
# define ucdIsMe(c)	( ucdGeneralCategory((c)) == UCDgc_Me )

# define ucdIsN(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_N_MASK )
# define ucdIsNd(c)	( ucdGeneralCategory((c)) == UCDgc_Nd )
# define ucdIsNl(c)	( ucdGeneralCategory((c)) == UCDgc_Nl )
# define ucdIsNo(c)	( ucdGeneralCategory((c)) == UCDgc_No )

# define ucdIsP(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_P_MASK )
# define ucdIsPc(c)	( ucdGeneralCategory((c)) == UCDgc_Pc )
# define ucdIsPd(c)	( ucdGeneralCategory((c)) == UCDgc_Pd )
# define ucdIsPs(c)	( ucdGeneralCategory((c)) == UCDgc_Ps )
# define ucdIsPe(c)	( ucdGeneralCategory((c)) == UCDgc_Pe )
# define ucdIsPi(c)	( ucdGeneralCategory((c)) == UCDgc_Pi )
# define ucdIsPf(c)	( ucdGeneralCategory((c)) == UCDgc_Pf )
# define ucdIsPo(c)	( ucdGeneralCategory((c)) == UCDgc_Po )

# define ucdIsS(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_S_MASK )
# define ucdIsSm(c)	( ucdGeneralCategory((c)) == UCDgc_Sm )
# define ucdIsSc(c)	( ucdGeneralCategory((c)) == UCDgc_Sc )
# define ucdIsSk(c)	( ucdGeneralCategory((c)) == UCDgc_Sk )
# define ucdIsSo(c)	( ucdGeneralCategory((c)) == UCDgc_So )

# define ucdIsZ(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_Z_MASK )
# define ucdIsZs(c)	( ucdGeneralCategory((c)) == UCDgc_Zs )
# define ucdIsZl(c)	( ucdGeneralCategory((c)) == UCDgc_Zl )
# define ucdIsZp(c)	( ucdGeneralCategory((c)) == UCDgc_Zp )

# define ucdIsC(c)	( ( ucdGeneralCategory((c)) & UCDgc_X_MASK ) == UCDgc_C_MASK )
# define ucdIsCc(c)	( ucdGeneralCategory((c)) == UCDgc_Cc )
# define ucdIsCf(c)	( ucdGeneralCategory((c)) == UCDgc_Cf )
# define ucdIsCs(c)	( ucdGeneralCategory((c)) == UCDgc_Cs )
# define ucdIsCo(c)	( ucdGeneralCategory((c)) == UCDgc_Co )
# define ucdIsCn(c)	( ucdGeneralCategory((c)) == UCDgc_Cn )

