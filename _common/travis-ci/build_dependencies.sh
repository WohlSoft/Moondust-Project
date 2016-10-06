#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ];
then
    bash ./build_deps.sh no-pause use-ccache
    sudo cp -a _Libs/_builds/linux/lib/*.so /usr/lib

elif [ $TRAVIS_OS_NAME == osx ];
then
    bash ./build_deps.sh no-pause

fi

