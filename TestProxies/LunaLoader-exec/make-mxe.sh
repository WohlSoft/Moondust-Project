#!/bin/bash

if [[ -d build ]]; then
    rm -Rf build
fi
mkdir build

cd build
export PATH=/opt/mxe/usr/bin:$PATH

i686-w64-mingw32.static-cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$1" ..

make -j 2

if [[ "$1" != "" ]]; then
    make install
fi

cd ..

