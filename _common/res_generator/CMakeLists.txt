cmake_minimum_required (VERSION 3.5...3.10)
project(ResGenerator CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include(../../_Libs/IniProcessor/IniProcessor.cmake)
include(../Utf8Main/utf8main.cmake)

add_executable(res_generator
    res_generator.cpp
    ${INIPROCESSOR_SRCS}
    ${UTF8MAIN_SRCS}
)
