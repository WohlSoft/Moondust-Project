cmake_minimum_required (VERSION 3.1.0)
project(LunaTester C CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include(../../_common/Utf8Main/utf8main.cmake)
include(../../_common/tclap/tclap.cmake)

add_executable(LunaTester
    lunatester.cpp
    ${UTF8MAIN_SRCS}
)

install(TARGETS LunaTester
    RUNTIME DESTINATION "bin/testers"
)
