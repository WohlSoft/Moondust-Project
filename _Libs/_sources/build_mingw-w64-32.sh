#!/bin/bash
InstallTo=~0/../_builds/win32-mingw-w64
MINGW_W64_ROOT=/c/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32
PATH=$MINGW_W64_ROOT/bin:$PATH
echo $InstallTo
#read -n 1
source ./___build_script.sh
cp $MINGW_W64_ROOT/bin/libgcc_s_dw2-1.dll	$InstallTo/bin
cp $MINGW_W64_ROOT/bin/libstdc++-6.dll 		$InstallTo/bin
cp $MINGW_W64_ROOT/bin/libwinpthread-1.dll 	$InstallTo/bin
