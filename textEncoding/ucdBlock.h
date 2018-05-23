/************************************************************************/
/*									*/
/*  A block in the UCD database.					*/
/*									*/
/************************************************************************/

typedef struct UcdBlock
    {
    unsigned int	ubFirst;
    unsigned int	ubLast;
    const char *	ubDescription;
    } UcdBlock;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern const UcdBlock	UCD_Blocks[];
extern const int	UCD_BlockCount;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int ucdGetBlock(	int sym );

