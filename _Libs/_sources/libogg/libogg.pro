TEMPLATE = lib
CONFIG = staticlib
TARGET = ogg

INCLUDEPATH += $$PWD/include

HEADERS += \
    include/ogg/config_types.h \
    include/ogg/ogg.h \
    include/ogg/os_types.h

SOURCES += \
    src/bitwise.c \
    src/framing.c


