cmake_minimum_required (VERSION 3.1.0)
project(LunaLoaderExec C CXX)

# =============================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

set(CMAKE_CXX_STANDARD 11)
# =============================================================================

add_executable(LunaLoaderExec
    LunaLoader.cpp
    LunaLoaderPatch.cpp
    LunaLoader.rc
)

target_compile_definitions(LunaLoaderExec PRIVATE -DLUNALOADER_EXEC)
set_target_properties(LunaLoaderExec PROPERTIES OUTPUT_NAME "LunaLoader-exec")

if(NOT MSVC)
    target_link_libraries(LunaLoaderExec -static -static-libgcc -static-libstdc++)
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message("== LunaTester: Non-32-bit compiler has been detected! Forcing 32-bit build...")
    set_target_properties(LunaLoaderExec PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

install(TARGETS LunaLoaderExec
    RUNTIME DESTINATION "bin/testers"
)

