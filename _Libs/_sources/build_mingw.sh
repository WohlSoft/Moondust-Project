#!/bin/bash
InstallTo=~0/../_builds/win32
echo $InstallTo
#read -n 1
source ./___build_script.sh
cp /mingw/bin/libgcc_s_dw2-1.dll    $InstallTo/bin
cp /mingw/bin/libstdc++-6.dll 	    $InstallTo/bin
cp /mingw/bin/libwinpthread-1.dll   $InstallTo/bin
