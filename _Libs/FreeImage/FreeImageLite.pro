TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG -= dll
CONFIG += staticlib
CONFIG += c++11

DEFINES += VER_MAJOR=3 VER_MINOR=17.0 FREEIMAGE_LITE

QMAKE_CFLAGS += -std=c11

QMAKE_CFLAGS_WARN_ON = -Wall \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-sign-compare -Wno-unused-function -Wno-implicit-function-declaration -Wno-pointer-sign \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-sign-compare -Wno-unused-value \
                -Wno-type-limits


QMAKE_CXXFLAGS_WARN_ON = -Wall \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-sign-compare -Wno-unused-function \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter  \
                -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-unused-value \
                -Wno-type-limits -Wno-reorder

macx:{
    QMAKE_CFLAGS_WARN_ON    += -Wno-unused-const-variable -Wno-uninitialized
    QMAKE_CXXFLAGS_WARN_ON  += -Wno-unused-const-variable -Wno-uninitialized -Wno-header-guard
} else {
    QMAKE_CFLAGS_WARN_ON    += -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-old-style-declaration
    QMAKE_CXXFLAGS_WARN_ON  += -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-clobbered -Wno-old-style-declaration
    QMAKE_LFLAGS    += -Wl,-rpath=\'\$\$ORIGIN\'
}

include($$PWD/../../_common/strip_garbage.pri)
include($$PWD/../../_common/lib_destdir.pri)
TARGET = freeimagelite
include($$PWD/../../_common/build_props.pri)
DESTDIR = $$PWD/../_builds/$$TARGETOS/lib

MAKEFILE = Makefile.FreeImageLITE

RC_FILE = FreeImage.rc

INCLUDEPATH += $$PWD/Source

TRANSLATIONS = ""

win32:{
    LIBS        += -L$$PWD/../_builds/win32/lib #-lws2_32
    INCLUDEPATH += $$PWD/../_builds/win32/include
    DEFINES     += OPJ_STATIC LIBRAW_NODLL FREEIMAGE_LIB=1 #__ANSI__ DISABLE_PERF_MEASUREMENT
}
linux-g++||unix:!macx:!android:{
    LIBS        += -L$$PWD/../_builds/linux/lib
    INCLUDEPATH += $$PWD/../_builds/linux/include
    CONFIG      += unversioned_libname
    DEFINES     += FREEIMAGE_LIB=1
}
android:{
    LIBS += -L$$PWD/../_builds/android/lib
    INCLUDEPATH += $$PWD/../_builds/android/include
    warning("NOT PORTED YET!")
}
macx:{
    LIBS += -L$$PWD/../_builds/macos/lib
    INCLUDEPATH += $$PWD/../_builds/macos/include
    QMAKE_CFLAGS    += -fexceptions -fvisibility=hidden
    QMAKE_CXXFLAGS  += -fexceptions -fvisibility=hidden -Wno-ctor-dtor-privacy -stdlib=libc++ -Wc++11-narrowing
    QMAKE_CFLAGS_RELEASE    += -Os
    QMAKE_CXXFLAGS_RELEASE  += -Os
    DEFINES += NO_LCMS __ANSI__ DISABLE_PERF_MEASUREMENT
}

LIBS += -L$$PWD/../_builds/$$TARGETOS/lib

FreeImageH.path =  $$PWD/../_builds/$$TARGETOS/include
FreeImageH.files += Source/FreeImageLite.h

linux-g++||unix:!macx:!android:{
    FreeImageSO.path = $$PWD/../_builds/linux/lib
    FreeImageSO.files += $$PWD/../_builds/sdl2_mixer_mod/*.so*
    INSTALLS =  FreeImageSO
}
macx:{
    FreeImageSO.path = $$PWD/../_builds/macos/lib
    FreeImageSO.files += $$PWD/../_builds/sdl2_mixer_mod/*.dylib*
}
android:{
    FreeImageSO.path = $$PWD/../_builds/android/lib
    FreeImageSO.files += $$PWD/../_builds/sdl2_mixer_mod/*.so*
    INSTALLS =  FreeImageSO
}
win32: {
    FreeImageSO.path = $$PWD/../_builds/win32/bin
    FreeImageSO.files += $$PWD/../_builds/win32/lib/*.dll
    INSTALLS =  FreeImageSO
}
INSTALLS = FreeImageH FreeImageSO

INCLUDEPATH +=  $$PWD/Source

#INCLUDEPATH +=  $$PWD/Source \
#                $$PWD/Source/Metadata \
#                $$PWD/Source/FreeImageToolkit \
#                $$PWD/Source/LibPNG \
#                $$PWD/Source/ZLib \
#                $$PWD/Source/OpenEXR \
#                $$PWD/Source/OpenEXR/Half \
#                $$PWD/Source/OpenEXR/Iex \
#                $$PWD/Source/OpenEXR/IlmImf \
#                $$PWD/Source/OpenEXR/IlmThread \
#                $$PWD/Source/OpenEXR/Imath \
#                $$PWD/Source/OpenEXR/IexMath \
#                $$PWD/Source/LibRawLite \
#                $$PWD/Source/LibRawLite/dcraw \
#                $$PWD/Source/LibRawLite/internal \
#                $$PWD/Source/LibRawLite/libraw \
#                $$PWD/Source/LibRawLite/src \
#                $$PWD/Source/LibWebP \
#                $$PWD/Source/LibJXR \
#                $$PWD/Source/LibJXR/common/include \
#                $$PWD/Source/LibJXR/image/sys \
#                $$PWD/Source/LibJXR/jxrgluelib

HEADERS += \
    Source/DeprecationManager/DeprecationMgr.h \
    Source/FreeImageToolkit/Filters.h \
    Source/FreeImageToolkit/Resize.h \
    Source/LibPNG/png.h \
    Source/LibPNG/pngconf.h \
    Source/LibPNG/pngdebug.h \
    Source/LibPNG/pnginfo.h \
    Source/LibPNG/pnglibconf.h \
    Source/LibPNG/pngpriv.h \
    Source/LibPNG/pngstruct.h \
    Source/Metadata/FIRational.h \
    Source/Metadata/FreeImageTag.h \
    Source/ZLib/crc32.h \
    Source/ZLib/deflate.h \
    Source/ZLib/gzguts.h \
    Source/ZLib/inffast.h \
    Source/ZLib/inffixed.h \
    Source/ZLib/inflate.h \
    Source/ZLib/inftrees.h \
    Source/ZLib/trees.h \
    Source/ZLib/zconf.h \
    Source/ZLib/zlib.h \
    Source/ZLib/zutil.h \
    Source/CacheFile.h \
    Source/FreeImageIO.h \
    Source/MapIntrospector.h \
    Source/Plugin.h \
    Source/Quantizers.h \
    Source/ToneMapping.h \
    Source/Utilities.h \
    Source/FreeImage.h \
    Source/FreeImageLite.h \
    Source/FreeImage/FreeImage_misc.h

SOURCES += \
    Source/FreeImage/BitmapAccess.cpp \
    Source/FreeImage/ColorLookup.cpp \
    Source/FreeImage/FreeImage.cpp \
    Source/FreeImage/FreeImageIO.cpp \
    Source/FreeImage/GetType.cpp \
    Source/FreeImage/MemoryIO.cpp \
    Source/FreeImage/PixelAccess.cpp \
    Source/FreeImage/Plugin.cpp \
    Source/FreeImage/PluginBMP.cpp \
    Source/FreeImage/PluginGIF.cpp \
    Source/FreeImage/PluginICO.cpp \
    Source/FreeImage/PluginPNG.cpp \
    Source/FreeImage/Conversion.cpp \
    Source/FreeImage/Conversion16_555.cpp \
    Source/FreeImage/Conversion16_565.cpp \
    Source/FreeImage/Conversion24.cpp \
    Source/FreeImage/Conversion32.cpp \
    Source/FreeImage/Conversion4.cpp \
    Source/FreeImage/Conversion8.cpp \
    Source/FreeImage/ConversionFloat.cpp \
    Source/FreeImage/ConversionRGB16.cpp \
    Source/FreeImage/ConversionRGBA16.cpp \
    Source/FreeImage/ConversionRGBAF.cpp \
    Source/FreeImage/ConversionRGBF.cpp \
    Source/FreeImage/ConversionType.cpp \
    Source/FreeImage/ConversionUINT16.cpp \
    Source/FreeImage/Halftoning.cpp \
    Source/FreeImage/tmoColorConvert.cpp \
    Source/FreeImage/tmoDrago03.cpp \
    Source/FreeImage/tmoFattal02.cpp \
    Source/FreeImage/tmoReinhard05.cpp \
    Source/FreeImage/ToneMapping.cpp \
    Source/FreeImage/LFPQuantizer.cpp \
    Source/FreeImage/NNQuantizer.cpp \
    Source/FreeImage/WuQuantizer.cpp \
    Source/DeprecationManager/Deprecated.cpp \
    Source/DeprecationManager/DeprecationMgr.cpp \
    Source/FreeImage/CacheFile.cpp \
    Source/FreeImage/MultiPage.cpp \
    Source/FreeImage/ZLibInterface.cpp \
    Source/Metadata/FIRational.cpp \
    Source/Metadata/FreeImageTag.cpp \
    Source/Metadata/IPTC.cpp \
    Source/Metadata/TagConversion.cpp \
    Source/Metadata/TagLib.cpp \
    Source/FreeImageToolkit/Background.cpp \
    Source/FreeImageToolkit/BSplineRotate.cpp \
    Source/FreeImageToolkit/Channels.cpp \
    Source/FreeImageToolkit/ClassicRotate.cpp \
    Source/FreeImageToolkit/Colors.cpp \
    Source/FreeImageToolkit/CopyPaste.cpp \
    Source/FreeImageToolkit/Display.cpp \
    Source/FreeImageToolkit/Flip.cpp \
    Source/FreeImageToolkit/MultigridPoissonSolver.cpp \
    Source/FreeImageToolkit/Rescale.cpp \
    Source/FreeImageToolkit/Resize.cpp \
    Source/LibPNG/png.c \
    Source/LibPNG/pngerror.c \
    Source/LibPNG/pngget.c \
    Source/LibPNG/pngmem.c \
    Source/LibPNG/pngpread.c \
    Source/LibPNG/pngread.c \
    Source/LibPNG/pngrio.c \
    Source/LibPNG/pngrtran.c \
    Source/LibPNG/pngrutil.c \
    Source/LibPNG/pngset.c \
    Source/LibPNG/pngtrans.c \
    Source/LibPNG/pngwio.c \
    Source/LibPNG/pngwrite.c \
    Source/LibPNG/pngwtran.c \
    Source/LibPNG/pngwutil.c \
    Source/ZLib/adler32.c \
    Source/ZLib/compress.c \
    Source/ZLib/crc32.c \
    Source/ZLib/deflate.c \
    Source/ZLib/gzclose.c \
    Source/ZLib/gzlib.c \
    Source/ZLib/gzread.c \
    Source/ZLib/gzwrite.c \
    Source/ZLib/infback.c \
    Source/ZLib/inffast.c \
    Source/ZLib/inflate.c \
    Source/ZLib/inftrees.c \
    Source/ZLib/trees.c \
    Source/ZLib/uncompr.c \
    Source/ZLib/zutil.c

win32: include($$PWD/Source/FreeImage/freeimage_misc.pro)

