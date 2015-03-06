#/bin/bash
set bak = ~+
cd $PWD

rm Makefile
rm _Libs/SDL2_mixer_modified/Makefile
rm _Libs/SDL2_mixer_modified/Makefile*
rm Editor/Makefile
rm Editor/Makefile*
rm Engine/Makefile
rm Engine/Makefile*
rm GIFs2PNG/Makefile
rm GIFs2PNG/Makefile*
rm LazyFixTool/Makefile
rm LazyFixTool/Makefile*
rm PNG2GIFs/Makefile
rm PNG2GIFs/Makefile*
rm PlayableCalibrator/Makefile
rm PlayableCalibrator/Makefile*

cd $bak
read -n 1

