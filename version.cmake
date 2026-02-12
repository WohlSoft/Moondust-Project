set(Moondust_MAJOR_VERSION 0)
set(Moondust_MINOR_VERSION 4)
set(Moondust_PATCH_VERSION 3)
set(Moondust_TWEAK_VERSION 2)
set(Moondust_VersionRel "-beta")

# Major
add_definitions(-DPGE_TOOLCHAIN_VERSION_1=${Moondust_MAJOR_VERSION})
# Minor
add_definitions(-DPGE_TOOLCHAIN_VERSION_2=${Moondust_MINOR_VERSION})
# Revision
add_definitions(-DPGE_TOOLCHAIN_VERSION_3=${Moondust_PATCH_VERSION})
# Patch
add_definitions(-DPGE_TOOLCHAIN_VERSION_4=${Moondust_TWEAK_VERSION})

# Config pack API version
add_definitions(-DPGE_CONFIG_PACK_API_VERSION=42)

# Type of version: "-alpha","-beta","-dev", or "" aka "release"
add_definitions(-DPGE_TOOLCHAIN_VERSION_REL="${Moondust_VersionRel}")

# Buildin the version name
set(PGE_TOOLCHAIN_VERSION_STRING "${Moondust_MAJOR_VERSION}.${Moondust_MINOR_VERSION}")

if(NOT ${Moondust_PATCH_VERSION} EQUAL 0 OR NOT ${Moondust_TWEAK_VERSION} EQUAL 0)
    string(CONCAT PGE_TOOLCHAIN_VERSION_STRING "${PGE_TOOLCHAIN_VERSION_STRING}" ".${Moondust_PATCH_VERSION}")
endif()

if(NOT ${Moondust_TWEAK_VERSION} EQUAL 0)
    string(CONCAT PGE_TOOLCHAIN_VERSION_STRING "${PGE_TOOLCHAIN_VERSION_STRING}" ".${Moondust_TWEAK_VERSION}")
endif()

if(NOT "${Moondust_VersionRel}" STREQUAL "")
    string(CONCAT PGE_TOOLCHAIN_VERSION_STRING "${PGE_TOOLCHAIN_VERSION_STRING}" "${Moondust_VersionRel}")
endif()
