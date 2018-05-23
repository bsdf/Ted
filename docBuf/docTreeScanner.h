#   ifndef		DOC_TREE_SCANNER_H
#   define		DOC_TREE_SCANNER_H

#   include		"docBuf.h"
#   include		"docSelect.h"

struct TreeScanner;

typedef int (*NodeVisitor)(	struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				through );

typedef enum TreeScanAdvice
    {
    ADVICEtsOK= 0,
    ADVICEtsSTOP,
    ADVICEtsSKIP,

    ADVICEts_COUNT
    } TreeScanAdvice;

#   define FLAGtsSCAN_MERGED_CELLS	0x01

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docScanTree(	BufferDocument *		bd,
			const DocumentTree *		dt,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			int				flags,
			void *				through );

extern int docScanSelection(
			BufferDocument *		bd,
			const DocumentSelection *	ds,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			int				flags,
			void *				through );

extern int docScanTreeNode(
			BufferDocument *		bd,
			struct BufferItem *		node,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			int				flags,
			void *				through );

#   endif	/*	DOC_TREE_SCANNER_H	*/
