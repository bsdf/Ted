#   ifndef	TED_FILE_CONVERT_H
#   define	TED_FILE_CONVERT_H

#   include	<appFrame.h>

#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>

#   include	<geo2DInteger.h>
#   include	<utilIndexMapping.h>

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedTtfToAfm(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedAfmToGSFontmap(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedAfmForFontFiles(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedFontsDocuments(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

#   endif	/*  TED_FILE_CONVERT_H	*/
