#   ifndef	SIO_ENDIAN_H
#   define	SIO_ENDIAN_H

#   include	"sioGeneral.h"

extern int	sioEndianGetLeInt16(SimpleInputStream *);
extern long	sioEndianGetLeInt32(SimpleInputStream *);
extern int	sioEndianPutLeInt16(int, SimpleOutputStream *);
extern int	sioEndianPutLeInt32(long, SimpleOutputStream *);

extern int	sioEndianGetBeInt16(SimpleInputStream *);
extern long	sioEndianGetBeInt32(SimpleInputStream *);
extern int	sioEndianPutBeInt16(int, SimpleOutputStream *);
extern int	sioEndianPutBeInt32(long, SimpleOutputStream *);

extern unsigned int	sioEndianGetLeUint16(SimpleInputStream *);
extern unsigned long	sioEndianGetLeUint32(SimpleInputStream *);
extern int	sioEndianPutLeUint16(unsigned int, SimpleOutputStream *);
extern int	sioEndianPutLeUint32(unsigned long, SimpleOutputStream *);

extern unsigned int	sioEndianGetBeUint16(SimpleInputStream *);
extern unsigned long	sioEndianGetBeUint32(SimpleInputStream *);
extern int	sioEndianPutBeUint16(unsigned int, SimpleOutputStream *);
extern int	sioEndianPutBeUint32(unsigned long, SimpleOutputStream *);

extern float	sioEndianGetLeFloat(SimpleInputStream *);
extern int	sioEndianPutLeFloat(float, SimpleOutputStream *);

extern float	sioEndianGetBeFloat(SimpleInputStream *);
extern int	sioEndianPutBeFloat(float, SimpleOutputStream *);

#   endif	/* SIO_ENDIAN_H */
