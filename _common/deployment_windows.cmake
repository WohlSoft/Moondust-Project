
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

if(SevenZipProgram)
    message("Detected 7Zip installed: ${SevenZipProgram}. Deployment is possible!")
    if($ENV{QtDir})
        string(REPLACE "\\" "/" QT_BINLIB_DIR $ENV{QtDir})
    endif()

    set(ZIP_PACK_DIR "${CMAKE_INSTALL_PREFIX_ORIG}/_packed")
    set(ZIP_SRC_DIR "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}")

    add_custom_target(mkdir_packed_create_zip
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_PACK_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_SRC_DIR}/styles" # WORKAROUND: when styles DLLs realy not used, just make empty folder
    )

    set(PGE_CommonQtFiles)
    if(NOT PGE_ENABLE_STATIC_QT)
        set(PGE_CommonQtFiles
            "${ZIP_SRC_DIR}/Qt5Core${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/Qt5Gui${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/Qt5Network${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/Qt5Widgets${PGE_DLL_SUFFIX}.dll"
            "${ZIP_SRC_DIR}/imageformats/"
            "${ZIP_SRC_DIR}/platforms/"
            "${ZIP_SRC_DIR}/styles/"
            ${MINGW_DLLS}
        )
    elseif(PGE_SHARED_SDLMIXER)
        set(PGE_CommonQtFiles
            ${MINGW_DLLS}
        )
    endif()

    set(PGE_CommonFiles
        "${ZIP_SRC_DIR}/*.dll"
        "${ZIP_SRC_DIR}/licenses/"
        "${ZIP_SRC_DIR}/imageformats/"
        "${ZIP_SRC_DIR}/platforms/"
        "${ZIP_SRC_DIR}/styles/"
        "${ZIP_SRC_DIR}/help/"
    )

    set(PGE_MusPlayFiles
        ${PGE_CommonQtFiles}
        "${ZIP_SRC_DIR}/SDL2${PGE_DLL_SUFFIX}.dll"
        "${ZIP_SRC_DIR}/SDL2_mixer_ext${PGE_DLL_SUFFIX}.dll"
        "${ZIP_SRC_DIR}/pge_musplay.exe"
        "${ZIP_SRC_DIR}/changelog.musplay.txt"
    )

    set(PGE_CalibratorFiles
        ${PGE_CommonQtFiles}
        "${ZIP_SRC_DIR}/calibrator"
        "${ZIP_SRC_DIR}/pge_calibrator.exe"
    )

    set(PGE_Gifs2PngFiles
        "${ZIP_SRC_DIR}/GIFs2PNG.exe"
        "${ZIP_SRC_DIR}/GIFs2PNG.readme.txt"
    )

    set(PGE_PalleteFilterFiles
        "${ZIP_SRC_DIR}/PalleteFilter.exe"
    )

    set(PGE_Png2GifsFiles
        "${ZIP_SRC_DIR}/PNG2GIFs.exe"
        "${ZIP_SRC_DIR}/PNG2GIFs.readme.txt"
    )

    set(PGE_LazyFixToolFiles
        "${ZIP_SRC_DIR}/LazyFixTool.exe"
        "${ZIP_SRC_DIR}/LazyFixTool.readme.txt"
    )

    set(PGE_MaintainerFiles
        ${PGE_CommonQtFiles}
        "${ZIP_SRC_DIR}/tools"
        ${PGE_Gifs2PngFiles}
        ${PGE_PalleteFilterFiles}
        ${PGE_Png2GifsFiles}
        "${ZIP_SRC_DIR}/pge_maintainer.exe"
    )

    set(PGE_InstallCommonFiles
        ${PGE_CommonFiles}
        "${ZIP_SRC_DIR}/tools"
    )
    set(PGE_InstallEditorFiles
        "${ZIP_SRC_DIR}/changelog.editor.txt"
        "${ZIP_SRC_DIR}/pge_editor.exe"
        "${ZIP_SRC_DIR}/themes"
        "${ZIP_SRC_DIR}/languages"
        -r "-x!engine_*.qm"
    )
    set(PGE_InstallEngineFiles
        "${ZIP_SRC_DIR}/pge_engine.exe"
        "${ZIP_SRC_DIR}/languages"
        "${ZIP_SRC_DIR}/pge_engine.*.txt"
        "${ZIP_SRC_DIR}/changelog.engine.txt"
        -r "-x!*.png" "-x!editor_*.qm" "-x!qt_*.qm"
    )
    set(PGE_InstallToolsFiles
        "${ZIP_SRC_DIR}/GIFs2PNG.exe"
        "${ZIP_SRC_DIR}/GIFs2PNG.readme.txt"
        "${ZIP_SRC_DIR}/PNG2GIFs.exe"
        "${ZIP_SRC_DIR}/PNG2GIFs.readme.txt"
        "${ZIP_SRC_DIR}/LazyFixTool.exe"
        "${ZIP_SRC_DIR}/LazyFixTool.readme.txt"
        "${ZIP_SRC_DIR}/PalleteFilter.exe"
        "${ZIP_SRC_DIR}/pge_musplay.exe"
        "${ZIP_SRC_DIR}/pge_calibrator.exe"
        "${ZIP_SRC_DIR}/pge_maintainer.exe"
        "${ZIP_SRC_DIR}/calibrator"
        "${ZIP_SRC_DIR}/tools"
    )

    add_custom_target(create_zip_install
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
            "${ZIP_PACK_DIR}/PalleteFilter-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
            ${PGE_PalleteFilterFiles}
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
        ${SevenZipProgram}
        a -tzip -mx9
        "${ZIP_PACK_DIR}/pge-project-${PACKAGE_SUFFIX}-win${PGE_ARCHITECTURE_BITS}.zip"
        "${ZIP_SRC_DIR}/"
        DEPENDS mkdir_packed_create_zip
        COMMENT "Packing ZIP archive..."
    )
else()
    message("!! 7Zip not found! Deployment will be inavailable!")
endif()

endif(WIN32)
