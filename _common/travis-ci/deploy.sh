#!/bin/bash

# ==============================================================================
# Deploying of built PGE
# ==============================================================================

if [ $TRAVIS_OS_NAME == linux ];
then
    _Misc/dev_scripts/deploy/deploy_linux.sh nopause

elif [ $TRAVIS_OS_NAME == osx ];
then
    cd _Misc/dev_scripts/deploy
    bash deploy_osx.sh nopause
    cd ../../..

fi
