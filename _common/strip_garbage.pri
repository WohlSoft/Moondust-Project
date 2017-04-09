#
# Strip dead code from the compiled binary in the release mode
#

if(linux-*|win32*):{
    !win*-msvc*: {
        QMAKE_CFLAGS_RELEASE    += -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
        QMAKE_CXXFLAGS_RELEASE  += -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
        QMAKE_LFLAGS_RELEASE    += -Wl,--gc-sections -Wl,-s
    }
}
if(macx):{
    QMAKE_LFLAGS_RELEASE    += -dead_strip
}
