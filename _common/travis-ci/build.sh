#!/bin/bash

# ==============================================================================
# Building of PGE
# ==============================================================================

if [ $TRAVIS_OS_NAME == linux ];
then
    TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /home/runner/build_date_dev_linux.txt
    bash build.sh no-pause use-ccache

elif [ $TRAVIS_OS_NAME == osx ];
then
    source _common/travis-ci/_osx_env.sh

    TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /Users/travis/build_date_dev_osx.txt
    bash build.sh no-pause

fi
