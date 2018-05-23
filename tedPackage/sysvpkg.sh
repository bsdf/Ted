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

PKGFILE=Ted_2.23_${PLATFORM}.pkg

trap "rm -rf $DESTDIR $DIR" 0 2

cat ted-2.23-${PLATFORM}.tar.gz | ( cd ${DESTDIR} && umask 0 && tar xfz - )

sh ./customize.sh "$@" < Ted.prototype.in > Ted.prototype
sh ./customize.sh "$@" < Ted.pkginfo.in >   Ted.pkginfo

POST=sysvpost.sh
( echo 'update-desktop-database -q || true' ) > ${POST}
chmod +x ${POST}

pkgmk -o -b ${DESTDIR}${PREFIX} -f ./Ted.prototype -r ${DESTDIR}${PREFIX} -d $DIR

#/usr/sbin/pkgchk -d $DIR Ted

pkgtrans -o -s $DIR `pwd`/$PKGFILE all

gzip -9 < $PKGFILE > $PKGFILE.gz

ls -l $PKGFILE $PKGFILE.gz

/usr/sbin/pkgchk -d $PKGFILE Ted

