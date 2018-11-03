#!/bin/bash
if [ ! -d .git ]
then
    echo "It's not a GIT repo!"
    exit 1
fi
GIT_REVISION=$(git --git-dir .git --work-tree . describe --always)
if [[ "$1" == "" ]]; then
    VERDIR=bin/versions
else
    VERDIR=$1
fi
CPPTOBUILD=_common/travis-ci/version_gen.c
TEMPELF=35hb13h51.tmp

echo "== Generating version files into ${VERDIR}... =="

mkdir -p $VERDIR

mkdir $TEMPELF
cd $TEMPELF
cmake ../_common/travis-ci/version_gen > /dev/null
make -s -j 4 > /dev/null
cd ..

$TEMPELF/version_gen_editor > $VERDIR/editor.txt
$TEMPELF/version_gen_editor_short > $VERDIR/editor_stable.txt

$TEMPELF/version_gen_engine > $VERDIR/engine.txt
$TEMPELF/version_gen_engine_short > $VERDIR/engine_stable.txt

if [ -d ./$TEMPELF ]
then
    rm -Rf ./$TEMPELF
fi

