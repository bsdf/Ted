#   ifndef		PS_TO_GLYPH_MAPPING_H
#   define		PS_TO_GLYPH_MAPPING_H

/************************************************************************/
/*									*/
/*  Map Unicode values to glyph numbers. Typically, a font has been	*/
/*  analyzed and we know the unicode values that belong to its glyphs.	*/
/*  Then inside a printing task, we have to to the other way: We have	*/
/*  unicodes and we want to know the glyph index that corresponds to	*/
/*  the unicode. As this is a per-character operation the		*/
/*  implementation must be fast.					*/
/*									*/
/*  Glyph indices are positive. All negative values indicate unmapped	*/
/*  characters.								*/
/*									*/
/************************************************************************/

typedef struct IndexMapping
    {
    int	**		imPages;
    int			imPageCount;
    } IndexMapping;

# define TGM_PSZ 256

# define utilIndexMappingGet( m, f ) \
	(((f)>=0&&(f)/TGM_PSZ<(m)->imPageCount&&(m)->imPages[(f)/TGM_PSZ])? \
	(m)->imPages[(f)/TGM_PSZ][(f)%TGM_PSZ]:-1)

# define utilIndexMappingGetU( m, f ) \
	(((f)/TGM_PSZ<(m)->imPageCount&&(m)->imPages[(f)/TGM_PSZ])? \
	(m)->imPages[(f)/TGM_PSZ][(f)%TGM_PSZ]:-1)

# define utilIndexMappingIsEmpty( m ) \
	( (m)->imPageCount == 0 )

typedef int (*IndexMappingForOne)(	int		from,
					int		to,
					void *		through );

/************************************************************************/
/*									*/
/*  Map a unicode value to a glyph index.				*/
/*									*/
/************************************************************************/

extern void utilInitIndexMapping(	IndexMapping *	im );
extern void utilCleanIndexMapping(	IndexMapping *	im );
extern void utilClearIndexMapping(	IndexMapping *	im );

extern int utilIndexMappingBuildBackward( IndexMapping *	im,
					const int *		forward,
					int			forwardCount );

extern int  utilIndexMappingPut(	IndexMapping *		im,
					int			from,
					int			to );

extern int utilIndexMappingAppend(	IndexMapping *		im,
					int *			pFrom,
					int			to );

extern int utilIndexMappingForAll(	const IndexMapping *	im,
					IndexMappingForOne	forOne,
					void *			through );

#   endif	/*	PS_TO_GLYPH_MAPPING_H	*/
