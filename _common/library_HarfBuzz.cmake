
set(harfbuzzArchive ${CMAKE_SOURCE_DIR}/_Libs/_sources/harfbuzz-2.5.3.tar.gz)
file(SHA256 ${harfbuzzArchive} harfbuzzArchive_hash)

# FreeType to render TTF fonts
ExternalProject_Add(
    FREETYPE_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/FreeType
    URL ${harfbuzzArchive}
    URL_HASH SHA256=${harfbuzzArchive_hash}
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        ${ANDROID_CMAKE_FLAGS}
        -DFT_WITH_ZLIB=ON -DFT_WITH_BZIP2=OFF -DFT_WITH_PNG=ON -DFT_WITH_HARFBUZZ=OFF
        $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
)

