/************************************************************************/
/*									*/
/*  Exchange values with a text widget.					*/
/*									*/
/************************************************************************/

#   ifndef	GUI_TEXT_UTIL_H
#   define	GUI_TEXT_UTIL_H

#   include	<stdio.h>

#   include	"appGuiBase.h"
#   include	<utilMemoryBuffer.h>

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appGetLengthFromTextWidget(	APP_WIDGET	w,
					int *		pValue,
					int *		pChanged,
					int		defaultUnit,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax );

extern int appGetIntegerFromTextWidget(	APP_WIDGET	w,
					int *		pValue,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax );

extern int appGetDoubleFromTextWidget(	APP_WIDGET	w,
					double *	pValue,
					double		minValue,
					int		adaptToMin,
					double		maxValue,
					int		adaptToMax );

extern void appIntegerToTextWidget(	APP_WIDGET		w,
					int			n );

extern void appIntervalToTextWidget(	APP_WIDGET		w,
					int			n1,
					int			n2 );

extern void appRectangleToTextWidget(	APP_WIDGET		w,
					int			row1,
					int			row2,
					int			col1,
					int			col2 );

extern void appDoubleToTextWidget(	APP_WIDGET		w,
					double			d );

extern void appLengthToTextWidget(	APP_WIDGET		w,
					int			l,
					int			unit );

extern int appBufferFromTextWidget(	MemoryBuffer *		mb,
					APP_WIDGET		w );

extern void guiBufferToText(		APP_WIDGET		w,
					const MemoryBuffer *	mb );

# ifdef __cplusplus
    }
# endif

#   endif
