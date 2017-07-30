#!/bin/bash
InstallTo=~0/../_builds/win64
MINGW_W64_ROOT=/c/mingw-w64/x86_64-7.1.0-posix-seh-rt_v5-rev0/mingw64
CACHE_DIR="_build_cache-w64"
PATH=$MINGW_W64_ROOT/bin:$PATH
echo $InstallTo
#read -n 1
source ./___build_script.sh
cp $MINGW_W64_ROOT/bin/libgcc_s_seh-1.dll 	$InstallTo/bin
cp $MINGW_W64_ROOT/bin/libstdc++-6.dll 		$InstallTo/bin
cp $MINGW_W64_ROOT/bin/libwinpthread-1.dll 	$InstallTo/bin
