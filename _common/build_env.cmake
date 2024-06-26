# Global environment variables and features
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

# Set debug postfix for everything
if(NOT ANDROID AND NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

if(WIN32)
    # Remove shared library "lib" prefix on Windows
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(NOT ANDROID AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    set(PGE_LIBS_DEBUG_SUFFIX "d")
else()
    set(PGE_LIBS_DEBUG_SUFFIX "")
endif()

# let macOS Sierra to be a minimal supported platform (as Qt 5.12 does)
if(APPLE)
    set(PGE_OSX_TARGET "10.12" CACHE STRING "Minimal macOS version target")
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET ${PGE_OSX_TARGET})

if(ANDROID)
    set(ANDROID_PLATFORM "android-16")
endif()

if(POLICY CMP0058) # Dependencies
    cmake_policy(SET CMP0058 NEW)
endif()

if(NOT DEFINED DEPENDENCIES_INSTALL_DIR)
    set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Allow install into space-containing paths
set(CMAKE_INSTALL_PREFIX_ORIG ${CMAKE_INSTALL_PREFIX})
string(REPLACE " " "\\ " CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
string(REPLACE "(" "\\(" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
string(REPLACE ")" "\\)" CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    add_definitions(-DNDEBUG)
endif()

include_directories(
    "${DEPENDENCIES_INSTALL_DIR}/include"
    "${DEPENDENCIES_INSTALL_DIR}/include/freetype2"
    "${DEPENDENCIES_INSTALL_DIR}/include/lua_includes"
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

