# A small function to pass text files through UNIX2DOS utiltiy on Windows or install them by the regular way
# Requires a custom build of UNIX2DOS utility!


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
        set(UNIX2DOS_ELEMENT ${_installfile_FILE}?${CMAKE_INSTALL_PREFIX}/${_installfile_DESTINATION}?${_installfile_RENAME})
        if(UNIX2DOS_LIST)
            # Add next elements as first element is already added
            set(UNIX2DOS_LIST "${UNIX2DOS_LIST};${UNIX2DOS_ELEMENT}" PARENT_SCOPE)
        else()
            # Add first element to avoid empty element in begin
            set(UNIX2DOS_LIST "${UNIX2DOS_ELEMENT}" PARENT_SCOPE)
        endif()
    else()
        install(FILES "${_installfile_FILE}" RENAME "${_installfile_RENAME}" DESTINATION "${_installfile_DESTINATION}")
    endif()
endfunction()

# Put this function into end of config without any arguments.
# Thus function initializes the installer script which will be ran on install step
function(InstallTextFiles)
    if(WIN32)
        # message("== UNIX2DOS List is ${UNIX2DOS_LIST}")
        set(custom_script ${CMAKE_BINARY_DIR}/unix2dos_list.cmake)
        configure_file(_common/unix2dos.cmake.in ${custom_script} @ONLY)
        install(SCRIPT ${custom_script})
    endif()
endfunction()
