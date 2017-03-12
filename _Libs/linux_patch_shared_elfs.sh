#!/bin/bash

echo "This script adds into rpath the path to shared Qt libraries."
echo "That need if applications are can't work with pre-installed or failing with .so not found or version QT_5 is not same"

if [ ! -f /usr/bin/patchelf ]; then
    echo "ERROR! Can't use this script without patchelf"
    echo "Install it first!"
    if [  -f /usr/bin/apt-get ]; then
        echo "   sudo apt-get install patchelf"
    elif [  -f /usr/bin/yum ]; then
        echo "   sudo yum install patchelf"
    elif [  -f /usr/bin/pacman ]; then
        echo "   sudo pacman install patchelf"
    fi
    echo ""

    if [[ "$1" != "nopause" ]]; then echo "Press any key to continue..."; read -n 1; fi
    exit 1
fi

source ../_paths.sh
BIN_PATH=../bin

patchBin()
{
    echo patchelf --set-rpath '$ORIGIN:'"$QT_LIB_PATH" $BIN_PATH/$1
    patchelf --set-rpath '$ORIGIN:'"$QT_LIB_PATH" $BIN_PATH/$1
}

patchBin pge_editor
patchBin pge_calibrator
patchBin pge_manager
patchBin pge_maintainer
patchBin pge_musplay

if [[ "$1" != "nopause" ]]; then echo "Press any key to continue..."; read -n 1; fi

