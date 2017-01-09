INCLUDEPATH += $$PWD/

win32:{
    SOURCES += $$PWD/dirman_winapi.cpp
} else {
    SOURCES += $$PWD/dirman_posix.cpp
}

SOURCES += \
    $$PWD/dirman.cpp

HEADERS += \
    $$PWD/dirman.h \
    $$PWD/dirman_private.h
