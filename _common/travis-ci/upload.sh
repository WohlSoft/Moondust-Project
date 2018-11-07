#!/bin/bash

source _common/travis-ci/_branch_env.sh

if ${IS_PULL_REQUEST};
then
    echo "Skipping packages upload produced by pull-requests..."
    exit 0;
fi

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then

    #Skip deploy on Travis-CI, since it done on Semaphore-CI
    if [[ $(whoami) != "travis" ]];
    then
        PROJECT_ROOT=/home/runner/PGE-Project
        HAMSTER_ROOT=/home/runner/

        if [[ ! -d ${PROJECT_ROOT}/bin/_packed && ! -d ${PROJECT_ROOT}/bin-cmake-release ]];
        then
            echo "Nothing built! Therefore is nothing to upload!"
            exit 1
        fi

        if [[ -d ${PROJECT_ROOT}/bin/_packed ]];
        then
            lftp -e "${UPLOAD_LIST} exit" -u ${FTP_USER},${FTP_PASSWORD} ${FTP_SERVER}

            UPLOAD_LIST="put -O ./ubuntu-14-04/ ${PROJECT_ROOT}/bin/_packed/pge-project-${GIT_BRANCH}-linux-ubuntu-14.04.tar.bz2;";
            # [DEPRECATED] SMBX config pack update patch
            # UPLOAD_LIST="${UPLOAD_LIST} put -O ./_common/ ${PROJECT_ROOT}/bin/_packed/SMBX-Config-Patch.zip;";
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${HAMSTER_ROOT}/build_date_${GIT_BRANCH}_linux.txt;";
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/editor_${GIT_BRANCH}.txt;";
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/editor_stable_${GIT_BRANCH}.txt;";
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/engine_${GIT_BRANCH}.txt;";
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin/versions/engine_stable_${GIT_BRANCH}.txt;";
            lftp -e "${UPLOAD_LIST} exit" -u ${FTP_USER},${FTP_PASSWORD} ${FTP_SERVER}
        fi

        if [[ -d ${PROJECT_ROOT}/bin-cmake-release ]];
        then
            # Build
            UPLOAD_LIST="put -O ./ubuntu-14-04/ ${PROJECT_ROOT}/bin-cmake-release/pge_project-linux-${GIT_BRANCH}-64.tar.bz2;"
            # [DEPRECATED] SMBX config pack update patch
            # UPLOAD_LIST="${UPLOAD_LIST} put -O ./_common/ ${PROJECT_ROOT}/bin-cmake-release/SMBX-Config-Patch.zip;"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${HAMSTER_ROOT}/build_date_${GIT_BRANCH}_linux.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_${GIT_BRANCH}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_stable_${GIT_BRANCH}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine_${GIT_BRANCH}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine_stable_${GIT_BRANCH}.txt;"
            lftp -e "${UPLOAD_LIST} exit" -u ${FTP_USER},${FTP_PASSWORD} ${FTP_SERVER}
        fi

    else
        echo "Travis CI-built packages doesn't needs for uploads..."
    fi

elif [[ "${TRAVIS_OS_NAME}" == "osx" ]];
then
# ==============================================================================
# Upload created DMG file to the server
# ==============================================================================
    if [[ -f bin/_packed/pge-project-${GIT_BRANCH}-macosx.dmg ]];
    then
        UPLOAD_LIST="put -O ./macosx/ ./bin/_packed/pge-project-${GIT_BRANCH}-macosx.dmg;"
        UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ /Users/travis/build_date_${GIT_BRANCH}_osx.txt;"
        lftp -e "${UPLOAD_LIST} exit" -u ${FTPUser},${FTPPassword} ${FTPServer}
    elif [[ -f bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg ]];
    then
        UPLOAD_LIST="put -O ./macosx/ ./bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg;"
        UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ /Users/travis/build_date_${GIT_BRANCH}_osx.txt;"
        lftp -e "${UPLOAD_LIST} exit" -u ${FTPUser},${FTPPassword} ${FTPServer}
    else
        echo "Built DMG was not found! Therefore is nothing to upload!"
        exit 1
    fi

fi

exit 0
