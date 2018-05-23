#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmgetrow.h"
#   include	"bmputrow.h"
#   include	<stdlib.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Apply convolution filters to an image.				*/
/*									*/
/************************************************************************/

typedef void (*BM_FILTER_OP)(	ColorValue *			cvTo,
				const FillJob *			fj );

/************************************************************************/
/*									*/
/*  1)  Sharpening filter.						*/
/*  2)  Components of the Sobel filter.					*/
/*  3)  Smoothing filter.						*/
/*									*/
/************************************************************************/

/*  1  */
static const int BM_laplace_P[3]= { -1, -1, -1 };
static const int BM_laplace_T[3]= { -1,  8, -1 };
static const int BM_laplace_N[3]= { -1, -1, -1 };
static const int * const BM_laplace_[3]=
    { BM_laplace_P+ 1, BM_laplace_T+ 1, BM_laplace_N+ 1 };
static const int * const * const BM_laplace= BM_laplace_+ 1;

/*  2  */
static const int BM_sobelx_P[3]= { -1,  0,  1 };
static const int BM_sobelx_T[3]= { -2,  0,  2 };
static const int BM_sobelx_N[3]= { -1,  0,  1 };
static const int * const BM_sobelx_[3]=
    { BM_sobelx_P+ 1, BM_sobelx_T+ 1, BM_sobelx_N+ 1 };
static const int * const * const BM_sobelx= BM_sobelx_+ 1;

static const int BM_sobely_P[3]= { -1, -2, -1 };
static const int BM_sobely_T[3]= {  0,  0,  0 };
static const int BM_sobely_N[3]= {  1,  2,  1 };
static const int * const BM_sobely_[3]=
    { BM_sobely_P+ 1, BM_sobely_T+ 1, BM_sobely_N+ 1 };
static const int * const * const BM_sobely= BM_sobely_+ 1;

/*  3  */
static const int BM_smoothe_P[3]= {  1,  1,  1 };
static const int BM_smoothe_T[3]= {  1,  1,  1 };
static const int BM_smoothe_N[3]= {  1,  1,  1 };
static const int * const BM_smoothe_[3]=
    { BM_smoothe_P+ 1, BM_smoothe_T+ 1, BM_smoothe_N+ 1 };
static const int * const * const BM_smoothe= BM_smoothe_+ 1;

/************************************************************************/
/*									*/
/*  Apply a 3x3 filter.							*/
/*									*/
/************************************************************************/

static void bmFilterRow3x3(	ColorValue *			cvTo,
				const FillJob *			fj,
				const int * const * const	f,
				int				absoluteValues,
				int				maxValue )
    {
    int			i;

    const ColorValue *	p= fj->fjPrevRow+ 1;
    const ColorValue *	t= fj->fjThisRow+ 1;
    const ColorValue *	n= fj->fjNextRow+ 1;

    for ( i= 0; i < fj->fjFrWide; cvTo++, p++, t++, n++, i++ )
	{
	long	val;

	cvTo->cvN++;

	/* R */
	val= 0;
	val += f[-1][-1]* p[-1].cvR+ f[-1][ 0]* p[0].cvR+ f[-1][ 1]* p[1].cvR;
	val += f[ 0][-1]* t[-1].cvR+ f[ 0][ 0]* t[0].cvR+ f[ 0][ 1]* t[1].cvR;
	val += f[ 1][-1]* n[-1].cvR+ f[ 1][ 0]* n[0].cvR+ f[ 1][ 1]* n[1].cvR;

	if  ( val < 0 && absoluteValues )	
	    { val= -val;	}
	if  ( val < 0 )	
	    { val= 0;	}
	if  ( val > maxValue )
	    { val= maxValue;	}

	cvTo->cvR += val;

	/* G */
	val= 0;
	val += f[-1][-1]* p[-1].cvG+ f[-1][ 0]* p[0].cvG+ f[-1][ 1]* p[1].cvG;
	val += f[ 0][-1]* t[-1].cvG+ f[ 0][ 0]* t[0].cvG+ f[ 0][ 1]* t[1].cvG;
	val += f[ 1][-1]* n[-1].cvG+ f[ 1][ 0]* n[0].cvG+ f[ 1][ 1]* n[1].cvG;

	if  ( val < 0 && absoluteValues )	
	    { val= -val;	}
	if  ( val < 0 )	
	    { val= 0;	}
	if  ( val > maxValue )
	    { val= maxValue;	}

	cvTo->cvG += val;

	/* B */
	val= 0;
	val += f[-1][-1]* p[-1].cvB+ f[-1][ 0]* p[0].cvB+ f[-1][ 1]* p[1].cvB;
	val += f[ 0][-1]* t[-1].cvB+ f[ 0][ 0]* t[0].cvB+ f[ 0][ 1]* t[1].cvB;
	val += f[ 1][-1]* n[-1].cvB+ f[ 1][ 0]* n[0].cvB+ f[ 1][ 1]* n[1].cvB;

	if  ( val < 0 && absoluteValues )	
	    { val= -val;	}
	if  ( val < 0 )	
	    { val= 0;	}
	if  ( val > maxValue )
	    { val= maxValue;	}

	cvTo->cvB += val;
	}
    }


/************************************************************************/
/*									*/
/*  Apply the Sobel edge detector filter.				*/
/*									*/
/************************************************************************/

static void bmSobel(		ColorValue *			cvTo,
				const FillJob *			fj )
    {
    const int	absoluteValues= 1;
    const int	maxValue= 4* 255;

    bmInitColorRow( cvTo, fj->fjFrWide );

    bmFilterRow3x3( cvTo, fj, BM_sobelx, absoluteValues, maxValue );
    bmFilterRow3x3( cvTo, fj, BM_sobely, absoluteValues, maxValue );

    return;
    }

/************************************************************************/
/*									*/
/*  Apply the Laplace sharpening filter.				*/
/*									*/
/************************************************************************/

static void bmLaplace(		ColorValue *			cvTo,
				const FillJob *			fj )
    {
    const int	absoluteValues= 1;
    const int	maxValue= 255;

    bmInitColorRow( cvTo, fj->fjFrWide );

    bmFilterRow3x3( cvTo, fj, BM_laplace, absoluteValues, maxValue );

    return;
    }

/************************************************************************/
/*									*/
/*  Apply a smoothing filter.						*/
/*									*/
/************************************************************************/

static void bmSmoothe(		ColorValue *			cvTo,
				const FillJob *			fj )
    {
    const int	absoluteValues= 0;
    const int	maxValue= 9* 255;

    int		i;

    bmInitColorRow( cvTo, fj->fjFrWide );

    bmFilterRow3x3( cvTo, fj, BM_smoothe, absoluteValues, maxValue );

    for ( i= 0; i < fj->fjFrWide; cvTo++, i++ )
	{
	cvTo->cvR /= 9;
	cvTo->cvG /= 9;
	cvTo->cvB /= 9;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Blur the image using moving averages.				*/
/*  Note that no vertical blurring is performed here. The final result	*/
/*  is obtained by averaging a blur and a flipped blur.			*/
/*									*/
/*  1)  Moving average left to right.					*/
/*  2)  Moving average right to left.					*/
/*  3)  Moving average in the direction of the scan.			*/
/*  4)  Remember the output: Will be the 'fj->fjPrevRow' of the next	*/
/*	iteration.							*/
/*									*/
/************************************************************************/

static void bmBlur(		ColorValue *			cvTo,
				const FillJob *			fj )
    {
    int				i;
    int				r, g, b;

    const int			h0= 32;
    const int			h1= 1;
    const int			h= ( h0+ h1 );

    const int			v0= 32;
    const int			v1= 1;
    const int			v= ( v0+ v1 );

    const ColorValue *		p= fj->fjPrevRow+ 1;
    const ColorValue *		t= fj->fjThisRow+ 1;

    ColorValue *		to= cvTo;

    /*  1  */
    r= t->cvR;
    g= t->cvG;
    b= t->cvB;
    for ( i= 0; i < fj->fjFrWide; to++, t++, i++ )
	{
	r= ( h0* r+ h1* t->cvR )/ h;
	g= ( h0* g+ h1* t->cvG )/ h;
	b= ( h0* b+ h1* t->cvB )/ h;

	to->cvR= r;
	to->cvG= g;
	to->cvB= b;
	to->cvN= 1;
	}

    /*  2  */

    to--; t--;

    r= t->cvR;
    g= t->cvG;
    b= t->cvB;
    for ( i= 0; i < fj->fjFrWide; to--, t--, i++ )
	{
	r= ( h0* r+ h1* t->cvR )/ h;
	g= ( h0* g+ h1* t->cvG )/ h;
	b= ( h0* b+ h1* t->cvB )/ h;

	to->cvR= ( to->cvR+ r )/ 2;
	to->cvG= ( to->cvG+ g )/ 2;
	to->cvB= ( to->cvB+ b )/ 2;
	}

    /*  3  */
    to= cvTo;
    p= fj->fjPrevRow+ 1;
    for ( i= 0; i < fj->fjFrWide; to++, p++, i++ )
	{
	to->cvR= ( v0* p->cvR+ v1* to->cvR )/ v;
	to->cvG= ( v0* p->cvG+ v1* to->cvG )/ v;
	to->cvB= ( v0* p->cvB+ v1* to->cvB )/ v;
	}

    /*  4  */
    to= fj->fjThisRow+ 1;
    t= cvTo;
    for ( i= 0; i < fj->fjFrWide; to++, t++, i++ )
	{ *to= *t; }

    return;
    }

/************************************************************************/
/*									*/
/*  Apply a 3x3 filter to the image.					*/
/*									*/
/*  1)  Collect this and revious row from the first row of the input	*/
/*	image.								*/
/*  2)  Collect next row from the second row of the input image.	*/
/*									*/
/************************************************************************/

static int bmFilter3Row(	unsigned char *			bufOut,
				ColorValue *			cvOut,
				const BitmapDescription *	bdOut,
				const RasterImage *		riIn,
				FillJob *			fj,
				int				flip,
				GetSourceRow			getRow,
				PutScreenRow			putRow,
				BM_FILTER_OP			how )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    int				rowTo;
    int				rowFrom;

    const unsigned char *	from;
    unsigned char *		to;

    /*  1  */
    if  ( flip )
	{ rowFrom= bdIn->bdPixelsHigh- 1;	}
    else{ rowFrom= 0;				}
    from= riIn->riBytes+ rowFrom* bdIn->bdBytesPerRow;

    bmInitColorRow( fj->fjPrevRow+ 1, fj->fjFrWide );
    bmInitColorRow( fj->fjThisRow+ 1, fj->fjFrWide );

    (*getRow)( fj->fjPrevRow+ 1, 0, from, 0, bdIn->bdPixelsWide, bdIn );
    (*getRow)( fj->fjThisRow+ 1, 0, from, 0, bdIn->bdPixelsWide, bdIn );

    fj->fjPrevRow[0]= fj->fjPrevRow[1];
    fj->fjPrevRow[bdOut->bdPixelsWide]= fj->fjPrevRow[bdOut->bdPixelsWide-1];
    fj->fjThisRow[0]= fj->fjThisRow[1];
    fj->fjThisRow[bdOut->bdPixelsWide]= fj->fjThisRow[bdOut->bdPixelsWide-1];

    /*  2  */
    rowTo= 0;
    if  ( flip )
	{ rowFrom= bdIn->bdPixelsHigh- rowTo- 2;	}
    else{ rowFrom= rowTo+ 1;				}
    from= riIn->riBytes+ rowFrom* bdIn->bdBytesPerRow;

    bmInitColorRow( fj->fjNextRow+ 1, fj->fjFrWide );
    (*getRow)( fj->fjNextRow+ 1, 0, from, 0, bdIn->bdPixelsWide, bdIn );

    fj->fjNextRow[0]= fj->fjNextRow[1];
    fj->fjNextRow[bdOut->bdPixelsWide]= fj->fjNextRow[bdOut->bdPixelsWide-1];

    /*  3  */
    for ( rowTo= 0; rowTo < bdOut->bdPixelsHigh- 1; rowTo++ )
	{
	ColorValue *	swap;

	if  ( flip )
	    { rowFrom= bdIn->bdPixelsHigh- rowTo- 2;	}
	else{ rowFrom= rowTo+ 1;			}
	from= riIn->riBytes+ rowFrom* bdIn->bdBytesPerRow;

	to= bufOut+ rowTo* bdOut->bdBytesPerRow;
	(*how)( cvOut+ 1, fj );
	if  ( (*putRow)( to, fj, cvOut+ 1 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	swap= fj->fjPrevRow; fj->fjPrevRow= fj->fjThisRow;
	fj->fjThisRow= fj->fjNextRow; fj->fjNextRow= swap;

	bmInitColorRow( fj->fjNextRow+ 1, fj->fjFrWide );
	(*getRow)( fj->fjNextRow+ 1, 0, from, 0, bdIn->bdPixelsWide, bdIn );

	fj->fjNextRow[0]= fj->fjNextRow[1];
	fj->fjNextRow[bdOut->bdPixelsWide]=
					fj->fjNextRow[bdOut->bdPixelsWide-1];
	}

    bmInitColorRow( fj->fjThisRow+ 1, fj->fjFrWide );
    (*getRow)( fj->fjThisRow+ 1, 0, from, 0, bdIn->bdPixelsWide, bdIn );
    fj->fjThisRow[0]= fj->fjThisRow[1];
    fj->fjThisRow[bdOut->bdPixelsWide]= fj->fjThisRow[bdOut->bdPixelsWide-1];

    to= bufOut+ rowTo* bdOut->bdBytesPerRow;
    (*how)( cvOut+ 1, fj );
    if  ( (*putRow )( to, fj, cvOut+ 1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Perform a filter operation.						*/
/*									*/
/*  1)  Copy input image description. Use 8 bits per sample however.	*/
/*  2)  Calculate memory requirements etc.				*/
/*  3)  Allocate output image buffer.					*/
/*  4)  Allocate color accumulator for the scan.			*/
/*									*/
/************************************************************************/

static int bmStartFilterOperation(
				RasterImage *			riOut,
				ColorValue **			pCvOut,
				ColorAllocator *		ca,
				PutScreenRow *			pPutRow,
				GetSourceRow *			pGetRow,
				FillJob *			fj,
				const BitmapDescription *	bdIn )
    {
    int			scratchSize;

    const int		swapBitmapUnit= 0;
    const int		swapBitmapBytes= 0;
    const int		swapBitmapBits= 0;
    const int		dither= 0;

    /*  1  */
    if  ( bmCopyDescription( &(riOut->riDescription), bdIn ) )
	{ LDEB(1); return -1;	}

    riOut->riDescription.bdBitsPerSample= 8;

    /*  2  */
    if  ( bmCalculateSizes( &(riOut->riDescription) ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    if  ( bmAllocateBuffer( riOut ) )
	{ LXDEB(riOut->riDescription.bdBufferLength,riOut->riBytes); return -1;	}

    /*  4  */
    *pCvOut= (ColorValue *)malloc( ( riOut->riDescription.bdPixelsWide+ 2 )* sizeof(ColorValue) );
    if  ( ! *pCvOut )
	{ LXDEB(riOut->riDescription.bdPixelsWide,*pCvOut); return -1;	}

    /*  5  */
    if  ( bmSetColorAllocatorForImage( ca, &(riOut->riDescription) ) )
	{ LDEB(1); return -1;	}

    /**/

    if  ( bmGetPutRow( pPutRow, &scratchSize, ca,
				    swapBitmapUnit, swapBitmapBytes,
				    swapBitmapBits, &(riOut->riDescription) ) )
	{ LDEB(1); return -1;	}

    if  ( bmGetGetRow( pGetRow, bdIn ) )
	{ LDEB(1); return -1; }

    /**/

    if  ( bmSetFillJob( fj, ca, bdIn->bdPixelsWide,
					    riOut->riDescription.bdPixelsWide,
					    scratchSize, dither ) )
	{ LDEB(scratchSize); return -1;	}


    return 0;
    }

static int bmFilterOperation(
			RasterImage *		riOut,
			const RasterImage *	riIn,
			int			flip,
			BM_FILTER_OP		how )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    ColorValue *		cvOut= (ColorValue *)0;
    PutScreenRow		putRow= (PutScreenRow)0;
    GetSourceRow		getRow= (GetSourceRow)0;

    RasterImage			ri;
    FillJob			fj;
    ColorAllocator		ca;

    bmInitRasterImage( &ri );
    bmInitFillJob( &fj );
    bmInitColorAllocator( &ca );

    if  ( bmStartFilterOperation( &ri, &cvOut, &ca,
					    &putRow, &getRow, &fj, bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmFilter3Row( ri.riBytes, cvOut, &(ri.riDescription), riIn, 
					    &fj, flip, getRow, putRow, how ) )
	{ LDEB(1); rval= -1; goto ready;	}
	    
    /*  steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    if  ( cvOut )
	{ free( cvOut );	}

    bmCleanRasterImage( &ri );
    bmCleanFillJob( &fj );
    bmCleanColorAllocator( &ca );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Invoking routines:							*/
/*									*/
/************************************************************************/

int bmFilterSobel(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const int		flip= 0;

    return bmFilterOperation( riOut, riIn, flip, bmSobel );
    }

int bmFilterLaplace(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const int		flip= 0;

    return bmFilterOperation( riOut, riIn, flip, bmLaplace );
    }

int bmFilterSmoothe(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const int		flip= 0;

    return bmFilterOperation( riOut, riIn, flip, bmSmoothe );
    }

/************************************************************************/
/*									*/
/*  Blurring operations.						*/
/*									*/
/************************************************************************/

static int bmStartBlurOperation(
				RasterImage *			riDown,
				RasterImage *			riUp,
				ColorValue **			pCvDown,
				ColorValue **			pCvUp,
				ColorAllocator *		ca,
				PutScreenRow *			pPutRowBlur,
				GetSourceRow *			pGetRowIn,
				GetSourceRow *			pGetRowBlur,
				FillJob *			fj,
				const BitmapDescription *	bdIn )
    {
    if  ( bmStartFilterOperation( riDown, pCvDown, ca,
				    pPutRowBlur, pGetRowIn, fj, bdIn ) )
	{ LDEB(1); return -1;	}

    if  ( bmCopyRasterImage( riUp, riDown ) )
	{ LDEB(1); return -1;	}

    *pCvUp= (ColorValue *)malloc( ( riUp->riDescription.bdPixelsWide+ 2 )* sizeof(ColorValue) );
    if  ( ! *pCvUp )
	{ LXDEB(riUp->riDescription.bdPixelsWide,*pCvUp); return -1;	}

    if  ( bmGetGetRow( pGetRowBlur, &(riDown->riDescription) ) )
	{ LDEB(1); return -1; }

    return 0;
    }

static int bmBlurAverage(	FillJob *			fj,
				const BitmapDescription *	bdBlur,
				ColorValue *			cvDown,
				ColorValue *			cvUp,
				unsigned char *			bufDown,
				const unsigned char *		bufUp,
				GetSourceRow			getRow,
				PutScreenRow			putRow )
    {
    int		row;

    for ( row= 0; row < bdBlur->bdPixelsHigh; row++ )
	{
	int			col;

	ColorValue *		cvd;
	ColorValue *		cvu;

	unsigned char *		to;
	const unsigned char *	from;

	to= bufDown+ row* bdBlur->bdBytesPerRow;
	from= bufUp+ ( bdBlur->bdPixelsHigh- row- 1 )* bdBlur->bdBytesPerRow;

	cvd= cvDown+ 1;
	cvu= cvUp+ 1;

	bmInitColorRow( cvd, fj->fjFrWide );
	bmInitColorRow( cvu, fj->fjFrWide );

	(*getRow)( cvd, 0, from, 0, bdBlur->bdPixelsWide, bdBlur );
	(*getRow)( cvu, 0, to,   0, bdBlur->bdPixelsWide, bdBlur );

	for ( col= 0; col < bdBlur->bdPixelsWide; cvd++, cvu++, col++ )
	    {
	    cvd->cvR= ( cvd->cvR+ cvu->cvR )/ 2;
	    cvd->cvG= ( cvd->cvG+ cvu->cvG )/ 2;
	    cvd->cvB= ( cvd->cvB+ cvu->cvB )/ 2;
	    }

	if  ( (*putRow)( to, fj, cvDown+ 1 ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int bmFilterBlur(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    int				rval= 0;
    const BitmapDescription *	bdIn= &(riIn->riDescription);

    ColorValue *		cvDown= (ColorValue *)0;
    ColorValue *		cvUp= (ColorValue *)0;

    PutScreenRow		putRow= (PutScreenRow)0;
    GetSourceRow		getRowIn= (GetSourceRow)0;
    GetSourceRow		getRowBlur= (GetSourceRow)0;

    FillJob			fj;
    RasterImage			riDown;
    RasterImage			riUp;
    ColorAllocator		ca;

    bmInitRasterImage( &riDown );
    bmInitRasterImage( &riUp );
    bmInitFillJob( &fj );
    bmInitColorAllocator( &ca );

    if  ( bmStartBlurOperation( &riDown, &riUp, &cvDown, &cvUp,
			    &ca, &putRow, &getRowIn, &getRowBlur, &fj, bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmFilter3Row( riDown.riBytes, cvDown, &(riDown.riDescription), riIn,
					    &fj, 0, getRowIn, putRow, bmBlur ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmFilter3Row( riUp.riBytes, cvUp, &(riUp.riDescription), riIn,
					    &fj, 1, getRowIn, putRow, bmBlur ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmBlurAverage( &fj, &(riDown.riDescription),
				cvDown, cvUp, riDown.riBytes, riUp.riBytes,
				getRowBlur, putRow ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  steal */
    *riOut= riDown; bmInitRasterImage( &riDown );

  ready:

    if  ( cvUp )
	{ free( cvUp );	}
    if  ( cvDown )
	{ free( cvDown );	}

    bmCleanRasterImage( &riDown );
    bmCleanRasterImage( &riUp );
    bmCleanFillJob( &fj );
    bmCleanColorAllocator( &ca );

    return rval;
    }

static int bmBlurRelative(	FillJob *			fj,
				const RasterImage *		riIn,
				const BitmapDescription *	bdBlur,
				ColorValue *			cvDown,
				ColorValue *			cvUp,
				unsigned char *			bufDown,
				const unsigned char *		bufUp,
				GetSourceRow			getRowIn,
				GetSourceRow			getRowBlur,
				PutScreenRow			putRowBlur )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				row;

    for ( row= 0; row < bdBlur->bdPixelsHigh; row++ )
	{
	int			col;

	ColorValue *		cvi;
	ColorValue *		cvd;
	ColorValue *		cvu;

	unsigned char *		to;
	const unsigned char *	fru;
	const unsigned char *	fri;

	to= bufDown+ row* bdBlur->bdBytesPerRow;
	fru= bufUp+ ( bdBlur->bdPixelsHigh- row- 1 )* bdBlur->bdBytesPerRow;
	fri= riIn->riBytes+ row* bdIn->bdBytesPerRow;

	cvi= fj->fjThisRow+ 1;
	cvd= cvDown+ 1;
	cvu= cvUp+ 1;

	bmInitColorRow( cvi, fj->fjFrWide );
	bmInitColorRow( cvd, fj->fjFrWide );
	bmInitColorRow( cvu, fj->fjFrWide );

	(*getRowBlur)( cvi, 0, fri, 0, bdIn  ->bdPixelsWide, bdIn   );
	(*getRowBlur)( cvd, 0, fru, 0, bdBlur->bdPixelsWide, bdBlur );
	(*getRowBlur)( cvu, 0, to,  0, bdBlur->bdPixelsWide, bdBlur );

	for ( col= 0; col < bdBlur->bdPixelsWide; cvd++, cvu++, cvi++, col++ )
	    {
	    int		blr;
	    int		ref;

	    blr= ( cvd->cvR+ cvu->cvR ) /2;
	    ref= ( blr+ 128 )/ 2;

	    if  ( cvi->cvR > blr || blr == 0 )
		{
		/* blr -> ref */
		/* 255 -> 255	*/

		cvd->cvR= ref+ ( ( 255- ref )* ( cvi->cvR- blr ) ) /
								( 255- blr );
		}
	    else{
		/*   0 -> 0   */
		/* blr -> ref */

		cvd->cvR= ( ref* cvi->cvR )/ blr;
		}

	    blr= ( cvd->cvG+ cvu->cvG ) /2;
	    ref= ( blr+ 128 )/ 2;

	    if  ( cvi->cvG > blr || blr == 0 )
		{
		cvd->cvG= ref+ ( ( 255- ref )* ( cvi->cvG- blr ) ) /
								( 255- blr );
		}
	    else{
		cvd->cvG= ( ref* cvi->cvG )/ blr;
		}

	    blr= ( cvd->cvB+ cvu->cvB ) /2;
	    ref= ( blr+ 128 )/ 2;

	    if  ( cvi->cvB > blr || blr == 0 )
		{
		cvd->cvB= ref+ ( ( 255- ref )* ( cvi->cvB- blr ) ) /
								( 255- blr );
		}
	    else{
		cvd->cvB= ( ref* cvi->cvB )/ blr;
		}
	    }

	if  ( (*putRowBlur)( to, fj, cvDown+ 1 ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int bmFilterRelative(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    int				rval= 0;
    const BitmapDescription *	bdIn= &(riIn->riDescription);

    ColorValue *		cvDown= (ColorValue *)0;
    ColorValue *		cvUp= (ColorValue *)0;

    PutScreenRow		putRow= (PutScreenRow)0;
    GetSourceRow		getRowIn= (GetSourceRow)0;
    GetSourceRow		getRowBlur= (GetSourceRow)0;

    FillJob			fj;
    RasterImage			riDown;
    RasterImage			riUp;
    ColorAllocator		ca;

    bmInitRasterImage( &riDown );
    bmInitRasterImage( &riUp );
    bmInitFillJob( &fj );
    bmInitColorAllocator( &ca );

    if  ( bmStartBlurOperation( &riDown, &riUp, &cvDown, &cvUp,
			    &ca, &putRow, &getRowIn, &getRowBlur, &fj, bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmFilter3Row( riDown.riBytes, cvDown, &(riDown.riDescription), riIn,
					    &fj, 0, getRowIn, putRow, bmBlur ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmFilter3Row( riUp.riBytes, cvUp, &(riUp.riDescription), riIn,
					    &fj, 1, getRowIn, putRow, bmBlur ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmBlurRelative( &fj, riIn, &(riDown.riDescription), cvDown, cvUp,
					riDown.riBytes, riUp.riBytes,
					getRowIn, getRowBlur, putRow ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  steal */
    *riOut= riDown; bmInitRasterImage( &riDown );

  ready:

    if  ( cvUp )
	{ free( cvUp );	}
    if  ( cvDown )
	{ free( cvDown );	}

    bmCleanRasterImage( &riDown );
    bmCleanRasterImage( &riUp );
    bmCleanFillJob( &fj );
    bmCleanColorAllocator( &ca );

    return rval;
    }
