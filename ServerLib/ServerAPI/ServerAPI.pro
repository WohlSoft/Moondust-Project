#-------------------------------------------------
#
# Project created by QtCreator 2015-04-12T18:04:18
#
#-------------------------------------------------

QT       += gui
QT       += widgets
QT       += network

TARGET = ServerAPI
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++11

DEFINES += LIBPGEServerAPI

DESTDIR = $$PWD/../../_Libs/_builds/commonlibs

android:{
    ARCH=android_arm
} else {
    !contains(QMAKE_TARGET.arch, x86_64) {
    ARCH=x32
    } else {
    ARCH=x64
    }
}
static: {
LINKTYPE=static
} else {
LINKTYPE=dynamic
}
debug: BUILDTP=debug
release: BUILDTP=release
OBJECTS_DIR = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.obj
MOC_DIR     = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.moc
RCC_DIR     = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.rcc
UI_DIR      = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.ui
message("$$TARGET will be built as $$BUILDTP $$ARCH ($$QMAKE_TARGET.arch) $${LINKTYPE}ally in $$OBJECTS_DIR")


SOURCES += \
    packetV2/packets/packetcursor.cpp

HEADERS += \
    packetV2/pgepacketregister.h \
    packetV2/pgepacketgloabls.h \
    packetV2/packets/packet.h \
    packetV2/packets/packetcursor.h

OTHER_FILES += \
    PacketInfo.txt


