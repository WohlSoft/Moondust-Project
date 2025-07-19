# Library that can convert some PCM formats

add_library(PGE_libarchive INTERFACE)

set_static_lib(libarchive_A_Lib "${DEPENDENCIES_INSTALL_DIR}" archive)

set(LIBARCHIVE_EXTRA_FLAGS)

if(WIN32)
    list(APPEND LIBARCHIVE_EXTRA_FLAGS
        "-DWINDOWS_VERSION=WINXP"
        "-DDISABLE_MKGMTIME=ON"
    )
endif()

ExternalProject_Add(
    archive_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/libarchive
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/libarchive
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        "-DDISABLE_FORCE_DEBUG_POSTFIX=ON"
        "-DCMAKE_DEBUG_POSTFIX=${PGE_LIBS_DEBUG_SUFFIX}"
        # Fine tune:
        "-DENABLE_INSTALL=ON"
        "-DBUILD_SHARED_LIBS=OFF"
        "-DENABLE_MBEDTLS=OFF"
        "-DENABLE_OPENSSL=OFF"
        "-DENABLE_LIBB2=OFF"
        # enable LZ4 for extraction
        "-DENABLE_LZ4=ON"
        "-DENABLE_LZO=OFF"
        "-DENABLE_ZSTD=OFF"
        "-DENABLE_ZLIB=ON"
        "-DENABLE_BZip2=OFF"
        "-DENABLE_LIBXML2=OFF"
        "-DENABLE_EXPAT=OFF"
        "-DENABLE_PCREPOSIX=OFF"
        "-DENABLE_PCRE2POSIX=OFF"
        "-DENABLE_LIBGCC=OFF"
        "-DENABLE_CNG=OFF"
        "-DENABLE_TAR=OFF"
        "-DENABLE_TAR_SHARED=OFF"
        "-DENABLE_CPIO=OFF"
        "-DENABLE_CPIO_SHARED=OFF"
        "-DENABLE_CAT=OFF"
        "-DENABLE_CAT_SHARED=OFF"
        "-DENABLE_UNZIP=OFF"
        "-DENABLE_UNZIP_SHARED=OFF"
        "-DENABLE_XATTR=OFF"
        "-DENABLE_ACL=OFF"
        "-DENABLE_ICONV=OFF"
        "-DENABLE_TEST=OFF"
        "-DENABLE_COVERAGE=OFF"
        "-DENABLE_WERROR=OFF"
        "-DLZ4_LIBRARY=${liblz4_A_Lib}"
        "-DLZ4_INCLUDE_DIR=${LZ4_INCLUDE_DIRS}"
        "-DLIBLZMA_LIBRARY=${liblzma_A_Lib}"
        "-DLIBLZMA_INCLUDE_DIR=${LZMA_INCLUDE_DIRS}"
        ${ANDROID_CMAKE_FLAGS}
        ${APPLE_CMAKE_FLAGS}
        ${LIBARCHIVE_EXTRA_FLAGS}
    BUILD_BYPRODUCTS
        "${libarchive_A_Lib}"
)

add_dependencies(archive_Local lz4_Local PGE_liblz4 lzma_Local gnurx_Local PGE_ZLib PGE_AudioCodecs)

message("-- libarchive will be built: ${libarchive_A_Lib} --")

target_link_libraries(PGE_libarchive INTERFACE "${libarchive_A_Lib}" PGE_liblzma)
target_include_directories(PGE_libarchive INTERFACE "${DEPENDENCIES_INSTALL_DIR}/include")
add_dependencies(PGE_libarchive archive_Local lzma_Local PGE_libgnurx)
