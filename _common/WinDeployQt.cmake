#
# CMake wrapper to call windeployqt in Windows
#
# The files are placed in windeployqt folder in the toplevel
# build directory

function(WinDeployQt TargetName)
    cmake_parse_arguments(_deploy
        "COMPILER_RUNTIME;FORCE;NOSWGL;NOANGLE;NOD3DCOMPILER"
        "TARGET"
        "TARGETS;INCLUDE_MODULES;EXCLUDE_MODULES"
        ${ARGN}
        )

    if(NOT _deploy_TARGET AND NOT _deploy_TARGETS)
        message(FATAL_ERROR "A TARGET (or TARGETS) must be specified")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND _ARGS --debug)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        list(APPEND _ARGS --release-with-debug-info)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND _ARGS --release)
    endif()

    if(_deploy_COMPILER_RUNTIME)
        list(APPEND _ARGS --compiler-runtime)
    endif()

    if(_deploy_NOD3DCOMPILER)
        list(APPEND _ARGS --no-system-d3d-compiler)
    endif()

    if(_deploy_FORCE)
        list(APPEND _ARGS --force)
    endif()

    if(_deploy_NOSWGL)
        list(APPEND _ARGS --no-opengl-sw)
    endif()

    if(_deploy_NOANGLE)
        list(APPEND _ARGS --no-angle)
    endif()

    foreach(mod ${_deploy_INCLUDE_MODULES})
        string(TOLOWER ${mod} mod)
        string(REPLACE "qt5::" "" mod ${mod})
        list(APPEND _ARGS "--${mod}")
    endforeach()

    foreach(mod ${_deploy_EXCLUDE_MODULES})
        string(TOLOWER ${mod} mod)
        string(REPLACE "qt5::" "" mod ${mod})
        list(APPEND _ARGS "--no-${mod}")
    endforeach()

    find_program(_deploy_PROGRAM windeployqt PATHS $ENV{QTDIR}/bin/)
    if(_deploy_PROGRAM)
        message(STATUS "Found ${_deploy_PROGRAM}")
    else()
        message(FATAL_ERROR "Unable to find windeployqt")
    endif()

    if(COMPILER_RUNTIME AND NOT $ENV{VVVV})
        message(STATUS "not set, the VC++ redistributable installer will NOT be bundled")
    endif()

    if(_deploy_TARGET)
        add_custom_target(${TargetName}
            ${_deploy_PROGRAM}
            --dir "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}" ${_ARGS}
            "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:${_deploy_TARGET}>"
            COMMENT "Preparing Qt runtime dependencies for ${_deploy_TARGET}"
        )
    else()
        set(DeployTargets)
        foreach(DepTarget ${_deploy_TARGETS})
            list(APPEND DeployTargets
                "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:${DepTarget}>"
            )
        endforeach()

        add_custom_target(${TargetName}
            ${_deploy_PROGRAM}
            --dir "${CMAKE_INSTALL_PREFIX_ORIG}/${PGE_INSTALL_DIRECTORY}"
            ${_ARGS}
            ${DeployTargets}
            COMMENT "Preparing Qt runtime dependencies for ${DeployTargets}"
        )
    endif()
endfunction()
