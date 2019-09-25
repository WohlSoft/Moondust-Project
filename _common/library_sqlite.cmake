
# SQLite a small database system

add_library(PGE_SQLite INTERFACE)

option(USE_SYSTEM_SQLITE "Use SQLite from the system" OFF)

if(USE_SYSTEM_SQLITE)
    add_library(SQLite_Local INTERFACE)

    if(${CMAKE_VERSION} VERSION_LESS "3.14.6")
        # Look for the necessary header
        find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h)
        mark_as_advanced(SQLite3_INCLUDE_DIR)

        # Look for the necessary library
        find_library(SQLite3_LIBRARY NAMES sqlite3 sqlite)
        mark_as_advanced(SQLite3_LIBRARY)

        if(SQLite3_INCLUDE_DIR AND SQLite3_LIBRARY)
            set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
            set(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
        else()
            message(FATAL_ERROR "Can't find SQLite3 library be installed!")
        endif()
    else()
        find_package(SQLite3 REQUIRED)
    endif()

    message("-- Found SQLIte: ${SQLite3_LIBRARIES} --")

    target_link_libraries(PGE_SQLite INTERFACE ${SQLite3_LIBRARIES})
    target_include_directories(PGE_SQLite INTERFACE ${SQLite3_INCLUDE_DIRS})

else()

    set(libSQLite3_Lib "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sqlite3${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")

    ExternalProject_Add(
        SQLite_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/SQLite
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/sqlite3
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
            ${ANDROID_CMAKE_FLAGS}
            $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
        BUILD_BYPRODUCTS
            "${libSQLite3_Lib}"
    )

    message("-- SQLite3 will be built: ${libSQLite3_Lib} --")

    target_link_libraries(PGE_SQLite INTERFACE ${libSQLite3_Lib})

endif()
