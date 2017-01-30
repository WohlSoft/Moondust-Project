#!/bin/bash

PREFIX=$1

if [[ $PREFIX == "" ]];
then
    PREFIX=./bin
fi

#use Clang on OS X hosts
if [[ "$OSTYPE" == "darwin"* ]]; then
    CC="clang"
else
#use on any other hosts GCC
    CC="gcc"
fi

LD="ar -cqs"
X_CFLAGS="-O2 -DNDEBUG -DFPM_DEFAULT -DHAVE_CONFIG_H"

if [[ "$OSTYPE" != "msys" ]]; then
    X_CFLAGS="$X_CFLAGS -fPIC"
fi

mkdir -p temp
TO_LINK=

errorofbuildMAD()
{
	printf "\n\n=========ERROR!!===========\n\n"
	exit 1
}

Build()
{
    echo $CC -c $X_CFLAGS $1.c -o temp/$1.o

    $CC -c $X_CFLAGS $1.c -o temp/$1.o

    if [ ! $? -eq 0 ]
    then
        errorofbuildMAD
    fi

    TO_LINK="$TO_LINK temp/$1.o"
}

printf "\nCompiling...\n\n"

Build bit
Build decoder
Build fixed
Build frame
Build huffman
Build layer12
Build layer3
Build stream
Build synth
Build timer
Build version

rm -f $PREFIX/lib/libmad.a
printf "\nLinking...\n\n"
echo $LD libmad.a $TO_LINK
$LD libmad.a $TO_LINK

if [ ! $? -eq 0 ]
then
    errorofbuildMAD
fi

echo "Installing into $PREFIX..."
mkdir -p $PREFIX/lib
mkdir -p $PREFIX/include

mv libmad.a $PREFIX/lib
cp include/mad.h $PREFIX/include

rm -Rf temp
printf "\nEVERYTHING HAS BEEN COMPLETED!\n\n"
