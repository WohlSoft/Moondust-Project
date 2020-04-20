#!/bin/bash
set -e

function buildIt()
{
    echo "------- $1.c -------"

    echo "Building..."
    i686-w64-mingw32-gcc -O2 $1.c -o $1.exe -static-libgcc

    echo "Stripping..."
    i686-w64-mingw32-strip $1.exe

    echo "Packing..."
    tar -czf $1.tar.gz $1.exe
    # remove exe after of packing
    rm $1.exe
}

buildIt luna_hwnd_show
buildIt 38a_ipc_bridge

# build LunaTester
echo "------- LunaLoader -------"
cd LunaLoader
echo "LunaLoader.cpp..."
i686-w64-mingw32-g++ -O2 -DLUNALOADER_EXEC LunaLoader.cpp -c -o LunaLoader.cpp.o
echo "LunaLoaderPatch.cpp..."
i686-w64-mingw32-g++ -O2 -DLUNALOADER_EXEC LunaLoaderPatch.cpp -c -o LunaLoaderPatch.cpp.o
echo "LunaLoader.rc..."
i686-w64-mingw32-windres -DLUNALOADER_EXEC LunaLoader.rc -o LunaLoaderRes.o
echo "Linking..."
i686-w64-mingw32-g++ LunaLoader.cpp.o LunaLoaderPatch.cpp.o LunaLoaderRes.o -o ../LunaLoader-exec.exe -static-libgcc -static-libstdc++
rm *.o
cd ..
echo "Stripping..."
i686-w64-mingw32-strip LunaLoader-exec.exe
echo "Packing..."
tar -czf LunaLoader-exec.tar.gz LunaLoader-exec.exe
rm LunaLoader-exec.exe

echo "------ Done! ------"

