
option(USE_SYSTEM_LIBFREETYPE "Use FreeType from the system" OFF)

add_library(PGE_FreeType INTERFACE)

if(USE_SYSTEM_LIBFREETYPE)
    add_library(FREETYPE_Local INTERFACE)

    find_package(Freetype REQUIRED)
    message("-- Found FreeType: ${FREETYPE_LIBRARIES} --")
    target_link_libraries(PGE_FreeType INTERFACE ${FREETYPE_LIBRARIES})
    target_include_directories(PGE_FreeType INTERFACE ${FREETYPE_INCLUDE_DIRS})

    set(libFreeType_Libs "${FREETYPE_LIBRARIES}")

else()
    set(freetypeArchive ${CMAKE_SOURCE_DIR}/_Libs/_sources/freetype-2.10.0.tar.gz)
    file(SHA256 ${freetypeArchive} freetypeArchive_hash)

    set(libFreeType_Libs "${DEPENDENCIES_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}freetype${PGE_LIBS_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")

    # FreeType to render TTF fonts
    ExternalProject_Add(
        FREETYPE_Local
        PREFIX ${CMAKE_BINARY_DIR}/external/FreeType
        URL ${freetypeArchive}
        URL_HASH SHA256=${freetypeArchive_hash}
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
            ${ANDROID_CMAKE_FLAGS}
            -DFT_WITH_ZLIB=ON
            -DFT_WITH_BZIP2=OFF
            -DFT_WITH_PNG=OFF
            -DPNG_FOUND=TRUE
            -DPNG_LIBRARIES=png
            "-DPNG_INCLUDE_DIRS=${CMAKE_BINARY_DIR}/include"
            "-DZLIB_INCLUDE_DIR=${CMAKE_BINARY_DIR}/include"
            "-DZLIB_LIBRARY=${libZLib_A_Lib}"
            -DFT_WITH_HARFBUZZ=OFF
            -DCMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE
            -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE
            -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE
            $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
        BUILD_BYPRODUCTS
            "${libFreeType_Libs}"
    )
    target_link_libraries(PGE_FreeType INTERFACE ${libFreeType_Libs})
    target_include_directories(PGE_FreeType INTERFACE ${DEPENDENCIES_INSTALL_DIR}/include/freetype2)

endif()
