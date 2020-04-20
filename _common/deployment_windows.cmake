
if(WIN32) # Deployment for Windows

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PGE_DLL_SUFFIX "d")
else()
    set(PGE_DLL_SUFFIX "")
endif()

if(PGE_SHARED_SDLMIXER)
    install(FILES
        "${Moondust_BINARY_DIR}/bin/SDL2${PGE_DLL_SUFFIX}.dll"
        "${Moondust_BINARY_DIR}/bin/SDL2_mixer_ext${PGE_DLL_SUFFIX}.dll"
        DESTINATION "${PGE_INSTALL_DIRECTORY}/"
    )
endif()

find_program(SevenZipProgram 7z
    PATHS
    "/usr/lib/p7zip/"
    "$ENV{ProgramFiles}/7-zip/"
    "C:/Program Files/7-zip/"
    "C:/Program Files (x86)/7-zip/"
    NO_DEFAULT_PATH
)

find_program(GitBashFind find
    PATHS
    "/usr/bin/"
    "$ENV{ProgramFiles}/Git/usr/bin/"
    "C:/Program Files/Git/usr/bin/"
    "C:/Program Files (x86)/Git/usr/bin/"
    NO_DEFAULT_PATH
)

if(GitBashFind)
    message("Detected 'find' from Git-Bash installed: ${GitBashFind}")
else()
    message("!! 'find' from Git-Bash was not found!")
endif()

if(SevenZipProgram)
    message("Detected 7Zip installed: ${SevenZipProgram}. Deployment is possible!")
else()
    message("!! 7Zip not found! Deployment will be inavailable!")
endif()


if(SevenZipProgram)
    if($ENV{QtDir})
        string(REPLACE "\\" "/" QT_BINLIB_DIR $ENV{QtDir})
    endif()

    set(ZIP_PACK_DIR "${CMAKE_INSTALL_PREFIX_ORIG}/_packed")
    set(ZIP_SRC_DIR "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}")

    add_custom_target(mkdir_packed_create_zip
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_PACK_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_SRC_DIR}/styles" # WORKAROUND: when styles DLLs realy not used, just make empty folder
    )

    set(MinGW_BuiltDlls
        "libgcc*.dll"
        "libstdc*.dll"
    )
    if(MINGWDLL_MINGWEX)
        list(APPEND MinGW_BuiltDlls "libmingwex-0.dll")
    endif()
    if(MINGWDLL_WINPTHREAD)
        list(APPEND MinGW_BuiltDlls "libwinpthread-1.dll")
    endif()
    if(MINGWDLL_WINPTHREADGC3)
        list(APPEND MinGW_BuiltDlls "pthreadGC-3.dll")
    endif()

    set(PGE_CommonQtFiles)
    if(NOT PGE_ENABLE_STATIC_QT)
        set(PGE_CommonQtFiles
            "Qt5Core${PGE_DLL_SUFFIX}.dll"
            "Qt5Gui${PGE_DLL_SUFFIX}.dll"
            "Qt5Network${PGE_DLL_SUFFIX}.dll"
            "Qt5Widgets${PGE_DLL_SUFFIX}.dll"
            "imageformats/"
            "platforms/"
            "styles/"
            "languages/qt_*.qm"
            ${MinGW_BuiltDlls}
        )
    elseif(PGE_SHARED_SDLMIXER)
        set(PGE_CommonQtFiles
            ${MinGW_BuiltDlls}
        )
    endif()

    set(PGE_CommonFiles
        "*.dll"
        "licenses/"
        "imageformats/"
        "platforms/"
        "styles/"
        "help/"
    )

    set(PGE_MusPlayFiles
        ${PGE_CommonQtFiles}
        "SDL2${PGE_DLL_SUFFIX}.dll"
        "SDL2_mixer_ext${PGE_DLL_SUFFIX}.dll"
        "pge_musplay.exe"
        "changelog.musplay.txt"
    )

    set(PGE_CalibratorFiles
        ${PGE_CommonQtFiles}
        "calibrator"
        "pge_calibrator.exe"
        "languages/calibrator_*.qm"
        "languages/*.png"
    )

    set(PGE_Gifs2PngFiles
        "GIFs2PNG.exe"
        "GIFs2PNG.readme.txt"
    )

    set(PGE_PaletteFilterFiles
        "PaletteFilter.exe"
        "PaletteFilter.readme.txt"
    )

    set(PGE_Png2GifsFiles
        "PNG2GIFs.exe"
        "PNG2GIFs.readme.txt"
    )

    set(PGE_LazyFixToolFiles
        "LazyFixTool.exe"
        "LazyFixTool.readme.txt"
    )

    set(PGE_MaintainerFiles
        ${PGE_CommonQtFiles}
        "tools"
        ${PGE_Gifs2PngFiles}
        ${PGE_PaletteFilterFiles}
        ${PGE_Png2GifsFiles}
        "pge_maintainer.exe"
        "languages/maintainer_*.qm"
        "languages/*.png"
    )

    set(PGE_InstallCommonFiles
        ${PGE_CommonFiles}
        "tools"
    )
    set(PGE_InstallEditorFiles
        "changelog.editor.txt"
        "pge_editor.exe"
        "themes"
        "ipc"
        "languages/editor_*.qm"
        "languages/*.png"
    )
    set(PGE_InstallEngineFiles
        "pge_engine.exe"
        "pge_engine.*.txt"
        "changelog.engine.txt"
        "languages/engine_*.qm"
    )
    set(PGE_InstallToolsFiles
        "GIFs2PNG.exe"
        "GIFs2PNG.readme.txt"
        "PNG2GIFs.exe"
        "PNG2GIFs.readme.txt"
        "LazyFixTool.exe"
        "LazyFixTool.readme.txt"
        "PaletteFilter.exe"
        "PaletteFilter.readme.txt"
        "pge_musplay.exe"
        "pge_calibrator.exe"
        "pge_maintainer.exe"
        "calibrator"
        "tools"
        "languages/calibrator_*.qm"
        "languages/maintainer_*.qm"
        "languages/qt_*.qm"
        "languages/*.png"
    )

    add_custom_target(create_zip_install
        WORKING_DIRECTORY "${ZIP_SRC_DIR}"
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/install-pge-common-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_InstallCommonFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/install-pge-editor-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_InstallEditorFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/install-pge-engine-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_InstallEngineFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/install-pge-tools-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_InstallToolsFiles}
        DEPENDS mkdir_packed_create_zip
        COMMENT "Packing common install files into ZIP archive..."
    )

    add_custom_target(create_zip_tools
        WORKING_DIRECTORY "${ZIP_SRC_DIR}"
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/pge-musplay-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_MusPlayFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/pge-maintainer-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_MaintainerFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/pge-calibrator-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_CalibratorFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/GIFs2PNG-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_Gifs2PngFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/PaletteFilter-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_PaletteFilterFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/PNG2GIFs-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_Png2GifsFiles}
        COMMAND ${SevenZipProgram} a -tzip -mx9
            "${ZIP_PACK_DIR}/LazyFixTool-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_LazyFixToolFiles}
        DEPENDS mkdir_packed_create_zip
        COMMENT "Packing tools into ZIP archive..."
    )

    add_custom_target(create_zip
        WORKING_DIRECTORY "${ZIP_SRC_DIR}"
        # Clean-up blank translations
        COMMAND ${GitBashFind} "${ZIP_SRC_DIR}/languages/" -maxdepth 1 -type f -name "*.qm" -size "-1000c" -delete
        # Pack a ZIP archive
        COMMAND ${SevenZipProgram}
            a -tzip -mx9
            "${ZIP_PACK_DIR}/pge-project-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            "${ZIP_SRC_DIR}/"
        DEPENDS mkdir_packed_create_zip
        COMMENT "Packing ZIP archive..."
    )
endif()

endif(WIN32)
