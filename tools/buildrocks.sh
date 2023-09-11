#!/bin/bash

LINKAGE=$1
if [ -z $LINKAGE ]; then
	LINKAGE="shared_lib"
fi

BRANCH=$2
if [ -z $BRANCH ]; then
	BRANCH="master"
fi

run ()
{
	COMMAND=$1
	# check for empty commands
	if test -z "$COMMAND" ; then
		echo -e "\033[1;5;31mERROR\033[0m No command specified!"
		return 1
	fi

	shift;
	OPTIONS="$@"
	# print a message
	if test -n "$OPTIONS" ; then
		echo -ne "\033[1m$COMMAND $OPTIONS\033[0m ... "
	else
		echo -ne "\033[1m$COMMAND\033[0m ... "
	fi

	# run or die
	$COMMAND $OPTIONS ; RESULT=$?
	if test $RESULT -ne 0 ; then
		echo -e "\033[1;5;31mERROR\033[0m $COMMAND failed. (exit code = $RESULT)"
		exit 1
	fi

	echo -e "\033[0;32myes\033[0m"
	return 0
}


echo "*** Building RocksDB... "

USERDIR=$PWD            # Save current dir and return to it later

run git submodule update --init deps/rocksdb

run cd deps/rocksdb
git fetch --tags --force
run git checkout $BRANCH
NJOBS=`nproc`
export CFLAGS="$CFLAGS -DROCKSDB_USE_RTTI"
export CFLAGS="$CFLAGS -frtti"
export CFLAGS="$CFLAGS -fPIC"
export CFLAGS="$CFLAGS -ftls-model=initial-exec"
export CFLAGS="$CFLAGS -Wno-error"
export CFLAGS="$CFLAGS -mtune=native"
export DISABLE_JEMALLOC=1
run make V=1 -j$NJOBS $LINKAGE
run cd $USERDIR         # Return to user's original directory
