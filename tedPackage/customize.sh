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

sed	-e "s:@SYSTEM@:${SYSTEM}:g"		\
	-e "s:@MACHINE@:${MACHINE}:g"		\
	-e "s:@ARCH@:${ARCH}:g"			\
	-e "s:@PLATFORM@:${PLATFORM}:g"		\
	-e "s:@BINDIST@:${BINDIST}:g"		\
	-e "s:@PREFIX@:${PREFIX}:g"		\
	-e "s:@DATADIR@:${DATADIR}:g"		\
	-e "s:@PKG_OWNER@:${PKG_OWNER}:g"	\
	-e "s:@PKG_GROUP@:${PKG_GROUP}:g"

