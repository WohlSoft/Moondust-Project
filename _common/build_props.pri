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
    BUILD_OBJ_DIR=$$PWD/../bin
} else {
    BUILD_OBJ_DIR=$$DESTDIR
}

debug: BUILDTP=debug
release: BUILDTP=release
OBJECTS_DIR = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.obj
MOC_DIR     = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.moc
RCC_DIR     = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.rcc
UI_DIR      = $$BUILD_OBJ_DIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.ui
message("$$TARGET will be built as $$BUILDTP $$ARCH ($$QMAKE_TARGET.arch) $${LINKTYPE}ally in $$OBJECTS_DIR")

