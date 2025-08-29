if(APPLE) #Create a DMG from built stuff

string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)
add_custom_target(create_dmg
    WORKING_DIRECTORY "${Moondust_SOURCE_DIR}/_common/deploy"
    # Delete blank translations
    COMMAND find "${Moondust_SOURCE_DIR}/_common/deploy/" -maxdepth 10 -type f -name "*.qm" -size "-1000c" -delete
    # Create a DMG
    COMMAND "./create-dmg.sh"
        --volname "Moondust Project Development Kit"
        --window-size 800 600
        --app-drop-link 450 320
        --subfolder-name "Moondust Project"
        --no-internet-enable
        --hdiutil-verbose
        "${CMAKE_INSTALL_PREFIX}/pge-project-${PACKAGE_SUFFIX}-macosx.dmg"
        "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}/"
        COMMENT "Packing DMG..."
)

add_custom_target(create_pkg
    WORKING_DIRECTORY "${Moondust_SOURCE_DIR}/_common/deploy"
    # Delete blank translations
    COMMAND find "${Moondust_SOURCE_DIR}/_common/deploy/" -maxdepth 10 -type f -name "*.qm" -size "-1000c" -delete
    # Create a DMG
    COMMAND "./create-pkg.sh"
        "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}/"
        "${CMAKE_INSTALL_PREFIX}/pge-project-${PACKAGE_SUFFIX}-macosx.pkg"
        COMMENT "Packing PKG..."
)

endif(APPLE)
