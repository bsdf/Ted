/************************************************************************/
/*									*/
/*  Operation names.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EDIT_COMMAND_H
#   define	DOC_EDIT_COMMAND_H

typedef enum EditCommand
    {
		/**
		 *  Convert the selection to a field. This is a special 
		 *  case: Only Hyperlinks and Bookmarks have content that 
		 *  comes from the document itself. Other kinds of field 
		 *  calculate their content. They are inserted through a 
		 *  REPLACE.
		 *
		 *  If there is an attribute change, we must remember that
		 *  and the old contents to restore the attributes as well.
		 */
    EDITcmdSET_HYPERLINK= 0,
    EDITcmdSET_BOOKMARK,

		/**
		 *  Delete a field.
		 *
		 *  If there is an attribute change, we must remember that
		 *  and the old contents to restore the attributes as well.
		 *
		 *  Uses the FieldKind for repeats.
		 */
    EDITcmdDEL_FIELD,

		/**
		 *  Change the properties of a field. NOT the type.
		 *  There will be no attribute change.
		 *
		 *  Uses the FieldKind for repeats.
		 */
    EDITcmdUPD_FIELD,

		/**
		 *  Change some properties of the first list that can be 
		 *  found in the current selection.
		 */
    EDITcmdUPD_LIST,
		/**
		 *  Make a new list and include the paragraphs in the current
		 *  selection in it.
		 */
    EDITcmdSET_NEW_LIST,
		/**
		 *  Add a note.
		 */
    EDITcmdINS_NOTE,
		/**
		 *  Change some properties of a note.
		 */
    EDITcmdUPD_NOTE,
		/**
		 *  Change some properties of a selection.
		 *  From text attributes to document properties.
		 */
    EDITcmdUPD_SPAN_PROPS,
    EDITcmdUPD_PARA_PROPS,
    EDITcmdUPD_TABLE_PROPS,
    EDITcmdUPD_SECT_PROPS,
    EDITcmdUPD_SECTDOC_PROPS,
    EDITcmdUPD_DOC_PROPS,
		/**
		 *  Replace a selection.
		 */
    EDITcmdREPLACE,
		/**
		 *  Replace a selection in a notes separator. (Not expected
		 *  in other locations)
		 */
    EDITcmdREPLACE_FTNSEP,
		/**
		 *  Replace a selection in the body (Not expected
		 *  in other locations)
		 */
    EDITcmdREPLACE_BODY_LEVEL,
		/**
		 *  Delete a selection.
		 *  Including two variants that tell where to place the 
		 *  cursor after an Undo.
		 */
    EDITcmdDELETE_SELECTION,
    EDITcmdDELETE_SELECTION_DEL,
    EDITcmdDELETE_SELECTION_BS,
		/**
		 *  Extend a previous replacement. EG by typing the next 
		 *  character.
		 */
    EDITcmdEXTEND_REPLACE,
		/**
		 *  Merge selected paragraphs
		 */
    EDITcmdMERGE_PARAS,

		/**
		 *  Delete a paragraph.
		 */
    EDITcmdDELETE_PARA,
		/**
		 *  Delete a section.
		 */
    EDITcmdDELETE_SECT,
		/**
		 *  Delete a table.
		 */
    EDITcmdDELETE_TABLE,
		/**
		 *  Delete a table row.
		 */
    EDITcmdDELETE_ROW,
		/**
		 *  Delete a table column.
		 */
    EDITcmdDELETE_COLUMN,
		/**
		 *  Insert a paragraph.
		 */
    EDITcmdINSERT_PARA,
    EDITcmdAPPEND_PARA,
		/**
		 *  Insert a table.
		 */
    EDITcmdINSERT_TABLE,
		/**
		 *  Insert a section.
		 */
    EDITcmdINSERT_SECT,
    EDITcmdAPPEND_SECT,
		/**
		 *  Insert a section break.
		 */
    EDITcmdSPLIT_SECT,
		/**
		 *  Insert/Append a table row.
		 */
    EDITcmdINSERT_ROW,
    EDITcmdAPPEND_ROW,
		/**
		 *  Insert/Append a table column.
		 */
    EDITcmdINSERT_COLUMN,
    EDITcmdAPPEND_COLUMN,

		/**
		 *  Add a page header or footer to a section.
		 */
    EDITcmdINSERT_HEADER,
    EDITcmdINSERT_FOOTER,
		/**
		 *  Delete a page header or footer from a section.
		 */
    EDITcmdDELETE_HEADER,
    EDITcmdDELETE_FOOTER,

		/**
		 *  Change the properties of an object (image)
		 */
    EDITcmdUPD_OBJECT,
		/**
		 *  Roll the rows in a strip of a table.
		 */
    EDITcmdSHIFT_ROWS_DOWN,
    EDITcmdSHIFT_ROWS_UP,
		/**
		 *  Shift the indentation of the selection.
		 */
    EDITcmdSHIFT_RIGHT,
    EDITcmdSHIFT_LEFT,

		/**
		 *  Not really a command: Number of real edit commands.
		 */
    EDITcmd_EDIT_COUNT,

		/**
		 *  Not really a command: Start editing a file.
		 */
    EDITcmd_OPEN,
		/**
		 *  Not really a command: Saved the file.
		 */
    EDITcmd_SAVE,
		/**
		 *  Not really a command: Created the file.
		 */
    EDITcmd_NEW,

    EDITcmd_COUNT
    } EditCommand;

#    endif	/*  DOC_EDIT_COMMAND_H	*/
