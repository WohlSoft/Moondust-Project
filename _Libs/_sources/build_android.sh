#!/bin/bash
InstallTo=$(echo ~+/../_builds/android)
InstallTo=$(readlink -f $InstallTo)

IsAndroid="android"
ANDR_PLTF=android-18
ANDR_HOST=arm-linux-androideabi-4.9
AN_ROOT=/home/vitaly/android-ndk-r10e

PREBUILT=$AN_ROOT/toolchains/$ANDR_HOST
PLATFORM=$AN_ROOT/platforms/$ANDR_PLTF/arch-arm

CONFIGURE_ARGS='--host=arm-linux-androideabi '

export NDK=$AN_ROOT
export SYSROOT=$NDK/platforms/$ANDR_PLTF/arch-arm/
export TOOLCHAIN=$NDK/toolchains/$ANDR_HOST/prebuilt/linux-x86_64/

export CC=$PREBUILT"/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc"
export CXX=$PREBUILT"/prebuilt/linux-x86_64/bin/arm-linux-androideabi-g++"
export CFLAGS="-fPIC -DANDROID -nostdlib"
export ANDROID_ROOT=$AN_ROOT/
export LDFLAGS="-Wl,-rpath-link=$ANDROID_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/ -L$ANDROID_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/"
export CPPFLAGS="-I$ANDROID_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/include/"
export LIBS="-lc "

CORE_LDFLAGS="-Wl,-rpath-link=$ANDROID_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/ -L$ANDROID_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/"
CMN_LDFLAGS=" -L/"$InstallTo"/lib"
export LDFLAGS=$CORE_LDFLAGS" "$CMN_LDFLAGS
#LDFLAGS=$LDFLAGS" -L/"$InstallTo"/lib"
CFLAGS=$CFLAGS" -I/"$InstallTo"/include"
CPPFLAGS=$CPPFLAGS" -I/"$InstallTo"/include"

#PATH="/usr/src/android-ndk-r9e:$PATH"          # for 'ndk-build'
PATH="/home/vitaly/_git_repos/android-sdk-linux/tools:$PATH"  # for 'android'
PATH="/home/vitaly/android-ndk-r10e:$PATH"

echo $InstallTo
source ./___build_script_android.sh

printf "\n\n=========BUILT!!===========\n\n"
read -n 1
exit 0

