#   ifndef		UTIL_INTEGER_SET_H
#   define		UTIL_INTEGER_SET_H

/************************************************************************/
/*									*/
/*  Implement sets of (positive) integers. The implementation through	*/
/*  a paged bit mask was originally designed for 16 bit unicode values.	*/
/*									*/
/************************************************************************/

typedef struct IndexSet
    {
    unsigned char **	isPages;
    int			isPageCount;
    } IndexSet;

# define ISET_PSZB	256
# define ISET_PSZb	(8*ISET_PSZB)

# define ISET_PAGE(v)	((v)/ISET_PSZb)
# define ISET_PIDX(v)	(((v)/8)%ISET_PSZB)
# define ISET_MBIT(v)	((v)%8)
# define ISET_MASK(v)	(1<<ISET_MBIT((v)))

# define ISET_INDEX(p,i,b) ( 8*(ISET_PSZB*(p)+(i))+(b) )

# define utilIndexSetContains( s, v ) \
	((ISET_PAGE(v)<(s)->isPageCount&&(s)->isPages[ISET_PAGE(v)])&& \
	((s)->isPages[ISET_PAGE(v)][ISET_PIDX(v)]&ISET_MASK(v))!=0)

typedef int (*IndexSetForOne)(		int		val,
					void *		through );

/************************************************************************/
/*									*/
/*  Manipulate sets of integers.					*/
/*									*/
/************************************************************************/

extern void utilInitIndexSet(		IndexSet *		is );
extern void utilCleanIndexSet(		IndexSet *		is );

extern int utilCopyIndexSet(		IndexSet *		to,
					const IndexSet *	from );

extern int utilIndexSetAdd(		IndexSet *		is,
					int			v );

extern void utilIndexSetRemove(		IndexSet *		is,
					int			v );

extern int utilIndexSetIntersect(	IndexSet *		res,
					const IndexSet *	is1,
					const IndexSet *	is2 );

extern int utilIndexSetUnion(		IndexSet *		res,
					const IndexSet *	is1,
					const IndexSet *	is2 );

extern int utilIndexSetRemoveAll(	IndexSet *		res,
					const IndexSet *	is1,
					const IndexSet *	is2 );

extern int utilIndexSetForAll(		const IndexSet *	is,
					IndexSetForOne		forOne,
					void *			through );

extern int utilIndexSetGetNext(		const IndexSet *	is,
					int			n );
# define utilIndexSetGetFirst( is ) utilIndexSetGetNext( (is), -1 )

extern int utilIndexSetGetNextHole(	const IndexSet *	is,
					int			n );

extern int utilIndexSetGetPrev(		const IndexSet *	is,
					int			n );
# define utilIndexSetGetLast( is ) utilIndexSetGetPrev( (is), -1 )

extern int utilIndexSetAddArray(	IndexSet *		is,
					const int *		indexes,
					int			indexCount );

#   endif	/*	UTIL_INTEGER_SET_H	*/
