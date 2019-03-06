
# Note: You must also include "library_luajit.cmake" too!

if(PGE_USE_LUAJIT)
    set(LUAJIT_USE_CMAKE_FLAG -DUSE_LUAJIT=ON)
    message("***** LuaJIT Lua in use! *****")
else()
    set(LUAJIT_USE_CMAKE_FLAG -DUSE_LUAJIT=OFF)
    message("***** PUC-Rio Lua in use! *****")
endif()

# LuaBind is a powerful lua binding library for C++
ExternalProject_Add(
    LuaBind_Local
    PREFIX ${CMAKE_BINARY_DIR}/external/luabind
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Libs/luabind
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_DEBUG_POSTFIX=d"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        ${ANDROID_CMAKE_FLAGS}
        ${LUAJIT_USE_CMAKE_FLAG}
        $<$<BOOL:APPLE>:-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}>
)

if(PGE_USE_LUAJIT)
    add_dependencies(LuaBind_Local LuaJIT_local)
endif()

