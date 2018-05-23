#   ifndef		DRAW_CONFIG_H
#   define		DRAW_CONFIG_H

#   ifdef USE_FONTCONFIG
#	include	<ft2build.h>
#	include	FT_FREETYPE_H
#   endif

#   ifdef USE_XFT
#	include	<X11/Xft/Xft.h>
#	include	<X11/extensions/Xrender.h>
#   endif

#   endif	/*	DRAW_CONFIG_H	*/
