@echo off

if exist build\* rmdir build /S /Q

md build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%1 ..

mingw32-make -j 2

if not X%1X == XX mingw32-make install

cd ..

