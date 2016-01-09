#!/bin/bash
CurDirXXXX=$PWD
echo "Clear Version..."
./clear_version.sh nopause

echo "Clear all makefiles..."
cd $CurDirXXXX/../..
./clean_make.sh nopause

echo "Launch building..."
./build.sh "no-pause"

echo "Launch deploying..."
cd $CurDirXXXX/deploy
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    ./deploy_linux.sh nopause
else
    ./deploy_osx.sh nopause
fi

cd $CurDirXXXX
echo "==========Everything has been completed!==========="
if [[ "$1" != "no-pause" ]]; then read -n 1; fi

