#####################################################################
##
##   Determine the platform. Use config.guess or if that does 
##   not work our own approximation.
##
#####################################################################

AC_DEFUN(AC_GUESS_PLATFORM,
[
    echo Guessing platform...

    MY_PLATFORM=
    MY_HOST=`(hostname||uname -n)`
    MY_DATE=`date`
    MY_HOST_DATE="$MY_HOST $MY_DATE"
    MY_RELEASE=`(lsb_release -ds || uname -s || echo Unknown ) | tr '"' "'"`

    MY_SYSTEM=`uname -s  | tr ':/ ' ___`
    MY_MACHINE=`uname -m | tr ':/ ' ___`

    if  ( config.guess ) >/dev/null 2>&1
    then
	MY_PLATFORM=`config.guess`
    else
	MY_PLATFORM=${MY_SYSTEM}_${MY_MACHINE}
    fi

    AC_SUBST(MY_SYSTEM)dnl
    AC_SUBST(MY_MACHINE)dnl
    AC_SUBST(MY_PLATFORM)dnl
    AC_SUBST(MY_HOST_DATE)dnl
    AC_SUBST(MY_RELEASE)dnl

    AC_DEFINE_UNQUOTED(MY_SYSTEM,"$MY_SYSTEM")dnl
    AC_DEFINE_UNQUOTED(MY_MACHINE,"$MY_MACHINE")dnl
    AC_DEFINE_UNQUOTED(MY_PLATFORM,"$MY_PLATFORM")dnl
    AC_DEFINE_UNQUOTED(MY_HOST_DATE,"$MY_HOST_DATE")dnl
    AC_DEFINE_UNQUOTED(MY_RELEASE,"$MY_RELEASE")dnl
])
#####################################################################
##
##   Find out how to link statically
##
##   1: From utter stupidity, we only know this for gcc. and very
##	few other systems.
##
#####################################################################

AC_DEFUN(AC_LD_FLAGS_STATIC,
[
    echo Checking for static link flags...

    LD_FLAGS_STATIC=

    #   1
    if  test x_$GCC = x_yes
    then
	LD_FLAGS_STATIC=-static
    else
	case `uname -s` in
	    OSF1)
		LD_FLAGS_STATIC=-non_shared
		;;
	    *)
		# 1
		;;
	esac
    fi


    AC_SUBST(LD_FLAGS_STATIC)dnl
])
#####################################################################
##
##   Look for gtk headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_GTK,
[
    echo Checking for Gtk etc.

    GTK_CFLAGS=
    GTK_LIBS=

    GTK_HEADERS_FOUND=NO
    GTK_LIBS_FOUND=NO

    # gtk 1.*
    if  ( gtk-config --cflags ) > /dev/null 2>&1
    then
	GTK_CFLAGS=`gtk-config --cflags`
	GTK_HEADERS_FOUND=YES
    fi

    if  ( gtk-config --libs ) > /dev/null 2>&1
    then
	GTK_LIBS=`gtk-config --libs`
	GTK_LIBS_FOUND=YES
    fi

    # gtk 2.0
    if  ( pkg-config --cflags gtk+-2.0 ) > /dev/null 2>&1
    then
	GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`
	GTK_HEADERS_FOUND=YES
    fi

    if  ( pkg-config --libs gtk+-2.0 ) > /dev/null 2>&1
    then
	GTK_LIBS=`pkg-config --libs gtk+-2.0`
	GTK_LIBS_FOUND=YES
    fi


    AC_SUBST(GTK_CFLAGS)dnl
    AC_SUBST(GTK_LIBS)dnl
])
#####################################################################
##
##   Look for freetype headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_FT2,
[
    echo Checking for FreeType etc.

    FT2_CFLAGS=
    FT2_LIBS=

    FT2_HEADERS_FOUND=NO
    FT2_LIBS_FOUND=NO
    FT2_FOUND=0

    if  ( freetype-config --cflags ) > /dev/null 2>&1
    then
	FT2_CFLAGS=`freetype-config --cflags`
	FT2_HEADERS_FOUND=YES
    fi

    if  ( freetype-config --libs ) > /dev/null 2>&1
    then
	FT2_LIBS=`freetype-config --libs`
	FT2_LIBS_FOUND=YES
    fi

    case $FT2_HEADERS_FOUND.$FT2_LIBS_FOUND in
	YES.YES)
	    FT2_FOUND=1
	    AC_DEFINE(HAVE_FT2,1)
	    ;;
	*)
	    FT2_FOUND=0
	    AC_DEFINE(HAVE_FT2,0)
	    ;;
    esac

    AC_SUBST(FT2_CFLAGS)dnl
    AC_SUBST(FT2_LIBS)dnl
])
#####################################################################
##
##   Look for fontconfig headers and libraries.
##
#####################################################################

AC_DEFUN(AC_HAVE_FONTCONFIG,
[
    echo Checking for Fontconfig etc.

    FONTCONFIG_CFLAGS=
    FONTCONFIG_LIBS=

    FONTCONFIG_HEADERS_FOUND=NO
    FONTCONFIG_LIBS_FOUND=NO
    FONTCONFIG_FOUND=0

    if  ( pkg-config fontconfig --cflags ) > /dev/null 2>&1
    then
	FONTCONFIG_CFLAGS=`pkg-config fontconfig --cflags`
	FONTCONFIG_HEADERS_FOUND=YES
    fi

    if  ( pkg-config fontconfig --libs ) > /dev/null 2>&1
    then
	FONTCONFIG_LIBS=`pkg-config fontconfig --libs`
	FONTCONFIG_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $FONTCONFIG_HEADERS_FOUND = NO -o $FONTCONFIG_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include fontconfig/fontconfig.h /usr/lib fontconfig"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a -f ${sod}/lib${so}.so
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    FONTCONFIG_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib)
		    FONTCONFIG_LIBS=-l${so}
		    ;;
		*)
		    FONTCONFIG_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		FONTCONFIG_HEADERS_FOUND=YES
		FONTCONFIG_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( FONTCONFIG,
	[ --with-FONTCONFIG Use Fontconfig if available],
	[
	    if  test $withval = yes
	    then
		USE_FONTCONFIG=YES
	    else
		USE_FONTCONFIG=NO
	    fi
	],
	[
	    USE_FONTCONFIG=TEST;
	])

    case $FONTCONFIG_HEADERS_FOUND.$FONTCONFIG_LIBS_FOUND in
	YES.YES)
	    FONTCONFIG_FOUND=1
	    HAVE_FONTCONFIG=YES
	    ;;
	*)
	    FONTCONFIG_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <fontconfig/fontconfig.h>],
	      [FcObjectSet *os; os = FcObjectSetBuild (FC_FAMILY, FC_WIDTH);],
		HAVE_FONTCONFIG=YES,HAVE_FONTCONFIG=NO,)
	    ;;
    esac

    case $HAVE_FONTCONFIG.$USE_FONTCONFIG in
	YES.TEST|YES.YES)
	    echo 'Using Fontconfig'
	    AC_DEFINE(HAVE_FONTCONFIG,1)
	    AC_DEFINE(USE_FONTCONFIG,1)
	    USE_FONTCONFIG=YES
	    ;;
	YES.NO)
	    echo 'Avoiding Fontconfig'
	    AC_DEFINE(HAVE_FONTCONFIG,1)
	    AC_DEFINE(USE_FONTCONFIG,0)
	    ;;
	NO.TEST)
	    echo 'No Fontconfig'
	    AC_DEFINE(HAVE_FONTCONFIG,0)
	    AC_DEFINE(USE_FONTCONFIG,0)
	    USE_FONTCONFIG=NO
	    ;;
	NO.YES)
	    echo '##### No Fontconfig found'
	    AC_DEFINE(HAVE_FONTCONFIG,0)
	    AC_DEFINE(USE_FONTCONFIG,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(FONTCONFIG_CFLAGS)dnl
    AC_SUBST(FONTCONFIG_LIBS)dnl
])
#####################################################################
##
##   Look for xft headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_XFT,
[
    echo Checking for XFT etc.

    XFT_CFLAGS=
    XFT_LIBS=

    XFT_HEADERS_FOUND=NO
    XFT_LIBS_FOUND=NO
    HAVE_XFT=NO

    #### Debian/Ubuntu
    if  ( xft-config --cflags ) > /dev/null 2>&1
    then
	XFT_CFLAGS=`xft-config --cflags`
	XFT_HEADERS_FOUND=YES
    fi

    #### Debian/Ubuntu
    if  ( xft-config --libs ) > /dev/null 2>&1
    then
	XFT_LIBS=`xft-config --libs`
	XFT_LIBS_FOUND=YES
    fi

    #### Redhat
    if  ( pkg-config xft --cflags ) > /dev/null 2>&1
    then
	XFT_CFLAGS=`pkg-config xft --cflags`
	XFT_HEADERS_FOUND=YES
    fi

    #### Redhat
    if  ( pkg-config xft --libs ) > /dev/null 2>&1
    then
	XFT_LIBS=`pkg-config xft --libs`
	XFT_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $XFT_HEADERS_FOUND = NO -o $XFT_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include X11/Xft/Xft.h /usr/lib64 Xft" \
	    "/usr/include X11/Xft/Xft.h /usr/lib Xft"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a -f ${sod}/lib${so}.so
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    XFT_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib)
		    XFT_LIBS=-l${so}
		    ;;
		*)
		    XFT_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		XFT_HEADERS_FOUND=YES
		XFT_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( XFT,
	[ --with-XFT Use Xft if available],
	[
	    if  test $withval = yes
	    then
		USE_XFT=YES
	    else
		USE_XFT=NO
	    fi
	],
	[
	    USE_XFT=TEST;
	])

    case $XFT_HEADERS_FOUND.$XFT_LIBS_FOUND in
	YES.YES)
	    HAVE_XFT=YES
	    ;;
	*)
	    HAVE_XFT=NO
	    ;;
    esac

    case $HAVE_XFT.$USE_XFT in
	YES.TEST|YES.YES)
	    case $USE_FONTCONFIG in
		YES)
		    echo 'Using Xft'
		    AC_DEFINE(HAVE_XFT,1)
		    AC_DEFINE(USE_XFT,1)
		    USE_XFT=YES
		    ;;
		*)
		    echo '##### Not using Xft without Fontconfig'
		    AC_DEFINE(HAVE_XFT,1)
		    AC_DEFINE(USE_XFT,0)
		    USE_XFT=NO
		    ;;
	    esac
	    ;;
	YES.NO)
	    echo 'Avoiding Xft'
	    AC_DEFINE(HAVE_XFT,1)
	    AC_DEFINE(USE_XFT,0)
	    ;;
	NO.TEST)
	    echo 'No Xft'
	    AC_DEFINE(HAVE_XFT,0)
	    AC_DEFINE(USE_XFT,0)
	    USE_XFT=NO
	    ;;
	NO.YES)
	    echo '##### No Xft found'
	    AC_DEFINE(HAVE_XFT,0)
	    AC_DEFINE(USE_XFT,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(XFT_CFLAGS)dnl
    AC_SUBST(XFT_LIBS)dnl
])

#####################################################################
##
##   Look for XPM X pixmap headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_XPM,
[
    echo Checking for XPM X pixmap...

    LIBXPM_CFLAGS=
    LIBXPM_LIBS=

    LIBXPM_HEADERS_FOUND=NO
    LIBXPM_LIBS_FOUND=NO
    LIBXPM_FOUND=0

    for xpm_name in xpm libxpm4
    do
	if  ( pkg-config ${xpm_name} --cflags ) > /dev/null 2>&1
	then
	    LIBXPM_CFLAGS=`pkg-config ${xpm_name} --cflags`
	    LIBXPM_HEADERS_FOUND=YES
	fi

	if  ( pkg-config ${xpm_name} --libs ) > /dev/null 2>&1
	then
	    LIBXPM_LIBS=`pkg-config ${xpm_name} --libs`
	    LIBXPM_LIBS_FOUND=YES
	fi
    done

    ########  The hard way
    if test $LIBXPM_HEADERS_FOUND = NO -o $LIBXPM_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include X11/xpm.h /usr/lib Xpm" \
	    "/usr/X11R6/include X11/xpm.h /usr/X11R6/lib Xpm" \
	    "/usr/X11/include X11/xpm.h /usr/X11/lib Xpm" \
	    "/usr/local/include X11/xpm.h /usr/local/lib Xpm" \
	    "/usr/pkg/include X11/xpm.h /usr/pkg/lib Xpm" \
	    "/usr/local/include/X11R6 X11/xpm.h /usr/local/lib/X11R6 Xpm" \
	    "/usr/apps/include X11/xpm.h /usr/apps/lib Xpm"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( -f ${sod}/lib${so}.so -o -f ${sod}/lib${so}.so.4 \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBXPM_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib)
		    LIBXPM_LIBS=-l${so}
		    ;;
		*)
		    LIBXPM_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBXPM_HEADERS_FOUND=YES
		LIBXPM_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBXPM,
	[ --with-LIBXPM Use LIBXPM X pixmap if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBXPM=YES
	    else
		USE_LIBXPM=NO
	    fi
	],
	[
	    USE_LIBXPM=TEST;
	])

    case $LIBXPM_HEADERS_FOUND.$LIBXPM_LIBS_FOUND in
	YES.YES)
	    LIBXPM_FOUND=1
	    HAVE_LIBXPM=YES
	    ;;
	*)
	    LIBXPM_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <X11/xpm.h>],
	      [XpmImage image; XpmReadFileToXpmImage( "/dev/null", &image, (XpmInfo *)0 );],
		HAVE_LIBXPM=YES,HAVE_LIBXPM=NO,)
	    ;;
    esac

    case $HAVE_LIBXPM.$USE_LIBXPM in
	YES.TEST|YES.YES)
	    echo 'Using libxpm'
	    AC_DEFINE(HAVE_LIBXPM,1)
	    AC_DEFINE(USE_LIBXPM,1)
	    USE_LIBXPM=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libxpm'
	    AC_DEFINE(HAVE_LIBXPM,1)
	    AC_DEFINE(USE_LIBXPM,0)
	    ;;
	NO.TEST)
	    echo 'No libxpm'
	    AC_DEFINE(HAVE_LIBXPM,0)
	    AC_DEFINE(USE_LIBXPM,0)
	    USE_LIBXPM=NO
	    ;;
	NO.YES)
	    echo '##### No libxpm found'
	    AC_DEFINE(HAVE_LIBXPM,0)
	    AC_DEFINE(USE_LIBXPM,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBXPM_CFLAGS)dnl
    AC_SUBST(LIBXPM_LIBS)dnl
])
#####################################################################
##
##   Look for libpaper headers and libraries.
##
#####################################################################

AC_DEFUN(AC_HAVE_LIBPAPER,
[
    echo Checking for libpaper etc.

    LIBPAPER_CFLAGS=
    LIBPAPER_LIBS=

    LIBPAPER_HEADERS_FOUND=NO
    LIBPAPER_LIBS_FOUND=NO
    LIBPAPER_FOUND=0

    if  ( pkg-config libpaper --cflags ) > /dev/null 2>&1
    then
	LIBPAPER_CFLAGS=`pkg-config libpaper --cflags`
	LIBPAPER_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpaper --libs ) > /dev/null 2>&1
    then
	LIBPAPER_LIBS=`pkg-config libpaper --libs`
	LIBPAPER_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBPAPER_HEADERS_FOUND = NO -o $LIBPAPER_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include paper.h /usr/lib64 paper" \
	    "/usr/include paper.h - paper"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( ${sod} = - -o -f ${sod}/lib${so}.so \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBPAPER_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBPAPER_LIBS=-l${so}
		    ;;
		*)
		    LIBPAPER_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBPAPER_HEADERS_FOUND=YES
		LIBPAPER_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBPAPER,
	[ --with-LIBPAPER Use libpaper if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBPAPER=YES
	    else
		USE_LIBPAPER=NO
	    fi
	],
	[
	    USE_LIBPAPER=TEST;
	])

    case $LIBPAPER_HEADERS_FOUND.$LIBPAPER_LIBS_FOUND in
	YES.YES)
	    LIBPAPER_FOUND=1
	    HAVE_LIBPAPER=YES
	    ;;
	*)
	    LIBPAPER_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <paper.h>],
	      [paperinit();],
		HAVE_LIBPAPER=YES,HAVE_LIBPAPER=NO,)
	    ;;
    esac

    case $HAVE_LIBPAPER.$USE_LIBPAPER in
	YES.TEST|YES.YES)
	    echo 'Using libpaper'
	    AC_DEFINE(HAVE_LIBPAPER,1)
	    AC_DEFINE(USE_LIBPAPER,1)
	    USE_LIBPAPER=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libpaper'
	    AC_DEFINE(HAVE_LIBPAPER,1)
	    AC_DEFINE(USE_LIBPAPER,0)
	    ;;
	NO.TEST)
	    echo 'No libpaper'
	    AC_DEFINE(HAVE_LIBPAPER,0)
	    AC_DEFINE(USE_LIBPAPER,0)
	    USE_LIBPAPER=NO
	    ;;
	NO.YES)
	    echo '##### No libpaper found'
	    AC_DEFINE(HAVE_LIBPAPER,0)
	    AC_DEFINE(USE_LIBPAPER,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBPAPER_CFLAGS)dnl
    AC_SUBST(LIBPAPER_LIBS)dnl
])
#####################################################################
##
##   Look for Motif
##
#####################################################################

AC_DEFUN(AC_PATH_MOTIF,
[
    echo Checking for Motif...

    XM_CFLAGS=
    XM_LIBS=
    XM_EXTRA_LIBS=
    XM_STATIC_REF=
    XM_SHARED_REF=

    MOTIF_HEADERS_FOUND=NO
    MOTIF_LIBS_FOUND=NO

    ac_xm_includes=${ac_xm_includes:-NO}
    ac_xm_libraries=${ac_xm_libraries:-NO}
    ac_xm_static_lib=NO
    ac_xm_shared_lib=NO

    if  test $ac_xm_includes = NO
    then
	# Includes
	for ac_dir in				\
	    /usr/include			\
	    /usr/X11R6/include			\
	    /usr/local/include			\
	    /usr/dt/include			\
	    /opt/SUNWmotif/include		\
	    /usr/include/Motif2.1		\
	    /usr/apps/include			\
	    /usr/pkg/include			\
	    /usr/local/LessTif/include		\
	    /usr/X11R6/LessTif/Motif1.2/include
	do
	if  test -r "$ac_dir/Xm/Xm.h"
	    then
		ac_xm_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_xm_libraries = NO
    then
	# Libraries
	for ac_dir in				\
	    /usr/lib				\
	    /usr/X11R6/lib			\
	    /usr/local/lib			\
	    /usr/dt/lib				\
	    /opt/SUNWmotif/lib			\
	    /usr/apps/lib			\
	    /usr/pkg/lib			\
	    /usr/local/LessTif/lib		\
	    /usr/X11R6/LessTif/Motif1.2/lib
	do
	found=no

	if  test -r "$ac_dir/libXm.a"
	    then
		ac_xm_libraries=$ac_dir
		ac_xm_static_lib=$ac_dir/libXm.a
		found=yes
	    fi

	if  test -r "$ac_dir/libXm.so"
	    then
		ac_xm_libraries=$ac_dir
		ac_xm_shared_lib=$ac_dir/libXm.so
		found=yes
	    fi

	if  test -r "$ac_dir/libXm.dylib"
	    then
		ac_xm_libraries=$ac_dir
		ac_xm_shared_lib=$ac_dir/libXm.dylib
		found=yes
	    fi

	if  test $found = yes
	    then
		break
	    fi
	done
    fi

    #echo Includes : $ac_xm_includes
    #echo Libraries: $ac_xm_libraries

    XM_EXTRA_LIBS=""
    #  Too simple..
    #  AC_CHECK_LIB( Xp, XpStartPage, XM_EXTRA_LIBS="-lXp" )
    if  test -r $x_libraries/libXp.a
    then
	XM_EXTRA_LIBS="${XM_EXTRA_LIBS} -lXp"
    else
	if  test -r $x_libraries/libXp.so
	then
	    XM_EXTRA_LIBS="${XM_EXTRA_LIBS} -lXp"
	fi
    fi

    if  test $ac_xm_includes != NO
    then
	XM_CFLAGS=" -I$ac_xm_includes"
	MOTIF_HEADERS_FOUND=YES

	if  test "$XM_CFLAGS" = "$X_CFLAGS"
	then
	    XM_CFLAGS=
	fi
    fi

    if  test $ac_xm_libraries != NO
    then
	XM_LIBS=" -L$ac_xm_libraries"
	MOTIF_LIBS_FOUND=YES

	if  test "$XM_LIBS" = "$X_LIBS"
	then
	    XM_LIBS=
	fi
    fi

    # Some SGI IRIX versions
    if  test $ac_xm_includes = NO
    then
	if test -f /usr/include/Xm.h
	then
	    XM_CFLAGS=
	    MOTIF_HEADERS_FOUND=YES
	fi
    fi

    # Some SGI IRIX versions
    if  test $ac_xm_libraries = NO
    then
	if test -f /usr/lib/libXm.a
	then
	    XM_LIBS=
	    MOTIF_LIBS_FOUND=YES
	fi

	if test -f /usr/lib/libXm.so
	then
	    XM_LIBS=
	    MOTIF_LIBS_FOUND=YES
	fi
    fi

    if  test $ac_xm_static_lib != NO
    then
	XM_STATIC_REF="$ac_xm_static_lib"
	if  nm $ac_xm_static_lib | grep -q XpStartPage 2>/dev/null
	then
	    XM_EXTRA_LIBS="${XM_EXTRA_LIBS} -lXp"
	fi
	if  nm $ac_xm_static_lib | grep -q XEditResPut8 2>/dev/null
	then
	    XM_EXTRA_LIBS="${XM_EXTRA_LIBS} -lXmu"
	fi
    else
	XM_STATIC_REF="$XM_LIBS -lXm"
    fi

    XM_SHARED_REF="$XM_LIBS -lXm"

    AC_SUBST(XM_CFLAGS)dnl
    AC_SUBST(XM_LIBS)dnl
    AC_SUBST(XM_STATIC_REF)dnl
    AC_SUBST(XM_SHARED_REF)dnl
    AC_SUBST(XM_EXTRA_LIBS)dnl
])
#####################################################################
##
##   Look for libpng
##
#####################################################################

AC_DEFUN(AC_PATH_PNG,
[
    echo Checking for libpng...

    LIBPNG_CFLAGS=
    LIBPNG_LIBS=

    LIBPNG_HEADERS_FOUND=NO
    LIBPNG_LIBS_FOUND=NO
    LIBPNG_FOUND=0

    if  ( pkg-config libpng --cflags ) > /dev/null 2>&1
    then
	LIBPNG_CFLAGS=`pkg-config libpng --cflags`
	LIBPNG_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpng --libs ) > /dev/null 2>&1
    then
	LIBPNG_LIBS=`pkg-config libpng --libs`
	LIBPNG_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBPNG_HEADERS_FOUND = NO -o $LIBPNG_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include png.h /usr/lib64 png" \
	    "/usr/include png.h /usr/lib png" \
	    "/usr/local/include png.h /usr/local/lib png" \
	    "/usr/pkg/include png.h /usr/pkg/lib png" \
	    "/usr/X11R6/include png.h /usr/X11R6/lib png" \
	    "/usr/apps/include png.h /usr/apps/lib png" \
	    "/usr/sfw/include png.h /usr/sfw/lib png" \
	    "/opt/sfw/include png.h /opt/sfw/lib png" \
	    "/usr/local/include/png png.h /usr/local/lib/png png" \
	    "/usr/local/include/libpng png.h /usr/local/lib/libpng png"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a -f ${sod}/lib${so}.so
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBPNG_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib)
		    LIBPNG_LIBS=-l${so}
		    ;;
		*)
		    LIBPNG_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBPNG_HEADERS_FOUND=YES
		LIBPNG_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBPNG,
	[ --with-LIBPNG Use libpng if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBPNG=YES
	    else
		USE_LIBPNG=NO
	    fi
	],
	[
	    USE_LIBPNG=TEST;
	])

    case $LIBPNG_HEADERS_FOUND.$LIBPNG_LIBS_FOUND in
	YES.YES)
	    LIBPNG_FOUND=1
	    HAVE_LIBPNG=YES
	    ;;
	*)
	    LIBPNG_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <png.h>],
	      [png_create_read_struct(LIBPNG_LIBPNG_VER_STRING,(void *)0,(png_error_ptr)0,(png_error_ptr)0);],
		HAVE_LIBPNG=YES,HAVE_LIBPNG=NO,)
	    ;;
    esac

    case $HAVE_LIBPNG.$USE_LIBPNG in
	YES.TEST|YES.YES)
	    echo 'Using libpng'
	    AC_DEFINE(HAVE_LIBPNG,1)
	    AC_DEFINE(USE_LIBPNG,1)
	    USE_LIBPNG=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libpng'
	    AC_DEFINE(HAVE_LIBPNG,1)
	    AC_DEFINE(USE_LIBPNG,0)
	    ;;
	NO.TEST)
	    echo 'No libpng'
	    AC_DEFINE(HAVE_LIBPNG,0)
	    AC_DEFINE(USE_LIBPNG,0)
	    USE_LIBPNG=NO
	    ;;
	NO.YES)
	    echo '##### No libpng found'
	    AC_DEFINE(HAVE_LIBPNG,0)
	    AC_DEFINE(USE_LIBPNG,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBPNG_CFLAGS)dnl
    AC_SUBST(LIBPNG_LIBS)dnl
])
#####################################################################
##
##   Look for zlib
##
#####################################################################

AC_DEFUN(AC_PATH_ZLIB,
[
    echo Checking for zlib...

    ZLIB_CFLAGS=
    ZLIB_LIBS=

    ZLIB_HEADERS_FOUND=NO
    ZLIB_LIBS_FOUND=NO
    ZLIB_FOUND=0

    if  ( pkg-config zlib --cflags ) > /dev/null 2>&1
    then
	ZLIB_CFLAGS=`pkg-config zlib --cflags`
	ZLIB_HEADERS_FOUND=YES
    fi

    if  ( pkg-config zlib --libs ) > /dev/null 2>&1
    then
	ZLIB_LIBS=`pkg-config zlib --libs`
	ZLIB_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $ZLIB_HEADERS_FOUND = NO -o $ZLIB_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include zlib.h /usr/lib64 z" \
	    "/usr/include zlib.h /usr/lib z" \
	    "/usr/local/include zlib.h /usr/local/lib z" \
	    "/usr/pkg/include zlib.h /usr/pkg/lib z"	\
	    "/usr/local/include/zlib zlib.h /usr/local/lib/zlib z" \
	    "/usr/apps/include zlib.h /usr/apps/lib z"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a -f ${sod}/lib${so}.so
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    ZLIB_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib)
		    ZLIB_LIBS=-l${so}
		    ;;
		*)
		    ZLIB_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		ZLIB_HEADERS_FOUND=YES
		ZLIB_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( ZLIB,
	[ --with-ZLIB Use zlib if available],
	[
	    if  test $withval = yes
	    then
		USE_ZLIB=YES
	    else
		USE_ZLIB=NO
	    fi
	],
	[
	    USE_ZLIB=TEST;
	])

    case $ZLIB_HEADERS_FOUND.$ZLIB_LIBS_FOUND in
	YES.YES)
	    ZLIB_FOUND=1
	    HAVE_ZLIB=YES
	    ;;
	*)
	    ZLIB_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <zlib.h>],
	      [deflateInit((z_stream *)0,Z_DEFAULT_COMPRESSION);],
		HAVE_ZLIB=YES,HAVE_ZLIB=NO,)
	    ;;
    esac

    case $HAVE_ZLIB.$USE_ZLIB in
	YES.TEST|YES.YES)
	    echo 'Using zlib'
	    AC_DEFINE(HAVE_ZLIB,1)
	    AC_DEFINE(USE_ZLIB,1)
	    USE_ZLIB=YES
	    ;;
	YES.NO)
	    echo 'Avoiding zlib'
	    AC_DEFINE(HAVE_ZLIB,1)
	    AC_DEFINE(USE_ZLIB,0)
	    ;;
	NO.TEST)
	    echo 'No zlib'
	    AC_DEFINE(HAVE_ZLIB,0)
	    AC_DEFINE(USE_ZLIB,0)
	    USE_ZLIB=NO
	    ;;
	NO.YES)
	    echo '##### No zlib found'
	    AC_DEFINE(HAVE_ZLIB,0)
	    AC_DEFINE(USE_ZLIB,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(ZLIB_CFLAGS)dnl
    AC_SUBST(ZLIB_LIBS)dnl
])
#####################################################################
##
##   Look for iconv
##
#####################################################################

AC_DEFUN(AC_PATH_ICONV,
[
    echo Checking for iconv...

    ICONV_CFLAGS=
    ICONV_LIBS=
    ICONV_STATIC_REF=
    ICONV_SHARED_REF=

    ICONV_FOUND=0

    ac_iconv_includes=${ac_iconv_includes:-NO}
    ac_iconv_libraries=${ac_iconv_libraries:-NO}
    ac_iconv_static_lib=NO
    ac_iconv_shared_lib=NO

    if  test $ac_iconv_includes = "NO"
    then
	# Includes
	for ac_dir in			\
	    /usr/include		\
	    /usr/local/include		\
	    /usr/pkg/include		\
	    /usr/local/include/iconv	\
	    /usr/apps/include		\
	    ../iconv
	do
	if  test -r "$ac_dir/iconv.h"
	    then
		ac_iconv_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_iconv_libraries = "NO"
    then
	# Libraries
	for ac_dir in			\
	    /usr/lib			\
	    /usr/local/lib		\
	    /usr/pkg/lib		\
	    /usr/local/lib/iconv	\
	    /usr/apps/lib		\
	    ../iconv
	do
	found=no

	if  test -r "$ac_dir/iconv.a"
	    then
		ac_iconv_libraries=$ac_dir
		ac_iconv_static_lib=$ac_dir/iconv.a
		found=yes
		break
	    fi

	if  test -r "$ac_dir/iconv.so"
	    then
		ac_iconv_libraries=$ac_dir
		ac_iconv_shared_lib=$ac_dir/iconv.so
		found=yes
		break
	    fi

	if  test -r "$ac_dir/iconv.dylib"
	    then
		ac_iconv_libraries=$ac_dir
		ac_iconv_shared_lib=$ac_dir/iconv.dylib
		found=yes
		break
	    fi

	if  test $found = yes
	    then
		break
	    fi

	done
    fi

    #echo Includes : $ac_iconv_includes
    #echo Libraries: $ac_iconv_libraries

    if  test $ac_iconv_includes = NO
    then
	ICONV_FOUND=0
	AC_DEFINE(HAVE_ICONV,0)
    else
	ICONV_FOUND=1
	AC_DEFINE(HAVE_ICONV,1)
	ICONV_CFLAGS=-I$ac_iconv_includes

	if  test "$ICONV_CFLAGS" = "-I/usr/include"
	then
	    ICONV_CFLAGS=
	fi
    fi

    if  test $ac_iconv_libraries != NO
    then
	ICONV_LIBS="-L$ac_iconv_libraries"

	if  test $ac_iconv_static_lib != NO
	then
	    ICONV_STATIC_REF="$ac_iconv_static_lib"
	else
	    ICONV_STATIC_REF="$ICONV_LIBS -liconv"
	fi

    ICONV_SHARED_REF="$ICONV_LIBS -liconv"

    fi

    AC_SUBST(ICONV_CFLAGS)dnl
    AC_SUBST(ICONV_LIBS)dnl
    AC_SUBST(ICONV_STATIC_REF)dnl
    AC_SUBST(ICONV_SHARED_REF)dnl
    AC_SUBST(ICONV_FOUND)dnl
])
#####################################################################
##
##   Look for pcre
##
#####################################################################

AC_DEFUN(AC_PATH_PCRE,
[
    echo Checking for pcre...

    PCRE_CFLAGS=
    PCRE_LIBS=

    PCRE_HEADERS_FOUND=NO
    PCRE_LIBS_FOUND=NO
    PCRE_FOUND=0

    if  ( pkg-config libpcre --cflags ) > /dev/null 2>&1
    then
	PCRE_CFLAGS=`pkg-config libpcre --cflags`
	PCRE_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpcre --libs ) > /dev/null 2>&1
    then
	PCRE_LIBS=`pkg-config libpcre --libs`
	PCRE_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $PCRE_HEADERS_FOUND = NO -o $PCRE_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include pcre.h /usr/lib64 pcre" \
	    "/usr/include pcre.h /usr/lib pcre" \
	    "/usr/local/include pcre.h /usr/local/lib pcre" \
	    "/usr/pkg/include pcre.h /usr/pkg/lib pcre" \
	    "/usr/include/pcre pcre.h /usr/lib/pcre pcre" \
	    "/usr/local/include/pcre pcre.h /usr/local/lib/pcre pcre" \
	    "/usr/apps/include pcre.h /usr/apps/lib pcre"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a -f ${sod}/lib${so}.so
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    PCRE_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib)
		    PCRE_LIBS=-l${so}
		    ;;
		*)
		    PCRE_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		PCRE_HEADERS_FOUND=YES
		PCRE_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    #echo Includes : $PCRE_CFLAGS
    #echo Libraries: $PCRE_LIBS

    if  test $PCRE_HEADERS_FOUND = NO
    then
	PCRE_FOUND=0
	AC_DEFINE(HAVE_PCRE,0)
    else
	PCRE_FOUND=1
	AC_DEFINE(HAVE_PCRE,1)

	if  test "$PCRE_CFLAGS" = "-I/usr/include"
	then
	    PCRE_CFLAGS=
	fi
    fi

    AC_SUBST(PCRE_CFLAGS)dnl
    AC_SUBST(PCRE_LIBS)dnl
    AC_SUBST(PCRE_FOUND)dnl
])
#####################################################################
##
##   Look for libjpeg
##
#####################################################################

AC_DEFUN(AC_PATH_JPEG,
[
    echo Checking for libjpeg...

    LIBJPEG_CFLAGS=
    LIBJPEG_LIBS=

    LIBJPEG_HEADERS_FOUND=NO
    LIBJPEG_LIBS_FOUND=NO
    LIBJPEG_FOUND=0

    if  ( pkg-config libjpeg --cflags ) > /dev/null 2>&1
    then
	LIBJPEG_CFLAGS=`pkg-config libjpeg --cflags`
	LIBJPEG_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libjpeg --libs ) > /dev/null 2>&1
    then
	LIBJPEG_LIBS=`pkg-config libjpeg --libs`
	LIBJPEG_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBJPEG_HEADERS_FOUND = NO -o $LIBJPEG_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include jpeglib.h /usr/lib64 jpeg" \
	    "/usr/include jpeglib.h - jpeg" \
	    "/usr/local/include jpeglib.h - jpeg" \
	    "/usr/pkg/include jpeglib.h /usr/pkg/lib jpeg" \
	    "/usr/apps/include jpeglib.h /usr/apps/lib jpeg" \
	    "/usr/sfw/include jpeglib.h /usr/sfw/lib jpeg" \
	    "/opt/sfw/include jpeglib.h /opt/sfw/lib jpeg"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( ${sod} = - -o -f ${sod}/lib${so}.so \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBJPEG_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBJPEG_LIBS=-l${so}
		    ;;
		*)
		    LIBJPEG_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBJPEG_HEADERS_FOUND=YES
		LIBJPEG_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBJPEG,
	[ --with-LIBJPEG Use libjpeg if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBJPEG=YES
	    else
		USE_LIBJPEG=NO
	    fi
	],
	[
	    USE_LIBJPEG=TEST;
	])

    case $LIBJPEG_HEADERS_FOUND.$LIBJPEG_LIBS_FOUND in
	YES.YES)
	    LIBJPEG_FOUND=1
	    HAVE_LIBJPEG=YES
	    ;;
	*)
	    LIBJPEG_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <jpeglib.h>],
	      [jpeg_create_decompress((struct jpeg_decompress_struct *)0);],
		HAVE_LIBJPEG=YES,HAVE_LIBJPEG=NO,)
	    ;;
    esac

    case $HAVE_LIBJPEG.$USE_LIBJPEG in
	YES.TEST|YES.YES)
	    echo 'Using libjpeg'
	    AC_DEFINE(HAVE_LIBJPEG,1)
	    AC_DEFINE(USE_LIBJPEG,1)
	    USE_LIBJPEG=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libjpeg'
	    AC_DEFINE(HAVE_LIBJPEG,1)
	    AC_DEFINE(USE_LIBJPEG,0)
	    ;;
	NO.TEST)
	    echo 'No libjpeg'
	    AC_DEFINE(HAVE_LIBJPEG,0)
	    AC_DEFINE(USE_LIBJPEG,0)
	    USE_LIBJPEG=NO
	    ;;
	NO.YES)
	    echo '##### No libjpeg found'
	    AC_DEFINE(HAVE_LIBJPEG,0)
	    AC_DEFINE(USE_LIBJPEG,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBJPEG_CFLAGS)dnl
    AC_SUBST(LIBJPEG_LIBS)dnl
])
#####################################################################
##
##   Look for libtiff
##
#####################################################################

AC_DEFUN(AC_PATH_TIFF,
[
    echo Checking for libtiff...

    LIBTIFF_CFLAGS=
    LIBTIFF_LIBS=

    LIBTIFF_HEADERS_FOUND=NO
    LIBTIFF_LIBS_FOUND=NO
    LIBTIFF_FOUND=0

    if  ( pkg-config libtiff --cflags ) > /dev/null 2>&1
    then
	LIBTIFF_CFLAGS=`pkg-config libtiff --cflags`
	LIBTIFF_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libtiff --libs ) > /dev/null 2>&1
    then
	LIBTIFF_LIBS=`pkg-config libtiff --libs`
	LIBTIFF_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBTIFF_HEADERS_FOUND = NO -o $LIBTIFF_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include tiffio.h /usr/lib64 tiff" \
	    "/usr/include tiffio.h - tiff" \
	    "/usr/local/include tiffio.h /usr/local/lib tiff" \
	    "/usr/pkg/include tiffio.h /usr/pkg/lib tiff" \
	    "/usr/local/include/tiff tiffio.h /usr/local/lib/tiff tiff" \
	    "/usr/apps/include tiffio.h /usr/apps/lib tiff" \
	    "/usr/sfw/include tiffio.h /usr/sfw/lib tiff" \
	    "/opt/sfw/include tiffio.h /opt/sfw/lib tiff"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( ${sod} = - -o -f ${sod}/lib${so}.so \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBTIFF_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBTIFF_LIBS=-l${so}
		    ;;
		*)
		    LIBTIFF_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBTIFF_HEADERS_FOUND=YES
		LIBTIFF_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBTIFF,
	[ --with-LIBTIFF Use libtiff if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBTIFF=YES
	    else
		USE_LIBTIFF=NO
	    fi
	],
	[
	    USE_LIBTIFF=TEST;
	])

    case $LIBTIFF_HEADERS_FOUND.$LIBTIFF_LIBS_FOUND in
	YES.YES)
	    LIBTIFF_FOUND=1
	    HAVE_LIBTIFF=YES
	    ;;
	*)
	    LIBTIFF_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <tiffio.h>],
	      [TIFFOpen("/tmp/some.tiff","r");],
		HAVE_LIBTIFF=YES,HAVE_LIBTIFF=NO,)
	    ;;
    esac

    case $HAVE_LIBTIFF.$USE_LIBTIFF in
	YES.TEST|YES.YES)
	    echo 'Using libtiff'
	    AC_DEFINE(HAVE_LIBTIFF,1)
	    AC_DEFINE(USE_LIBTIFF,1)
	    USE_LIBTIFF=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libtiff'
	    AC_DEFINE(HAVE_LIBTIFF,1)
	    AC_DEFINE(USE_LIBTIFF,0)
	    ;;
	NO.TEST)
	    echo 'No libtiff'
	    AC_DEFINE(HAVE_LIBTIFF,0)
	    AC_DEFINE(USE_LIBTIFF,0)
	    USE_LIBTIFF=NO
	    ;;
	NO.YES)
	    echo '##### No libtiff found'
	    AC_DEFINE(HAVE_LIBTIFF,0)
	    AC_DEFINE(USE_LIBTIFF,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBTIFF_CFLAGS)dnl
    AC_SUBST(LIBTIFF_LIBS)dnl
])
#####################################################################
##
##   Give the caller the possibility to interfere with install
##   locations.
##
#####################################################################

AC_DEFUN(AC_INSTALL_LOCATIONS,
[
    dnl Installation locations ..

    AC_PREFIX_DEFAULT("/usr")

    if  test x_$prefix = x_NONE
    then
	Q_PREFIX=/usr
	Q_EXEC_PREFIX=/usr
	Q_BINDIR=/usr/bin
	Q_DATADIR=/usr/share
    else
	Q_PREFIX=$prefix
	Q_EXEC_PREFIX=$prefix
	Q_BINDIR=$bindir
	Q_DATADIR=$datadir
    fi

    if  test x_$exec_prefix = x_NONE
    then
	: ok
    else
	Q_EXEC_PREFIX=$exec_prefix
	Q_BINDIR=$exec_prefix/bin
    fi

    case $bindir in
	''|x_NONE)
	    : ok
	    ;;
	\$\{*\}/bin)
	    x_var=`eval echo $bindir`
	    case $x_var in
		NONE/bin)
		    Q_BINDIR=/usr/bin
		    ;;
		*)
		    Q_BINDIR=$x_var
		    ;;
	    esac
	    unset x_var
	    ;;
	*)
	    Q_BINDIR=$bindir
	    ;;
    esac

    PREFIX='"'$Q_PREFIX'"'
    EXEC_PREFIX='"'$Q_EXEC_PREFIX'"'
    BINDIR='"'$Q_BINDIR'"'
    DATADIR='"'$Q_DATADIR'"'

    AC_DEFINE_UNQUOTED( PREFIX, $PREFIX )
    AC_DEFINE_UNQUOTED( EXEC_PREFIX, $EXEC_PREFIX )
    AC_DEFINE_UNQUOTED( BINDIR, $BINDIR )
    AC_DEFINE_UNQUOTED( DATADIR, $DATADIR )

    AC_DEFINE_UNQUOTED( Q_PREFIX, $Q_PREFIX )
    AC_DEFINE_UNQUOTED( Q_EXEC_PREFIX, $Q_EXEC_PREFIX )
    AC_DEFINE_UNQUOTED( Q_BINDIR, $Q_BINDIR )
    AC_DEFINE_UNQUOTED( Q_DATADIR, $Q_DATADIR )

    AC_SUBST(PREFIX)
    AC_SUBST(EXEC_PREFIX)
    AC_SUBST(BINDIR)
    AC_SUBST(DATADIR)

    AC_SUBST(Q_PREFIX)
    AC_SUBST(Q_EXEC_PREFIX)
    AC_SUBST(Q_BINDIR)
    AC_SUBST(Q_DATADIR)
])
#####################################################################
##
##   Choose the GUI thet is used. Relies on settings from the
##   Motif and/or Gtk configuration macros.
##
#####################################################################

AC_DEFUN(AC_CHOOSE_GUI,
[
    echo Choosing a GUI...

    GUI=NONE
    NO_MOTIF=NO
    NO_GTK=NO
    NO_QT=NO

    AC_ARG_WITH( MOTIF,
	[  --with-MOTIF            Use Motif/LessTif user interface],
	[
	if  test $withval = yes
	then
	    echo Choosing MOTIF ...

	    if  test	x_$MOTIF_HEADERS_FOUND = x_YES	-a \
			x_$MOTIF_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'MOTIF_HEADERS_FOUND=' $MOTIF_HEADERS_FOUND
		echo 'MOTIF_LIBS_FOUND=' $MOTIF_HEADERS_FOUND
	    fi
	    
	    GUI=MOTIF
	else
	    NO_MOTIF=YES
	fi
	]
    )

    AC_ARG_WITH( GTK,
	[  --with-GTK              Use Gtk+ user interface ],
	[
	if  test $withval = yes
	then
	    echo Choosing GTK ...

	    if  test	x_$GTK_HEADERS_FOUND = x_YES	-a \
			x_$GTK_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'GTK_HEADERS_FOUND=' $GTK_HEADERS_FOUND
		echo 'GTK_LIBS_FOUND=' $GTK_HEADERS_FOUND
	    fi
	    
	    GUI=GTK
	else
	    NO_GTK=YES
	fi
	]
    )

    AC_ARG_WITH( QT,
	[  --with-QT              Use Qt user interface ],
	[
	if  test $withval = yes
	then
	    echo Choosing QT ...

	    if  test	x_$QT_HEADERS_FOUND = x_YES	-a \
			    x_$QT_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'QT_HEADERS_FOUND=' $QT_HEADERS_FOUND
		echo 'QT_LIBS_FOUND=' $QT_HEADERS_FOUND
	    fi
	    
	    GUI=QT
	else
	    NO_QT=YES
	fi
	]
    )

    if  test	$GUI = NONE				-a \
		$NO_GTK = NO				-a \
		x_$GTK_HEADERS_FOUND = x_YES		-a \
		x_$GTK_LIBS_FOUND = x_YES
    then
	echo Found GTK
	GUI=GTK
    fi

    if  test	$GUI = NONE				-a \
		$NO_MOTIF = NO				-a \
		x_$MOTIF_HEADERS_FOUND = x_YES		-a \
		x_$MOTIF_LIBS_FOUND = x_YES
    then
	echo Found MOTIF
	GUI=MOTIF
    fi

#    if  test	$GUI = NONE				-a \
#		$NO_QT = NO				-a \
#		x_$QT_HEADERS_FOUND = x_YES		-a \
#		x_$QT_LIBS_FOUND = x_YES
#    then
#	echo Found QT
#	GUI=QT
#    fi

    case $GUI in
    MOTIF)
	AC_SUBST(GUI)
	AC_DEFINE( USE_MOTIF )
	;;
    GTK)
	AC_SUBST(GUI)
	AC_DEFINE( USE_GTK )
	;;
    QT)
	AC_SUBST(GUI)
	AC_DEFINE( USE_QT )
	;;
    *)
	echo '############' WARNING '############'
	echo 'GUI=' $GUI
	;;
    esac
])
