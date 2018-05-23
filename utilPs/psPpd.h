/************************************************************************/
/*									*/
/*  PostScript Printer Descriptions.					*/
/*									*/
/************************************************************************/

typedef struct Ppd
    {
    char *		ppdAdobe;

    char *		ppdModelName;
    char *		ppdShortNickName;
    char *		ppdNickName;
    char *		ppdProduct;
    char *		ppdManufacturer;
    char *		ppdPSVersion;

    unsigned char	ppdColorDevice;
    int			ppdDefaultColorSpace;
    unsigned char	ppdFileSystem;
    } Ppd;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void psInitPpd(		Ppd *		ppd );
extern void psCleanPpd(		Ppd *		ppd );

extern int psReadPpdFile(	Ppd *		ppd,
				const char *	filename );
