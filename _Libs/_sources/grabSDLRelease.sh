#!/bin/bash

#*****************************************************************************
# A tiny script to retreive fresh tarball from the official HG of the libSDL #
#*****************************************************************************

LatestSDL=""

cleanUp()
{
    rm -Rf SDL-main/VisualC
    rm -Rf SDL-main/VisualC-WinRT
    rm -Rf SDL-main/visualtest
    rm -Rf SDL-main/test
    rm -Rf SDL-main/Xcode
    rm -Rf SDL-main/Xcode-iOS
}

freshLatestSDL()
{
    LatestSDL=$(find . -maxdepth 1 -name "SDL-*.tar.gz" | sed "s/\.tar\.gz//;s/\.\///");
}

freshLatestSDL
echo "Previous version was $LatestSDL"

wget --content-disposition https://github.com/libsdl-org/SDL/archive/release-2.0.14.tar.gz -O SDL-2.0.14-new.tar.gz

tar -xf SDL-2.0.14-new.tar.gz
rm -f SDL-2.0.14-new.tar.gz
mv SDL-release-2.0.14 SDL-main
cleanUp
tar -czf SDL-main-new.tar.gz SDL-main
rm -Rf SDL-main

wget --content-disposition https://github.com/libsdl-org/SDL/archive/release-2.0.14.zip -O SDL-2.0.14-new.zip

7z x SDL-2.0.14-new.zip
# Remove unnecessary folders from a Windows-used archive
rm -f SDL-2.0.14-new.zip
mv SDL-release-2.0.14 SDL-main
cleanUp
rm -Rf SDL-main/android-project
rm -Rf SDL-main/android-project-ant
7z a SDL-main-new.zip SDL-main
rm -Rf SDL-main

if [ $? -eq 0 ];
then
    OldSDL=$LatestSDL
    freshLatestSDL
    if [[ "$OldSDL" != "$LatestSDL" ]];
    then
        rm $OldSDL'.tar.gz'
        rm $OldSDL'.zip'
    fi
    mv SDL-main-new.tar.gz SDL-main.tar.gz
    mv SDL-main-new.zip SDL-main.zip

    echo "Fresh now is $LatestSDL"
else
    echo "Failed to retreive latest SDL!"
fi
