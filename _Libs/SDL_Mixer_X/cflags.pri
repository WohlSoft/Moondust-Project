
QMAKE_CFLAGS += -std=c99

QMAKE_CFLAGS += \
    -Wno-missing-field-initializers \
    -Wno-unused-variable \
    -Wno-unused-parameter \
    -Wno-sign-compare \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-pointer-sign

QMAKE_CXXFLAGS += \
    -Wno-missing-field-initializers \
    -Wno-unused-variable \
    -Wno-unused-parameter

macx:{
    QMAKE_CXXFLAGS += -Wno-header-guard
} else {
    QMAKE_CFLAGS    += -Wno-unused-but-set-variable -Wno-deprecated-register
    QMAKE_CXXFLAGS  += -Wno-unused-but-set-variable -Wno-deprecated-register
    QMAKE_LFLAGS    += -Wl,-rpath=\'\$\$ORIGIN\'
}

