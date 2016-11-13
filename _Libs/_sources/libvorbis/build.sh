#!/bin/bash

LIBOGG=$PWD/../libogg

source ./build-vorbis.sh $1
source ./build-vorbisfile.sh $1
source ./build-vorbisenc.sh $1
