include(CheckCXXCompilerFlag)

if(NOT WIN32 AND CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    check_cxx_compiler_flag("-no-pie" HAS_NO_PIE)
endif()

function(pge_set_nopie _target)
    if(HAS_NO_PIE AND CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS " -no-pie")
    endif()
endfunction()

# WORKAROUND: Avoid ImpLib file creation on a non-library project
if(CYGWIN OR MINGW)
    # Prune --out-implib from executables. It doesn't make sense even
    # with --export-all-symbols.
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " "
        CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " "
        CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
endif()

