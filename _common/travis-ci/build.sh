#!/bin/bash

# ==============================================================================
# Building of PGE
# ==============================================================================

source _common/travis-ci/_branch_env.sh

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then
    #Don't do duplicated build! (it already proceeded by Coverity plugin!)
    if [[ "${IS_COVERITY_SCAN}" == "" ]] ;
    then
        TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /home/runner/build_date_${GIT_BRANCH}_linux.txt
        bash build.sh no-pause ninja use-ccache deploy static-qt portable
    fi
elif [[ "$TRAVIS_OS_NAME" == "osx" ]];
then
    source _common/travis-ci/_osx_env.sh

    TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /Users/travis/build_date_${GIT_BRANCH}_osx.txt
    bash build.sh no-pause ninja deploy static-qt

fi

