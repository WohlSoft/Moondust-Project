#!/bin/bash

#*****************************************************************************
# A tiny script to retreive fresh tarball from the official HG of the libSDL #
#*****************************************************************************

LatestSDL=""

cleanUp()
{
    rm -Rf SDL-default/VisualC
    rm -Rf SDL-default/VisualC-WinRT
    rm -Rf SDL-default/visualtest
    rm -Rf SDL-default/test
    rm -Rf SDL-default/Xcode
    rm -Rf SDL-default/Xcode-iOS
}

freshLatestSDL()
{
    LatestSDL=$(find . -maxdepth 1 -name "SDL-*.tar.gz" | sed "s/\.tar\.gz//;s/\.\///");
}

freshLatestSDL
echo "Previous version was $LatestSDL"

wget --content-disposition https://hg.libsdl.org/SDL/archive/release-2.0.14.tar.gz -O SDL-2.0.14-new.tar.gz
# http://hg.libsdl.org/SDL/archive/tip.tar.gz

tar -xf SDL-2.0.14-new.tar.gz
rm -f SDL-2.0.14-new.tar.gz
mv SDL-release-2.0.14 SDL-default
cleanUp
tar -czf SDL-default-new.tar.gz SDL-default
rm -Rf SDL-default

wget --content-disposition https://hg.libsdl.org/SDL/archive/release-2.0.14.zip -O SDL-2.0.14-new.zip
# http://hg.libsdl.org/SDL/archive/tip.zip
7z x SDL-2.0.14-new.zip
# Remove unnecessary folders from a Windows-used archive
rm -f SDL-2.0.14-new.zip
mv SDL-release-2.0.14 SDL-default
cleanUp
rm -Rf SDL-default/android-project
rm -Rf SDL-default/android-project-ant
7z a SDL-default-new.zip SDL-default
rm -Rf SDL-default

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
