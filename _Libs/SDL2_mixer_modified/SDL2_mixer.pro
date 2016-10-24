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

QMAKE_CFLAGS += -std=c99

QMAKE_CFLAGS += \
    -Wno-missing-field-initializers \
    -Wno-unused-variable \
    -Wno-unused-parameter \
    -Wno-sign-compare \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-pointer-sign

QMAKE_CXXFLAGS += \
    -Wno-missing-field-initializers \
    -Wno-unused-variable \
    -Wno-unused-parameter

macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx:{
    QMAKE_CFLAGS    += -Wno-unused-but-set-variable -Wno-deprecated-register
    QMAKE_CXXFLAGS  += -Wno-unused-but-set-variable -Wno-deprecated-register
    QMAKE_LFLAGS    += -Wl,-rpath=\'\$\$ORIGIN\'
}

include ($$PWD/../../_common/lib_destdir.pri)

static-sdlmixer:{
    TARGET = SDL_Mixer_Xstatic
} else {
    TARGET = SDL2_mixer_ext
    win32:enable-stdcalls:{ #Required for building a VB6-compatible version
        TARGET = SDL2MixerVB
    }
}

include($$PWD/../../_common/build_props.pri)
DESTDIR = $$PWD/../_builds/$$TARGETOS/lib
COPY=cp
win32:{
    LIBS += -L$$PWD/../_builds/win32/lib
    LIBS += -lmingw32 -lSDL2main -mwindows
    INCLUDEPATH += $$PWD/../_builds/win32/include
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
    LIBS += -L$$PWD/../_builds/linux/lib
    INCLUDEPATH += $$PWD/../_builds/linux/include
    CONFIG += plugin
    CONFIG += unversioned_libname
    CONFIG += skip_target_version_ext
    DEFINES += HAVE_INTTYPES_H HAVE_SETENV HAVE_SINF
}
android:{
    LIBS += -L$$PWD/../_builds/android/lib
    INCLUDEPATH += $$PWD/../_builds/android/include
    DEFINES += HAVE_INTTYPES_H HAVE_SETENV HAVE_SINF
}
macx:{
    LIBS += -L$$PWD/../_builds/macos/lib
    INCLUDEPATH += $$PWD/../_builds/macos/include
    INCLUDEPATH += $$PWD/../_builds/macos/frameworks/SDL2.framework/Headers
    LIBS += -F$$PWD/../_builds/macos/frameworks -framework SDL2
    CONFIG += plugin
    DEFINES += HAVE_INTTYPES_H HAVE_SETENV HAVE_SINF
} else {
    !win32: LIBS += -lSDL2
}

win32:{
    LIBS += -lwinmm -lm -lwinmm
}

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
    OGG_MUSIC \
    FLAC_MUSIC \
    MP3_MAD_MUSIC \
    GME_MUSIC \
    SPC_MORE_ACCURACY \#NO_OLDNAMES
    MODPLUG_MUSIC \
    MODPLUG_STATIC \
    MODPLUG_BUILD=1 \
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

!android:{
    win32:{
        DEFINES -= UNICODE _UNICODE
        enable-stdcalls: {
            LIBS += -static -l:libSDL2.a -l:libFLAC.a -l:libvorbisfile.a -l:libvorbis.a -l:libogg.a -l:libmad.a -static-libgcc -static-libstdc++ -static -lpthread -luuid # -l:libfluidsynth.a
        } else {
            LIBS += -lSDL2.dll -l:libFLAC.a -l:libvorbisfile.a -l:libvorbis.a -l:libogg.a -l:libmad.a #-l:libfluidsynth.a
        }
        LIBS += -lwinmm -lole32 -limm32 -lversion -loleaut32
    }
    #linux-g++||unix:!macx:!android: {
    linux-g++||macx||unix:!android:{
        macx: {
        LIBS += -static -lFLAC -lvorbisfile -lvorbis -logg -lmad #-lfluidsynth
        } else {
        LIBS += -Wl,-Bstatic -l:libFLAC.a -l:libvorbisfile.a -l:libvorbis.a -l:libogg.a -l:libmad.a -Wl,-Bdynamic #-l:libfluidsynth.a
        }
    }
} else {
    LIBS += -lvorbisfile -lvorbis -logg #-lvorbisidec
}

LIBS += -lm

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
    SDL2MixerH.path =  $$PWD/../_builds/macos/frameworks/SDL2.framework/Headers/SDL2
}
SDL2MixerH.files += SDL_mixer_ext.h

linux-g++||unix:!macx:!android:{
    SDL2MixerSO.path = $$PWD/../_builds/linux/lib
    SDL2MixerSO.files += $$PWD/../_builds/sdl2_mixer_mod/*.so*
    INSTALLS =  SDL2MixerSO
}
macx:{
    SDL2MixerSO.path = $$PWD/../_builds/macos/lib
    SDL2MixerSO.files += $$PWD/../_builds/sdl2_mixer_mod/*.dylib*
}
android:{
    SDL2MixerSO.path = $$PWD/../_builds/android/lib
    SDL2MixerSO.files += $$PWD/../_builds/sdl2_mixer_mod/*.so*
    INSTALLS =  SDL2MixerSO
}
win32: {
    SDL2MixerSO.path = $$PWD/../_builds/win32/bin
    SDL2MixerSO.files += $$PWD/../_builds/win32/lib/*.dll
    INSTALLS =  SDL2MixerSO
}
INSTALLS = SDL2MixerH SDL2MixerSO

HEADERS += \
    SDL_mixer_ext.h \
    begin_code.h \
    close_code.h \
    dynamic_flac.h \
    dynamic_fluidsynth.h \
    dynamic_mod.h \
    dynamic_modplug.h \
    dynamic_mp3.h \
    dynamic_ogg.h \
    effects_internal.h \
    load_aiff.h \
    load_flac.h \
    load_mp3.h \
    load_ogg.h \
    load_voc.h \
    music_cmd.h \
    music_flac.h \
    music_mad.h \
    music_mod.h \
    music_modplug.h \
    music_ogg.h \
    wavestream.h \
    native_midi/native_midi.h \
    native_midi/native_midi_common.h \
    timidity/common.h \
    timidity/config.h \
    timidity/ctrlmode.h \
    timidity/filter.h \
    timidity/instrum.h \
    timidity/mix.h \
    timidity/output.h \
    timidity/playmidi.h \
    timidity/readmidi.h \
    timidity/resample.h \
    timidity/tables.h \
    timidity/timidity.h \
    libid3tag/compat.h \
    libid3tag/crc.h \
    libid3tag/debug.h \
    libid3tag/field.h \
    libid3tag/file.h \
    libid3tag/frame.h \
    libid3tag/frametype.h \
    libid3tag/genre.h \
    libid3tag/global.h \
    libid3tag/id3tag.h \
    libid3tag/latin1.h \
    libid3tag/parse.h \
    libid3tag/render.h \
    libid3tag/tag.h \
    libid3tag/ucs4.h \
    libid3tag/utf8.h \
    libid3tag/utf16.h \
    libid3tag/util.h \
    libid3tag/version.h \
    gme/Ay_Apu.h \
    gme/Ay_Cpu.h \
    gme/Ay_Emu.h \
    gme/blargg_common.h \
    gme/blargg_config.h \
    gme/blargg_endian.h \
    gme/blargg_source.h \
    gme/Blip_Buffer.h \
    gme/Classic_Emu.h \
    gme/Data_Reader.h \
    gme/Dual_Resampler.h \
    gme/Effects_Buffer.h \
    gme/Fir_Resampler.h \
    gme/Gb_Apu.h \
    gme/Gb_Cpu.h \
    gme/gb_cpu_io.h \
    gme/Gb_Oscs.h \
    gme/Gbs_Emu.h \
    gme/gme.h \
    gme/Gme_File.h \
    gme/gme_types.h \
    gme/gme_types.h.in \
    gme/Gym_Emu.h \
    gme/Hes_Apu.h \
    gme/Hes_Cpu.h \
    gme/hes_cpu_io.h \
    gme/Hes_Emu.h \
    gme/Kss_Cpu.h \
    gme/Kss_Emu.h \
    gme/Kss_Scc_Apu.h \
    gme/M3u_Playlist.h \
    gme/Multi_Buffer.h \
    gme/Music_Emu.h \
    gme/Nes_Apu.h \
    gme/Nes_Cpu.h \
    gme/nes_cpu_io.h \
    gme/Nes_Fme7_Apu.h \
    gme/Nes_Namco_Apu.h \
    gme/Nes_Oscs.h \
    gme/Nes_Vrc6_Apu.h \
    gme/Nsfe_Emu.h \
    gme/Nsf_Emu.h \
    gme/Sap_Apu.h \
    gme/Sap_Cpu.h \
    gme/sap_cpu_io.h \
    gme/Sap_Emu.h \
    gme/Sms_Apu.h \
    gme/Sms_Oscs.h \
    gme/Snes_Spc.h \
    gme/Spc_Cpu.h \
    gme/Spc_Dsp.h \
    gme/Spc_Emu.h \
    gme/Spc_Filter.h \
    gme/Vgm_Emu.h \
    gme/Vgm_Emu_Impl.h \
    gme/Ym2413_Emu.h \
    gme/Ym2612_Emu.h \
    music_gme.h \
    ADLMIDI/adldata.hh \
    ADLMIDI/adlmidi.h \
    ADLMIDI/dbopl.h \
    ADLMIDI/fraction.h \
    music_midi_adl.h \
    gme/ZLib/crc32.h \
    gme/ZLib/deflate.h \
    gme/ZLib/gzguts.h \
    gme/ZLib/inffast.h \
    gme/ZLib/inffixed.h \
    gme/ZLib/inflate.h \
    gme/ZLib/inftrees.h \
    gme/ZLib/trees.h \
    gme/ZLib/zconf.h \
    gme/ZLib/zlib.h \
    gme/ZLib/zutil.h \
    modplug/config.h \
    modplug/it_defs.h \
    modplug/load_pat.h \
    modplug/modplug.h \
    modplug/sndfile.h \
    modplug/stdafx.h \
    modplug/tables.h \
    resample/my_resample.h \
    gme/GZipHelper.h \
    mixer.h \
    music_fluidsynth.h

SOURCES += \
    dynamic_flac.c \
    dynamic_fluidsynth.c \
    dynamic_modplug.c \
    dynamic_mp3.c \
    dynamic_ogg.c \
    effect_position.c \
    effect_stereoreverse.c \
    effects_internal.c \
    load_aiff.c \
    load_flac.c \
    load_mp3.c \
    load_ogg.c \
    load_voc.c \
    mixer.c \
    music.c \
    music_cmd.c \
    music_flac.c \
    music_mad.c \
    music_mod.c \
    music_modplug.c \
    music_ogg.c \
    wavestream.c \
    native_midi/native_midi_haiku.cpp \
    native_midi/native_midi_common.c \
    native_midi/native_midi_mac.c \
    native_midi/native_midi_macosx.c \
    native_midi/native_midi_win32.c \
    timidity/common.c \
    timidity/filter.c \
    timidity/instrum.c \
    timidity/mix.c \
    timidity/output.c \
    timidity/playmidi.c \
    timidity/readmidi.c \
    timidity/sdl_a.c \
    timidity/sdl_c.c \
    timidity/tables.c \
    timidity/timidity.c \
    timidity/ctrlmode.c \
    dynamic_mod.c \
    libid3tag/compat.c \
    libid3tag/crc.c \
    libid3tag/debug.c \
    libid3tag/field.c \
    libid3tag/file.c \
    libid3tag/frame.c \
    libid3tag/frametype.c \
    libid3tag/genre.c \
    libid3tag/latin1.c \
    libid3tag/parse.c \
    libid3tag/render.c \
    libid3tag/tag.c \
    libid3tag/ucs4.c \
    libid3tag/utf8.c \
    libid3tag/utf16.c \
    libid3tag/util.c \
    libid3tag/version.c \
    gme/Ay_Apu.cpp \
    gme/Ay_Cpu.cpp \
    gme/Ay_Emu.cpp \
    gme/Blip_Buffer.cpp \
    gme/Classic_Emu.cpp \
    gme/Data_Reader.cpp \
    gme/Dual_Resampler.cpp \
    gme/Effects_Buffer.cpp \
    gme/Fir_Resampler.cpp \
    gme/Gb_Apu.cpp \
    gme/Gb_Cpu.cpp \
    gme/Gb_Oscs.cpp \
    gme/Gbs_Emu.cpp \
    gme/gme.cpp \
    gme/Gme_File.cpp \
    gme/Gym_Emu.cpp \
    gme/Hes_Apu.cpp \
    gme/Hes_Cpu.cpp \
    gme/Hes_Emu.cpp \
    gme/Kss_Cpu.cpp \
    gme/Kss_Emu.cpp \
    gme/Kss_Scc_Apu.cpp \
    gme/M3u_Playlist.cpp \
    gme/Multi_Buffer.cpp \
    gme/Music_Emu.cpp \
    gme/Nes_Apu.cpp \
    gme/Nes_Cpu.cpp \
    gme/Nes_Fme7_Apu.cpp \
    gme/Nes_Namco_Apu.cpp \
    gme/Nes_Oscs.cpp \
    gme/Nes_Vrc6_Apu.cpp \
    gme/Nsfe_Emu.cpp \
    gme/Nsf_Emu.cpp \
    gme/Sap_Apu.cpp \
    gme/Sap_Cpu.cpp \
    gme/Sap_Emu.cpp \
    gme/Sms_Apu.cpp \
    gme/Snes_Spc.cpp \
    gme/Spc_Cpu.cpp \
    gme/Spc_Dsp.cpp \
    gme/Spc_Emu.cpp \
    gme/Spc_Filter.cpp \
    gme/Vgm_Emu.cpp \
    gme/Vgm_Emu_Impl.cpp \
    gme/Ym2413_Emu.cpp \
    gme/Ym2612_Emu.cpp \
    music_gme.c \
    ADLMIDI/dbopl.cpp \
    music_midi_adl.c \
    ADLMIDI/adlmidi.cpp \
    ADLMIDI/adldata.cpp \
    gme/ZLib/adler32.c \
    gme/ZLib/compress.c \
    gme/ZLib/crc32.c \
    gme/ZLib/deflate.c \
    gme/ZLib/gzclose.c \
    gme/ZLib/gzlib.c \
    gme/ZLib/gzread.c \
    gme/ZLib/gzwrite.c \
    gme/ZLib/infback.c \
    gme/ZLib/inffast.c \
    gme/ZLib/inflate.c \
    gme/ZLib/inftrees.c \
    gme/ZLib/trees.c \
    gme/ZLib/uncompr.c \
    gme/ZLib/zutil.c \
    modplug/fastmix.cpp \
    modplug/load_669.cpp \
    modplug/load_abc.cpp \
    modplug/load_amf.cpp \
    modplug/load_ams.cpp \
    modplug/load_dbm.cpp \
    modplug/load_dmf.cpp \
    modplug/load_dsm.cpp \
    modplug/load_far.cpp \
    modplug/load_it.cpp \
    modplug/load_j2b.cpp \
    modplug/load_mdl.cpp \
    modplug/load_med.cpp \
    modplug/load_mid.cpp \
    modplug/load_mod.cpp \
    modplug/load_mt2.cpp \
    modplug/load_mtm.cpp \
    modplug/load_okt.cpp \
    modplug/load_pat.cpp \
    modplug/load_psm.cpp \
    modplug/load_ptm.cpp \
    modplug/load_s3m.cpp \
    modplug/load_stm.cpp \
    modplug/load_ult.cpp \
    modplug/load_umx.cpp \
    modplug/load_wav.cpp \
    modplug/load_xm.cpp \
    modplug/mmcmp.cpp \
    modplug/modplug.cpp \
    modplug/snd_dsp.cpp \
    modplug/snd_flt.cpp \
    modplug/snd_fx.cpp \
    modplug/sndfile.cpp \
    modplug/sndmix.cpp \
    vb6_sdl_binds.c \
    timidity/resample_timidity.c \
    resample/my_resample.c \
    music_fluidsynth.c


