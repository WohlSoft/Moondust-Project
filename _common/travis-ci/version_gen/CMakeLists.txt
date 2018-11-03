cmake_minimum_required (VERSION 2.8.11)
project (PGEvergen C)

# Get the current working branch
set(OVERRIDE_GIT_BRANCH "" CACHE STRING "Override name of GIT branch")
if(OVERRIDE_GIT_BRANCH)
    set(GIT_BRANCH ${OVERRIDE_GIT_BRANCH})
elseif(DEFINED ENV{APPVEYOR_REPO_BRANCH})
    set(GIT_BRANCH $ENV{APPVEYOR_REPO_BRANCH})
elseif(DEFINED ENV{TRAVIS_BRANCH})
    set(GIT_BRANCH $ENV{TRAVIS_BRANCH})
else()
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include(../../git_version.cmake)
include(../../../version.cmake)

include(../../../Editor/version.cmake)
add_executable(version_gen_editor ../version_gen.c)
target_compile_definitions(version_gen_editor PRIVATE -DEDITOR -DVER_FULL)

add_executable(version_gen_editor_short ../version_gen.c)
target_compile_definitions(version_gen_editor_short PRIVATE -DEDITOR -DVER_SHORT)


include(../../../Engine/version.cmake)
add_executable(version_gen_engine ../version_gen.c)
target_compile_definitions(version_gen_engine PRIVATE -DENGINE -DVER_FULL)

add_executable(version_gen_engine_short ../version_gen.c)
target_compile_definitions(version_gen_engine_short PRIVATE -DENGINE -DVER_SHORT)

