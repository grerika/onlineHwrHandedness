#!/bin/bash

#
#       Author: Peter Csaszar <csjpeter@gmail.com>
#       Copyright (C) 2015 Peter Csaszar
#

JOBS=$(expr $(cat /proc/cpuinfo | grep processor | wc -l) + 1)

#export LDFLAGS="-Wl,-subsystem,console"
#export LIBS="-lpthread"
#export LIBS="-lgnurx"

source /etc/lsb-release
source config

function exec_in_dir ()
{
	pushd $1 > /dev/null || exit $?
	shift
	$@
	let RET=$?
	popd > /dev/null
	return ${RET}
}

TCROOT=opt/mxe

export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:/${TCROOT}/lib
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/${TCROOT}/lib/pkgconfig

./dist-config.sh \
	--target=mxe \
	--exec-postfix=.exe \
	--packaging=windows \
	-- \
	--relative-path \
	--target=i686-pc-mingw32 \
	--prefix=${PKGNAME_BASE} \
	--gnu-source \
	--static \
	--libs=\\\"-lgnurx -lpq\\\" \
	--ldflags=\\\"-static-libgcc -static-libstdc++\\\" \
	--ldflags=-Wl,-subsystem,windows \
	|| exit $?

#--ldflags=-Wl,-subsystem,console

exec_in_dir ${DISTRIB_CODENAME}-x-mxe ./configure || exit $?
exec_in_dir ${DISTRIB_CODENAME}-x-mxe make -j${JOBS} $@ || exit $?

exec_in_dir ${DISTRIB_CODENAME}-x-mxe unix2dos nsis/license.txt || exit $?
exec_in_dir ${DISTRIB_CODENAME}-x-mxe makensis nsis/$(basename $(ls -1 ${DISTRIB_CODENAME}-x-mxe/nsis/*.nsi))

