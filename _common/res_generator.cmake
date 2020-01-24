# A small resource generator

set(RES_GENERATOR_EXE res_generator)
if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(RES_GENERATOR_EXE res_generator.exe)
endif()

set(RES_GENERATOR_PATH ${CMAKE_CURRENT_BINARY_DIR}/res_generator/${RES_GENERATOR_EXE})

if (NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/res_generator)
    set(RES_GENERATOR_PATH $<TARGET_FILE:res_generator>)
else()
    make_directory(${CMAKE_CURRENT_BINARY_DIR}/res_generator)
    add_custom_command(OUTPUT ${RES_GENERATOR_PATH}
        COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_LIST_DIR}/res_generator
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/res_generator
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
