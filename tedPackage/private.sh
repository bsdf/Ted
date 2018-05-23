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

APPLDIR="${HOME}/.local/share/applications"
TEDEXE="${HOME}/bin/Ted"
TEDDIR="${HOME}/Ted"
PROPFILE="${HOME}/.Ted.properties"

mkdir -p "${TEDDIR}" "${HOME}/bin" "${APPLDIR}"

sed -e "s!@DATADIR@!${HOME}!" Ted.desktop.in > "${APPLDIR}/Ted.desktop"
( cp ../Ted/Ted.static "${TEDEXE}" || \
  cp ../Ted/Ted        "${TEDEXE}" ) && strip "${TEDEXE}"

HERE=`pwd`

( cd ${TEDDIR} && tar xf ${HERE}/TedDatadir.tar )

echo ========= Updating "${PROPFILE}" ........
if  grep '^Ted.documentFileName:' "${PROPFILE}"
then
    : ok
else
    echo Ted.documentFileName: ${TEDDIR}/TedDocument-en_US.rtf | \
					    tee -a "${PROPFILE}"
fi

if  grep '^Ted.afmDirectory:' "${PROPFILE}"
then
    : ok
else
    echo Ted.afmDirectory: ${TEDDIR}/afm | tee -a "${PROPFILE}"
fi

if  grep '^Ted.spellToolSystemDicts:' "${PROPFILE}"
then
    : ok
else
    echo Ted.spellToolSystemDicts: ${TEDDIR}/dfa | tee -a "${PROPFILE}"
fi

update-desktop-database -v "${APPLDIR}" || true

Ted --fullVersion || ( echo $PATH; ls -l "${TEDEXE}" )
