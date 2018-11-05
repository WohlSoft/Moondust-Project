#!/bin/bash

# ==============================================================================
# Deploying of built PGE
# ==============================================================================

source _common/travis-ci/_branch_env.sh

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then
    #Don't deploy on Travis-CI, because it used for Coverity Scan
    if [[ $(whoami) != "travis" ]];
    then
        if [[ -d "bin-cmake-release" ]]; then
            _Misc/dev_scripts/deploy/deploy_linux.sh nopause
        else
            echo "CMake-made deployment detected! Nothing to do!"
            exit 0;
        fi
    fi

elif [[ "${TRAVIS_OS_NAME}" == "osx" ]];
then
    if [[ ! -d "bin-cmake-release" ]];
    then
        source _common/travis-ci/_osx_env.sh

        cd _Misc/dev_scripts/deploy
        bash deploy_osx.sh nopause
        cd ../../..
    else
        echo "CMake-made deployment detected! Nothing to do!"
        exit 0;
    fi
fi

exit 0;
