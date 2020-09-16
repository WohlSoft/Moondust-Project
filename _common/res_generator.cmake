# A small resource generator

set(RES_GENERATOR_EXE res_generator)

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(RES_GENERATOR_EXE res_generator.exe)
endif()

set(RES_GENERATOR_PATH ${CMAKE_CURRENT_BINARY_DIR}/res_generator/${RES_GENERATOR_EXE})

if(NOT CMAKE_CROSSCOMPILING AND NOT EMSCRIPTEN)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/res_generator)
    set(RES_GENERATOR_PATH $<TARGET_FILE:res_generator>)
else()
    find_program(DEFAULT_HOST_C_COMPILER "gcc")
    find_program(DEFAULT_HOST_CXX_COMPILER "g++")
    find_program(DEFAULT_HOST_CMAKE_COMMAND "cmake")
    mark_as_advanced(DEFAULT_HOST_C_COMPILER DEFAULT_HOST_CXX_COMPILER DEFAULT_HOST_CMAKE_COMMAND)

    set(HOST_PATH_ENV "$ENV{PATH}" CACHE STRING "Bypass the host path environment")
    set(HOST_CMAKE_COMMAND ${DEFAULT_HOST_CMAKE_COMMAND} CACHE STRING "Bypass the host CMake command")
    set(HOST_C_COMPILER ${DEFAULT_HOST_C_COMPILER} CACHE STRING "Bypass the host C compiler command")
    set(HOST_CXX_COMPILER ${DEFAULT_HOST_CXX_COMPILER} CACHE STRING "Bypass the host C++ compiler command")
    make_directory(${CMAKE_CURRENT_BINARY_DIR}/res_generator)
    add_custom_command(OUTPUT ${RES_GENERATOR_PATH}
        COMMAND ${HOST_CMAKE_COMMAND} -E env PATH="${HOST_PATH_ENV}" CC="${HOST_C_COMPILER}" CXX="${HOST_CXX_COMPILER}"
                ${HOST_CMAKE_COMMAND} ${CMAKE_CURRENT_LIST_DIR}/res_generator
        COMMAND ${HOST_CMAKE_COMMAND} -E env PATH="${HOST_PATH_ENV}" CC="${HOST_C_COMPILER}" CXX="${HOST_CXX_COMPILER}"
                ${HOST_CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/res_generator
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/res_generator"
    )
    add_custom_target(res_generator ALL
        DEPENDS ${RES_GENERATOR_PATH}
    )
endif()

macro(add_res_generator _input_directory _output_directory _header_name _data_target)
    make_directory(${_output_directory})
    add_custom_command(OUTPUT "${_output_directory}/${_header_name}"
        COMMAND "${RES_GENERATOR_PATH}"
                "--config" "resource.ini"
                "--out-dir" "${_output_directory}"
                "--header" "${_header_name}"
                "${_input_directory}"
        WORKING_DIRECTORY "${_input_directory}"
        DEPENDS res_generator
    )
    add_custom_target(${_data_target} ALL
        DEPENDS "${_output_directory}/${_header_name}"
    )
endmacro(add_res_generator)
