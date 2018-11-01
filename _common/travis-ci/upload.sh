#!/bin/bash

GIT_BRANCH=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')

if [ $TRAVIS_OS_NAME == linux ];
then

    #Skip deploy on Travis-CI, since it done on Semaphore-CI
    if [[ $(whoami) != "travis" ]];
    then
        PROJECT_ROOT=/home/runner/PGE-Project
        if [ -d ${PROJECT_ROOT}/bin/_packed -o -d ${PROJECT_ROOT}/bin-cmake-release ];
        then
            cd ${PROJECT_ROOT}/Content/configs
            if [ -d ${PROJECT_ROOT}/bin-cmake-release ]; then
                zip -9 -r ${PROJECT_ROOT}/bin-cmake-release/SMBX-Config-Patch.zip SMBX
            else
                zip -9 -r ${PROJECT_ROOT}/bin/_packed/SMBX-Config-Patch.zip SMBX
            fi
        else
            echo "Nothing built! Therefore is nothing to upload!"
            exit 1
        fi

        if [ -d ${PROJECT_ROOT}/bin/_packed ];
        then
            lftp -e "put -O ./ubuntu-14-04/ ${PROJECT_ROOT}/bin/_packed/pge-project-dev-linux-ubuntu-14.04.tar.bz2; put -O ./_common/ ${PROJECT_ROOT}/bin/_packed/SMBX-Config-Patch.zip; exit" -u $FTP_USER,$FTP_PASSWORD $FTP_SERVER
            lftp -e "put -O ./_versions/ /home/runner/build_date_dev_linux.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/editor.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/editor_stable.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/engine.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/engine_stable.txt; exit" -u $FTP_USER,$FTP_PASSWORD $FTP_SERVER
        fi

        if [ -d ${PROJECT_ROOT}/bin-cmake-release ];
        then
            lftp -e "put -O ./ubuntu-14-04/ ${PROJECT_ROOT}/bin-cmake-release/pge_project-linux-${GIT_BRANCH}-64.tar.bz2; put -O ./_common/ ${PROJECT_ROOT}/bin-cmake-release/SMBX-Config-Patch.zip; exit" -u $FTP_USER,$FTP_PASSWORD $FTP_SERVER
            lftp -e "put -O ./_versions/ /home/runner/build_date_${GIT_BRANCH}_linux.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_stable.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine.txt; put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine_stable.txt; exit" -u $FTP_USER,$FTP_PASSWORD $FTP_SERVER
        fi
    fi

elif [ $TRAVIS_OS_NAME == osx ];
then
# ==============================================================================
# Upload created DMG file to the server
# ==============================================================================
    if [ -f bin/_packed/pge-project-dev-macosx.dmg ];
    then
        lftp -e "put -O ./macosx/ ./bin/_packed/pge-project-dev-macosx.dmg; put -O ./_versions/ /Users/travis/build_date_${GIT_BRANCH}_osx.txt; exit" -u $FTPUser,$FTPPassword $FTPServer
    elif [ -f bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg ];
    then
        lftp -e "put -O ./macosx/ ./bin-cmake-release/pge-project-dev-macosx.dmg; put -O ./_versions/ /Users/travis/build_date_${GIT_BRANCH}_osx.txt; exit" -u $FTPUser,$FTPPassword $FTPServer
    else
        echo "Built DMG was not found! Therefore is nothing to upload!"
        exit 1
    fi

fi

