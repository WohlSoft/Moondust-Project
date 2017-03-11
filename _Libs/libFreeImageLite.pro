
include($$PWD/FreeImage/FreeImageLite.pri)

include($$PWD/../_common/strip_garbage.pri)
include($$PWD/../_common/lib_destdir.pri)
TARGET = freeimagelite
include($$PWD/../_common/build_props.pri)
DESTDIR = $$PWD/_builds/$$TARGETOS/lib

TRANSLATIONS = ""

LIBS                += -L$$PWD/_builds/$$TARGETOS/lib
INCLUDEPATH         += $$PWD/_builds/$$TARGETOS/include
FreeImageH.path      = $$PWD/_builds/$$TARGETOS/include
FreeImageH.files    += $$PWD/FreeImage/Source/FreeImageLite.h

INSTALLS = FreeImageH
