/************************************************************************/
/*									*/
/*  UTF-8 Calculations.							*/
/*									*/
/************************************************************************/

#   ifdef	__cplusplus
    extern "C"  {
#   endif

# define UTF_FH		0x80
# define UTF_FM		0x3f
# define UTF_IS_F(b)	(((b)&0xc0)==UTF_FH)

typedef struct Utf8Range
    {
    unsigned int	urFirstPast;
    unsigned int	urFirstMask;
    unsigned int	urFirstHead;
    unsigned int	urValuePast;
    } Utf8Range;

extern const Utf8Range Utf8Ranges[];

extern const int Utf8RangeCount;

extern int unixGetUtf8(	unsigned int *		pSymbol,
			const unsigned char *	buf );

#   ifdef	__cplusplus
    }
#   endif

