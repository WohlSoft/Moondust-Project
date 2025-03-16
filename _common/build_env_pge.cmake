# Global environment variables and features

string(COMPARE EQUAL "${Moondust_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
if(insource)
    file(REMOVE ${Moondust_SOURCE_DIR}/CMakeCache.txt)
    file(REMOVE_RECURSE ${Moondust_SOURCE_DIR}/CMakeFiles)
    message(FATAL_ERROR "
==== Build in source directory is not allowed! ====
Please run cmake configure from another directory, for example, create a 'build' sub-directory, and run 'cmake ..' from it."
)
endif()

# Needed for external projects
include(ExternalProject)
include(GNUInstallDirs)

# Destinition bitness of the build
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PGE_ARCHITECTURE_BITS "32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PGE_ARCHITECTURE_BITS "64")
else()
    set(PGE_ARCHITECTURE_BITS "unk")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/TargetArch.cmake)
target_architecture(TARGET_PROCESSOR)
message(STATUS "Target architecture: ${TARGET_PROCESSOR}")

# Date and time when build was initiated
string(TIMESTAMP MOONDUST_DATETIME "%Y-%m-%d %H:%m:%S UTC" UTC)
# Keep this string being fixed to don't lead unfair rebuilds during local debugs
set(MOONDUST_DATETIME_CACHED "${MOONDUST_DATETIME}" CACHE STRING "The cached date and time when build was configured a first time")
add_definitions(-DV_DATE_OF_BUILD="${MOONDUST_DATETIME_CACHED}")
mark_as_advanced(MOONDUST_DATETIME_CACHED)

# Path to build dependencies
if(NOT DEFINED DEPENDENCIES_INSTALL_DIR)
    set(DEPENDENCIES_INSTALL_DIR ${Moondust_BINARY_DIR})
endif()
include(${CMAKE_CURRENT_LIST_DIR}/build_env.cmake)

if(ANDROID)
    if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
        # Disable NEON support for old devices
        set(ANDROID_ARM_NEON FALSE)
    elseif(NOT DEFINED ANDROID_ARM_NEON)
        set(ANDROID_ARM_NEON TRUE)
    endif()

    if(NOT DEFINED ANDROID_STL)
        # include(ndk-stl-config.cmake)
        set(ANDROID_STL "c++_static")
    endif()

    if(NOT DEFINED ANDROID_PLATFORM)
        set(ANDROID_PLATFORM 16)
    endif()

    set(ANDROID_CMAKE_FLAGS
        "-DANDROID_ABI=${ANDROID_ABI}"
        "-DANDROID_NDK=${ANDROID_NDK}"
        "-DANDROID_STL=c++_static"
        "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        "-DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}"
        "-DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}"
        "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
        "-DANDROID_ARM_NEON=${ANDROID_ARM_NEON}"
    )
endif()

if(UNIX) # When include/library/binary directory name is not usual in a system, make symbolic links for them
    if(NOT "${CMAKE_INSTALL_LIBDIR}" STREQUAL "lib")
        message("${CMAKE_INSTALL_LIBDIR} IS NOT STREQUAL lib")
        file(MAKE_DIRECTORY "${DEPENDENCIES_INSTALL_DIR}")
        execute_process(COMMAND ln -s "lib" "${DEPENDENCIES_INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}")
    endif()
    if(NOT "${CMAKE_INSTALL_BINDIR}" STREQUAL "bin")
        message("${CMAKE_INSTALL_BINDIR} IS NOT STREQUAL bin")
        file(MAKE_DIRECTORY "${DEPENDENCIES_INSTALL_DIR}")
        execute_process(COMMAND ln -s "bin" "${DEPENDENCIES_INSTALL_DIR}/${CMAKE_INSTALL_BINDIR}")
    endif()
    if(NOT "${CMAKE_INSTALL_INCLUDEDIR}" STREQUAL "include")
        message("${CMAKE_INSTALL_INCLUDEDIR} IS NOT STREQUAL lib")
        file(MAKE_DIRECTORY "${DEPENDENCIES_INSTALL_DIR}")
        execute_process(COMMAND ln -s "include" "${DEPENDENCIES_INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
    endif()
endif()

if(APPLE)
    # Prevent "directory not exists" warnings when building XCode as project
    file(MAKE_DIRECTORY ${DEPENDENCIES_INSTALL_DIR}/lib/Debug)
    file(MAKE_DIRECTORY ${DEPENDENCIES_INSTALL_DIR}/lib/Release)
    # Don't store built executables into "Debug" and "Release" folders
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_BUNDLE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_BUNDLE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

    set(APPLE_CMAKE_FLAGS
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
        "-DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}"
    )
endif()
