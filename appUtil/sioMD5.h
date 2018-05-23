/************************************************************************/
/*									*/
/*  Read/Write data and calculate an MD5 on the fly.			*/
/*									*/
/*  NOTE: That the digest of an input stream only makes sense if you	*/
/*	read the whole file. Of you do not, the digest is about what	*/
/*	was buffered.. Not about what you read or about the file.	*/
/*									*/
/************************************************************************/

#   include	"sioGeneral.h"

extern SimpleInputStream * sioInMD5Open(
					unsigned char *		digest,
					SimpleInputStream *	sisIn );

extern SimpleOutputStream * sioOutMD5Open(
					unsigned char *		digest,
					SimpleOutputStream *	sosOut );

