if(UNIX AND NOT APPLE) # Deployment for UNIX-like operating system except macOS

string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)
add_custom_target(create_tar
    WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
    COMMAND ${CMAKE_COMMAND} -E tar "-cjvf"
        "pge_project-${CMAKE_SYSTEM_NAME_LOWER}-${PACKAGE_SUFFIX}-${PGE_ARCHITECTURE_BITS}.tar.bz2"
        "${CMAKE_INSTALL_PREFIX}/${PGE_INSTALL_DIRECTORY}"
        COMMENT "Packing tarball..."
)

endif(UNIX AND NOT APPLE)
