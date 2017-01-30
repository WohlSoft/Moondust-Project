#!/bin/bash

# User parameters

PREFIX=$1
TARGET=libFLAC.a
X_CFLAGS="-O2 -DNDEBUG -DHAVE_CONFIG_H -I$PWD/include -I$PWD/include_p -I$PWD/src/ -I$PWD/../libogg/include"
FILES=(bitmath.c bitreader.c bitwriter.c cpu.c crc.c fixed.c fixed_intrin_sse2.c fixed_intrin_ssse3.c float.c format.c lpc.c lpc_intrin_avx2.c lpc_intrin_sse.c lpc_intrin_sse2.c lpc_intrin_sse41.c md5.c memory.c metadata_iterators.c metadata_object.c ogg_decoder_aspect.c ogg_encoder_aspect.c ogg_helper.c ogg_mapping.c stream_decoder.c stream_encoder.c stream_encoder_framing.c stream_encoder_intrin_avx2.c stream_encoder_intrin_sse2.c stream_encoder_intrin_ssse3.c window.c)
INCLUDE_TO_COPY="-a include/FLAC"

# System parameters

if [[ $PREFIX == "" ]];
then
    PREFIX=./bin
fi

if [[ "$OSTYPE" != "msys" ]]; then
    X_CFLAGS="$X_CFLAGS -fPIC"
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    #use Clang on OS X hosts
    CC="clang"
else
    #use on any other hosts GCC
    CC="gcc"
fi
LD="ar -cqs"

if [[ "$OSTYPE" == "msys" ]]; then
    FILES+=(win_utf8_io.c)
    X_CFLAGS="$X_CFLAGS -DUNICODE"
    #mkdir -p temp/utf8
    #FILES+=(utf8/charset.c utf8/iconvert.c utf8/utf8.c)
fi


# ===========Ready? GO!!!===========

mkdir -p temp
TO_LINK=

errorofbuildOGG()
{
	printf "\n\n=========ERROR!!===========\n\n"
	exit 1
}

Build()
{
    echo $CC -c $X_CFLAGS src/$1 -o temp/$1.o

    $CC -c $X_CFLAGS src/$1 -o temp/$1.o

    if [ ! $? -eq 0 ]
    then
        errorofbuildOGG
    fi

    TO_LINK="$TO_LINK temp/$1.o"
}

printf "\nCompiling...\n\n"

for i in ${FILES[@]}; do
    Build ${i}
done

rm -f $PREFIX/lib/libmad.a
printf "\nLinking...\n\n"
echo $LD $TARGET $TO_LINK
$LD $TARGET $TO_LINK

if [ ! $? -eq 0 ]
then
    errorofbuildMAD
fi

echo "Installing into $PREFIX..."
mkdir -p $PREFIX/lib
mkdir -p $PREFIX/include

mv $TARGET $PREFIX/lib
cp $INCLUDE_TO_COPY $PREFIX/include

rm -Rf temp
printf "\nEVERYTHING HAS BEEN COMPLETED!\n\n"
