/************************************************************************/
/*									*/
/*  Windows Meta File specific definitions.				*/
/*									*/
/************************************************************************/

#   include	<drawDrawingSurface.h>
#   include	<drawMetafile.h>

typedef int (*MetafilePlayX11)(	void **			pPrivate,
				DrawingSurface		ds,
				const MetafilePlayer *	player );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfX11(	void **			pPrivate,
				DrawingSurface		ds,
				const MetafilePlayer *	player );

extern int appMetaPlayEmfX11(	void **			pPrivate,
				DrawingSurface		ds,
				const MetafilePlayer *	player );

extern int appMacPictPlayFileX11( void **			pPrivate,
				DrawingSurface		ds,
				const MetafilePlayer *	player );

