
if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/WinDeployQt.cmake)
endif()

set(WINDEPLOY_LIST)

function(qtDeployAdd _target)
    list(APPEND WINDEPLOY_LIST ${_target})
    set(WINDEPLOY_LIST ${WINDEPLOY_LIST} PARENT_SCOPE)
endfunction()

function(pgeSetupQtDeploymet _is_static_qt _is_shared_mixer)
    if(WIN32)
        add_custom_target(windeploy)
    endif()

    # Shared deployment on Windows
    if(WIN32 AND NOT ${_is_static_qt})
        if($ENV{QtDir})
            string(REPLACE "\\" "/" QT_BINLIB_DIR $ENV{QtDir})
        endif()
        WinDeployQt(pge_windeploy TARGETS ${WINDEPLOY_LIST} NOSWGL NOANGLE NOD3DCOMPILER)

        set(JUNK_DLLS)
        list(APPEND JUNK_DLLS
            "languages/qt_en.qm"
            "translations/qt_en.qm"
            "imageformats/qjpeg.dll"
            "imageformats/qjpegd.dll"
            "imageformats/qsvg.dll"
            "imageformats/qsvgd.dll"
            "imageformats/qtga.dll"
            "imageformats/qtgad.dll"
            "imageformats/qtiff.dll"
            "imageformats/qtiffd.dll"
            "imageformats/qwbmp.dll"
            "imageformats/qwbmpd.dll"
            "imageformats/qwebp.dll"
            "imageformats/qwebpd.dll"
            "opengl32sw.dll"
            "libEGL.dll"
            "libGLESV2.dll"
            "D3Dcompiler_47.dll"
        )
        add_custom_target(cleanup_files_windeploy DEPENDS pge_windeploy)
        foreach(JunkFile ${JUNK_DLLS})
            add_custom_command(TARGET cleanup_files_windeploy POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E remove
                                "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/${JunkFile}"
            )
        endforeach()

        set(JUNK_DIRS)
        list(APPEND JUNK_DIRS
            "bearer"
            "iconengines"
            "qmltooling"
            "platforminputcontexts"
            "scenegraph"
        )
        add_custom_target(cleanup_dirs_windeploy DEPENDS pge_windeploy)
        foreach(JunkDir ${JUNK_DIRS})
            add_custom_command(TARGET cleanup_dirs_windeploy POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E remove_directory
                                "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/${JunkDir}"
            )
        endforeach()
    else()
        add_custom_target(pge_windeploy) # DUMMY
    endif()

    if(WIN32 AND (NOT ${_is_static_qt} OR ${_is_shared_mixer}))
        function(find_mingw_dll _FieldName _FileName _DestList _SearchPaths)
            find_file(MINGWDLL_${_FieldName} ${_FileName} PATHS "${_SearchPaths}")
            if(MINGWDLL_${_FieldName})
                list(APPEND ${_DestList} "${MINGWDLL_${_FieldName}}")
                set(${_DestList} ${${_DestList}} PARENT_SCOPE)
            endif()
        endfunction()

        set(MINGW_BIN_PATH $ENV{MinGW})

        if(NOT MINGW_BIN_PATH)
            set(MINGW_BIN_PATH "${QT_BINLIB_DIR}")
        else()
            string(REPLACE "\\" "/" MINGW_BIN_PATH $ENV{MinGW})
        endif()

        set(MINGW_DLLS)
        find_mingw_dll(LIBGCCDW         "libgcc_s_dw2-1.dll" MINGW_DLLS "${MINGW_BIN_PATH}")
        find_mingw_dll(LIBGCCSJLJ       "libgcc_s_sjlj-1.dll" MINGW_DLLS "${MINGW_BIN_PATH}")
        find_mingw_dll(LIBGCCSEC        "libgcc_s_seh-1.dll" MINGW_DLLS "${MINGW_BIN_PATH}")
        find_mingw_dll(MINGWEX          "libmingwex-0.dll" MINGW_DLLS "${MINGW_BIN_PATH}")
        find_mingw_dll(WINPTHREAD       "libwinpthread-1.dll" MINGW_DLLS "${MINGW_BIN_PATH}")
        find_mingw_dll(WINPTHREADGC3    "pthreadGC-3.dll" MINGW_DLLS "${MINGW_BIN_PATH}")
        find_mingw_dll(STDCPP           "libstdc++-6.dll" MINGW_DLLS "${MINGW_BIN_PATH}")

        message("MinGW DLLs: [${MINGW_DLLS}]")

        install(FILES
            ${MINGW_DLLS}
            DESTINATION "${PGE_INSTALL_DIRECTORY}/"
        )
#        file(COPY ${MINGW_DLLS} DESTINATION "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/")
        add_custom_target(copy_mingw_dlls DEPENDS pge_windeploy)
        foreach(MingwRuntimeDll ${MINGW_DLLS})
            add_custom_command(TARGET copy_mingw_dlls POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E
                               copy ${MingwRuntimeDll} "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}"
            )
        endforeach()
    endif()

    if(WIN32)# Also deploy tools
        add_custom_target(mkdir_tools_windeploy
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/tools"
        )
        add_custom_target(install_tools_windeploy
            WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/tools"
            COMMAND ${CMAKE_COMMAND} -E tar "-xjvf"
                "${CMAKE_SOURCE_DIR}/_Misc/tools/sox-win32.tar.gz"
            DEPENDS mkdir_tools_windeploy
            COMMENT "Unpacking extra Win32 tools..."
        )
        if(NOT ${_is_static_qt})
            add_dependencies(windeploy pge_windeploy cleanup_files_windeploy cleanup_dirs_windeploy)
        endif()
        add_dependencies(windeploy pge_windeploy install_tools_windeploy)
    endif()
endfunction()



# ====== Deploy of standalone manual ======
set(PGE_FILE_STANDALONE_EDITOR_MANUAL
    "<html><head><meta http-equiv=\\\"refresh\\\" content=\\\"0; url=http://pgehelp.wohlsoft.ru/manual_editor.html\\\"/></head><body></body></html>")
file(WRITE "${CMAKE_BINARY_DIR}/install_online_help.cmake" "
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/help/manual_editor.htm\"
        \"${PGE_FILE_STANDALONE_EDITOR_MANUAL}\")
")

add_custom_target(put_online_help
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX_ORIG}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/help"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/help"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/install_online_help.cmake"
)



# ====== Enable portable ======
set(PGE_FILE_PORTABLE_APP_INI
    "[Main]\nforce-portable=true\n\n")

file(WRITE "${CMAKE_BINARY_DIR}/install_portable.cmake" "
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/pge_editor.ini\"
        \"${PGE_FILE_PORTABLE_APP_INI}\")
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/pge_engine.ini\"
        \"${PGE_FILE_PORTABLE_APP_INI}\")
file(WRITE
        \"${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/pge_calibrator.ini\"
        \"${PGE_FILE_PORTABLE_APP_INI}\")
")

add_custom_target(enable_portable
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX_ORIG}"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/install_portable.cmake"
)

