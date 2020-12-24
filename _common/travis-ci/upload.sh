#!/bin/bash

source _common/travis-ci/_branch_env.sh

if ${IS_PULL_REQUEST};
then
    echo "Skipping packages upload produced by pull-requests..."
    exit 0;
fi

UPLOAD_LIST="set ssl:verify-certificate no;"

function addUploadfile()
{
    if [[ -f $2 ]]; then
        UPLOAD_LIST="${UPLOAD_LIST} echo \"Uploading archive $2...\";"
        UPLOAD_LIST="${UPLOAD_LIST} put -O $1 $2;"
    else
        echo "warning: File $2 doesn't exist, skipping..."
    fi
}

if [[ "${TRAVIS_OS_NAME}" == "linux" ]];
then

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
        fi

        if [[ ! -d ${PROJECT_ROOT}/bin-cmake-release ]];
        then
            echo "Nothing built! Therefore is nothing to upload!"
            exit 1
        fi

        if [[ "${UPLOAD_TARGET}" == "" ]]; then
            UPLOAD_TARGET=ubuntu-18-04
            UPLOAD_VERSION_SUFFIX=linux
        fi

        if [[ -d ${PROJECT_ROOT}/bin-cmake-release ]];
        then
            # Build
            addUploadfile ./${UPLOAD_TARGET}/ ${PROJECT_ROOT}/bin-cmake-release/pge_project-linux-${GIT_BRANCH}-64.tar.bz2
            # [DEPRECATED] SMBX config pack update patch
            # UPLOAD_LIST="${UPLOAD_LIST} put -O ./_common/ ${PROJECT_ROOT}/bin-cmake-release/SMBX-Config-Patch.zip;"
            addUploadfile ./_versions/ ${HAMSTER_ROOT}/build_date_${GIT_BRANCH}_${UPLOAD_VERSION_SUFFIX}.txt
            addUploadfile ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_${GIT_BRANCH}.txt
            addUploadfile ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/editor_stable_${GIT_BRANCH}.txt
            addUploadfile ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine_${GIT_BRANCH}.txt
            addUploadfile ./_versions/ ${PROJECT_ROOT}/bin-cmake-release/versions/engine_stable_${GIT_BRANCH}.txt
            lftp -e "${UPLOAD_LIST} exit" -u ${FTPUser},${FTPPassword} ${FTPServer}
        fi

    else
        echo "CoverityScan based Travis CI-built packages doesn't needs for uploads, skipping..."
    fi

elif [[ "${TRAVIS_OS_NAME}" == "osx" ]];
then
# ==============================================================================
# Upload created DMG file to the server
# ==============================================================================
    if [[ -f bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg ]];
    then
        addUploadfile ./macosx/ ./bin-cmake-release/pge-project-${GIT_BRANCH}-macosx.dmg
        addUploadfile ./_versions/ /Users/travis/build_date_${GIT_BRANCH}_osx.txt
        lftp -e "${UPLOAD_LIST} exit" -u ${FTPUser},${FTPPassword} ${FTPServer}
    else
        echo "Built DMG was not found! Therefore is nothing to upload!"
        exit 1
    fi

fi

exit 0
