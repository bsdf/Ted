#!/bin/sh

set -e -u

SYSTEM=${1}
MACHINE=${2}
ARCH=${3}
PLATFORM=${4}
BINDIST=${5}
PREFIX=${6}
DATADIR=${7}
PKG_OWNER=${8}
PKG_GROUP=${9}

DESTDIR=`pwd`/scratch
DIR=`pwd`/pkgscratch

rm -rf $DESTDIR $DIR
mkdir -p $DESTDIR $DIR

PKGFILE=Ted_2.23_${ARCH}.freebsdpkg

trap "rm -rf $DESTDIR $DIR" 0 2

cat ted-2.23-${PLATFORM}.tar.gz | ( cd ${DESTDIR} && umask 0 && tar xfz - )

PCRE_VERSION=`pkg_info -E -x '^pcre-[0-9.,_]*$' || echo pcre`
X11_VERSION=`pkg_info -E -x '^libX11-[0-9.,_]*$' || echo libX11`
XPM_VERSION=`pkg_info -E -x '^libXpm-[0-9.,_]*$' || echo libXpm`
TIFF_VERSION=`pkg_info -E -x '^tiff-[0-9.,_]*$' || echo tiff`
JPEG_VERSION=`pkg_info -E -x '^jpeg-[0-9.,_]*$' || echo jpeg`
PNG_VERSION=`pkg_info -E -x '^png-[0-9.,_]*$' || echo png`
GTK_VERSION=`pkg_info -E -x '^gtk-2[0-9.,_]*$' || echo gtk`

sed -e "s/@PCRE_VERSION@/${PCRE_VERSION}/"	\
    -e "s/@X11_VERSION@/${X11_VERSION}/"	\
    -e "s/@XPM_VERSION@/${XPM_VERSION}/"	\
    -e "s/@TIFF_VERSION@/${TIFF_VERSION}/"	\
    -e "s/@JPEG_VERSION@/${JPEG_VERSION}/"	\
    -e "s/@PNG_VERSION@/${PNG_VERSION}/"	\
    -e "s/@GTK_VERSION@/${GTK_VERSION}/"	\
	freebsd-pkglist.in | sh ./customize.sh "$@" > freebsd-pkglist

sed -e 's/^Description:/ /'		\
    -e '/^[A-Z]/d'			\
    -e 's/^[ 	]*//'			\
	${DESTDIR}${DATADIR}/Ted/varia/ted-2.23.lsm > freebsd-pkgdescr

pkg_create	-d freebsd-pkgdescr			\
		-c '-Ted, an easy Rich Text Processor'	\
		-f freebsd-pkglist			\
		-S ${DESTDIR}				\
		-v					\
		-z					\
		$PKGFILE
