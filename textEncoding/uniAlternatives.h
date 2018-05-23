/************************************************************************/
/*									*/
/*  Map unicodes to alternatives.					*/
/*									*/
/*  The purpose is not to reject a font because it misses a few glyphs	*/
/*  and resonable alternatives are available.				*/
/*									*/
/************************************************************************/

#   ifndef	UNI_ALTERNATIVES_H
#   define	UNI_ALTERNATIVES_H

#   include	<utilIndexSet.h>
#   include	<utilIndexMapping.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int uniGetAlternative(		int 			unicode );

extern int uniIncludeWithAlternatives(	IndexSet *		is );

extern int uniMapToAlternatives(	IndexMapping *		im );

#   endif /*	UNI_ALTERNATIVES_H	*/
