
!win*-msvc*:{
    QMAKE_CFLAGS += -std=c99

    QMAKE_CFLAGS_WARN_ON += \
        -Wno-unused-variable \
        -Wno-implicit-fallthrough \
        #-Wno-missing-field-initializers \
        #-Wno-unused-parameter \
        #-Wno-sign-compare \
        #-Wno-unused-function \
        #-Wno-implicit-function-declaration \
        #-Wno-pointer-sign

    QMAKE_CXXFLAGS_WARN_ON += \
        #-Wno-missing-field-initializers \
        #-Wno-unused-variable \
        #-Wno-unused-parameter

    macx:{
        QMAKE_CXXFLAGS_WARN_ON += -Wno-header-guard
    } else {
        QMAKE_CFLAGS_WARN_ON    += -Wno-unused-but-set-variable
        QMAKE_CXXFLAGS_WARN_ON  += -Wno-unused-but-set-variable
        !win64:{
            QMAKE_CFLAGS_WARN_ON    += -Wno-deprecated-register
            QMAKE_CXXFLAGS_WARN_ON  += -Wno-deprecated-register
        }
        QMAKE_LFLAGS    += -Wl,-rpath=\'\$\$ORIGIN\'
    }
} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON   += /wd4100 /wd4244 /wd4005 /wd4013 /wd4047 /wd4996
    QMAKE_CXXFLAGS_WARN_ON += /wd4100 /wd4101 /wd4244 /wd4800 /wd4104 /wd4146
}
