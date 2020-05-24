#!/bin/bash

# ==============================================================================
# Building of PGE
# ==============================================================================

source _common/travis-ci/_branch_env.sh

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then
    if [[ "${UPLOAD_TARGET}" == "" ]]; then
        UPLOAD_TARGET=ubuntu-14-04
        UPLOAD_VERSION_SUFFIX=linux
    fi
    #Don't do duplicated build! (it already proceeded by Coverity plugin!)
    if [[ "${IS_COVERITY_SCAN}" == "" ]] ;
    then
        TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > $HOME/build_date_${GIT_BRANCH}_${UPLOAD_VERSION_SUFFIX}.txt
        bash build.sh no-pause ninja use-ccache deploy static-qt portable
    fi
elif [[ "$TRAVIS_OS_NAME" == "osx" ]];
then
    source _common/travis-ci/_osx_env.sh

    TZ=Europe/Moscow date +"%Y-%m-%d %H:%M:%S" > /Users/travis/build_date_${GIT_BRANCH}_osx.txt
    bash build.sh no-pause ninja deploy static-qt

fi

