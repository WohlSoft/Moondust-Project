#!/bin/bash
InstallTo=~0/../_builds/macos
export CC=/Applications/XCode.app/Contents/Developer/usr/bin/gcc
echo $InstallTo
read -n 1
SDL_ARGS='x86_64-apple-darwin '
source ./___build_script.sh
