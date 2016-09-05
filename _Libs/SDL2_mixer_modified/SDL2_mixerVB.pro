!win32: error("VB6-Compatible version can be built for Microsoft Windows operating system only!")

CONFIG+=enable-stdcalls
include($$PWD/SDL2_mixer.pro)
