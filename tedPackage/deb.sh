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

umask 022

DEBARCHIVE=ted-2.23-${ARCH}.deb

HERE=`pwd`
DESTDIR=${HERE}/pkgscratch
rm -rf ${DESTDIR}
# To provoke a message:
if [ -d $DESTDIR ]
then
    rm -r ${DESTDIR}
fi
mkdir -p ${DESTDIR}${PREFIX}

cat ted-2.23-linux-*.tar.gz | ( cd ${DESTDIR} && umask 0 && tar xfz - )

chown -R root:root ${DESTDIR}${PREFIX}/*
INSTSIZE=`du -sk ${DESTDIR}${PREFIX} | ( read instsize name; echo $instsize )`

mkdir -p ${DESTDIR}/DEBIAN ${DESTDIR}/usr/share/doc/ted
chmod 755 ${DESTDIR}/DEBIAN

sed -e "s/@INSTSIZE@/${INSTSIZE}/g" debian-control.in | \
    sh ./customize.sh "$@" > ${DESTDIR}/DEBIAN/control

sed -e "s/@INSTSIZE@/${INSTSIZE}/g" debian-copyright.in | \
    sh ./customize.sh "$@" > ${DESTDIR}/usr/share/doc/ted/copyright

#gzip -9 < debian-changelog > ${DESTDIR}/usr/share/doc/ted/changelog.Debian.gz
gzip -9 < debian-changelog > ${DESTDIR}/usr/share/doc/ted/changelog.gz

dpkg-deb --build ${DESTDIR} ${DEBARCHIVE}

(
set -x
dpkg-deb --info ${DEBARCHIVE}
dpkg-deb --contents ${DEBARCHIVE}

lintian -v ${DEBARCHIVE}
)

rm -rf ${DESTDIR}
