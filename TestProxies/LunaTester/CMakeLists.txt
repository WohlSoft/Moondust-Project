cmake_minimum_required (VERSION 3.1.0)
project(LunaTester C CXX)

# =============================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Strip garbage
if(APPLE)
    set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")
elseif(NOT MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s")
    set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
endif()

# Global optimization flags
if(NOT MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    add_definitions(-DNDEBUG)
endif()

# Disable bogus MSVC warnings
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# =============================================================================

set(CMAKE_CXX_STANDARD 14)

include(../../_common/Utf8Main/utf8main.cmake)
include(../../_common/tclap/tclap.cmake)

add_executable(LunaTester
    lunatester.cpp
    ${UTF8MAIN_SRCS}
)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message("== LunaTester: Non-32-bit compiler has been detected! Forcing 32-bit build...")
    set_target_properties(LunaTester PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

install(TARGETS LunaTester
    RUNTIME DESTINATION "bin/testers"
)
