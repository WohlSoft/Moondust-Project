#!/bin/bash
bak=~+
cd $PWD
wasRemoved=0

kfile()
{
	if [ -f $1/Makefile ]; then
		rm "$1/Makefile"
		echo "file $1/Makefile removed"
		wasRemoved=1
	fi
	if [[ $(find $1 -maxdepth 1 -type f -name 'Makefile*') != "" ]]; then
		rm "$1/Makefile"*
		echo "file $1/Makefile* removed"
		wasRemoved=1
	fi
	if [[ $(find $1 -maxdepth 1 -type f -name 'object_script*') != "" ]]; then
		rm "$1/object_script"*
		echo "file $1/object_script* removed"
		wasRemoved=1
	fi
}

kfile .
kfile ServerLib/ServerAPI
kfile ServerLib/ServerApp
kfile _Libs
kfile _Libs/AudioCodecs
kfile _Libs/AudioCodecs/libogg
kfile _Libs/AudioCodecs/libvorbis
kfile _Libs/AudioCodecs/libFLAC
kfile _Libs/AudioCodecs/libADLMIDI
kfile _Libs/AudioCodecs/libOPNMIDI
kfile _Libs/AudioCodecs/libtimidity-sdl
kfile _Libs/AudioCodecs/libid3tag-sdl
kfile _Libs/AudioCodecs/libmad
kfile _Libs/AudioCodecs/zlib
kfile _Libs/AudioCodecs/libgme
kfile _Libs/AudioCodecs/libmodplug
kfile _Libs/SDL_Mixer_X
kfile _Libs/FreeImage
kfile _Libs/luabind/_project
kfile _Libs/oolua/project
kfile Editor
kfile Engine
kfile GIFs2PNG
kfile LazyFixTool
kfile PNG2GIFs
kfile PlayableCalibrator
kfile Manager
kfile Maintainer
kfile MusicPlayer

if [ $wasRemoved -eq 0 ]; then
echo "Everything already clean!"
fi

cd $bak
if [[ "$1" != "nopause" ]]; then read -n 1; fi
