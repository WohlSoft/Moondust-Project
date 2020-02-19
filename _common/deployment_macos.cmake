if(APPLE) #Create a DMG from built stuff

string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)
add_custom_target(create_dmg
    WORKING_DIRECTORY "${Moondust_SOURCE_DIR}/_common/deploy"
    COMMAND "./create-dmg.sh"
        --volname "PGE Project"
        --window-size 800 600
        --app-drop-link 450 320
        --subfolder
        "${CMAKE_INSTALL_PREFIX}/pge-project-${PACKAGE_SUFFIX}-macosx.dmg"
        "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}/"
        COMMENT "Packing DMG..."
)

endif(APPLE)
