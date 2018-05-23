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

HERE=`pwd`
DESTDIR=${HERE}/pkgscratch
rm -rf ${DESTDIR}
# To provoke a message:
if [ -d $DESTDIR ]
then
    rm -r ${DESTDIR}
fi
mkdir -p ${DESTDIR}${PREFIX}

SLACKARCHIVE=ted-2.23-${ARCH}-1mdd.tgz
INSTDIR=${DESTDIR}/install

cat ted-2.23-linux-*.tar.gz | ( cd ${DESTDIR} && umask 0 && tar xfz - )

chown -R root:root ${DESTDIR}${PREFIX}/*

mkdir -p ${INSTDIR} ${DESTDIR}/usr/share/doc/ted
chmod 755 ${INSTDIR}

sh ./customize.sh "$@" < slack-desc.in > ${INSTDIR}/slack-desc

echo 'update-desktop-database -q || true' > ${INSTDIR}/doinst.sh
chmod 755 ${INSTDIR}/doinst.sh


( cd ${DESTDIR} && makepkg -l y -c y ../${SLACKARCHIVE} )

rm -rf ${DESTDIR}
