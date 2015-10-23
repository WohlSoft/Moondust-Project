TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += dll
CONFIG -= static

QMAKE_CFLAGS += -std=c99

QMAKE_CFLAGS += -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-sign-compare -Wno-unused-function -Wno-implicit-function-declaration -Wno-pointer-sign
QMAKE_CXXFLAGS += -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable
macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx:{
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
}

include (../../_common/lib_destdir.pri)
DESTDIR = ../_builds/$$TARGETOS/lib
TARGET = SDL2_mixer_ext

include(../../_common/build_props.pri)

win32:{
    LIBS += -L../_builds/win32/lib
    LIBS += -lmingw32 -lSDL2main -mwindows
    INCLUDEPATH += ../_builds/win32/include
    DEFINES -= USE_NATIVE_MIDI
}
linux-g++||unix:!macx:!android:{
    LIBS += -L../_builds/linux/lib
    INCLUDEPATH += ../_builds/linux/include
    CONFIG += unversioned_libname
}
android:{
    LIBS += -L../_builds/android/lib
    INCLUDEPATH += ../_builds/android/include
}
macx:{
    LIBS += -L../_builds/macos/lib
    INCLUDEPATH += ../_builds/macos/include
    LIBS += -F../_builds/macos/frameworks -framework SDL2
    INCLUDEPATH += ../_builds/macos/frameworks/SDL2.framework/Headers
} else {
    LIBS += -lSDL2
}

win32:{
    LIBS += -lwinmm -lm -lwinmm
}

DEFINES += main=SDL_main HAVE_SIGNAL_H HAVE_SETBUF WAV_MUSIC MID_MUSIC \
USE_TIMIDITY_MIDI USE_ADL_MIDI OGG_MUSIC FLAC_MUSIC MP3_MAD_MUSIC GME_MUSIC NO_OLDNAMES SPC_MORE_ACCURACY
DEFINES += MODPLUG_MUSIC

android: {
DEFINES += HAVE_STRCASECMP HAVE_STRNCASECMP #OGG_USE_TREMOR
DEFINES -= FLAC_MUSIC #temopary with no FLAC, because I wasn't built it because compilation bug
}

LIBS += -L../_builds/$$TARGETOS/lib

!android:{
    win32:{
        LIBS += -lvorbisfile.dll -lvorbis.dll -lmodplug.dll -lFLAC.dll -logg.dll -static-libgcc -static-libstdc++ -static -lpthread
    } else {
        LIBS += -lvorbisfile -lvorbis -lmodplug -lFLAC -logg
    }
} else {
    LIBS += -lvorbisfile -lvorbis -lmodplug -logg #-lvorbisidec
}

LIBS += -lmad -lm

linux-g++||unix:!macx:!android:{
    SDL2MixerH.path =  ../_builds/linux/include/SDL2
}
android: {
    SDL2MixerH.path =  ../_builds/android/include/SDL2
}
win32: {
    SDL2MixerH.path =  ../_builds/win32/include/SDL2
}
macx: {
    SDL2MixerH.path =  ../_builds/macos/frameworks/SDL2.framework/Headers/SDL2
}
SDL2MixerH.files += SDL_mixer_ext.h

linux-g++||unix:!macx:!android:{
    SDL2MixerSO.path = ../_builds/linux/lib
    SDL2MixerSO.files += ../_builds/sdl2_mixer_mod/*.so*
    INSTALLS =  SDL2MixerSO
}
android:{
    SDL2MixerSO.path = ../_builds/android/lib
    SDL2MixerSO.files += ../_builds/sdl2_mixer_mod/*.so*
    INSTALLS =  SDL2MixerSO
}
win32: {
    SDL2MixerSO.path = ../_builds/win32/bin
    SDL2MixerSO.files += ../_builds/win32/lib/*.dll
    INSTALLS =  SDL2MixerSO
}
INSTALLS = SDL2MixerH SDL2MixerSO



HEADERS += \
    begin_code.h \
    close_code.h \
    dynamic_flac.h \
    dynamic_fluidsynth.h \
    dynamic_mod.h \
    dynamic_modplug.h \
    dynamic_mp3.h \
    dynamic_ogg.h \
    effects_internal.h \
    fluidsynth.h \
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
    resample/audio.h \
    resample/global.h \
    resample/mad_resample.h \
    SDL_mixer_ext.h \
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
    music_midi_adl.h

SOURCES += \
    dynamic_flac.c \
    dynamic_fluidsynth.c \
    dynamic_modplug.c \
    dynamic_mp3.c \
    dynamic_ogg.c \
    effect_position.c \
    effect_stereoreverse.c \
    effects_internal.c \
    fluidsynth.c \
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
    timidity/resample.c \
    timidity/sdl_a.c \
    timidity/sdl_c.c \
    timidity/tables.c \
    timidity/timidity.c \
    resample/mad_resample.c \
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
    ADLMIDI/adldata.cpp

