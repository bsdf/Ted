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

BASE=ted-2.23-${ARCH}
PETARCHIVE=${BASE}.pet

HERE=`pwd`
DESTDIR=${HERE}/${BASE}
rm -rf ${DESTDIR}
# To provoke a message:
if [ -d $DESTDIR ]
then
    rm -r ${DESTDIR}
fi
mkdir -p ${DESTDIR}${PREFIX}

cat ted-2.23-linux-*.tar.gz | ( cd ${DESTDIR} && umask 0 && tar xfz - )

INSTSIZE=`du -sk ${DESTDIR}${PREFIX} | ( read instsize name; echo $instsize )`
sed -e "s/@INSTSIZE@/${INSTSIZE}/g" pet.specs.in | \
    sh ./customize.sh "$@" > pet.specs

chown -R root:root ${DESTDIR}${PREFIX}/*

tar czf ${BASE}.tar.gz ${BASE}
tgz2pet ${BASE}.tar.gz

rm -rf ${DESTDIR}
