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
