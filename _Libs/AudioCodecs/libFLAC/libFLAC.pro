#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = FLAC
INSTALLINCLUDES = $$PWD/include/FLAC/*
INSTALLINCLUDESTO = FLAC
include($$PWD/../audio_codec_common.pri)

DEFINES     += HAVE_CONFIG_H FLAC__NO_DLL
win*-msvc*: {
DEFINES += _CRT_SECURE_NO_WARNINGS
QMAKE_CFLAGS += /wd4244
}
#win32:DEFINES += HAVE_FSEEKO
macx: QMAKE_CFLAGS_WARN_ON = -Wall -Wno-unused-variable

INCLUDEPATH += $$PWD $$PWD/include $$PWD/include_p $$PWD/src/

INCLUDEPATH += $$PWD/../libogg/include

HEADERS += \
    include/FLAC/all.h \
    include/FLAC/assert.h \
    include/FLAC/callback.h \
    include/FLAC/export.h \
    include/FLAC/format.h \
    include/FLAC/metadata.h \
    include/FLAC/ordinals.h \
    include/FLAC/stream_decoder.h \
    include/FLAC/stream_encoder.h \
    include_p/share/grabbag/cuesheet.h \
    include_p/share/grabbag/file.h \
    include_p/share/grabbag/picture.h \
    include_p/share/grabbag/replaygain.h \
    include_p/share/grabbag/seektable.h \
    include_p/share/alloc.h \
    include_p/share/compat.h \
    include_p/share/endswap.h \
    include_p/share/getopt.h \
    include_p/share/grabbag.h \
    include_p/share/macros.h \
    include_p/share/private.h \
    include_p/share/replaygain_analysis.h \
    include_p/share/replaygain_synthesis.h \
    include_p/share/safe_str.h \
    include_p/share/utf8.h \
    include_p/share/win_utf8_io.h \
    src/ia32/nasm.h \
    src/include/private/all.h \
    src/include/private/bitmath.h \
    src/include/private/bitreader.h \
    src/include/private/bitwriter.h \
    src/include/private/cpu.h \
    src/include/private/crc.h \
    src/include/private/fixed.h \
    src/include/private/float.h \
    src/include/private/format.h \
    src/include/private/lpc.h \
    src/include/private/macros.h \
    src/include/private/md5.h \
    src/include/private/memory.h \
    src/include/private/metadata.h \
    src/include/private/ogg_decoder_aspect.h \
    src/include/private/ogg_encoder_aspect.h \
    src/include/private/ogg_helper.h \
    src/include/private/ogg_mapping.h \
    src/include/private/stream_encoder.h \
    src/include/private/stream_encoder_framing.h \
    src/include/private/window.h \
    src/include/protected/all.h \
    src/include/protected/stream_decoder.h \
    src/include/protected/stream_encoder.h \
    config.h

SOURCES += \
    src/bitmath.c \
    src/bitreader.c \
    src/bitwriter.c \
    src/cpu.c \
    src/crc.c \
    src/fixed.c \
    src/format.c \
    src/lpc.c \
    src/md5.c \
    src/memory.c \
    src/metadata_iterators.c \
    src/metadata_object.c \
    src/ogg_decoder_aspect.c \
    src/ogg_encoder_aspect.c \
    src/ogg_helper.c \
    src/ogg_mapping.c \
    src/stream_decoder.c \
    src/stream_encoder.c \
    src/stream_encoder_framing.c \
    src/window.c

# Include this if you want use integer-only version
#SOURCES += \
#    src/float.c

# Uncoment this if you want use ASM optimizations
#SOURCES += \
#    src/lpc_intrin_avx2.c \
#    src/lpc_intrin_sse.c \
#    src/lpc_intrin_sse2.c \
#    src/lpc_intrin_sse41.c \
#    src/fixed_intrin_sse2.c \
#    src/fixed_intrin_ssse3.c \
#    src/stream_encoder_intrin_avx2.c \
#    src/stream_encoder_intrin_sse2.c \
#    src/stream_encoder_intrin_ssse3.c

win32: include($$PWD/win_utf8_io.pri)

