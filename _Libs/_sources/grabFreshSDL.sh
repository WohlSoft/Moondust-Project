#!/bin/bash

#*****************************************************************************
# A tiny script to retreive fresh tarball from the official HG of the libSDL #
#*****************************************************************************

LatestSDL=""

freshLatestSDL()
{
    LatestSDL=$(find . -maxdepth 1 -name "SDL-*.tar.gz" | sed "s/\.tar\.gz//;s/\.\///");
}

freshLatestSDL
echo "Previous version was $LatestSDL"

wget --content-disposition https://github.com/libsdl-org/SDL/archive/main.tar.gz -O SDL-main-new.tar.gz
# http://hg.libsdl.org/SDL/archive/tip.tar.gz

wget --content-disposition https://github.com/libsdl-org/SDL/archive/main.zip -O SDL-main-new.zip
# http://hg.libsdl.org/SDL/archive/tip.zip

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

    # Remove unnecessary folders from a Windows-used archive
    zip -d SDL-main.zip \
        SDL-main/VisualC/\* \
        SDL-main/VisualC-WinRT/\* \
        SDL-main/visualtest/\* \
        SDL-main/test/\* \
        SDL-main/Xcode/\* \
        SDL-main/Xcode-iOS/\* \
        SDL-main/android-project/\* \
        SDL-main/android-project-ant/\*

    echo "Fresh now is $LatestSDL"
else
    echo "Failed to retreive latest SDL!"
fi
