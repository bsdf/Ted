#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>
#   include	<time.h>

#   include	"csv.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping on InputRow's/InputSheet's				*/
/*									*/
/************************************************************************/

void csvInitInputRow(	InputRow *	ir )
    {
    ir->irBytes= (char *)0;
    ir->irFields= (int *)0;
    ir->irByteCount= 0;
    ir->irFieldCount= 0;

    return;
    }

void csvCleanInputRow(	InputRow *	ir )
    {
    if  ( ir->irBytes )
	{ free( ir->irBytes );	}
    if  ( ir->irFields )
	{ free( ir->irFields );	}

    return;
    }

void csvInitInputSheet(	InputSheet *	is )
    {
    is->isFileName= (char *)0;
    is->isSheetName= (char *)0;
    is->isSheetNameLength= 0;
    is->isRows= (InputRow *)0;
    is->isRowCount= 0;
    }

void csvCleanInputSheet(	InputSheet *	is )
    {
    int		i;

    if  ( is->isFileName )
	{ free( is->isFileName );	}
    if  ( is->isSheetName )
	{ free( is->isSheetName );	}
    
    for ( i= 0; i < is->isRowCount; i++ )
	{ csvCleanInputRow( is->isRows+ i );	}

    if  ( is->isRows )
	{ free( is->isRows );	}
    
    return;
    }

void csvInitInputBook(	InputBook *	ib )
    {
    ib->ibSheets= (InputSheet *)0;
    ib->ibSheetCount= 0;
    }

void csvCleanInputBook(	InputBook *	ib )
    {
    int		i;

    for ( i= 0; i < ib->ibSheetCount; i++ )
	{ csvCleanInputSheet( ib->ibSheets+ i );	}

    if  ( ib->ibSheets )
	{ free( ib->ibSheets );	}
    
    return;
    }

int csvAddSheet(	InputBook *		ib,
			const char *		fileName,
			const char *		sheetName )
    {
    InputSheet *	fresh;

    fresh= (InputSheet *)realloc( ib->ibSheets,
			    ( ib->ibSheetCount+ 1 )* sizeof( InputSheet ) );
    if  ( ! fresh )
	{ LXDEB(ib->ibSheetCount,fresh); return -1;	}
    ib->ibSheets= fresh;

    fresh += ib->ibSheetCount;
    csvInitInputSheet( fresh );

    fresh->isFileName= strdup( fileName );
    fresh->isSheetName= strdup( sheetName );
    if  ( ! fresh->isFileName	||
	  ! fresh->isSheetName	)
	{
	XDEB(fresh->isFileName);
	XDEB(fresh->isSheetName);
	csvCleanInputSheet( fresh );
	return -1;
	}

    fresh->isSheetNameLength= strlen( fresh->isSheetName );

    ib->ibSheetCount++;

    return 0;
    }

int csvGetSheet(	const InputBook *	ib,
			const char *		sheetName )
    {
    const InputSheet *	is= ib->ibSheets;
    int			i;

    for ( i= 0; i < ib->ibSheetCount; is++, i++ )
	{
	if  ( ! strcmp( is->isSheetName, sheetName ) )
	    { return i;	}
	}

    SDEB(sheetName); return -1;
    }

int csvGetInclude(	int *			pSheet,
			const InputBook *	ib,
			const char *		field0 )
    {
    const char *	here= field0+ 10;
    int			sheet;

    while( *here == ' ' )
	{ here++;	}

    sheet= csvGetSheet( ib, here );
    if  ( sheet < 0 )
	{ SLDEB(field0,sheet); return -1;	}

    *pSheet= sheet;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an input line and divide it in fields.				*/
/*									*/
/************************************************************************/

int csvReadRow(		InputRow *	ir,
			int		sep,
			FILE *		csvInFile )
    {
    int		c;

    int		byteCount= 0;
    int		fieldCount= 0;

    c= getc( csvInFile );
    if  ( c == EOF )
	{ return 1;	}

    ir->irFieldCount= 0;
    ir->irByteCount= 0;

    for (;;)
	{
	int		quoted= 0;

	if  ( ir->irFieldCount >= fieldCount )
	    {
	    int		count= ( 3* fieldCount )/ 2+ 2;
	    int *	fresh= (int *)realloc( ir->irFields, count* sizeof(int) );

	    if  ( ! fresh )
		{ LXDEB(count,fresh); return -1; }
	    ir->irFields= fresh;

	    fieldCount= count;
	    }

	ir->irFields[ir->irFieldCount++]= ir->irByteCount;

	if  ( c == '\'' || c == '"' )
	    { quoted= c; c= getc( csvInFile );	}

	for (;;)
	    {
	    if  ( c == EOF )
		{ LDEB(c); return -1;	}

	    if  ( c == '\r' )
		{
		c= getc( csvInFile );
		if  ( c != '\n' )
		    { CDEB(c); return -1;	}
		}

	    if  ( ir->irByteCount >= byteCount )
		{
		int		count= ( 3* byteCount )/ 2+ 2;
		char *		fresh= (char *)realloc( ir->irBytes, count+ 1 );

		if  ( ! fresh )
		    { LXDEB(count,fresh); return -1; }
		ir->irBytes= fresh;

		byteCount= count;
		}

	    if  ( quoted )
		{
		if  ( c == '\n' )
		    { CDEB(c); return -1;	}

		if  ( c == quoted )
		    {
		    c= getc( csvInFile );
		    if  ( c == '\r' )
			{ c= getc( csvInFile ); }
		    if  ( c != '\n' && c != sep && c != quoted )
			{ CDEB(c); return -1;	}

		    if  ( c == quoted )
			{ ir->irBytes[ir->irByteCount++]= c;		}
		    else{ ir->irBytes[ir->irByteCount++]= '\0'; break;	}
		    }
		else{ ir->irBytes[ir->irByteCount++]= c;		}
		}
	    else{
		if  ( c == '\n' || c == sep )
		    { ir->irBytes[ir->irByteCount++]= '\0'; break;	}
		else{ ir->irBytes[ir->irByteCount++]= c;		}
		}

	    c= getc( csvInFile );
	    }

	if  ( c == '\n' )
	    { break;	}

	c= getc( csvInFile );
	}

    if  ( ir->irByteCount > 0 )
	{ ir->irBytes[ir->irByteCount]= '\0';	}

    return 0;
    }

int csvReadSheet(	InputSheet *	is,
			int		sep,
			FILE *		csvInFile )
    {
    int			rowCount= 0;

    for (;;)
	{
	int		res;

	if  ( is->isRowCount >= rowCount )
	    {
	    int		count= ( 3* rowCount )/ 2+ 2;
	    InputRow *	fresh= (InputRow *)realloc( is->isRows, count* sizeof(InputRow) );
	    int		i;

	    if  ( ! fresh )
		{ LXDEB(count,fresh); return -1; }
	    is->isRows= fresh;

	    for ( i= is->isRowCount; i < count; i++ )
		{ csvInitInputRow( is->isRows+ i );	}

	    rowCount= count;
	    }

	res= csvReadRow( is->isRows+ is->isRowCount, sep, csvInFile );
	if  ( res < 0 )
	    { LDEB(res); return -1;	}
	if  ( res > 0 )
	    { break;			}

	is->isRowCount++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a sheet to file. In csv format. E.G. for debugging purposes.	*/
/*									*/
/************************************************************************/

int csvWriteCsvOutFile(	const InputSheet *	is,
			int			sep,
			FILE *			csvOutFile )
    {
    int			row;
    int			col;

    const InputRow *	ir;

    ir= is->isRows;
    for ( row= 0; row < is->isRowCount; ir++, row++ )
	{
	int *		field;
	const char *	f;

	field= ir->irFields;
	for ( col= 0; col < ir->irFieldCount; field++, col++ )
	    {
	    int		quoted= 0;

	    if  ( col > 0 )
		{ putc( sep, csvOutFile );	}

	    if  ( strchr( ir->irBytes+ *field, sep ) )
		{
		if  ( ! strchr( ir->irBytes+ *field, '"' ) )
		    { quoted= '"';	}
		else{
		    if  ( ! strchr( ir->irBytes+ *field, '\'' ) )
			{ quoted= '\'';		}
		    else{ quoted= '"';		}
		    }
		}

	    if  ( quoted )
		{ putc( quoted, csvOutFile );	}

	    f= ir->irBytes+ *field;
	    while( *f )
		{
		putc( *f, csvOutFile );
		if  ( *f == quoted )
		    { putc( *f, csvOutFile );	}
		f++;
		}

	    if  ( quoted )
		{ putc( quoted, csvOutFile );	}
	    }

	putc( '\n', csvOutFile );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a row by the value of its leftmost column.			*/
/*									*/
/************************************************************************/

int csvFindRowByLabel(	const InputSheet *	is,
			const char *		label,
			int			col,
			int			filterColumn )
    {
    int			row;
    const InputRow *	ir;

    int			len;

    while( isspace( *label ) )
	{ label++;	}
    len= strlen( label );
    while ( len > 0 && isspace( label[len-1] ) )
	{ len--;	}

    ir= is->isRows;
    for ( row= 0; row < is->isRowCount; ir++, row++ )
	{
	const char *	field0;
	const char *	past;

	if  ( ir->irFieldCount <= col )
	    { continue;	}

	field0= ir->irBytes+ ir->irFields[col];

	while( isspace( *field0 ) )
	    { field0++;	}

	if  ( strncmp( field0, label, len ) )
	    { continue;	}

	past= field0+ len;
	while( isspace( *past ) )
	    { past++;	}

	if  ( *past )
	    { continue;	}

	if  ( ir->irFieldCount > filterColumn	)
	    {
	    const char *	field1;

	    field1= ir->irBytes+ ir->irFields[filterColumn];
	    while( isspace ( *field1 ) )
		{ field1++;	}

	    if  ( *field1 )
		{ continue;	}
	    }

	break;
	}

    if  ( row >= is->isRowCount )
	{ /* SDEB(label); */ return -1;	}

    return row;
    }

/************************************************************************/
/*									*/
/*  Find a column by the value in a certain row.			*/
/*									*/
/************************************************************************/

int csvFindColumnByLabel(	const InputRow *	ir,
				const char *		label )
    {
    int			col;
    int			len;

    csvStripLength( &label, &len );

    for ( col= 0; col < ir->irFieldCount; col++ )
	{
	const char *	field0;
	const char *	past;

	field0= ir->irBytes+ ir->irFields[col];

	while( isspace( *field0 ) )
	    { field0++;	}

	if  ( strncmp( field0, label, len ) )
	    { continue;	}

	past= field0+ len;
	while( isspace( *past ) )
	    { past++;	}
	if  ( *past )
	    { continue;	}

	break;
	}

    if  ( col >= ir->irFieldCount )
	{ /* SDEB(label); */ return -1;	}

    return col;
    }

/************************************************************************/
/*									*/
/*  Find a row by the value of its leftmost column.			*/
/*									*/
/************************************************************************/

int csvFindRowByLabelIndirect(	int *			pSheet,
				int *			pRow,
				int *			pNrFound,
				int *			pNr,
				const InputBook *	ib,
				int			sheet,
				const char *		label,
				int			filterColumn )
    {
    int			row;
    const InputSheet *	is= ib->ibSheets+ sheet;
    const InputRow *	ir;
    int			len;

    int			nr= *pNr;

    csvStripLength( &label, &len );

    ir= is->isRows;
    for ( row= 0; row < is->isRowCount; ir++, row++ )
	{
	const char *	field0;
	const char *	past;

	if  ( ir->irFieldCount == 0 )
	    { continue;	}

	field0= ir->irBytes+ ir->irFields[0];

	while( isspace( *field0 ) )
	    { field0++;	}

	if  ( ! strncmp( field0, "::include ", 10 ) )
	    {
	    int		n;
	    int		ret;

	    if  ( csvGetInclude( &n, ib, field0 ) )
		{ SDEB(field0); return -1;	}

	    ret= csvFindRowByLabelIndirect( pSheet, pRow, pNrFound, &nr,
						ib, n, label, filterColumn );
	    if  ( ret < 0 )
		{ LDEB(ret); return -1;	}

	    if  ( ret == 0 )
		{ *pNr= nr; return 0;	}

	    continue;
	    }

	if  ( ! isalpha( field0[0] ) )
	    { continue;	}

	if  ( filterColumn >= 0 )
	    {
	    const char *	field1;

	    field1= ir->irBytes+ ir->irFields[1];
	    while( isspace ( *field1 ) )
		{ field1++;	}

	    if  ( *field1 )
		{ continue;	}
	    }

	nr++;
	if  ( strncmp( field0, label, len ) )
	    { continue;	}
	past= field0+ len;
	while( isspace( *past ) )
	    { past++;	}
	if  ( *past )
	    { continue;	}

	*pSheet= sheet; *pRow= row; *pNrFound= nr;
	return 0;
	}

    if  ( row >= is->isRowCount )
	{ *pNr=nr; return 1;	}

    *pNr= nr;
    return row;
    }

/************************************************************************/
/*									*/
/*  Remove formatting in a field that is supposed to contain a number.	*/
/*  First check whether the field really is a number.			*/
/*									*/
/************************************************************************/

int csvNormaliseNumber(		char *		field )
    {
    const char *	from;
    char *		to;

    int			gotDot= 0;

    from= to= field;
    while( *from )
	{
	if  ( isspace( *from ) || *from == ',' )
	    { from++; continue;	}

	if  ( ! isdigit( *from ) && *from != '.' && *from != '-' )
	    { SDEB(from); return -1;	}

	if  ( *from == '.' )
	    { gotDot= 1;	}

	to++; from++;
	}

    if  ( to == field )
	{ SDEB(field); return -1;	}

    from= to= field;
    while( *from )
	{
	if  ( isspace( *from ) || *from == ',' )
	    { from++; continue;	}

	if  ( ! isdigit( *from ) && *from != '.' && *from != '-' )
	    { SDEB(from); return -1;	}

	*(to++)= *(from++);
	}

    *to= '\0';

    if  ( to- field > 8		&&
	  ! gotDot		&&
	  from- field > 10	)
	{ strcpy( to, ".0" );	}

    return 0;
    }

void csvStripLength(	const char **	pS,
			int *		pL )
    {
    const char *	s= *pS;
    const char *	r;
    int			i;
    int			l= 0;

    while( *s == ' ' )
	{ s++;	}

    i= 0; r= s;
    while( *r )
	{
	if  ( *r != ' ' )
	    { l= i+ 1;	}

	r++; i++;
	}

    *pS= s; *pL= l; return;
    }

int csvGetIntFromRow(		int *			pI,
				const InputRow *	ir,
				int			col )
    {
    int			i;
    char *		past;

    if  ( col >= ir->irFieldCount )
	{ LLDEB(col,ir->irFieldCount); return -1;	}

    i= strtol( ir->irBytes+ ir->irFields[col], &past, 10 );

    if  ( past != ir->irBytes+ ir->irFields[col] )
	{
	while( *past == ' ' )
	    { past++; }
	}

    if  ( past == ir->irBytes+ ir->irFields[col] || *past )
	{ SDEB(ir->irBytes+ ir->irFields[col]); return -1;	}

    *pI= i; return 0;
    }

int csvGetDoubleFromRow(	double *		pD,
				const InputRow *	ir,
				int			col )
    {
    double		d;
    char *		past;

    if  ( col >= ir->irFieldCount )
	{ LLDEB(col,ir->irFieldCount); return -1;	}

    d= strtod( ir->irBytes+ ir->irFields[col], &past );

    if  ( past != ir->irBytes+ ir->irFields[col] )
	{
	while( *past == ' ' )
	    { past++; }
	}

    if  ( past == ir->irBytes+ ir->irFields[col] || *past )
	{ SDEB(ir->irBytes+ ir->irFields[col]); return -1;	}

    *pD= d; return 0;
    }

