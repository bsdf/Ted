/************************************************************************/
/*									*/
/*  Translation between unicode values and utf-8.			*/
/*									*/
/************************************************************************/

#   ifdef	__cplusplus
    extern "C"  {
#   endif

/************************************************************************/

extern int uni_GetUtf8(		unsigned short *	pSymbol,
				const char *		buf );

extern int uni_PutUtf8(		char *			buf,
				int			symbol );

/*
# define uniGetUtf8(ps,b) (*(b)<128?((*(ps)=*(b)),1):uni_GetUtf8((ps),(b)))
# define uniPutUtf8(b,s) ((s)<128?((*(b)=(s)),1):uni_PutUtf8((b),(s)))
*/
# define uniGetUtf8(ps,b) uni_GetUtf8((ps),(b))
# define uniPutUtf8(b,s) uni_PutUtf8((b),(s))

extern unsigned short * uniUtf8ToUnicodes(
				int *		pUlen,
				const char *	word );

extern char * uniUnicodesToUtf8(
				int *			pMBlen,
				const unsigned short *	codes );

#   ifdef	__cplusplus
    }
#   endif

