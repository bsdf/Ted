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
