cmake_minimum_required (VERSION 3.5)

if(POLICY CMP0069) # Allow CMAKE_INTERPROCEDURAL_OPTIMIZATION (lto) to be set
    cmake_policy(SET CMP0069 NEW)
endif()

if(POLICY CMP0135) # Supress the DOWNLOAD_EXTRACT_TIMESTAMP warning at External Projects
    cmake_policy(SET CMP0135 NEW)
endif()

project(Moondust LANGUAGES C CXX)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(ExternalProject)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(FindBacktrace)
include(TestBigEndian)
include(CheckIncludeFile)

set(PGE_INSTALL_DIRECTORY "Moondust" CACHE STRING "Name of directory where all binary and data files are will be installed")

include(_common/git_info.cmake)
message("== Current GIT hash [${GIT_COMMIT_HASH}], branch [${GIT_BRANCH}], package [${PACKAGE_SUFFIX}] ==")

include(_common/build_env_pge.cmake)
include(_common/deployment.cmake)
include(_common/library_qt.cmake)

if(WIN32)
    set(PGE_STATIC_SDLMIXER_PLATFORM_DEFAULT OFF)
else()
    set(PGE_STATIC_SDLMIXER_PLATFORM_DEFAULT ON)
endif()

option(PGE_STATIC_SDLMIXER "Prefer static assembly of SDL Mixer X and libraries" ${PGE_STATIC_SDLMIXER_PLATFORM_DEFAULT})

if(PGE_STATIC_SDLMIXER)
    set(PGE_SHARED_SDLMIXER OFF)
else()
    set(PGE_SHARED_SDLMIXER ON)
endif()

# Version of complete toolchain
include(version.cmake)

option(USE_SYSTEM_LIBC "Don't link libc and libstdc++ statically and use directly from the system" OFF)

# Unix2Dos utility to converyt and install text files for Windows platform
include(_common/tool_unix2dos.cmake)
# Resources generator (for non-Qt applications)
include(_common/res_generator.cmake)
# Dependent libraries
include(_common/library_libpng.cmake)
include(_common/library_libjpeg.cmake)
#include(_common/library_sqlite.cmake)
include(_common/library_luajit.cmake)
include(_common/library_luabind.cmake)
include(_common/library_SDL2.cmake)
include(_common/library_AudioCodecs.cmake)
include(_common/library_FreeImage.cmake)
include(_common/library_FreeType.cmake)
include(_common/library_SDLMixerX.cmake)
include(_common/library_PGE_FL.cmake)

if(NOT USE_SYSTEM_LIBPNG)
    add_dependencies(libpng_Local AudioCodecs_Local) #Zlib is needed to libPNG
    add_dependencies(FREETYPE_Local libpng_Local) #libPNG is needed to FreeType
    add_dependencies(FreeImage_Local libpng_Local libJPEG_Local) #libPNG and libJPEG are needed to FreeImage
    if(UNIX AND NOT APPLE AND NOT HAIKU) # Disable native file dialogs when locally-built libPNG was been used to avoid the crash
        set(QT_DISABLE_NATIVE_FILE_DIALOG TRUE)
    endif()
endif()

if(NOT USE_SYSTEM_LIBPNG AND NOT USE_SYSTEM_LIBFREETYPE)
    add_dependencies(FREETYPE_Local AudioCodecs_Local) #Zlib is needed to FreeType
endif()

set(LIBS_TARGETS
    SDLMixerX_Local
    SDL2_Local
    LuaBind_Local
    FreeImage_Local
#    SQLite_Local
    FREETYPE_Local
    pgefl
)

if(PGE_ENABLE_QT) # Build if Qt support enabled
    list(APPEND LIBS_TARGETS
        QtPropertyBrowser_Local
        pgefl_qt
    )
endif()

# Build dependent libraries only
add_custom_target(libs DEPENDS ${LIBS_TARGETS})

option(PGE_BUILD_ENGINE "Build the Runtime Engine component" ON)
if(PGE_BUILD_ENGINE)
    add_subdirectory(Engine)
    # Append changelog file
    InstallTextFile(FILE "${Moondust_SOURCE_DIR}/changelog.engine.txt" RENAME "changelog.engine.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    # Append license files
    InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Engine/LICENSE.mit" RENAME "pge_engine.license.mit.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Engine/LICENSE.gpl3" RENAME "pge_engine.license.gpl3.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Content/readmes/Engine.Readme.txt" RENAME "pge_engine.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
endif()

if(NOT EMSCRIPTEN AND NOT ANDROID)
    option(PGE_BUILD_GIFS2PNG "Build the GIFs2PNG utility component" ON)
    if(PGE_BUILD_GIFS2PNG)
        add_subdirectory(GIFs2PNG)
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Content/readmes/GIFs2PNG_Readme.txt" RENAME "GIFs2PNG.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()

    option(PGE_BUILD_PNG2GIFS "Build the GIFs2PNG utility component" ON)
    if(PGE_BUILD_PNG2GIFS)
        add_subdirectory(PNG2GIFs)
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Content/readmes/PNG2GIFs_Readme.txt" RENAME "PNG2GIFs.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()

    option(PGE_BUILD_LAZYFIXTOOL "Build the LazyFixTool utility component" ON)
    if(PGE_BUILD_LAZYFIXTOOL)
        add_subdirectory(LazyFixTool)
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Content/readmes/LazyFixTool_Readme.txt" RENAME "LazyFixTool.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()

    option(PGE_BUILD_PALETTEFILTER "Build the PaletteFilter utility component" ON)
    if(PGE_BUILD_PALETTEFILTER)
        add_subdirectory(PaletteFilter)
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/Content/readmes/PaletteFilter_Readme.txt" RENAME "PaletteFilter.readme.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
    endif()
endif()

if(PGE_ENABLE_QT AND NOT ANDROID AND NOT EMSCRIPTEN)
    include(_common/qt-modules/qt_modules_langs.cmake)

    option(PGE_BUILD_EDITOR "Build the PGE Editor component (Qt5 required)" ON)
    if(PGE_BUILD_EDITOR)
        add_subdirectory(Editor)
        # Append changelog file
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/changelog.editor.txt" RENAME "changelog.editor.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
        # Deployment list
        qtDeployAdd(pge_editor)
    endif()

    option(PGE_BUILD_PLAYERCALIBRATOR "Build the Player Calibrator utility component (Qt5 required)" ON)
    if(PGE_BUILD_PLAYERCALIBRATOR)
        add_subdirectory(PlayableCalibrator)
        # Append changelog file
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/changelog.calibrator.txt" RENAME "changelog.calibrator.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
        # Deployment list
        qtDeployAdd(pge_calibrator)
    endif()

    option(PGE_BUILD_MUSICPLAYER "Build the SDL Mixer X Music Player component (Qt5 required)" ON)
    if(PGE_BUILD_MUSICPLAYER)
        add_subdirectory(MusicPlayer)
        # Append changelog file
        InstallTextFile(FILE "${Moondust_SOURCE_DIR}/changelog.musplay.txt" RENAME "changelog.musplay.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")
        # Deployment list
        qtDeployAdd(pge_musplay)
    endif()

    option(PGE_BUILD_MAINTAINER "Build the Maintainer utility component (Qt5 required)" ON)
    if(PGE_BUILD_MAINTAINER)
        add_subdirectory(Maintainer)
        qtDeployAdd(pge_maintainer)
    endif()

    pgeSetupQtDeploymet(${PGE_ENABLE_STATIC_QT} ${PGE_SHARED_SDLMIXER})
endif()

# Common changelog file
InstallTextFile(FILE "${Moondust_SOURCE_DIR}/changelog.common.txt" RENAME "changelog.common.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")

# Append Project License file
InstallTextFile(FILE "${Moondust_SOURCE_DIR}/LICENSE" RENAME "LICENSE.txt" DESTINATION "${PGE_INSTALL_DIRECTORY}")

# Process UNIX2DOS if needed
InstallTextFiles()

# =============== Deployment stuff ===============
include(_common/deployment_macos.cmake)
include(_common/deployment_unix.cmake)
include(_common/deployment_windows.cmake)
