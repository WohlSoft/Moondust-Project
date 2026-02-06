#!/bin/bash

src=../_Libs/SDL_Mixer_X/examples/MusPlay-Qt

rm -Rf AssocFiles
cp -av $src/AssocFiles .

rm -Rf Effects
cp -av $src/Effects .

rm -Rf MainWindow
cp -av $src/MainWindow .

rm -Rf Player
cp -av $src/Player .

rm -Rf _resources
cp -av $src/_resources .

rm -Rf SingleApplication
cp -av $src/SingleApplication .

rm *.h
rm *.c
rm *.cpp

cp -av $src/*.h .
cp -av $src/*.c .
cp -av $src/*.cpp .

cp -av $src/sources.cmake sources.cmake
cp -av $src/changelog.musplay.txt ../changelog.musplay.txt
