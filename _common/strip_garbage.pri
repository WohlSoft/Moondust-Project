
if(linux-g++*|win32*): release: static: {
QMAKE_CFLAGS += -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
QMAKE_CXXFLAGS += -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
QMAKE_LFLAGS += -Wl,--gc-sections -Wl,-s
}
