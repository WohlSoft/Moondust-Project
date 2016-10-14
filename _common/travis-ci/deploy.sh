#!/bin/bash

# ==============================================================================
# Deploying of built PGE
# ==============================================================================

if [ $TRAVIS_OS_NAME == linux ];
then
    #Don't deploy on Travis-CI, because it used for Coverity Scan
    if [[ $(whoami) != "travis" ]];
    then
        _Misc/dev_scripts/deploy/deploy_linux.sh nopause
    fi

elif [ $TRAVIS_OS_NAME == osx ];
then
    source _common/travis-ci/_osx_env.sh

    cd _Misc/dev_scripts/deploy
    bash deploy_osx.sh nopause
    cd ../../..

fi
