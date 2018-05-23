/************************************************************************/
/*									*/
/*  Manage notes.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	<docDocumentNote.h>
#   include	"docEditImpl.h"

/************************************************************************/
/*									*/
/*  Make a note at the given position.					*/
/*									*/
/*  1)  Claim memory for a note.					*/
/*  2)  Insert a particule that refers to the note.			*/
/*  4)  Insert an empty paragraph in the note to begin with.		*/
/*  5)  Make a chftn field to represent the note in the external	*/
/*	paragraph.							*/
/*  6)  Insert a space after the note number field. This also is to	*/
/*	work around a bug in the editing around fields.			*/
/*									*/
/*  !)  The chftn field that precedes the note that was inserted by the	*/
/*	caller.								*/
/*									*/
/************************************************************************/

int docEditMakeNote(	DocumentNote **			pDn,
			BufferDocument *		bd,
			DocumentField *			dfNote,
			const BufferItem *		bodyNode,
			int				treeType,
			int				fieldKind )
    {
    TextAttribute	ta;
    int			textAttributeNumberPlain;
    int			textAttributeNumberSuper;

    DocumentNote *	dn;

    int			noteIndex;

    int			stroffShift= 0;
    const int		autoNumber= 1;

    BufferItem *	noteParaNode;

    DocumentField *	dfHead;
    DocumentSelection	dsInsideHead;
    DocumentSelection	dsAroundHead;
    int			partHead= -1;
    int			partTail= -1;

    TextParticule *	tp;

    /*  1  */
    noteIndex= docInsertNote( &dn, bd, dfNote, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return -1;	}

    dn->dnNoteProperties.npTreeType= treeType;

    docPlainTextAttribute( &ta, bd );
    ta.taFontSizeHalfPoints= 20;

    textAttributeNumberPlain= docTextAttributeNumber( bd, &ta );

    /*  4  */
    noteParaNode= docMakeExternalParagraph( bd, &(dn->dnDocumentTree), treeType,
					bodyNode, dfNote->dfFieldNumber,
					textAttributeNumberPlain );
    if  ( ! noteParaNode )
	{ XDEB(noteParaNode); return -1;	}

    {
    TextAttribute	taSet;

    taSet= ta;

    taSet.taSuperSub= TEXTvaSUPERSCRIPT;

    textAttributeNumberSuper= docTextAttributeNumber( bd, &taSet );
    }

    /*  5  */
    if  ( docInsertParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
		    &partHead, &partTail,
		    noteParaNode, bd, &(dn->dnDocumentTree),
		    fieldKind, textAttributeNumberSuper ) )
	{ LDEB(1); return -1;	}
    noteParaNode->biParaParticules[partTail].tpTextAttrNr=
						    textAttributeNumberPlain;
    /*  6  */
    if  ( docParaStringReplace( &stroffShift, noteParaNode,
					    dsAroundHead.dsTail.dpStroff,
					    dsAroundHead.dsTail.dpStroff,
					    " ", 1 ) )
	{ LDEB(dsAroundHead.dsTail.dpStroff); return -1; }

    tp= &(noteParaNode->biParaParticules[noteParaNode->biParaParticuleCount- 1]);
    if  ( tp->tpKind == DOCkindSPAN		&&
	  tp->tpStrlen == 0			)
	{
	tp->tpStrlen++;
	tp->tpTextAttrNr= textAttributeNumberPlain;
	}
    else{
	tp= docInsertTextParticule( noteParaNode,
				    noteParaNode->biParaParticuleCount,
				    docParaStrlen( noteParaNode )- 1, 1,
				    DOCkindSPAN, textAttributeNumberPlain );
	if  ( ! tp )
	    { LDEB(noteParaNode->biParaParticuleCount); return -1; }
	}

    *pDn= dn; return 0;
    }

