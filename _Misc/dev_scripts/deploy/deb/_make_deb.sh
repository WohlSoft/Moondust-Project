#!/bin/bash
#FOLDER_SRC=$1
#FOLDER_DST=$1
FOLDER_SRC=/home/vitaly/_git_repos/PGE-Project/bin/_linux_deploy/PGE_Project
FOLDER_DST=/home/vitaly/_git_repos/PGE-Project/bin/_packed/deb

REL_PATH=/usr/lib/pge-wohlstand/

InitDir=$PWD
#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  CurDir="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
CurDir="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================
cd $CurDir

preparePackage()
{
    if [ ! -d $FOLDER_DST/$1 ]; then
	    mkdir -p "$FOLDER_DST/$1/DEBIAN"
    	mkdir -p "$FOLDER_DST/$1$REL_PATH"
    fi
    cp "$FOLDER_SRC/license.txt" "$FOLDER_DST/$1/DEBIAN/copyright"
    cp "$CurDir/DEBIAN/control-$1" "$FOLDER_DST"
}

preparePackage common
preparePackage editor
preparePackage engine
preparePackage calibrator
preparePackage maintainer
preparePackage LazyFixTool
preparePackage PNG2GIFs
preparePackage GIFs2PNG

cd $InitDir


