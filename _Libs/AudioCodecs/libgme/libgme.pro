#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = gme
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = gme
include($$PWD/../audio_codec_common.pri)

INCLUDEPATH += $$PWD $$PWD/include $$PWD/../zlib/include

!win*-msvc*:{
    QMAKE_CXXFLAGS_WARN_ON  += \
        -Wno-implicit-fallthrough
} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON   += /wd4100 /wd4244 /wd4005 /wd4013 /wd4047 /wd4996
    QMAKE_CXXFLAGS_WARN_ON += /wd4100 /wd4101 /wd4244 /wd4800 /wd4104 /wd4146
}

DEFINES += \
    SPC_MORE_ACCURACY \
    _REENTRANT \
    _USE_MATH_DEFINES

LIBS += -lzlib

HEADERS += \
    include/gme.h \
    Ay_Apu.h \
    Ay_Cpu.h \
    Ay_Emu.h \
    blargg_common.h \
    blargg_config.h \
    blargg_endian.h \
    blargg_source.h \
    Blip_Buffer.h \
    Classic_Emu.h \
    Data_Reader.h \
    Dual_Resampler.h \
    Effects_Buffer.h \
    Fir_Resampler.h \
    Gb_Apu.h \
    Gb_Cpu.h \
    gb_cpu_io.h \
    Gb_Oscs.h \
    Gbs_Emu.h \
    Gme_File.h \
    gme_types.h \
    Gym_Emu.h \
    GZipHelper.h \
    Hes_Apu.h \
    Hes_Cpu.h \
    hes_cpu_io.h \
    Hes_Emu.h \
    Kss_Cpu.h \
    Kss_Emu.h \
    Kss_Scc_Apu.h \
    M3u_Playlist.h \
    Multi_Buffer.h \
    Music_Emu.h \
    Nes_Apu.h \
    Nes_Cpu.h \
    nes_cpu_io.h \
    Nes_Fme7_Apu.h \
    Nes_Namco_Apu.h \
    Nes_Oscs.h \
    Nes_Vrc6_Apu.h \
    Nsfe_Emu.h \
    Nsf_Emu.h \
    Sap_Apu.h \
    Sap_Cpu.h \
    sap_cpu_io.h \
    Sap_Emu.h \
    Sms_Apu.h \
    Sms_Oscs.h \
    Snes_Spc.h \
    Spc_Cpu.h \
    Spc_Dsp.h \
    Spc_Emu.h \
    Spc_Filter.h \
    Vgm_Emu.h \
    Vgm_Emu_Impl.h \
    Ym2413_Emu.h \
    Ym2612_Emu.h

SOURCES += \
    Ay_Apu.cpp \
    Ay_Cpu.cpp \
    Ay_Emu.cpp \
    Blip_Buffer.cpp \
    Classic_Emu.cpp \
    Data_Reader.cpp \
    Dual_Resampler.cpp \
    Effects_Buffer.cpp \
    Fir_Resampler.cpp \
    Gb_Apu.cpp \
    Gb_Cpu.cpp \
    Gb_Oscs.cpp \
    Gbs_Emu.cpp \
    gme.cpp \
    Gme_File.cpp \
    Gym_Emu.cpp \
    Hes_Apu.cpp \
    Hes_Cpu.cpp \
    Hes_Emu.cpp \
    Kss_Cpu.cpp \
    Kss_Emu.cpp \
    Kss_Scc_Apu.cpp \
    M3u_Playlist.cpp \
    Multi_Buffer.cpp \
    Music_Emu.cpp \
    Nes_Apu.cpp \
    Nes_Cpu.cpp \
    Nes_Fme7_Apu.cpp \
    Nes_Namco_Apu.cpp \
    Nes_Oscs.cpp \
    Nes_Vrc6_Apu.cpp \
    Nsfe_Emu.cpp \
    Nsf_Emu.cpp \
    Sap_Apu.cpp \
    Sap_Cpu.cpp \
    Sap_Emu.cpp \
    Sms_Apu.cpp \
    Snes_Spc.cpp \
    Spc_Cpu.cpp \
    Spc_Dsp.cpp \
    Spc_Emu.cpp \
    Spc_Filter.cpp \
    Vgm_Emu.cpp \
    Vgm_Emu_Impl.cpp \
    Ym2413_Emu.cpp \
    Ym2612_Emu.cpp

