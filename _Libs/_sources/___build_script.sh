#!/bin/bash

#Install Builds into _builds/linux directory
#InstallTo=~0/../_builds/linux

#Install Builds into _builds/macos directory
#InstallTo=~0/../_builds/macos

#Install Builds into /usr/ directory globally
#InstallTo = /usr/

#=======================================================================
errorofbuid()
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
	tar -xvf ../$1.tar.*z
	fi
}

BuildSrc()
{
# $1 - dir name   #2 additional props

cd $1
./configure $2
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

make
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

make install
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi
cd ..
}

UnArch 'SDL2-2.0.3'
UnArch 'SDL2_mixer-2.0.0'
UnArch 'libogg-1.3.2'
UnArch 'libvorbis-1.3.4'
UnArch 'flac-1.3.0'
UnArch 'libmikmod-3.3.7'
UnArch 'libmodplug-0.8.8.5'
UnArch 'libmad-0.15.1b'
cp ../libmad-0.15.1b.patched_configure.txt .
#UnArch "SDL2_ttf-2.0.12"

#############################Build libraries#####################

###########SDL2###########
BuildSrc 'SDL2-2.0.3' '--prefix='$InstallTo

#apply fix of SDL2 bug
cp ../SDL_platform.h $InstallTo/include/SDL_platform.h


###########OGG###########
BuildSrc 'libogg-1.3.2' '--prefix='$InstallTo


###########VORBIS###########
BuildSrc 'libvorbis-1.3.4' '--prefix='$InstallTo

###########FLAC###########
BuildSrc 'flac-1.3.0' '--disable-xmms-plugin --prefix='$InstallTo


###########MIKMOD###########
BuildSrc 'libmikmod-3.3.7' '--prefix='$InstallTo

###########MODPLUG###########
BuildSrc 'libmodplug-0.8.8.5' '--prefix='$InstallTo

###########LibMAD###########
cd libmad-0.15.1b
if [ ! -f ./configure_before_patch ]
then
	mv ./configure ./configure_before_patch
	cp ../libmad-0.15.1b.patched_configure.txt ./configure
	chmod u+x ./configure
fi
cd ..
BuildSrc 'libmad-0.15.1b' '--prefix='$InstallTo

###########SDL2_mixer###########
#cd SDL2_mixer-2.0.0
#BuildSrc 'SDL2_mixer-2.0.0' '--prefix=$InstallTo --enable-music-mp3 --enable-music-mp3-mad-gpl --disable-music-mp3-smpeg  --enable-music-midi --enable-music-midi-timidity --disable-music-midi-fluidsynth --enable-music-mod-modplug --enable-music-mod --disable-music-mod-mikmod --disable-music-mod-mikmod-shared --enable-music-flac=yes --disable-music-flac-shared --with-gnu-ld --disable-sdltest'

#=======================================================================
echo Libraries installed into $InstallTo


