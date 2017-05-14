TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
static-sdlmixer:{
    CONFIG -= dll
    CONFIG += static
} else {
    CONFIG += dll
    CONFIG -= static
}

include($$PWD/../../_common/lib_destdir.pri)
include($$PWD/cflags.pri)

static-sdlmixer:{
    TARGET = SDL_Mixer_Xstatic
} else {
    TARGET = SDL2_mixer_ext
    win32:enable-stdcalls:{ #Required for building a VB6-compatible version
        TARGET = SDL2MixerVB
    }
}

# Dependent audio codecs
include($$PWD/../AudioCodecs/audio_codec_includes.pri)

# Build properties
include($$PWD/../../_common/build_props.pri)
DESTDIR = $$PWD/../_builds/$$TARGETOS/lib
COPY=cp

LIBS += -L$$PWD/../_builds/$$TARGETOS/lib
INCLUDEPATH += $$PWD/../_builds/$$TARGETOS/include

win32:{
    DEFINES += USE_NATIVE_MIDI
    enable-stdcalls:{ #Useful for VB6 usage
        TARGET = SDL2MixerVB
        QMAKE_LFLAGS += -Wl,--add-stdcall-alias
        DEFINES += FORCE_STDCALLS
    }
    RC_FILE = version.rc
    COPY=copy
}
linux-g++||unix:!macx:!android:{
    DEFINES += HAVE_INTTYPES_H HAVE_SETENV HAVE_SINF
    CONFIG -= dll
    CONFIG -= static
    CONFIG += static_and_shared
    CONFIG += unversioned_libname
    CONFIG += skip_target_version_ext
}
android:{
    DEFINES += HAVE_INTTYPES_H HAVE_SETENV HAVE_SINF
}
macx:{
    DEFINES += HAVE_INTTYPES_H HAVE_SETENV HAVE_SINF
    # Build as static library
    CONFIG -= dll
    CONFIG += static
    CONFIG += staticlib
}

!win32: LIBS += -lSDL2

QMAKE_POST_LINK = $$COPY $$shell_path($$PWD/SDL_mixer_ext.h) $$shell_path($$PWD/../_builds/$$TARGETOS/include/SDL2)

DEFINES += \
    main=SDL_main \
    HAVE_SIGNAL_H \
    HAVE_SETBUF \
    WAV_MUSIC \
    MID_MUSIC \
    USE_TIMIDITY_MIDI \
    #USE_FLUIDSYNTH_MIDI \
    USE_ADL_MIDI \
    USE_OPN2_MIDI \
    OGG_MUSIC \
    FLAC_MUSIC \
    MP3_MAD_MUSIC \
    GME_MUSIC \
    SPC_MORE_ACCURACY \#NO_OLDNAMES
    MODPLUG_MUSIC \
    PIC \
    _REENTRANT \
    _USE_MATH_DEFINES

android: {
DEFINES += \
    HAVE_STRCASECMP \
    HAVE_STRNCASECMP \
    MID_MUSIC \
    USE_ADL_MIDI \
    GME_MUSIC \
    NO_OLDNAMES \
    SPC_MORE_ACCURACY
    #OGG_USE_TREMOR
    DEFINES -= FLAC_MUSIC #temopary with no FLAC, because I wasn't built it because compilation bug
}

LIBS += -L$$PWD/../_builds/$$TARGETOS/lib

android:{
    LIBS += -lvorbisfile -lvorbis -logg #-lvorbisidec
} else {
    win32:{
        DEFINES -= UNICODE _UNICODE
        enable-stdcalls: {
            LIBS += -static -l:libSDL2.a -l:libFLAC.a -l:libvorbisfile.a -l:libvorbis.a -l:libogg.a -l:libmad.a -static-libgcc -static-libstdc++ -static -lpthread -luuid # -l:libfluidsynth.a
            SOURCES += vb6_sdl_binds.c
        } else {
            !win*-msvc*:{
                LIBS += -lSDL2main -lSDL2.dll -l:libFLAC.a -l:libvorbisfile.a -l:libvorbis.a -l:libogg.a -l:libmad.a #-l:libfluidsynth.a
            } else {
                LIBS += -lSDL2main -lSDL2 -lFLAC -lvorbisfile -lvorbis -logg -lmad
            }
        }
        LIBS += -lwinmm -lole32 -limm32 -lversion -loleaut32 -luser32 -lgdi32
    }
    #linux-g++||unix:!macx:!android: {
    linux-g++||macx||unix:!android:{
        macx: {
            LIBS += -static -lFLAC -lvorbisfile -lvorbis -logg -lmad #-lfluidsynth
        } else {
            LIBS += -Wl,-Bstatic -l:libFLAC.a -l:libvorbisfile.a -l:libvorbis.a -l:libogg.a -l:libmad.a -Wl,-Bdynamic #-l:libfluidsynth.a
        }
    }
}

!win*-msvc*:{
    LIBS += -lm
}

linux-g++||unix:!macx:!android:{
    SDL2MixerH.path =  $$PWD/../_builds/linux/include/SDL2
}
android: {
    SDL2MixerH.path =  $$PWD/../_builds/android/include/SDL2
}
win32: {
    SDL2MixerH.path =  $$PWD/../_builds/win32/include/SDL2
}
macx: {
    SDL2MixerH.path =  $$PWD/../_builds/macos/include/SDL2
}

SDL2MixerH.files += SDL_mixer_ext.h
INSTALLS += SDL2MixerH

win32: {
    SDL2MixerSO.path = $$PWD/../_builds/win32/bin
    SDL2MixerSO.files += $$PWD/../_builds/win32/lib/*.dll
    INSTALLS += SDL2MixerSO
}

contains(DEFINES, USE_ADL_MIDI):        include($$PWD/play_midi_adl.pri)
contains(DEFINES, USE_OPN2_MIDI):       include($$PWD/play_midi_opn.pri)
contains(DEFINES, USE_TIMIDITY_MIDI):   include($$PWD/timidity/timidity.pri)
contains(DEFINES, USE_NATIVE_MIDI):     include($$PWD/play_midi_native.pri)
contains(DEFINES, USE_FLUIDSYNTH_MIDI): include($$PWD/play_midi_fluid.pri)
contains(DEFINES, OGG_MUSIC):           include($$PWD/play_ogg.pri)
contains(DEFINES, FLAC_MUSIC):          include($$PWD/play_flac.pri)
contains(DEFINES, MOD_MUSIC):           include($$PWD/play_mikmod.pri)
contains(DEFINES, MODPLUG_MUSIC):       include($$PWD/play_modplug.pri)
contains(DEFINES, MP3_MAD_MUSIC):       include($$PWD/play_mp3.pri)
contains(DEFINES, GME_MUSIC):           include($$PWD/play_gme.pri)
contains(DEFINES, CMD_MUSIC):           include($$PWD/play_cmdmusic.pri)

HEADERS += \
    SDL_mixer_ext.h \
    begin_code.h \
    close_code.h \
    effects_internal.h \
    load_aiff.h \
    load_voc.h \
    wavestream.h \
    resample/my_resample.h \
    mixer.h \

SOURCES += \
    effect_position.c \
    effect_stereoreverse.c \
    effects_internal.c \
    load_aiff.c \
    load_voc.c \
    mixer.c \
    music.c \
    wavestream.c \
    resample/my_resample.c
