cmake_minimum_required (VERSION 2.8.11)
project (PGEvergen C)

include(../../git_info.cmake)
include(../../git_version.cmake)
include(../../../version.cmake)

# ========= Editor version =========
include(../../../Editor/version.cmake)
add_executable(version_gen_editor ../version_gen.c)
target_compile_definitions(version_gen_editor PRIVATE -DEDITOR -DVER_FULL)

add_executable(version_gen_editor_short ../version_gen.c)
target_compile_definitions(version_gen_editor_short PRIVATE -DEDITOR -DVER_SHORT)

# ========= Engine version =========
include(../../../Engine/version.cmake)
add_executable(version_gen_engine ../version_gen.c)
target_compile_definitions(version_gen_engine PRIVATE -DENGINE -DVER_FULL)

add_executable(version_gen_engine_short ../version_gen.c)
target_compile_definitions(version_gen_engine_short PRIVATE -DENGINE -DVER_SHORT)
