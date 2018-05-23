/************************************************************************/
/*									*/
/*  Operating system related functionality.				*/
/*									*/
/************************************************************************/

#   ifndef	APP_SYSTEM_H
#   define	APP_SYSTEM_H

#   include	<utilMemoryBuffer.h>

typedef int (*FILE_CALLBACK)	(	const MemoryBuffer *	name,
					void *			through );

typedef void (*APP_COMPLAIN)(		void *		through,
					int		errorId,
					const char *	errorSubject );

/************************************************************************/
/*									*/
/*  Error numbers. (Can be translated to strings by the caller.)	*/
/*									*/
/************************************************************************/

#	define APP_SYSTEMeNOERROR		0
#	define APP_SYSTEMeNOMEM			1
#	define APP_SYSTEMeFROM			2
#	define APP_SYSTEMeRCPT			3
#	define APP_SYSTEMeSMTP			4
#	define APP_SYSTEMeWRITE			5
#	define APP_SYSTEMeREAD			6
#	define APP_SYSTEMeUNAME			7
#	define APP_SYSTEMeHOST			8
#	define APP_SYSTEMeSERV			9
#	define APP_SYSTEMeSOCK			10
#	define APP_SYSTEMeCONN			11
#	define APP_SYSTEMeBIND			12
#	define APP_SYSTEMeLISTEN		13
#	define APP_SYSTEMeACCEPT		14

#	define APP_SYSTEMeCOUNT			15

/************************************************************************/
/*									*/
/*  Operating system related functionality.. Declarations.		*/
/*									*/
/************************************************************************/

extern int appHomeDirectory(	MemoryBuffer *	mb );
extern int appCurrentDirectory(	MemoryBuffer *	mb );

extern int appTestDirectory(	const MemoryBuffer *	dir );
extern int appTestFileWritable( const MemoryBuffer *	file );
extern int appTestFileExists( const MemoryBuffer *	mb );
extern int appTestFileReadable( const MemoryBuffer *	file );

extern int appMakeDirectory(	const MemoryBuffer *	dir );
extern int appMakeDirectories(	const MemoryBuffer *	dir );

extern long appGetTimestamp( void );

extern int appMakeUniqueString(	char *		target,
				unsigned int	maxlen );

extern int appFileNameIsAbsolute( const char *	filename );

extern int appAbsoluteName(	MemoryBuffer *		absolute,
				const MemoryBuffer *	relative,
				int			relativeIsFile,
				const MemoryBuffer *	nameRelativeTo );

extern int appRemoveFile(	const MemoryBuffer *	filename );
extern int appRenameFile(	const MemoryBuffer *	newName,
				const MemoryBuffer *	oldName );

extern int appCopyFile(		const MemoryBuffer *	newName,
				const MemoryBuffer *	oldName );

extern int appForAllFiles(	const MemoryBuffer *	dir,
				const char *		ext,
				void *			through,
				FILE_CALLBACK		callback );

extern int appOpenSocket(	const char *		hostName,
				const char *		portName,
				void *			through,
				APP_COMPLAIN		complain );
extern int appListenSocket(	const char *		portName,
				unsigned int *		pPort,
				void *			through,
				APP_COMPLAIN		complain );
extern int appAcceptSocket(	int			lfd,
				void *			through,
				APP_COMPLAIN		complain );

extern int appFileGetFileExtension(	MemoryBuffer *		extension,
					const MemoryBuffer *	filename );

extern int appFileGetRelativeName(	MemoryBuffer *		relative,
					const MemoryBuffer *	filename );

extern int appDirectoryOfFileName(	MemoryBuffer *		dir,
					const MemoryBuffer *	name );

extern int appFileSetExtension(		MemoryBuffer *		filename,
					const char *		extension );
extern int appFileAddExtension(		MemoryBuffer *		filename,
					const char *		extension );

#   endif	/*  APP_SYSTEM_H	*/
