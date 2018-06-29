# Search and link SDL Mixer X and it's dependencies

macro(mixerx_FindLibrary WhereSearch LibToFind ListToAppend)
    find_library(${LibToFind}_DEST_LIBRARY
        NAMES ${LibToFind}
        HINTS ${WhereSearch}
    )
    # message("Search ${LibToFind} but found ${${LibToFind}_DEST_LIBRARY}...")
    if(${LibToFind}_DEST_LIBRARY)
        # message("Append To List ${ListToAppend}...")
        list(APPEND "${ListToAppend}" ${${LibToFind}_DEST_LIBRARY})
    else()
        message(FATAL "Library ${LibToFind} NOT FOUND!")
    endif()
endmacro()

function(find_mixer_x_real)
    cmake_parse_arguments(_mixerx
        "STATIC;SHARED"
        "TARGET"
        "SEARCH_PATHS"
        ${ARGN}
    )

    set(CMAKE_FIND_LIBRARY_SUFFIXES_OLD ${CMAKE_FIND_LIBRARY_SUFFIXES})

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(MIX_DEBUG_SUFFIX "d")
    else()
        set(MIX_DEBUG_SUFFIX "")
    endif()

    if(_mixerx_STATIC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES
            "-static${MIX_DEBUG_SUFFIX}.a"
            "-static${MIX_DEBUG_SUFFIX}.lib"
            "${MIX_DEBUG_SUFFIX}.a"
            "${MIX_DEBUG_SUFFIX}.lib"
            "-static.a"
            "-static.lib"
            ".a"
            ".lib"
        )
    else()
        if(WIN32)
            set(CMAKE_FIND_LIBRARY_SUFFIXES
                "${MIX_DEBUG_SUFFIX}.dll.a"
                "${MIX_DEBUG_SUFFIX}.dll.lib"
                "${MIX_DEBUG_SUFFIX}.a"
                "${MIX_DEBUG_SUFFIX}.lib"
                ".dll.a"
                ".dll.lib"
                ".a"
                ".lib"
            )
        endif()
    endif()

    set(FOUND_LIBS)

    mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} SDL2_mixer_ext FOUND_LIBS)

    if(_mixerx_STATIC)
        if(NOT EMSCRIPTEN)
            mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} FLAC FOUND_LIBS)
        endif()
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} opusfile FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} opus FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} vorbisfile FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} vorbis FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} ogg FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} mad FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} id3tag FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} modplug FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} ADLMIDI FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} OPNMIDI FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} timidity FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} gme FOUND_LIBS)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} zlib FOUND_LIBS)
    endif()

    mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} SDL2 FOUND_LIBS)
    if((WIN32 OR HAIKU) AND NOT EMSCRIPTEN)
        mixerx_FindLibrary(${_mixerx_SEARCH_PATHS} SDL2main FOUND_LIBS)
    endif()

    if(WIN32 AND NOT EMSCRIPTEN)
        list(APPEND FOUND_LIBS
            version opengl32 dbghelp advapi32 kernel32 winmm imm32
        )
    endif()

    if(NOT WIN32 AND NOT EMSCRIPTEN AND NOT APPLE)
        list(APPEND FOUND_LIBS GL)
        if(NOT HAIKU)
            list(APPEND FOUND_LIBS dl)
        endif()
    endif()

    if(HAIKU)
        list(APPEND FOUND_LIBS
            be device game media
        )
    endif()

    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_OLD})

    # message("XXX FOUND LIBS: ${FOUND_LIBS} XXX")
    set("${_mixerx_TARGET}" ${FOUND_LIBS} PARENT_SCOPE)
endfunction()

function(find_mixer_x)
    cmake_parse_arguments(_mixerx
        "STATIC;SHARED"
        "TARGET"
        "SEARCH_PATHS"
        ${ARGN}
    )

    set(CMAKE_FIND_LIBRARY_SUFFIXES_OLD ${CMAKE_FIND_LIBRARY_SUFFIXES})

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(MIX_DEBUG_SUFFIX "d")
    else()
        set(MIX_DEBUG_SUFFIX "")
    endif()

    if(_mixerx_STATIC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES
            "-static${MIX_DEBUG_SUFFIX}.a"
            "-static${MIX_DEBUG_SUFFIX}.lib"
            "${MIX_DEBUG_SUFFIX}.a"
            "${MIX_DEBUG_SUFFIX}.lib"
            "-static.a"
            "-static.lib"
            ".a"
            ".lib"
        )
    else()
        if(WIN32)
            set(CMAKE_FIND_LIBRARY_SUFFIXES
                "${MIX_DEBUG_SUFFIX}.dll.a"
                "${MIX_DEBUG_SUFFIX}.dll.lib"
                "${MIX_DEBUG_SUFFIX}.a"
                "${MIX_DEBUG_SUFFIX}.lib"
                ".dll.a"
                ".dll.lib"
                ".a"
                ".lib"
            )
        endif()
    endif()

    set(FOUND_LIBS)

    if(_mixerx_STATIC)
        if(WIN32)
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext-static${MIX_DEBUG_SUFFIX}.a")
        else()
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext${MIX_DEBUG_SUFFIX}.a")
        endif()
    else()
        if(WIN32)
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext${MIX_DEBUG_SUFFIX}.dll.a")
        elseif(APPLE)
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext${MIX_DEBUG_SUFFIX}.dylib")
        else()
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2_mixer_ext${MIX_DEBUG_SUFFIX}.so")
        endif()
    endif()

    if(_mixerx_STATIC)
        if(NOT EMSCRIPTEN)
            list(APPEND FOUND_LIBS "FLAC${MIX_DEBUG_SUFFIX}")
        endif()
        list(APPEND FOUND_LIBS "opusfile${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "opus${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "vorbisfile${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "vorbis${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "ogg${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "mad${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "id3tag${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "modplug${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "ADLMIDI${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "OPNMIDI${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "timidity${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "gme${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "timidity${MIX_DEBUG_SUFFIX}")
        list(APPEND FOUND_LIBS "zlib${MIX_DEBUG_SUFFIX}")
    endif()

    if(_mixerx_STATIC)
        if(WIN32)
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2-static${MIX_DEBUG_SUFFIX}.a")
        else()
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2${MIX_DEBUG_SUFFIX}.a")
        endif()
    else()
        if(WIN32)
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2${MIX_DEBUG_SUFFIX}.dll.a")
        elseif(APPLE)
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2${MIX_DEBUG_SUFFIX}.dylib")
        else()
            list(APPEND FOUND_LIBS "${_mixerx_SEARCH_PATHS}/libSDL2${MIX_DEBUG_SUFFIX}.so")
        endif()
    endif()

    if((WIN32 OR HAIKU) AND NOT EMSCRIPTEN)
        list(APPEND FOUND_LIBS "SDL2main${MIX_DEBUG_SUFFIX}")
    endif()

    if(WIN32 AND NOT EMSCRIPTEN)
        list(APPEND FOUND_LIBS
            version opengl32 dbghelp advapi32 kernel32 winmm imm32
        )
    endif()

    if(NOT WIN32 AND NOT EMSCRIPTEN AND NOT APPLE)
        list(APPEND FOUND_LIBS GL)
        if(NOT HAIKU)
            list(APPEND FOUND_LIBS dl)
        endif()
    endif()

    if(HAIKU)
        list(APPEND FOUND_LIBS
            be device game media
        )
    endif()

    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_OLD})

    # message("XXX FOUND LIBS: ${FOUND_LIBS} XXX")
    set("${_mixerx_TARGET}" ${FOUND_LIBS} PARENT_SCOPE)
endfunction()
