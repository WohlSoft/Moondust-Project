set(UNIX2DOS_LIST PARENT_SCOPE)

function(InstallTextFile)
    cmake_parse_arguments(_installfile
        ""
        "FILE;DESTINATION;RENAME"
        ""
        ${ARGN}
    )

    if(NOT _installfile_FILE)
         message(FATAL_ERROR "A FILE must be specified")
    endif()

    if(WIN32)
        message("File ${_installfile_FILE} will be UNIX2DOS-ed")
        set(UNIX2DOS_LIST "${UNIX2DOS_LIST};${_installfile_FILE}?${CMAKE_INSTALL_PREFIX}/${_installfile_DESTINATION}?${_installfile_RENAME}" PARENT_SCOPE)
    else()
        install(FILES "${_installfile_FILE}" RENAME "${_installfile_RENAME}" DESTINATION "${_installfile_DESTINATION}")
    endif()
endfunction()


function(InstallTextFiles)
    if(WIN32)
        set(custom_script ${CMAKE_BINARY_DIR}/unix2dos_list.cmake)
        configure_file(_common/unix2dos.cmake.in ${custom_script} @ONLY)
        install(SCRIPT ${custom_script})
    endif()
endfunction()
