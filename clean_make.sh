#!/bin/bash
bak=~+
cd $PWD
wasRemoved=0

kfile()
{
	if [ -f $1 ]; then
		rm $1
		echo "file $1 removed"
		wasRemoved=1
	fi
}

kfile Makefile
kfile ServerLib/ServerAPI/Makefile
kfile ServerLib/ServerAPI/Makefile*
kfile ServerLib/ServerAPP/Makefile
kfile ServerLib/ServerAPP/Makefile*
kfile _Libs/SDL2_mixer_modified/Makefile
kfile _Libs/SDL2_mixer_modified/Makefile*
kfile _Libs/FreeImage/Makefile
kfile _Libs/FreeImage/Makefile*
kfile Editor/Makefile
kfile Editor/Makefile*
kfile Engine/Makefile
kfile Engine/Makefile*
kfile GIFs2PNG/Makefile
kfile GIFs2PNG/Makefile*
kfile LazyFixTool/Makefile
kfile LazyFixTool/Makefile*
kfile PNG2GIFs/Makefile
kfile PNG2GIFs/Makefile*
kfile PlayableCalibrator/Makefile
kfile PlayableCalibrator/Makefile*
kfile Manager/Makefile
kfile Manager/Makefile*
kfile Maintainer/Makefile
kfile Maintainer/Makefile*
kfile MusicPlayer/Makefile
kfile MusicPlayer/Makefile*

if [ $wasRemoved -eq 0 ]; then
echo "Everything already clean!"
fi

cd $bak
if [[ "$1" != "nopause" ]]; then read -n 1; fi

