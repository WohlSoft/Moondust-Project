#!/bin/bash

source _common/travis-ci/_branch_env.sh

if ${IS_PULL_REQUEST};
then
    echo "Skipping packages upload produced by pull-requests..."
    exit 0;
fi

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then

    UPLOAD_LIST="set ssl:verify-certificate no;"

    # Skip deploy on Coverity-Scan platform
    if [[ "${IS_COVERITY_SCAN}" != "true" ]];
    then
        #PROJECT_ROOT=/home/runner/PGE-Project
        #HAMSTER_ROOT=/home/runner/
        PROJECT_ROOT=$HOME/PGE-Project
        HAMSTER_ROOT=$HOME/

        #Different project root path on Travis-CI
        if [[ -d /home/travis/build/WohlSoft ]]; then
            PROJECT_ROOT=/home/travis/build/WohlSoft/PGE-Project
            FTP_USER=${FTPUser}
            FTP_PASSWORD=${FTPPassword}
            FTP_SERVER=${FTPServer}
        fi

        if [[ ! -d ${PROJECT_ROOT}/bin-cmake-release ]];
        then
            echo "Nothing built! Therefore is nothing to upload!"
            exit 1
        fi

        if [[ "${UPLOAD_TARGET}" == "" ]]; then
            UPLOAD_TARGET=ubuntu-14-04
            UPLOAD_VERSION_SUFFIX=linux
        fi

        if [[ -d ${PROJECT_ROOT}/bin-cmake-release ]];
        then
            # Build
            UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading archive pge_project-linux-${GIT_BRANCH}-64.tar.bz2...\";"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./${UPLOAD_TARGET}/ ${PROJECT_ROOT}/bin-cmake-release/pge_project-linux-${GIT_BRANCH}-64.tar.bz2;"
            # [DEPRECATED] SMBX config pack update patch
            # UPLOAD_LIST="${UPLOAD_LIST} put -O ./_common/ ${PROJECT_ROOT}/bin-cmake-release/SMBX-Config-Patch.zip;"
            UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading build_date_${GIT_BRANCH}_${UPLOAD_VERSION_SUFFIX}.txt...\";"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${HAMSTER_ROOT}/build_date_${GIT_BRANCH}_${UPLOAD_VERSION_SUFFIX}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading editor_${GIT_BRANCH}.txt\";"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_${GIT_BRANCH}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading editor_stable_${GIT_BRANCH}.txt\";"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_stable_${GIT_BRANCH}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading engine_${GIT_BRANCH}.txt\";"
            UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine_${GIT_BRANCH}.txt;"
            UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading engine_stable_${GIT_BRANCH}.txt\";"
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
    UPLOAD_LIST="set ssl:verify-certificate no;"

    if [[ -f bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg ]];
    then
        UPLOAD_LIST="${UPLOAD_LIST} put -O ./macosx/ ./bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg;"
        UPLOAD_LIST="${UPLOAD_LIST} put -O ./_versions/ /Users/travis/build_date_${GIT_BRANCH}_osx.txt;"
        lftp -e "${UPLOAD_LIST} exit" -u ${FTPUser},${FTPPassword} ${FTPServer}
    else
        echo "Built DMG was not found! Therefore is nothing to upload!"
        exit 1
    fi

fi

exit 0
