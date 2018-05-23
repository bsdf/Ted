/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in UTF16 format 	*/
/*  while the program communicates in UTF8 format.			*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInUtf8_16Open(
				    SimpleInputStream * 	sisUtf16,
				    int				endian );

extern SimpleOutputStream * sioOutUtf8_16Open(
				    SimpleOutputStream *	sosUtf16,
				    int				endian,
				    int				bom );
