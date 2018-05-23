/************************************************************************/
/*									*/
/*  Calculate the dos representation of a date/time value.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"
#   include	"utilDosTime.h"

#   include	<appDebugon.h>

#   include	<time.h>

/************************************************************************/
/*									*/
/*  Calculate the dos representation of a date/time value.		*/
/*									*/
/************************************************************************/

void utilDosTimeDate(		int *		pTime,
				int *		pDate,
				const long	unixTime )
    {
    time_t	unixTime_t;

    struct tm	struct_tm= *localtime( &unixTime_t );

    if  ( pTime )
	{
	*pTime=	( ( struct_tm.tm_sec/ 2 )	<< 0  ) +
		( ( struct_tm.tm_min )		<< 5  ) +
		( ( struct_tm.tm_hour )		<< 11 ) ;
	}

    if  ( pDate )
	{
	*pDate=	( ( struct_tm.tm_mday )		<< 0  ) +
		( ( struct_tm.tm_mon+ 1 )	<< 5  ) +
		( ( struct_tm.tm_year- 80 )	<< 9  ) ;
	}

    return;
    }
