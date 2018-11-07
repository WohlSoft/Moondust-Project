#!/bin/bash
if [[ ! -d .git ]]
then
    echo "It's not a GIT repo!"
    exit 1
fi

source _common/travis-ci/_branch_env.sh

GIT_REVISION=$(git --git-dir .git --work-tree . describe --always)
if [[ "$1" == "" ]]; then
    VER_DIR=bin/versions
else
    VER_DIR=$1
fi

# CPP_TO_BUILD=_common/travis-ci/version_gen.c
TEMP_ELF=35hb13h51.tmp

echo "== Generating version files into ${VER_DIR}... =="

mkdir -p ${VER_DIR}

mkdir ${TEMP_ELF}
cd ${TEMP_ELF}
cmake ../_common/travis-ci/version_gen > /dev/null
make -s -j 4 > /dev/null
cd ..

${TEMP_ELF}/version_gen_editor > ${VER_DIR}/editor_${GIT_BRANCH}.txt
${TEMP_ELF}/version_gen_editor_short > ${VER_DIR}/editor_stable_${GIT_BRANCH}.txt

${TEMP_ELF}/version_gen_engine > ${VER_DIR}/engine_${GIT_BRANCH}.txt
${TEMP_ELF}/version_gen_engine_short > ${VER_DIR}/engine_stable_${GIT_BRANCH}.txt

if [[ -d ./${TEMP_ELF} ]]
then
    rm -Rf ./${TEMP_ELF}
fi
