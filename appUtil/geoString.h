/************************************************************************/
/*									*/
/*  Geometry Specification strings.					*/
/*									*/
/************************************************************************/

#   ifndef GEO_STRING_H
#   define GEO_STRING_H

extern void geoLengthToString(		char *		target,
					int		twips,
					int		unitInt );

extern void geoRectangleToString(	char *		target,
					int		widthTwips,
					int		heightTwips,
					int		unitInt );

extern int geoRectangleFromString(	const char *	s,
					int		defaultUnitInt,
					int *		pWidth,
					int *		pHeight );

extern int geoLengthFromString(		const char *	s,
					int		defaultUnitInt,
					int *		pValue );

#   endif /* GEO_STRING_H */
