#!/bin/bash

LIBOGG=$PWD/../libogg

source ./mk-vorbis.sh $1
source ./mk-vorbisfile.sh $1
source ./mk-vorbisenc.sh $1
