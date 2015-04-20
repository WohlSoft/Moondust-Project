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

function relocateLibraryInCurrentLib() {
  echo $1$2
  install_name_tool -change $1$2 @loader_path/$2 $CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH
}

fetchPathsForApp()
{
FILES=$SCRIPTPATH"/../_Libs/_builds/macos/lib/*.dylib"
for f in $FILES
do
filename="${f##*/}"
#echo "Processing $filename file..."
relocateLibraryInCurrentApp "" $filename #note the space
done
}

fetchPathsForLib()
{
FILES=$SCRIPTPATH"/../bin/_Libs/*.dylib"
for f in $FILES
do
filename="${f##*/}"
#echo "Processing $filename file..."
relocateLibraryInCurrentLib "/lib/" $filename #note the space
relocateLibraryInCurrentLib "/Users/*/Wohlstand/PGE-Project/_Libs/_builds/macos/lib/" $filename #note the space
done
}

echo "Installing libs for $TARGET_APP..."
fetchPathsForApp # Fetch for executable
relocateLibraryInCurrentApp "@rpath/" SDL2.framework/Versions/A/SDL2 #note the space

EXECUTABLE_PATH="_Libs/libSDL2_mixer.1.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libSDL2_mixer.1.0.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libSDL2_mixer.1.0.0.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libvorbisfile.3.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libvorbis.0.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libvorbisenc.2.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libFLAC.8.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libFLAC++.6.dylib"
fetchPathsForLib #fetch for our dylib
EXECUTABLE_PATH="_Libs/libFLAC++.6.dylib"
fetchPathsForLib #fetch for our dylib


cd $bak
#if [[ $1 != "no-pause" ]]; then read -n 1; fi
exit 0
