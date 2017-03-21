#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ];
then
    # Don't build dependencies if there are cached
    if [ ! -d _Libs/_builds/linux/lib ];
    then
        bash ./build_deps.sh no-pause use-ccache
    fi
    sudo cp -a _Libs/_builds/linux/lib/*.so /usr/lib

elif [ $TRAVIS_OS_NAME == osx ];
then
    source _common/travis-ci/_osx_env.sh
    # Don't build dependencies if there are cached
    if [ ! -d _Libs/_builds/macos/lib ];
    then
        bash ./build_deps.sh no-pause
    fi
fi

