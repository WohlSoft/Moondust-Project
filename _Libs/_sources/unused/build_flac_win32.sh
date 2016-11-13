#!/bin/bash

osx_realpath() {
  case "${1}" in
    [./]*)
    echo "$(cd ${1%/*}; pwd)/${1##*/}"
    ;;
    *)
    echo "${PWD}/${1}"
    ;;
  esac
}

cd SDL/flac-1.3.1

InstallTo=~0/../../../_builds/win32
InstallTo=$(osx_realpath $InstallTo)
echo $InstallTo
./configure --disable-xmms-plugin --enable-static=yes --enable-shared=no --prefix=$InstallTo CFLAGS=-fPIC CXXFLAGS=-fPIC --disable-asm-optimizations
make
make install

cd ../..
