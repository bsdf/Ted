/************************************************************************/
/*									*/
/*  Translation between unicode values and utf-8.			*/
/*  Case shifts.							*/
/*									*/
/************************************************************************/

#   ifdef	__cplusplus
    extern "C"  {
#   endif

# define SHIFT_ASIS	0
# define SHIFT_LOWER	1
# define SHIFT_UPPER	2
# define SHIFT_FIRSTCAP	3

/************************************************************************/

extern int uniShiftUtf8String(	char **			pShiftedString,
				int **			pSegments,
				int *			pSegmentCount,
				int			how,
				const char *		string,
				int			len );

#   ifdef	__cplusplus
    }
#   endif

