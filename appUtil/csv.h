/************************************************************************/
/*									*/
/*  Read comma separated data into a data structure.			*/
/*									*/
/************************************************************************/

#   ifndef	CSV_H
#   define	CSV_H

#   include	<stdio.h>

typedef struct InputRow
    {
    char *		irBytes;
    int *		irFields;
    int			irByteCount;
    int			irFieldCount;
    } InputRow;

typedef struct InputSheet
    {
    char *		isFileName;
    char *		isSheetName;
    int			isSheetNameLength;
    InputRow *		isRows;
    int			isRowCount;
    } InputSheet;

typedef struct InputBook
    {
    InputSheet *	ibSheets;
    int			ibSheetCount;
    } InputBook;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void csvInitInputRow(	InputRow *	ir );

extern void csvCleanInputRow(	InputRow *	ir );

extern void csvInitInputSheet(	InputSheet *	is );

extern void csvCleanInputSheet(	InputSheet *	is );

extern void csvInitInputBook(	InputBook *	ib );

extern void csvCleanInputBook(	InputBook *	ib );

extern int csvAddSheet(	InputBook *		ib,
			const char *		fileName,
			const char *		sheetName );

extern int csvGetSheet(	const InputBook *	ib,
			const char *		sheetName );

extern int csvReadRow(		InputRow *	ir,
				int		sep,
				FILE *		csvInFile );

extern int csvReadSheet(	InputSheet *	is,
				int		sep,
				FILE *		csvInFile );

extern int csvWriteCsvOutFile(	const InputSheet *	is,
				int			sep,
				FILE *			csvOutFile );

extern int csvGetInclude(	int *			pSheet,
				const InputBook *	ib,
				const char *		field0 );

extern int csvFindRowByLabel(	const InputSheet *	is,
				const char *		label,
				int			col,
				int			filterColumn );

extern int csvFindRowByLabelIndirect(	int *			pSheet,
					int *			pRow,
					int *			pNrFound,
					int *			pNr,
					const InputBook *	ib,
					int			sheet,
					const char *		label,
					int			filterColumn );

extern int csvNormaliseNumber(		char *		field );

extern int csvWriteHOutDefine(	const char *		prefix,
				const char *		field0,
				int			nr,
				FILE *			hOutFile );

extern int csvWriteHOutDefines(	const InputBook *	ib,
				int			col,
				const char *		prefix,
				int			sheet,
				int *			pNr,
				FILE *			hOutFile );

extern int csvFindColumnByLabel(	const InputRow *	ir,
					const char *		label );

extern void csvStripLength(			const char **	pS,
						int *		pL );

extern int csvGetIntFromRow(		int *			pI,
					const InputRow *	ir,
					int			col );

extern int csvGetDoubleFromRow(		double *		pD,
					const InputRow *	ir,
					int			col );

#   endif	/*  CSV_H  */
