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
##   Find out whether there is a domain name in the struct utsname.
##   An what is is called
##
#####################################################################

AC_DEFUN(AC_STRUCT_UTSNAME_DOMAINNAME,
[
    dnl Is the domain in the utsname struct? What is it same..

    AC_TRY_LINK(
	[#include <sys/utsname.h>],
	[struct utsname u; return u.domainname;],
	AC_DEFINE(UTSNAME_DOMAIN,domainname),)
    AC_TRY_LINK(
	[#include <sys/utsname.h>],
	[struct utsname u; return u.__domainname;],
	AC_DEFINE(UTSNAME_DOMAIN,__domainname),)
])
