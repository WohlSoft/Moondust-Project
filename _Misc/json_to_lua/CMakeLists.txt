cmake_minimum_required (VERSION 3.2)
project (JsonToLua LANGUAGES C CXX)

include(ExternalProject)
set(PGE_USE_LUAJIT OFF)

set(CMAKE_CXX_STANDARD 14)

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${DEPENDENCIES_INSTALL_DIR}/include/lua_includes
)

set(LUABIND_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../_Libs/luabind")

include(../../_common/build_env.cmake)
include(../../_common/build_props.cmake)
include(../../_common/library_luabind.cmake)

add_executable(jsontolua
    json_to_lua.cpp
)

add_dependencies(jsontolua LuaBind_Local)

target_compile_definitions(jsontolua PRIVATE
    "-DPATH_TO_TESTLUA=\"${CMAKE_CURRENT_SOURCE_DIR}/lua/test.lua\""
    "-DPATH_TO_SAMPLE_JSON=\"${CMAKE_CURRENT_SOURCE_DIR}/lua/sample.json\""
    "-DPATH_TO_LUA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/lua\""
)

target_link_libraries(jsontolua ${libLuaBind_Lib})

find_library(DL_LIB dl)
if(DL_LIB)
    target_link_libraries(jsontolua ${DL_LIB})
endif()

