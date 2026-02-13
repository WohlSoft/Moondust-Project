if(UNIX AND NOT APPLE) # Deployment for UNIX-like operating system except macOS

string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)
add_custom_target(create_tar
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
    # Delete blank translations
    COMMAND find "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}/languages/" -maxdepth 1 -type f -name "*.qm" -size "-1000c" -delete
    # Pack a tarball
    COMMAND ${CMAKE_COMMAND} -E tar "-cjvf"
        "pge_project-${CMAKE_SYSTEM_NAME_LOWER}-${PACKAGE_SUFFIX}-${PGE_ARCHITECTURE_BITS}.tar.bz2"
        "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}"
        COMMENT "Packing tarball..."
)

if(NOT EMSCRIPTEN)
    set(CPACK_PACKAGE_NAME "moondust" CACHE STRING "The resulting package name")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The game engine toolkit for platformer genre games" CACHE STRING "Package description for the package metadata")

    set(CPACK_PACKAGE_VENDOR "Moondust Team")

    set(CPACK_VERBATIM_VARIABLES YES)

    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
    set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/_packages")

    set(CPACK_PACKAGE_VERSION ${PGE_TOOLCHAIN_VERSION_STRING})

    if(NOT CPACK_DEBIAN_PACKAGE_RELEASE)
        set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
    endif()

    set(PGE_TOOLCHAIN_DEB_VERSION "${PGE_TOOLCHAIN_VERSION_STRING}-${CPACK_DEBIAN_PACKAGE_RELEASE}")

    set(CPACK_PACKAGE_CONTACT "admin@wohlnet.ru")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vitaliy Novichkov <${CPACK_PACKAGE_CONTACT}>")

    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

    set(CPACK_STRIP_FILES YES)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)

    set(CPACK_DEBIAN_PACKAGE_SECTION games)
    set(CPACK_DEBIAN_PACKAGE_PRIORITY optional)

    # =======================COMPONENTS=======================
    set(CPACK_DEBIAN_MOONDUSTLIBS_PACKAGE_NAME "moondust-libs")
    set(CPACK_DEBIAN_MOONDUSTLIBS_PACKAGE_SECTION libs)
    set(CPACK_DEBIAN_MOONDUSTLIBS_DESCRIPTION "Set of dependencies for Moondust Project components")

    set(CPACK_DEBIAN_MOONDUSTCOMMON_PACKAGE_NAME "moondust-common")
    set(CPACK_DEBIAN_MOONDUSTCOMMON_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_MOONDUSTCOMMON_PACKAGE_DEPENDS "")
    set(CPACK_DEBIAN_MOONDUSTCOMMON_PACKAGE_SECTION misc)
    set(CPACK_DEBIAN_MOONDUSTCOMMON_DESCRIPTION "Set of shared resources for Moondust Project components")

    set(CPACK_DEBIAN_MOONDUSTEDITOR_PACKAGE_NAME "moondust-editor")
    set(CPACK_DEBIAN_MOONDUSTEDITOR_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION});moondust-common (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTEDITOR_PACKAGE_SECTION editors)
    set(CPACK_DEBIAN_MOONDUSTEDITOR_DESCRIPTION "Levels and world maps editor for Moondust Project based games")

    set(CPACK_DEBIAN_MOONDUSTEDITORTHEMES_PACKAGE_NAME "moondust-editor-themes")
    set(CPACK_DEBIAN_MOONDUSTEDITORTHEMES_PACKAGE_DEPENDS "moondust-editor")
    set(CPACK_DEBIAN_MOONDUSTEDITORTHEMES_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_MOONDUSTEDITORTHEMES_PACKAGE_SECTION misc)
    set(CPACK_DEBIAN_MOONDUSTEDITORTHEMES_DESCRIPTION "Themes pack for the Moondust Editor")

    set(CPACK_DEBIAN_MOONDUSTDOCS_PACKAGE_NAME "moondust-docs")
    set(CPACK_DEBIAN_MOONDUSTDOCS_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_MOONDUSTDOCS_PACKAGE_DEPENDS "")
    set(CPACK_DEBIAN_MOONDUSTDOCS_PACKAGE_SECTION doc)
    set(CPACK_DEBIAN_MOONDUSTDOCS_DESCRIPTION "HTML documentation collection for Moondust Project")

    set(CPACK_DEBIAN_MOONDUSTRUNTIME_PACKAGE_NAME "moondust-runtime")
    set(CPACK_DEBIAN_MOONDUSTRUNTIME_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTRUNTIME_PACKAGE_SECTION games)
    set(CPACK_DEBIAN_MOONDUSTRUNTIME_DESCRIPTION "Runtime engine to play Moondust Project based games")

    set(CPACK_DEBIAN_MOONDUSTGIFS2PNG_PACKAGE_NAME "moondust-tool-gifs2png")
    set(CPACK_DEBIAN_MOONDUSTGIFS2PNG_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTGIFS2PNG_PACKAGE_SECTION utils)
    set(CPACK_DEBIAN_MOONDUSTGIFS2PNG_DESCRIPTION "Utility to convert pair of GIF files for bitmask render (Legacy SMBX format) into alpha-channel based PNG graphics (Moondust Project native format)")

    set(CPACK_DEBIAN_MOONDUSTLAZYFIXTOOL_PACKAGE_NAME "moondust-tool-lazyfixtool")
    set(CPACK_DEBIAN_MOONDUSTLAZYFIXTOOL_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTLAZYFIXTOOL_PACKAGE_SECTION utils)
    set(CPACK_DEBIAN_MOONDUSTLAZYFIXTOOL_DESCRIPTION "A small tool to fix improperly created bitmask based graphics into valid format.")

    set(CPACK_DEBIAN_MOONDUSTMAINTAINER_PACKAGE_NAME "moondust-maintainer")
    set(CPACK_DEBIAN_MOONDUSTMAINTAINER_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION});moondust-common (= ${PGE_TOOLCHAIN_DEB_VERSION});moondust-tool-gifs2png;moondust-tool-lazyfixtool;moondust-tool-palletefilter;moondust-tool-png2gifs")
    set(CPACK_DEBIAN_MOONDUSTMAINTAINER_PACKAGE_RECOMMENDS "ffmpeg, sox, libsox-fmt-mp3")
    set(CPACK_DEBIAN_MOONDUSTMAINTAINER_PACKAGE_SECTION utils)
    set(CPACK_DEBIAN_MOONDUSTMAINTAINER_DESCRIPTION "A multi-functional utility to process and/or restore data and resources for Moondust Project based games.")

    set(CPACK_DEBIAN_MOONDUSTMUSICPLAYER_PACKAGE_NAME "moondust-musplay")
    set(CPACK_DEBIAN_MOONDUSTMUSICPLAYER_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION});moondust-common (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTMUSICPLAYER_PACKAGE_SECTION sound)
    set(CPACK_DEBIAN_MOONDUSTMUSICPLAYER_DESCRIPTION "A simple music player based on the MixerX sound library.")

    set(CPACK_DEBIAN_MOONDUSTPALLETEFILTER_PACKAGE_NAME "moondust-tool-palletefilter")
    set(CPACK_DEBIAN_MOONDUSTPALLETEFILTER_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTPALLETEFILTER_PACKAGE_SECTION utils)
    set(CPACK_DEBIAN_MOONDUSTPALLETEFILTER_DESCRIPTION "A simple utility to filter pallete of grahics using reference image.")

    set(CPACK_DEBIAN_MOONDUSTCALIBRATOR_PACKAGE_NAME "moondust-calibrator")
    set(CPACK_DEBIAN_MOONDUSTCALIBRATOR_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION});moondust-common (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTCALIBRATOR_PACKAGE_SECTION utils)
    set(CPACK_DEBIAN_MOONDUSTCALIBRATOR_DESCRIPTION "A tool to create and adjust settings for playable characters using in the Moondust Project based games.")

    set(CPACK_DEBIAN_MOONDUSTPNG2GIFS_PACKAGE_NAME "moondust-tool-png2gifs")
    set(CPACK_DEBIAN_MOONDUSTPNG2GIFS_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTPNG2GIFS_PACKAGE_SECTION utils)
    set(CPACK_DEBIAN_MOONDUSTPNG2GIFS_DESCRIPTION "Utility to create valid pair of GIF files for bitmask render (Legacy SMBX format) from alha-channel based PNG graphics.")

    set(CPACK_DEBIAN_MOONDUSTTRANSLATOR_PACKAGE_NAME "moondust-translator")
    set(CPACK_DEBIAN_MOONDUSTTRANSLATOR_PACKAGE_DEPENDS "moondust-libs (= ${PGE_TOOLCHAIN_DEB_VERSION});moondust-common (= ${PGE_TOOLCHAIN_DEB_VERSION})")
    set(CPACK_DEBIAN_MOONDUSTTRANSLATOR_PACKAGE_SECTION localization)
    set(CPACK_DEBIAN_MOONDUSTTRANSLATOR_DESCRIPTION "A tool to create and edit translations for Moondust Project compatible levels and episodes.")

    # ========================================================

    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEB_COMPONENT_INSTALL YES)

    include(CPack)
endif()

endif(UNIX AND NOT APPLE)
