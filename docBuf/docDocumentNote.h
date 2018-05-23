/************************************************************************/
/*									*/
/*  Pieces of text, physically implemented as a 'Section' thay are	*/
/*  outside the normal flow of text.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_NOTE_H
#   define	DOC_DOCUMENT_NOTE_H

#   include	"docDocumentTree.h"
#   include	<docEditPosition.h>
#   include	<docSelectionScope.h>
#   include	<docNoteProperties.h>

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;

/************************************************************************/
/*									*/
/*  A footnote/endnote/annotation.					*/
/*									*/
/*  1)  Though the notes are stored with the section, the paragraph	*/
/*	number is relative to the body of the document as a whole.	*/
/*									*/
/************************************************************************/

typedef struct DocumentNote
    {
    DocumentTree	dnDocumentTree;
    short int		dnNoteNumber;
    short int		dnReferringPage;
    short int		dnReferringColumn;
    NoteProperties	dnNoteProperties;
    } DocumentNote;

# define NOTE_IS_DELETED( dn )	((dn)->dnNoteNumber < 0 )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitNote(		DocumentNote *		dn );

#   endif
