#!/bin/bash

source config

source /etc/lsb-release

BUILD_DIST=${DISTRIB_CODENAME}
HOST_DIST=${DISTRIB_CODENAME}
TARGET_DIST=${DISTRIB_CODENAME}
CURRENT_DATE=$(LC_ALL=en_US.UTF-8 date +"%a, %d %b %Y %H:%M:%S %z")
CURRENT_YEAR=$(LC_ALL=en_US.UTF-8 date +"%Y")
VERSION_PACKAGING=${VERSION}
PACKAGING="debian"

while ! test "x$1" = "x"; do
	! test "x$1" = "x--" || {
		shift
		break
	}

	KEY=$(echo $1 | sed 's|=.*||')
	VALUE=$(echo $1 | sed 's|.*=||')
	case ${KEY} in
		(--build)
			! test "x${VALUE}" = "x" || {
				echo "Missing build architecture value."
				exit $?
			}
			BUILD_DIST="${VALUE}"
		;;
		(--host)
			! test "x${VALUE}" = "x" || {
				echo "Missing host architecture value."
				exit $?
			}
			HOST_DIST="${VALUE}"
		;;
		(--target)
			! test "x${VALUE}" = "x" || {
				echo "Missing target architecture value."
				exit $?
			}
			TARGET_DIST="${VALUE}"
		;;
		(--version-postfix)
			! test "x${VALUE}" = "x" || {
				echo "Missing version postfix value."
				exit $?
			}
			VERSION_PACKAGING=${VERSION}-${VALUE}
		;;
		(--packaging)
			PACKAGING=${VALUE}
		;;
		(--exec-postfix)
			EXEC_POSTFIX=${VALUE}
		;;
		(*)
			echo "Invalid argument ($1) found on command line."
			exit 1
		;;
	esac
	shift
done

PRECONFIGURATION=$*

test "x${HOST_DIST}" = "x${TARGET_DIST}" && {
	PKGNAME=${PKGNAME_BASE}
	DIST_DIR=${HOST_DIST}
} || {
	PKGNAME=${TARGET_DIST}-${PKGNAME_BASE}
	DIST_DIR=${HOST_DIST}-x-${TARGET_DIST}
}

# param 1 : inflie
# stdout is the outfile
function dist-generator ()
{
cat << EOF > generator.${PACKAGING}.sh
	cat \$1 | sed \\
		-e "s|@PRJNAME@|${PRJNAME}|g" \\
		-e "s|@PRJDESC@|${PRJDESC}|g" \\
		-e "s|@PRJDESCRIPTION@|${PRJDESCRIPTION}|g" \\
		-e "s|@PKGNAME@|${PKGNAME}|g" \\
		-e "s|@PKGNAME_BASE@|${PKGNAME_BASE}|g" \\
		-e "s|@VERSION_MAJOR@|${VERSION_MAJOR}|g" \\
		-e "s|@VERSION_MINOR@|${VERSION_MINOR}|g" \\
		-e "s|@VERSION_PATCH@|${VERSION_PATCH}|g" \\
		-e "s|@VERSION_API@|${VERSION_API}|g" \\
		-e "s|@VERSION@|${VERSION}|g" \\
		-e "s|@VERSION_PACKAGING@|${VERSION_PACKAGING}|g" \\
		-e "s|@DEVELOPMENT_START_YEAR@|${DEVELOPMENT_START_YEAR}|g" \\
		-e "s|@AUTHOR@|${AUTHOR}|g" \\
		-e "s|@EMAIL@|${EMAIL}|g" \\
		-e "s|@ORGANIZATION@|${ORGANIZATION}|g" \\
		-e "s|@ORGANIZATION_DOMAIN_NAME@|${ORGANIZATION_DOMAIN_NAME}|g" \\
		-e "s|@TARGET_DIST@|${TARGET_DIST}|g" \\
		-e "s|@HOST_DIST@|${HOST_DIST}|g" \\
		-e "s|@CURRENT_DATE@|${CURRENT_DATE}|g" \\
		-e "s|@CURRENT_YEAR@|${CURRENT_YEAR}|g" \\
		-e "s|@PRECONFIGURATION@|${PRECONFIGURATION}|g" \\
		-e "s|@EXEC_POSTFIX@|${EXEC_POSTFIX}|g" \\
		|| exit \$?
EOF
	chmod u+x generator.${PACKAGING}.sh
}

#
# project directory
#

dist-generator

echo "Copying changed files..."
make -f source.mk DIST_DIR=${DIST_DIR} PKGNAME=${PKGNAME} PACKAGING=${PACKAGING} source || exit $?
make -f source.mk DIST_DIR=${DIST_DIR} PKGNAME=${PKGNAME} PACKAGING=${PACKAGING} ${PACKAGING} || exit $?
echo "Copying changed done"

