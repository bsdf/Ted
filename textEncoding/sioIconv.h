/************************************************************************/
/*									*/
/*  Simple io streams for character set conversions.			*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInIconvOpen(
					SimpleInputStream *	sisIn,
					const char *		tocode,
					const char *		frcode );

extern SimpleOutputStream * sioOutIconvOpen(
					SimpleOutputStream *	sosOut,
					const char *		tocode,
					const char *		frcode );

