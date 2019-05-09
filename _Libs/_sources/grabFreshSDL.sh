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

wget --content-disposition https://hg.libsdl.org/SDL/archive/default.tar.gz -O SDL-default-new.tar.gz
# http://hg.libsdl.org/SDL/archive/tip.tar.gz

wget --content-disposition https://hg.libsdl.org/SDL/archive/default.zip -O SDL-default-new.zip
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
    mv SDL-default-new.tar.gz SDL-default.tar.gz
    mv SDL-default-new.zip SDL-default.zip
    echo "Fresh now is $LatestSDL"
else
    echo "Failed to retreive latest SDL!"
fi

