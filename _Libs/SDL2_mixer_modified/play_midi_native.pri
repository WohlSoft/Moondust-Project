SOURCES += $$PWD/native_midi/native_midi_common.c

win32:
{
    SOURCES += \
        $$PWD/native_midi/native_midi_win32.c
}

macx:
{
    SOURCES += \
        $$PWD/native_midi/native_midi_mac.c \
        $$PWD/native_midi/native_midi_macosx.c
}

SOURCES += \
    $$PWD/native_midi/native_midi_haiku.cpp

HEADERS += \
    $$PWD/native_midi/native_midi.h \
    $$PWD/native_midi/native_midi_common.h

