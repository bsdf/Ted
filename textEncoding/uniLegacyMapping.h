/************************************************************************/
/*									*/
/*  Two unicode to byte mappings for legacy encodings.			*/
/*									*/
/************************************************************************/

#   ifndef	UNI_LEGACY_MAPPING_H
#   define	UNI_LEGACY_MAPPING_H

#   include	<utilIndexMapping.h>
#   include	<utilIndexSet.h>

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern IndexMapping UNI_SymbolToGlyphMapping;
extern IndexMapping UNI_DingbatsToGlyphMapping;

extern IndexSet UNI_SymbolCodeSet;
extern IndexSet UNI_DingbatsCodeSet;

#   endif
