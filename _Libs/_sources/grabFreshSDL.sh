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

wget --content-disposition http://hg.libsdl.org/SDL/archive/tip.tar.gz

if [ $? -eq 0 ];
then
    OldSDL=$LatestSDL
    freshLatestSDL
    if [[ "$OldSDL" != "$LatestSDL" ]];
    then
        rm $LatestSDL'.tar.gz'
    fi
    echo "Fresh now is $LatestSDL"
else
    echo "Failed to retreive latest SDL!"
fi


