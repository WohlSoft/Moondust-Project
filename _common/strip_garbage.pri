if(linux-*|win32*):{
QMAKE_CFLAGS_RELEASE    += -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
QMAKE_CXXFLAGS_RELEASE  += -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
QMAKE_LFLAGS_RELEASE    += -Wl,--gc-sections -Wl,-s
}
