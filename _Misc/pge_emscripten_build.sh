#!/bin/bash
bak=$PWD
PGE_ROOT=$PWD/..

source ../_common/functions.sh

if [[ "$EMSDK_ROOT" == "" ]]; then
    #EMSDK_ROOT=/home/wohlstand/Qt/Tools/emsdk
    echo "Can't build this!"
    echo "Please specify the EMSDK_ROOT= environment variable to point the location of Emscripten SDK"
    echo ""
    exit 1
fi

if [[ "$PRELOAD_EPISODE" == "" ]]; then
    PRELOAD_EPISODE="${HOME}/.PGE_Project/worlds/the invasion customized"
fi

if [[ "$PRELOAD_CONFIG_PACK" == "" ]]; then
    PRELOAD_CONFIG_PACK="${HOME}/.PGE_Project/configs/A2XT"
fi


HOST_PATH=$PATH

cd $EMSDK_ROOT
source ./emsdk_env.sh
cd $bak

if [[ ! -d "${PGE_ROOT}/build-pge-emscripten" ]]; then
    mkdir "${PGE_ROOT}/build-pge-emscripten"
fi

emcmake cmake \
    -G Ninja \
    -S "${PGE_ROOT}" \
    -B "${PGE_ROOT}/build-pge-emscripten" \
    -DCMAKE_BUILD_TYPE=Release \
    -DPGE_PRELOAD_CONFIG_PACK="${PRELOAD_CONFIG_PACK}" \
    -DPGE_PRELOAD_CONFIG_PACK_NAME="A2XT" \
    -DPGE_PRELOAD_EPISODE="${PRELOAD_EPISODE}" \
    -DPGE_PRELOAD_EPISODE_NAME="ti" \
    -DHOST_PATH_ENV="$HOST_PATH" \
    -DHOST_CMAKE_COMMAND="/usr/bin/cmake" \
    -DHOST_C_COMPILER="/usr/bin/gcc" \
    -DHOST_CXX_COMPILER="/usr/bin/g++"
checkState

emmake cmake --build "${PGE_ROOT}/build-pge-emscripten" --target all
checkState

cd $bak

