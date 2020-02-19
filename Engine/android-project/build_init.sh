#!/bin/bash

# Remove old version if presented
if [[ -d SDL-default ]]; then
    rm -Rf SDL-default
fi

# Unpack recent SDL2 tarball
if [[ ! -d SDL-default ]]; then
    tar -xf ../../_Libs/_sources/SDL-default.tar.gz
fi

if [[ ! -d moondust/jni/SDL ]]; then
    mkdir moondust/jni/SDL
fi

# Make a link for SDL2 sources
if [[ ! -e moondust/jni/SDL/src ]]; then
    ln -s ../../../SDL-default/src moondust/jni/SDL/src
fi

# Make a link for SDL2 includes
if [[ ! -e moondust/jni/SDL/include ]]; then
    ln -s ../../../SDL-default/src moondust/jni/SDL/include
fi

# Remove old SDL2 Java files
rm -f moondust/src/main/java/org/libsdl/app/*.java

# Copy SDL2 Java files
cp SDL-default/android-project/app/src/main/java/org/libsdl/app/*.java moondust/src/main/java/org/libsdl/app

# Copy Android NDK makefile
cp SDL-default/Android.mk moondust/jni/SDL

echo "Done!"

