#   ifndef	TAGVAL_H					/*  {{	*/
#   define	TAGVAL_H

/************************************************************************/
/*  Procedure declarations.						*/
/************************************************************************/

extern int appTagvalGetStringValue(	const char **		pValue,
					int *			pNull,
					const void *		tree,
					const char *		name );

extern int appTagvalGetLongValue(	long *			pValue,
					int *			pNull,
					const void *		tree,
					const char *		name );

extern int appTagvalGetDoubleValue(	double *		pValue,
					int *			pNull,
					const void *		tree,
					const char *		name );

#   endif						/*  }}	*/
