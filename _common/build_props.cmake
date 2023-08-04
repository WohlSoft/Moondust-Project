# ============================ Generic setup ==================================
# If platform is Emscripten
if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(EMSCRIPTEN 1 BOOLEAN)
    unset(WIN32)
    unset(APPLE)
endif()

if(APPLE AND CMAKE_HOST_SYSTEM_VERSION VERSION_LESS 9)
    message("-- MacOS X 10.4 Tiger detected!")
    set(XTECH_MACOSX_TIGER TRUE)
endif()

# =========================== Architecture info ===============================

include(${CMAKE_CURRENT_LIST_DIR}/TargetArch.cmake)
target_architecture(TARGET_PROCESSOR)
message(STATUS "Target architecture: ${TARGET_PROCESSOR}")

test_big_endian(MOONDUST_IS_BIG_ENDIAN)
if(MOONDUST_IS_BIG_ENDIAN)
    message(STATUS "Target processor endianess: BIG ENDIAN")
else()
    message(STATUS "Target processor endianess: LITTLE ENDIAN")
endif()

message(STATUS "Size of void pointer is ${CMAKE_SIZEOF_VOID_P}!")

# =============================== Policies ====================================

# Ninja requires custom command byproducts to be explicit.
if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

# ExternalProject step targets fully adopt their steps.
if(POLICY CMP0114)
    cmake_policy(SET CMP0114 NEW)
endif()

# ========================= Macros and Functions ==============================

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

macro(pge_add_warning_flag WARNINGFLAG WARNING_VAR)
    check_c_compiler_flag("${WARNINGFLAG}" HAVE_W_C_${WARNING_VAR})
    if(HAVE_W_C_${WARNING_VAR})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNINGFLAG}")
    endif()

    check_cxx_compiler_flag("${WARNINGFLAG}" HAVE_W_CXX_${WARNING_VAR})
    if(HAVE_W_CXX_${WARNING_VAR})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNINGFLAG}")
    endif()
endmacro()

macro(pge_disable_warning_flag WARNINGFLAG WARNING_VAR)
    check_c_compiler_flag("-W${WARNINGFLAG}" HAVE_W_C_${WARNING_VAR})
    if(HAVE_W_C_${WARNING_VAR})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-${WARNINGFLAG}")
    endif()

    check_cxx_compiler_flag("-W${WARNINGFLAG}" HAVE_W_CXX_${WARNING_VAR})
    if(HAVE_W_CXX_${WARNING_VAR})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-${WARNINGFLAG}")
    endif()
endmacro()

macro(pge_add_opt_flag OPTFLAG OPT_VAR)
    check_c_compiler_flag("${OPTFLAG}" HAVE_M_C_${OPT_VAR})
    if(HAVE_M_C_${OPT_VAR})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTFLAG}")
    endif()

    check_cxx_compiler_flag("${OPTFLAG}" HAVE_M_CXX_${OPT_VAR})
    if(HAVE_M_CXX_${OPT_VAR})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTFLAG}")
    endif()
endmacro()

function(pge_cxx_standard STDVER)
    if(NOT WIN32)
        set(CMAKE_CXX_STANDARD ${STDVER} PARENT_SCOPE)
    elseif(MSVC AND CMAKE_VERSION VERSION_LESS "3.9.0" AND MSVC_VERSION GREATER_EQUAL "1900")
        CHECK_CXX_COMPILER_FLAG("/std:c++${STDVER}" _cpp_stdxx_flag_supported)
        if (_cpp_stdxx_flag_supported)
            add_compile_options("/std:c++${STDVER}")
        else()
            CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
            if (_cpp_latest_flag_supported)
                add_compile_options("/std:c++latest")
            endif()
        endif()
    else()
        set(CMAKE_CXX_STANDARD ${STDVER} PARENT_SCOPE)
    endif()
endfunction()


# ============================ Optimisations ==================================

# Strip garbage
if(APPLE)
    string(REGEX REPLACE "-O3" ""
        CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "-O3" ""
        CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")

    # Unify visibility to meet llvm's default.
    check_cxx_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
    if(SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
    endif()
elseif(NOT MSVC)
    if(EMSCRIPTEN)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -Os -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Os -fdata-sections -ffunction-sections")
        set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -O3 -Os -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -O3 -Os -fdata-sections -ffunction-sections")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3 -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -fdata-sections -ffunction-sections")

        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")
            set(LINK_FLAGS_MINSIZEREL  "${LINK_FLAGS_MINSIZEREL} -dead_strip")
            set(LINK_FLAGS_RELWITHDEBINFO  "${LINK_FLAGS_RELWITHDEBINFO} -dead_strip")
        else()
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
            set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Wl,--gc-sections -Wl,-s")
            set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Wl,--gc-sections -Wl,-s")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
            set(LINK_FLAGS_MINSIZEREL  "${LINK_FLAGS_MINSIZEREL} -Wl,--gc-sections -Wl,-s")
            set(LINK_FLAGS_RELWITHDEBINFO  "${LINK_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections -Wl,-s")
        endif()
    else()
        if(WIN32) # on Windows, disable the rest of optimisations to allow work on the rest of CPUs
            pge_add_opt_flag("-mno-mmx" NO_MMX)
            pge_add_opt_flag("-mno-3dnow" NO_3DNOW)
            pge_add_opt_flag("-mno-sse2" NO_SSE2)
            pge_add_opt_flag("-mno-sse3" NO_SSE3)
            pge_add_opt_flag("-mno-ssse3" NO_SSSE3)
            pge_add_opt_flag("-mno-sse4.1" NO_SSE41)
            pge_add_opt_flag("-mno-sse4.2" NO_SSE42)
            pge_add_opt_flag("-mno-avx" NO_AVX)
            pge_add_opt_flag("-mno-avx2" NO_AVX2)
            pge_add_opt_flag("-mno-avx512f" NO_AVX512F)
        endif()

        string(REGEX REPLACE "-O3" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        string(REGEX REPLACE "-O3" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fdata-sections -ffunction-sections")

        if(ANDROID)
            set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -funwind-tables")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -funwind-tables")
        elseif(NINTENDO_DS)
            # use -Os by default for all build types
            string(REGEX REPLACE "-O3" "-Os"
                CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
            string(REGEX REPLACE "-O3" "-Os"
                CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
            string(REGEX REPLACE "-O2" "-Os"
                CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
            string(REGEX REPLACE "-O2" "-Os"
                CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

            # Supress the std::vector::insert() GCC change warning
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcompare-debug-second")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcompare-debug-second")
            # use --gc-sections for all build types
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -Wl,--gc-sections")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Wl,--gc-sections")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -g -Wl,--gc-sections")
            set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
            set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
            set(LINK_FLAGS_RELWITHDEBINFO  "${LINK_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
            set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -g -Wl,--gc-sections")
            set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -g -Wl,--gc-sections")
            set(LINK_FLAGS_MINSIZEREL  "${LINK_FLAGS_MINSIZEREL} -g -Wl,--gc-sections")
        elseif(NINTENDO_3DS OR NINTENDO_WII OR NINTENDO_WIIU OR NINTENDO_SWITCH)
            # Supress the std::vector::insert() GCC change warning
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcompare-debug-second")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcompare-debug-second")
            if(NINTENDO_WII OR NINTENDO_WIIU)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-altivec -Ubool -Uvector -U_GNU_SOURCE")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-altivec -Ubool -Uvector -U_GNU_SOURCE")
            endif()
            # use --gc-sections for all build types
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -Wl,--gc-sections")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Wl,--gc-sections")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -g -Wl,--gc-sections")
            set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
            set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
            set(LINK_FLAGS_RELWITHDEBINFO  "${LINK_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
            set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -g -Wl,--gc-sections")
            set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -g -Wl,--gc-sections")
            set(LINK_FLAGS_MINSIZEREL  "${LINK_FLAGS_MINSIZEREL} -g -Wl,--gc-sections")
        elseif(VITA)
            # Supress the std::vector::insert() GCC change warning
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVITA=1 -fcompare-debug-second")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVITA=1 -fcompare-debug-second")
            # VitaSDK specifies -O2 for release configurations. PS Vita Support - Axiom 2022
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -I../src -Wl,--gc-sections -DVITA=1")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -I../src -Wl,--gc-sections -DVITA=1 -fpermissive -fno-optimize-sibling-calls -Wno-class-conversion")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -Wl,--gc-sections")
        elseif(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -Wl,--gc-sections -Wl,-s")
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -Wl,--gc-sections -Wl,-s")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
        else()
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")
        endif()
    endif()
endif()

# Global optimization flags
if(NOT MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    add_definitions(-DNDEBUG)
endif()

if(NOT ANDROID AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    set(PGE_LIBS_DEBUG_SUFFIX "d")
else()
    set(PGE_LIBS_DEBUG_SUFFIX "")
endif()


# ============================= Warnings ======================================

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()

    # Remove "/showIncludes" flag
    if(CMAKE_CXX_FLAGS MATCHES "/showIncludes")
        string(REGEX REPLACE "/showIncludes" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
    if(CMAKE_C_FLAGS MATCHES "/showIncludes")
        string(REGEX REPLACE "/showIncludes" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    endif()

    # Disable bogus MSVC warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4244 /wd4551 /wd4276 /wd6388")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4551 /wd4276 /wd6388")

else()
    pge_add_warning_flag("-Wall" ALL)
    pge_add_warning_flag("-Wextra" EXTRA)
    if(NOT HAVE_W_EXTRA)
        pge_add_warning_flag("-W" W)
    endif()
    pge_add_warning_flag("-Wpedantic" PEDANTIC_WARNING)
    pge_disable_warning_flag("variadic-macros" NO_VARIADIC_MACROS_WARNING)
    pge_disable_warning_flag("psabi" NO_PSABI_WARNING)
    pge_disable_warning_flag("dangling-reference" NO_DANGLING_REFERENCE_WARNING)
endif()


# ================================ Tweaks ====================================

# -fPIC thing
if(LIBRARY_PROJECT AND NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

function(pge_cxx_standard STDVER)
    if(NOT WIN32)
        set(CMAKE_CXX_STANDARD ${STDVER} PARENT_SCOPE)
    elseif(MSVC AND CMAKE_VERSION VERSION_LESS "3.9.0" AND MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++${STDVER}" _cpp_stdxx_flag_supported)
        if (_cpp_stdxx_flag_supported)
            add_compile_options("/std:c++${STDVER}")
        else()
            CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
            if (_cpp_latest_flag_supported)
                add_compile_options("/std:c++latest")
            endif()
        endif()
    else()
        set(CMAKE_CXX_STANDARD ${STDVER} PARENT_SCOPE)
    endif()
endfunction()
