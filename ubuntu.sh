#!/bin/bash

#
#       Author: Peter Csaszar <csjpeter@gmail.com>
#       Copyright (C) 2015 Peter Csaszar
#

JOBS=$(expr $(cat /proc/cpuinfo | grep processor | wc -l) + 1)

# ubuntu ditributions
trusty=14.04
raring=13.04
quantal=12.10
precise=12.04
oneiric=11.10
lucid=10.04
hardy=8.04

# debian distributions
squeeze=6.0
lenny=5.0
etch=4.0

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

function config ()
{
	local DIST=$1
	shift
	eval VERSION_POSTFIX=\$${DIST}-${DIST}
	./dist-config.sh \
		--build=${DIST} \
		--host=${DIST} \
		--target=${DIST} \
		--version-postfix=${VERSION_POSTFIX} \
		-- \
		--pkg-config-path=/opt/extras.ubuntu.com/csjp/lib/pkgconfig \
		--prefix=usr \
		--cflags=\\\"-DPGSQL\\\" \
		--libs=\\\"-lpq\\\" \
		--appsdir=usr/share/applications \
		--fpic \
		--gnu-source \
		$@ \
		--relative-path \
		|| exit $?

		#--cflags=\\\"-fno-candidate-functions\\\" \
		#--prefix=opt/${PKGNAME_BASE}

	exec_in_dir ${DIST} ./configure || exit $?
}

CMD=$1

case "${CMD}" in
	(pump)
		shift
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir ${DISTRIB_CODENAME} pump make CXX=distcc $@ || exit $?
	;;
	(debian)
		shift
		config ${DISTRIB_CODENAME} || exit $?
		exec_in_dir ${DISTRIB_CODENAME} debuild \
			--no-tgz-check \
			--preserve-envvar PATH \
			--preserve-envvar PKG_CONFIG_LIBDIR \
			--preserve-envvar PKG_CONFIG_PATH \
			-B $@ \
			--lintian-opts --no-lintian || exit $?
	;;
	(code)
		shift
		config ${DISTRIB_CODENAME} --debug || exit $?
		exec_in_dir ${DISTRIB_CODENAME} make -j1 $@ || exit $?
	;;
	(run)
		exec_in_dir ${DISTRIB_CODENAME} make run
	;;
	(debug)
		exec_in_dir ${DISTRIB_CODENAME} make debug
	;;
	(*)
		config ${DISTRIB_CODENAME} --debug || exit $?
		exec_in_dir ${DISTRIB_CODENAME} make -j${JOBS} $@ || exit $?
	;;
esac

