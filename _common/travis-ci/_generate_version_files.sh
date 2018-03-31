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

mkdir -p $VERDIR

gcc -DGIT_VERSION=\"$GIT_REVISION\" -DEDITOR -DVER_FULL $CPPTOBUILD -o $TEMPELF
if [ -f ./$TEMPELF ]
then
    ./$TEMPELF > $VERDIR/editor.txt
fi

gcc -DGIT_VERSION=\"$GIT_REVISION\" -DEDITOR -DVER_SHORT $CPPTOBUILD -o $TEMPELF
if [ -f ./$TEMPELF ]
then
    ./$TEMPELF > $VERDIR/editor_stable.txt
fi

gcc -DGIT_VERSION=\"$GIT_REVISION\" -DENGINE -DVER_FULL $CPPTOBUILD -o $TEMPELF
if [ -f ./$TEMPELF ]
then
    ./$TEMPELF > $VERDIR/engine.txt
fi

gcc -DGIT_VERSION=\"$GIT_REVISION\" -DENGINE -DVER_SHORT $CPPTOBUILD -o $TEMPELF
if [ -f ./$TEMPELF ]
then
    ./$TEMPELF > $VERDIR/engine_stable.txt
fi

if [ -f ./$TEMPELF ]
then
    rm ./$TEMPELF
fi

