set(CMAKE_MAJOR_VERSION 0)
set(CMAKE_MINOR_VERSION 4)
set(CMAKE_PATCH_VERSION 2)
set(CMAKE_TWEAK_VERSION 0)

# Major
add_definitions(-DPGE_TOOLCHAIN_VERSION_1=${CMAKE_MAJOR_VERSION})
# Minor
add_definitions(-DPGE_TOOLCHAIN_VERSION_2=${CMAKE_MINOR_VERSION})
# Revision
add_definitions(-DPGE_TOOLCHAIN_VERSION_3=${CMAKE_PATCH_VERSION})
# Patch
add_definitions(-DPGE_TOOLCHAIN_VERSION_4=${CMAKE_TWEAK_VERSION})

# Config pack API version
add_definitions(-DPGE_CONFIG_PACK_API_VERSION=41)

# Type of version: "-alpha","-beta","-dev", or "" aka "release"
add_definitions(-DPGE_TOOLCHAIN_VERSION_REL="")

