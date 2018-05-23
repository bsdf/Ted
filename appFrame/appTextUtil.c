#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>
#   include	<utilMemoryBuffer.h>

#   include	"guiTextUtil.h"
#   include	"guiWidgets.h"
#   include	<geoString.h>

/************************************************************************/
/*									*/
/*  Get a 'Length', or an 'integer' from a text widget.			*/
/*									*/
/************************************************************************/

int appGetLengthFromTextWidget(		APP_WIDGET	w,
					int *		pValue,
					int *		pChanged,
					int		defaultUnit,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax )
    {
    int		value;

    char	scratch[50];
    char *	s;

    s= appGetStringFromTextWidget( w );
    if  ( geoLengthFromString( s, defaultUnit, &value )	||
	  ( ! adaptToMin && value < minValue )		||
	  ( ! adaptToMax && value > maxValue )		)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( w ); return -1;
	}

    geoLengthToString( scratch, *pValue, defaultUnit );
    if  ( ! strcmp( scratch, s ) )
	{ value= *pValue;	}

    appFreeStringFromTextWidget( s );

    if  ( adaptToMin && value < minValue )
	{ value=  minValue;	}
    if  ( adaptToMax && value > maxValue )
	{ value=  maxValue;	}

    *pChanged= ( value != *pValue );
    *pValue= value;

    return 0;
    }

int appGetIntegerFromTextWidget(	APP_WIDGET	w,
					int *		pValue,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax )
    {
    int		value;

    char *	s;
    char *	past;

    s= appGetStringFromTextWidget( w );

    value= strtol( s, &past, 10 );

    if  ( past != s )
	{
	while( *past == ' ' )
	    { past++;	}
	}

    if  ( past == s						||
	  *past							||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( w ); return -1;
	}

    appFreeStringFromTextWidget( s );

    if  ( adaptToMin && value < minValue )
	{ value=  minValue;	}
    if  ( adaptToMax && value > maxValue )
	{ value=  maxValue;	}

    *pValue= value;

    return 0;
    }

int appGetDoubleFromTextWidget(		APP_WIDGET	w,
					double *	pValue,
					double		minValue,
					int		adaptToMin,
					double		maxValue,
					int		adaptToMax )
    {
    double	value;

    char *	s;
    char *	past;

    s= appGetStringFromTextWidget( w );

    value= strtod( s, &past );

    if  ( past != s )
	{
	while( *past == ' ' )
	    { past++;	}
	}

    if  ( past == s						||
	  *past							||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( w ); return -1;
	}

    appFreeStringFromTextWidget( s );

    if  ( value < minValue )
	{ value=  minValue;	}
    if  ( value > maxValue )
	{ value=  maxValue;	}

    *pValue= value;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void appLengthToTextWidget(		APP_WIDGET	w,
					int		l,
					int		unit )
    {
    char	scratch[30];

    geoLengthToString( scratch, l, unit );
    appStringToTextWidget( w, scratch );

    return;
    }

void appIntegerToTextWidget(		APP_WIDGET	w,
					int		n )
    {
    char	scratch[20];

    sprintf( scratch, "%d", n );
    appStringToTextWidget( w, scratch );

    return;
    }

void appIntervalToTextWidget(		APP_WIDGET	w,
					int		n1,
					int		n2 )
    {
    char	scratch[30];

    if  ( n1 == n2 )
	{ sprintf( scratch, "%d", n1 );			}
    else{ sprintf( scratch, "%d - %d", n1, n2 );	}

    appStringToTextWidget( w, scratch );

    return;
    }

extern void appRectangleToTextWidget(	APP_WIDGET		w,
					int			row1,
					int			row2,
					int			col1,
					int			col2 )
    {
    char	scratch[50];
    char *	to= scratch;

    if  ( row1 == row2 )
	{ sprintf( to, "%d", row1 );		}
    else{ sprintf( to, "%d - %d", row1, row2 );	}

    strcat( scratch, ", " );
    to= scratch+ strlen( scratch );

    if  ( col1 == col2 )
	{ sprintf( to, "%d", col1 );		}
    else{ sprintf( to, "%d - %d", col1, col2 );	}

    appStringToTextWidget( w, scratch );
    }

void appDoubleToTextWidget(		APP_WIDGET	w,
					double		d )
    {
    char	scratch[30];

    sprintf( scratch, "%g", d );
    appStringToTextWidget( w, scratch );

    return;
    }

int appBufferFromTextWidget(		MemoryBuffer *	mb,
					APP_WIDGET	w )
    {
    char *	s= appGetStringFromTextWidget( w );

    if  ( utilMemoryBufferSetString( mb, s ) )
	{ LDEB(1); return -1;	}

    appFreeStringFromTextWidget( s );

    return 0;
    }

void guiBufferToText(	APP_WIDGET		w,
			const MemoryBuffer *	mb )
    {
    appStringToTextWidget( w, utilMemoryBufferGetString( mb ) );

    return;
    }
