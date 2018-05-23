/************************************************************************/
/*									*/
/*  Administration of paper sizes					*/
/*									*/
/************************************************************************/

extern const char * utilPaperDefaultSize( void );

extern int utilPaperGetInfoByNumber(	int		n,
					int *		pWidth,
					int *		pHeight,
					const char **	pLabel );

extern int utilPaperGetNumberBySize(	int		width,
					int		height );

extern int utilPaperSizeFromString(	int *		pFormat,
					int *		pWidth,
					int *		pHeight,
					int		unitType,
					const char *	paperString );
