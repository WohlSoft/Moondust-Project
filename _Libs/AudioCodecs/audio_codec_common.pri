# ========= library Intsalling properties ============
include($$PWD/../../_common/lib_destdir.pri)
include($$PWD/../../_common/build_props.pri)
DESTDIR = $$PWD/../_builds/$$TARGETOS/lib
# ================ Includes to install ===============
LibIncludes.path =  $$PWD/../_builds/$$TARGETOS/include/$$INSTALLINCLUDESTO
LibIncludes.files += $$INSTALLINCLUDES
INSTALLS += LibIncludes
# ====================================================

release: DEFINES += NDEBUG
!win32:{
    QMAKE_CFLAGS += -fPIC
} else {
    QMAKE_CFLAGS -= -fPIC
}
