set(Moondust_MAJOR_VERSION 0)
set(Moondust_MINOR_VERSION 4)
set(Moondust_PATCH_VERSION 3)
set(Moondust_TWEAK_VERSION 1)

# Major
add_definitions(-DPGE_TOOLCHAIN_VERSION_1=${Moondust_MAJOR_VERSION})
# Minor
add_definitions(-DPGE_TOOLCHAIN_VERSION_2=${Moondust_MINOR_VERSION})
# Revision
add_definitions(-DPGE_TOOLCHAIN_VERSION_3=${Moondust_PATCH_VERSION})
# Patch
add_definitions(-DPGE_TOOLCHAIN_VERSION_4=${Moondust_TWEAK_VERSION})

# Config pack API version
add_definitions(-DPGE_CONFIG_PACK_API_VERSION=41)

# Type of version: "-alpha","-beta","-dev", or "" aka "release"
add_definitions(-DPGE_TOOLCHAIN_VERSION_REL="")
