#!/bin/bash

#Install Builds into _builds/linux directory
#InstallTo=~0/../_builds/linux

#Install Builds into _builds/macos directory
#InstallTo=~0/../_builds/macos

#Install Builds into /usr/ directory globally
#InstallTo = /usr/

source "./build_packagesList.sh"

OurOS="linux_defaut"
if [[ "$OSTYPE" == "darwin"* ]]; then
OurOS="macos"
elif [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "linux" ]]; then
OurOS="linux"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
OurOS="freebsd"
fi

if [[ "$OurOS" != "macos" ]]; then
    Sed=sed
else
    Sed=gsed
fi

#=======================================================================
errorofbuild()
{
	printf "\n\n=========ERROR!!===========\n\n"
	exit 1
}
#=======================================================================

if [ ! -d SDL ] 
	then
	mkdir SDL
	fi

cd SDL

UnArch()
{
# $1 - archive name
    if [ ! -d $1 ]
	    then
        printf "tar -xf ../$1.tar.*z* ..."
	    tar -xf ../$1.tar.*z*
        if [ $? -eq 0 ]; then
          printf "OK!\n"
        else
          printf "FAILED!\n"
        fi
    fi
}

BuildSrc()
{
# $1 - dir name   #2 additional props

    cd $1
    #Build debug version of SDL
    #CFLAGS='-O0 -g' ./configure $2
    ./configure $2  
    if [ $? -eq 0 ]
    then
      echo "[good]"
    else
      errorofbuild
    fi

    make
    if [ $? -eq 0 ]
    then
      echo "[good]"
    else
      errorofbuild
    fi

    #if [[ "$OurOS" != "macos" ]]; then
        make install
        if [ $? -eq 0 ]
        then
          echo "[good]"
        else
          errorofbuild
        fi
    #fi
    cd ..
}



#############################Build libraries#####################

BuildSDL()
{
        #LatestSDL='SDL-afd286e26823'
        UnArch $LatestSDL

        ###########SDL2###########
        echo "=======SDL2========="
        #sed  -i 's/-version-info [^ ]\+/-avoid-version /g' $LatestSDL'/src/Makefile.am'
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' $LatestSDL'/Makefile.in'
        if [[ "$OurOS" != "macos" ]]; then
            #on any other OS'es build via autotools
            BuildSrc $LatestSDL $SDL_ARGS'--prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib'
        else
            #on Mac OS X build via X-Code
            cd $LatestSDL
                UNIVERSAL_OUTPUTFOLDER=$InstallTo/frameworks
                if [ -d $UNIVERSAL_OUTPUTFOLDER ]; then
                    #Deletion of old builds
                    rm -Rf $UNIVERSAL_OUTPUTFOLDER
                fi
                mkdir -p -- "$UNIVERSAL_OUTPUTFOLDER"

                xcodebuild -target Framework -project Xcode/SDL/SDL.xcodeproj -configuration Release BUILD_DIR="${InstallTo}/frameworks"

                if [ $? -eq 0 ]
                then
                  echo "[good]"
                else
                  errorofbuild
                fi

                #move out built framework from "Release" folder
                mv -f $InstallTo/frameworks/Release/SDL2.framework $InstallTo/frameworks/
                rm -Rf $InstallTo/frameworks/Release

                #make RIGHT headers organization in the SDL Framework
                mkdir -p -- ${InstallTo}/frameworks/SDL2.framework/Headers/SDL2
                mv ${InstallTo}/frameworks/SDL2.framework/Headers/*.h ${InstallTo}/frameworks/SDL2.framework/Headers/SDL2

            cd ..
        fi

        #apply fix of SDL2 bug (no more needed!)
        #cp ../SDL_platform.h $InstallTo/include/SDL_platform.h
}

BuildOGG()
{
        UnArch 'libogg-1.3.2'

        ###########OGG###########
        echo "=========OGG==========="
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libogg-1.3.2/src/Makefile.am'
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libogg-1.3.2/src/Makefile.in'
        BuildSrc 'libogg-1.3.2' '--prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib --enable-static=yes --enable-shared=no CFLAGS=-fPIC CXXFLAGS=-fPIC'

        #if [[ "$OurOS" == "macos" ]]; then
            #install libOGG
        #    cp -a ./libogg-1.3.2/src/.libs/*.dylib $installTo/lib
        #    cp -a ./libogg-1.3.2/src/.libs/*.a $installTo/lib
        #    cp -a ./libogg-1.3.2/src/.libs/*.la* $installTo/lib
        #    mkdir -p $InstallTo/include/ogg
        #    cp -a ./libogg-1.3.2/include/ogg/*.h $InstallTo/include/ogg
        #    cd ./libogg-1.3.2/src
        #        bash ../libtool --mode=install ginstall -c libogg.la $InstallTo/lib
        #        gmkdir -p $InstallTo/share/aclocal
        #        ginstall -c -m 644 ogg.m4 $InstallTo/share/aclocal
        #        gmkdir -p $InstallTo/lib/pkgconfig
        #        ginstall -c -m 644 ogg.pc $InstallTo/lib/pkgconfig
        #    cd ../../
        #fi
}

BuildVORBIS()
{
        UnArch 'libvorbis-1.3.4'

        ###########VORBIS###########
        echo "============VORBIS=========="
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libvorbis-1.3.4/lib/Makefile.am'
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libvorbis-1.3.4/lib/Makefile.in'
        BuildSrc 'libvorbis-1.3.4' '--prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib --with-ogg='$InstallTo' --disable-oggtest --enable-static=yes --enable-shared=no CFLAGS=-fPIC CXXFLAGS=-fPIC'
}

BuildFLAC()
{
        UnArch 'flac-1.3.1'

        #Debug output of environment (because sometimes flac failing to build because "missing of automake-1.14")
        #... however, second attempt to run build is successful
        set > env_flac.txt
        unset COLUMNS
        unset LINES
        ###########FLAC###########
        echo "==========FLAC========="
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'flac-1.3.1/src/libFLAC++/Makefile.in'
        $Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'flac-1.3.1/src/libFLAC++/Makefile.am'
        $Sed  -i 's/-version-info 11:0:3/-avoid-version /g' 'flac-1.3.1/src/libFLAC/Makefile.in'
        $Sed  -i 's/-version-info 11:0:3/-avoid-version /g' 'flac-1.3.1/src/libFLAC/Makefile.am'
        BuildSrc 'flac-1.3.1' '--disable-xmms-plugin --disable-cpplibs --enable-static=yes --enable-shared=no --prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib CFLAGS=-fPIC CXXFLAGS=-fPIC'
}

#BuildMikMOD()
#{
#        UnArch 'libmikmod-3.3.7'
#
#        ###########MIKMOD###########
#        echo "=============MIKMOD=========="
#        BuildSrc 'libmikmod-3.3.7' '--prefix='$InstallTo
#}

BuildMODPLUG()
{
        UnArch 'libmodplug-0.8.8.5'

        ###########MODPLUG###########
        echo "==========MODPLUG=========="
        $Sed -i 's/-version-info \$(MODPLUG_LIBRARY_VERSION)/-avoid-version/g' 'libmodplug-0.8.8.5/src/Makefile.am'
        $Sed -i 's/-version-info \$(MODPLUG_LIBRARY_VERSION)/-avoid-version/g' 'libmodplug-0.8.8.5/src/Makefile.in'
        BuildSrc 'libmodplug-0.8.8.5' '--prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib CFLAGS=-fPIC CXXFLAGS=-fPIC'
}

BuildMAD()
{
        UnArch 'libmad-0.15.1b'

        ###########LibMAD###########
        echo "==========LibMAD============"
        #cp ../libmad-0.15.1b.patched_osx_configure.txt .
        #cp ../libmad-0.15.1b.patched_configure.txt .
        cd libmad-0.15.1b
        if [ ! -f ./configure_before_patch ]
        then
	        mv ./configure ./configure_before_patch
            if [[ "$OurOS" == "macos" ]]; then
                cp ../../libmad-0.15.1b.patched_osx_configure.txt ./configure
            else
                cp ../../libmad-0.15.1b.patched_configure.txt ./configure
            fi
            chmod u+x ./configure
        fi
        cd ..
        $Sed -i 's/-version-info \$(version_info)/-avoid-version/g' 'libmad-0.15.1b/Makefile.am'
        $Sed -i 's/-version-info \$(version_info)/-avoid-version/g' 'libmad-0.15.1b/Makefile.in'
        if [[ "$OurOS" == "macos" ]]; then
            BuildSrc 'libmad-0.15.1b' 'x86_64-apple-darwin --enable-shared=no --enable-static=yes --prefix='$InstallTo' CFLAGS=-fPIC CXXFLAGS=-fPIC'
        else
            BuildSrc 'libmad-0.15.1b' '--enable-shared=no --enable-static=yes --prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib CFLAGS=-fPIC CXXFLAGS=-fPIC'
        fi
}

BuildFluidSynth()
{
        UnArch 'fluidsynth-1.1.6'
        ###########MODPLUG###########
        echo "==========FLUIDSYNTH=========="
        #Build minimalistic FluidSynth version to just generate raw audio output to handle in the SDL Mixer X
        #./configure CFLAGS=-fPIC --prefix=/home/vitaly/_git_repos/PGE-Project/_Libs/_builds/linux/ --disable-dbus-support --disable-pulse-support --disable-alsa-support --disable-portaudio-support --disable-oss-support --disable-jack-support --disable-midishare --disable-coreaudio --disable-coremidi --disable-dart --disable-lash --disable-ladcca --enable-static=yes --enable-shared=no
        BuildSrc 'fluidsynth-1.1.6' '--prefix='$InstallTo' --includedir='$InstallTo'/include --libdir='$InstallTo'/lib CFLAGS=-fPIC CXXFLAGS=-fPIC --disable-dbus-support --disable-pulse-support --disable-alsa-support --disable-portaudio-support --disable-oss-support --disable-jack-support --disable-midishare --disable-coreaudio --disable-coremidi --disable-dart --disable-lash --disable-ladcca --enable-static=yes --enable-shared=no'
}

BuildLUAJIT()
{
        UnArch 'luajit'
        
        ###########LuaJIT###########
        echo "==========LuaJIT============"
        cd LuaJIT
        make PREFIX=$InstallTo BUILDMODE=static
        if [ $? -eq 0 ]
        then
          echo "[good]"
        else
          errorofbuild
        fi

            make install PREFIX=$InstallTo BUILDMODE=static
            if [ $? -eq 0 ]
            then
              echo "[good]"
            else
              errorofbuild
            fi
            if [[ "$OurOS" == "macos" ]]; then
                cp -a ./src/libluajit.a $InstallTo/lib/libluajit.a
                cp -a ./src/libluajit.a $InstallTo/lib/libluajit-5.1.a
            fi
        cd ..
}

BuildGLEW()
{
        UnArch 'glew-1.13.0'

        ###########GLEW###########
        echo "==========GLEW============"
        cd glew-1.13.0
        make GLEW_PREFIX=$InstallTo GLEW_DEST=$InstallTo CFLAGS.EXTRA="-DGLEW_STATIC -fPIC" GLEW_NO_GLU="-DGLEW_NO_GLU"
        if [ $? -eq 0 ]
        then
          echo "[good]"
        else
          errorofbuild
        fi
            make install GLEW_PREFIX=$InstallTo GLEW_DEST=$InstallTo CFLAGS.EXTRA="-DGLEW_STATIC" GLEW_NO_GLU="-DGLEW_NO_GLU"
            if [ $? -eq 0 ]
            then
              echo "[good]"
            else
              errorofbuild
            fi
        cd ..
}

############ZLib###########
#echo "=======ZLib========="
#BuildSrc 'zlib-1.2.8' '--prefix='$InstallTo' --64 --static --shared'

###########LibPNG###########
#echo "=======LibPNG========="
#BuildSrc 'libpng-1.6.20' '--prefix='$InstallTo' --enable-shared=yes --enable-static=yes'

###########SDL_Image###########
#echo "=======SDL_Image========="
##$Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libvorbis-1.3.4/lib/Makefile.am'
##$Sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libvorbis-1.3.4/lib/Makefile.in'
#if [[ "$OurOS" != "macos" ]]; then
#        BuildSrc 'SDL2_image-2.0.0' '--prefix='$InstallTo' --disable-sdltest --enable-shared=no --enable-static=yes --enable-bmp=yes --enable-gif=yes --enable-jpg=no --enable-jpg-shared=no --enable-lbm=no --enable-pcx=no --enable-png=yes --enable-png-shared=yes --enable-pnm=no --enable-tga=no --enable-tif=no --enable-tif-shared=no --enable-xcf=no --enable-xpm=no --enable-xv=no --enable-webp=no --enable-webp-shared=no'
#else
#            #on Mac OS X build via X-Code
#    cd "SDL2_image-2.0.0"
#        UNIVERSAL_OUTPUTFOLDER=$InstallTo/frameworks
#        if [ -d $UNIVERSAL_OUTPUTFOLDER ]; then
#            #Deletion of old builds
#            rm -Rf $UNIVERSAL_OUTPUTFOLDER
#        fi
#        mkdir -p -- "$UNIVERSAL_OUTPUTFOLDER"
#
#        HEADER_SEARCH_PATHS="$HEADER_SEARCH_PATHS $InstallTo/frameworks/SDL.framework/Headers/"
#        FRAMEWORK_SEARCH_PATHS="$FRAMEWORK_SEARCH_PATHS $InstallTo/frameworks"
#        $Sed  -i 's/#include \"SDL.h\"/#include <SDL2\/SDL.h>/g' 'SDL_image.h'
#        $Sed  -i 's/#include \"SDL_version.h\"/#include <SDL2\/SDL_version.h>/g' 'SDL_image.h'
#        $Sed  -i 's/#include \"begin_code.h\"/#include <SDL2\/begin_code.h>/g' 'SDL_image.h'
#        $Sed  -i 's/#include \"close_code.h\"/#include <SDL2\/close_code.h>/g' 'SDL_image.h'
#
#        xcodebuild -target Framework -project Xcode/SDL_image.xcodeproj -configuration Release BUILD_DIR="${InstallTo}/frameworks"
#
#        if [ $? -eq 0 ]
#        then
#          echo "[good]"
#        else
#          errorofbuild
#        fi
#
#        #move out built framework from "Release" folder
#        mv -f $InstallTo/frameworks/Release/SDL2_image.framework $InstallTo/frameworks/
#        rm -Rf $InstallTo/frameworks/Release
#
#        #make RIGHT headers organization in the SDL Framework
#        mkdir -p -- ${InstallTo}/frameworks/SDL2_image.framework/Headers/SDL2
#        mv ${InstallTo}/frameworks/SDL2_image.framework/Headers/*.h ${InstallTo}/frameworks/SDL2_image.framework/Headers/SDL2
#
#    cd ..
#fi

###########SDL2_mixer###########
#cd SDL2_mixer-2.0.0
#BuildSrc 'SDL2_mixer-2.0.0' '--prefix=$InstallTo --enable-music-mp3 --enable-music-mp3-mad-gpl --disable-music-mp3-smpeg  --enable-music-midi --enable-music-midi-timidity --disable-music-midi-fluidsynth --enable-music-mod-modplug --enable-music-mod --disable-music-mod-mikmod --disable-music-mod-mikmod-shared --enable-music-flac=yes --disable-music-flac-shared --with-gnu-ld --disable-sdltest'

#=======================================================================
#if [[ "$OurOS" == "macos" ]]; then
    #run manual installation script for OS-X to avoid sudi usage
#    source ./install_osx.sh
#fi

#UnArch "SDL2_ttf-2.0.12"
#UnArch "zlib-1.2.8"
#UnArch "libpng-1.6.20"
#UnArch "SDL2_image-2.0.0"

########################Build & Install libraries##################################

BuildOGG
BuildVORBIS
BuildFLAC
#BuildMODPLUG
BuildMAD
BuildFluidSynth
BuildSDL
BuildLUAJIT
#BuildGLEW

echo Libraries installed into $InstallTo


