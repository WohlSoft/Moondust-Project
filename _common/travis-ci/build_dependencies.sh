#!/bin/bash

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then
    # Don't build dependencies if there are cached
    if [[ ! -d _Libs/_builds/linux/lib ]]; then
        bash ./build_deps.sh no-pause use-ccache
    else
        echo "Pre-built dependencies are already cached! Do nothing!"
    fi
    sudo cp -a _Libs/_builds/linux/lib/*.so /usr/lib

elif [[ "${TRAVIS_OS_NAME}" == "osx" ]];
then
    source _common/travis-ci/_osx_env.sh
    # == Dependencies pre-build removed since CMake build is in use ==
    # Don't build dependencies if there are cached
    #if [ ! -d _Libs/_builds/macos/lib ];
    #then
    #    bash ./build_deps.sh no-pause
    #else
    #    echo "Pre-built dependencies are already cached! Do nothing!"
    #fi
fi
