#!/bin/bash

errorofbuild()
{
	printf "\n\n=========AN ERROR OCCURED!==========\n"
	cd $bak
	exit 1
}

checkState()
{
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuild
	fi
}

CurDirXXXX=$PWD
echo "Clear Version..."
./clear_version.sh nopause
checkState

echo "Clear all makefiles..."
cd $CurDirXXXX/../..
./clean_make.sh nopause
checkState

echo "Launch building..."
./build.sh "no-pause" use-ccache
checkState

echo "Launch deploying..."
cd $CurDirXXXX/deploy
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    ./deploy_linux.sh nopause
else
    ./deploy_osx.sh nopause
fi
checkState

cd $CurDirXXXX
echo "==========Everything has been completed!==========="
if [[ "$1" != "no-pause" ]]; then read -n 1; fi

