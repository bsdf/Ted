#   include	<sioGeneral.h>
#   include	"psTtfTableEntry.h"

typedef struct TrueTypeNameRecord
    {
    unsigned int	ttnrPlatformID;		/*  1  */
    unsigned int	ttnrEncodingID;		/*  2  */
    unsigned int	ttnrLanguageID;
    unsigned int	ttnrNameID;
    unsigned int	ttnrLength;
    unsigned int	ttnrOffset;

    unsigned char *	ttnrSavedBytes;
    } TrueTypeNameRecord;

typedef struct TrueTypeNameTable
    {
    unsigned int		ttntFormat;
    unsigned int		ttntRecordCount;
    unsigned int		ttntOffset;

    TrueTypeNameRecord *	ttntRecords;
    } TrueTypeNameTable;

extern void psTtfInitTrueTypeNameRecord(	TrueTypeNameRecord *	ttnr );
extern void psTtfCleanTrueTypeNameRecord(	TrueTypeNameRecord *	ttnr );
extern void psTtfInitTrueTypeNameTable(		TrueTypeNameTable *	ttnt );
extern void psTtfCleanTrueTypeNameTable(	TrueTypeNameTable *	ttnt );

extern int psTtfExtractNameRecords(	SimpleInputStream *		sisName,
					const TrueTypeTableEntry *	ttte,
					TrueTypeNameTable *		ttnt );

extern int psTtfGetName(	char **				pName,
				const TrueTypeNameTable *	ttnt,
				int				which );

