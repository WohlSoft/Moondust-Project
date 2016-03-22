lessThan(QT_MAJOR_VERSION, 6):{
lessThan(QT_MAJOR_VERSION, 5): error("You are using an old version of Qt. To build project need a Qt 5.2 and greater!")
lessThan(QT_MINOR_VERSION, 2): error("You are using an old version of Qt. To build project need a Qt 5.2 and greater!")
} else {
    warning("Building under Qt 6 and greater may be incompatible with current project Please use Qt 5.4 before project will be ported to Qt6 or greater!")
}

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

contains(TEMPLATE, lib) {
	win32: {
	BUILD_OBJ_DIR=$$PWD/../bin-w32
	} else {
	BUILD_OBJ_DIR=$$PWD/../bin
	}
} else {
    BUILD_OBJ_DIR=$$DESTDIR
}

GIT_VERSION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD/../ describe --always)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

debug: {
BUILDTP=debug
DEFINES += DEBUG_BUILD=1
}
release: BUILDTP=release
OBJECTS_DIR = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.obj
MOC_DIR     = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.moc
RCC_DIR     = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.rcc
UI_DIR      = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.ui
message("$$TARGET will be built as $$BUILDTP $$ARCH $$TARGETOS ($$QMAKE_TARGET.arch) $${LINKTYPE}ally in $$OBJECTS_DIR, GIT version $$GIT_VERSION")

useccache: {
#To speed-up building process http://www.ysbl.york.ac.uk/~lohkamp/speedup_compilation.html
QMAKE_CC = ccache gcc
QMAKE_CXX = ccache g++
QMAKE_LINK = g++
QMAKE_LINK_SHLIB = g++
QMAKE_LINK_C = gcc
QMAKE_LINK_C_SHLIB = gcc
message("CHACHE with GCC will be used")
}
