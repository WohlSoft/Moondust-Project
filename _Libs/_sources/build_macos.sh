#!/bin/bash
InstallTo=~0/../_builds/macos
#export CC=/Applications/XCode.app/Contents/Developer/usr/bin/gcc
echo $InstallTo
read -n 1
SDL_ARGS='x86_64-apple-darwin '
source ./___build_script.sh

<<<<<<< Updated upstream
=======
#install SDL
#mkdir -p $InstallTo/frameworks/SDL2.framework/Versions/A
#mkdir -p $InstallTo/frameworks/SDL2.framework/Versions/A/_CodeSignature
#mkdir -p $InstallTo/frameworks/SDL2.framework/Versions/A/Headers/SDL2
#mkdir -p $InstallTo/frameworks/SDL2.framework/Versions/A/Resources

#cp -a SDL/SDL-dbcbdc2940ed/include/* $InstallTo/frameworks/SDL2.framework/Versions/A/Headers/SDL2
mkdir -p $InstallTo/lib
mkdir -p $InstallTo/include

instLib()
{
    cp -a ./SDL/$1/*.dylib $InstallTo/lib
    cp -a ./SDL/$1/*.a $InstallTo/lib
    cp -a ./SDL/$1/*.la* $InstallTo/lib
}

#install libOGG
mkdir -p $InstallTo/include/ogg
instLib libogg-1.3.2/src/.libs
cp -a ./SDL/libogg-1.3.2/include/ogg/*.h $InstallTo/include/ogg

mkdir -p $InstallTo/include/vorbis
#/Users/9pipbubbles/Desktop/PGE-Project/_Libs/_sources/SDL/libvorbis-1.3.4/lib/.libs/libvorbis.a
instLib  libvorbis-1.3.4/lib/.libs/
cp -a ./SDL/libvorbis-1.3.4/include/vorbis/*.h $InstallTo/include/vorbis

mkdir -p $InstallTo/include/FLAC
mkdir -p $InstallTo/include/FLAC++
cp -a ./SDL/flac-1.3.1/include/FLAC/*.h $InstallTo/include/FLAC
cp -a ./SDL/flac-1.3.1/include/FLAC++/*.h $InstallTo/include/FLAC++
instLib flac-1.3.1/src/libFLAC/.libs
instLib flac-1.3.1/src/libFLAC++/.libs
#/Users/9pipbubbles/Desktop/PGE-Project/_Libs/_sources/SDL/libmad-0.15.1b/.libs/libmad.a

#/Users/9pipbubbles/Desktop/PGE-Project/_Libs/_sources/SDL/flac-1.3.1/src/libFLAC/.libs

mkdir -p $InstallTo/include/libmodplug
instLib libmodplug-0.8.8.5/src/.libs
cp -a ./SDL/libmodplug-0.8.8.5/src/libmodplug/*.h $InstallTo/include/libmodplug
cp -a ./SDL/libmodplug-0.8.8.5/src/modplug.h $InstallTo/include/libmodplug

instLib libmad-0.15.1b/.libs
cp -a ./SDL/libmad-0.15.1b/

mkdir -p $InstallTo/include/luajit-2.0
>>>>>>> Stashed changes
