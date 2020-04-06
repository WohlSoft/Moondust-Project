TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lSDL2

DESTDIR = $$PWD/bin

DEFINES += IMGSIZE_DEBUG

INCLUDEPATH += ../../../_common/

SOURCES += \
        ../../../_common/ConfigPackManager/image_size.cpp \
        ../../../_common/Utils/files.cpp \
        main.cpp

HEADERS += \
    ../../../_common/ConfigPackManager/image_size.h \
    ../../../_common/Utils/files.h
