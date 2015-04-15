#/bin/bash

if [[ "$1" == "" ]]; then
 echo "Please define name of application!"
 exit 1
fi

TARGET_APP=$1

bak=~+

realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname $SCRIPT)

curd="$SCRIPTPATH/../"
CONFIGURATION_BUILD_DIR=$curd'bin'

if [[ "$curd" == "" ]]; then
   exit 1
fi

echo "copy libraries..."
if [ ! -d $curd"/bin/_Libs" ]; then
    mkdir -p $curd"/bin/_Libs"
    if [ -d $curd"/bin/_Libs/SDL2.framework" ]; then
        rm -Rf $curd"/bin/_Libs/SDL2.framework"
    fi
    cp -Rfa "$curd/_Libs/_builds/macos/frameworks/SDL2.framework" $curd"/bin/_Libs"
    cp -a $curd/_Libs/_builds/macos/lib/*.dylib $curd"/bin/_Libs"
fi

EXECUTABLE_PATH="$TARGET_APP.app/Contents/MacOS/$TARGET_APP"

function relocateLibraryInCurrentApp() {
  install_name_tool -change $1$2 @executable_path/../../../_Libs/$2 $CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH
}

echo "Installing libs for $TARGET_APP..."

FILES=$SCRIPTPATH"/../_Libs/_builds/macos/lib/*.dylib"
for f in $FILES
do
  filename="${f##*/}"
  #echo "Processing $filename file..."
  relocateLibraryInCurrentApp "" $filename #note the space
done

relocateLibraryInCurrentApp "@rpath/" SDL2.framework/Versions/A/SDL2 #note the space

cd $bak
#if [[ $1 != "no-pause" ]]; then read -n 1; fi
exit 0
