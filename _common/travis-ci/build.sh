#!/bin/bash

# ==============================================================================
# Building of PGE
# ==============================================================================

if [ $TRAVIS_OS_NAME == linux ];
then
    #Don't do duplicated build! (it already proceeded by coverity plugin!)
    if [[ $(whoami) != "travis" ]];
    then
        TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /home/runner/build_date_dev_linux.txt
        bash build.sh no-pause use-ccache
    fi
elif [ $TRAVIS_OS_NAME == osx ];
then
    source _common/travis-ci/_osx_env.sh

    TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /Users/travis/build_date_dev_osx.txt
    bash build.sh no-pause

fi
