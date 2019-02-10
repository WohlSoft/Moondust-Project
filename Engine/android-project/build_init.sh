#!/bin/bash

if [[ ! -d SDL-default ]]; then
    tar -xf ../../_Libs/_sources/SDL-default.tar.gz
fi

if [[ ! -d moondust/jni/SDL ]]; then
    mkdir moondust/jni/SDL
fi

if [[ ! -e moondust/jni/SDL/src ]]; then
    ln -s ../../../SDL-default/src moondust/jni/SDL/src
fi

if [[ ! -e moondust/jni/SDL/include ]]; then
    ln -s ../../../SDL-default/src moondust/jni/SDL/include
fi

cp SDL-default/Android.mk moondust/jni/SDL

