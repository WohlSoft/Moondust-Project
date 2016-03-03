
if(linux-g++*|win32*): release: {
QMAKE_CFLAGS += -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
QMAKE_CXXFLAGS += -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
QMAKE_LFLAGS += -Wl,--gc-sections -Wl,-s
}
