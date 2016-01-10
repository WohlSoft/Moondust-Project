#!/bin/bash
bak=~+
cd $PWD
wasRemoved=0

kfile()
{
	if [ -f "$1/Makefile" ]; then
		rm "$1/Makefile"
		echo "file $1/Makefile removed"
		wasRemoved=1
	fi
	if [ -f "$1/Makefile*" ]; then
		rm "$1/Makefile*"
		echo "file $1/Makefile* removed"
		wasRemoved=1
	fi
	if [ -f "$1/object_script*" ]; then
		rm "$1/object_script*"
		echo "file $1/object_script* removed"
		wasRemoved=1
	fi
}

kfile .
kfile ServerLib/ServerAPI
kfile ServerLib/ServerAPP
kfile _Libs
kfile _Libs/SDL2_mixer_modified
kfile _Libs/FreeImage
kfile _Libs/luabind/_project
kfile _Libs/oolua/_project
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

