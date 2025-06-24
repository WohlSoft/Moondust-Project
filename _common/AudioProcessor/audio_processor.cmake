
set(AUDIO_PROCESSOR_SRC
    ${CMAKE_CURRENT_LIST_DIR}/audio_processor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio_processor.h
    ${CMAKE_CURRENT_LIST_DIR}/audio_file.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio_file.h
    ${CMAKE_CURRENT_LIST_DIR}/audio_format.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_vorbis.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_vorbis.h
)

set(AUDIO_PROCESSOR_LIBS SDL2 SDL2main ogg opus opusfile FLAC vorbis vorbisfile vorbisenc mpg123 mp3lame)
set(AUDIO_PROCESSOR_INCLUDES ${CMAKE_CURRENT_LIST_DIR})
