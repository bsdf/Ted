#   ifndef		PSTTF_TABLE_ENTRY_H
#   define		PSTTF_TABLE_ENTRY_H

#   include	<sioGeneral.h>
#   include	<utilMemoryBuffer.h>

typedef struct TrueTypeTableEntry
    {
    char		ttteTag[4+1];
    unsigned long	ttteChecksum;
    unsigned long	ttteOffset;
    MemoryBuffer	ttteMemoryBuffer;

    unsigned long	ttteSaveOffset;
    } TrueTypeTableEntry;

extern void utilInitTrueTypeTableEntry(		TrueTypeTableEntry *	ttte );
extern void utilCleanTrueTypeTableEntry(	TrueTypeTableEntry *	ttte );

extern TrueTypeTableEntry * psTtfReadTableEntries(
					SimpleInputStream *	sisTtf,
					int			tableCount,
					int			filePos );

#   endif	/*	PSTTF_TABLE_ENTRY_H	*/
