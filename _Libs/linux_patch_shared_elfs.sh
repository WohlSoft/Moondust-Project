#!/bin/bash

echo "This script adds into rpath the path to shared Qt libraries."
echo "That need if applications are can't work with pre-installed or failing with .so not found or version QT_5 is not same"

source ../_paths.sh
BIN_PATH=../bin

patchBin()
{
    echo patchelf --set-rpath '$ORIGIN:'"$QT_LIB_PATH" $BIN_PATH/$1
    patchelf --set-rpath '$ORIGIN:'"$QT_LIB_PATH" $BIN_PATH/$1
}

patchBin pge_editor
patchBin pge_engine
patchBin pge_calibrator
patchBin pge_manager
patchBin pge_maintainer
patchBin pge_musplay


