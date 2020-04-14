#!/bin/bash
set -e

function buildIt()
{
    echo "------- $1.c -------"

    echo "Building..."
    i686-w64-mingw32-gcc -O2 $1.c -o $1.exe -static-libgcc

    echo "Stripping..."
    i686-w64-mingw32-strip $1.exe

    echo "Packing..."
    tar -czf $1.tar.gz $1.exe
    # remove exe after of packing
    rm $1.exe
}

buildIt luna_hwnd_show
buildIt 38a_ipc_bridge

echo "------ Done! ------"

