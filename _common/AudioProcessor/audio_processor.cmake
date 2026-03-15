
option(ENABLE_AUDIO_PROCESSOR_FFMPEG_ENCODE "Enable encoding support for FFMPEG" OFF)
option(ENABLE_AUDIO_PROCESSOR_FFMPEG_DECODE "Enable decoding support for FFMPEG" ON)

option(ENABLE_AUDIO_PROCESSOR_OPUS_ENCODE "Enable encoding support for OPUS" ON)
option(ENABLE_AUDIO_PROCESSOR_OPUS_DECODE "Enable decoding support for OPUS" ON)

option(ENABLE_AUDIO_PROCESSOR_MP3_ENCODE "Enable encoding support for MP3 using Lame" ON)
option(ENABLE_AUDIO_PROCESSOR_MP3_DECODE_MPG123 "Enable decoding support for MP3 using MPG123" ON)

add_library(libMoondustAudio STATIC)

if(ENABLE_AUDIO_PROCESSOR_MP3_ENCODE OR ENABLE_AUDIO_PROCESSOR_MP3_DECODE_MPG123)
    target_sources(libMoondustAudio PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/codec/audio_mp3.cpp
        ${CMAKE_CURRENT_LIST_DIR}/codec/audio_mp3.h
        ${CMAKE_CURRENT_LIST_DIR}/codec/mp3/mp3utils.c
        ${CMAKE_CURRENT_LIST_DIR}/codec/mp3/mp3utils.h
    )

    if(ENABLE_AUDIO_PROCESSOR_MP3_ENCODE)
        target_compile_definitions(libMoondustAudio PRIVATE -DMOONDUST_ENCODE_MP3)
    endif()

    if(ENABLE_AUDIO_PROCESSOR_MP3_DECODE_MPG123)
        target_compile_definitions(libMoondustAudio PRIVATE -DMOONDUST_DECODE_MP3)
    endif()
endif()

if(ENABLE_AUDIO_PROCESSOR_OPUS_ENCODE OR ENABLE_AUDIO_PROCESSOR_OPUS_DECODE)
    target_sources(libMoondustAudio PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/codec/audio_opus.cpp
        ${CMAKE_CURRENT_LIST_DIR}/codec/audio_opus.h
    )

    target_include_directories(libMoondustAudio PRIVATE
        /usr/include/opus
    )

    if(ENABLE_AUDIO_PROCESSOR_OPUS_ENCODE)
        target_compile_definitions(libMoondustAudio PRIVATE -DMOONDUST_ENCODE_OPUS)
    endif()

    if(ENABLE_AUDIO_PROCESSOR_OPUS_DECODE)
        target_compile_definitions(libMoondustAudio PRIVATE -DMOONDUST_DECODE_OPUS)
    endif()
endif()

if(ENABLE_AUDIO_PROCESSOR_FFMPEG_ENCODE OR ENABLE_AUDIO_PROCESSOR_FFMPEG_DECODE)
    target_sources(libMoondustAudio PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/codec/audio_ffmpeg.cpp
        ${CMAKE_CURRENT_LIST_DIR}/codec/audio_ffmpeg.h
    )

    if(ENABLE_AUDIO_PROCESSOR_FFMPEG_ENCODE)
        target_compile_definitions(libMoondustAudio PRIVATE -DMOONDUST_ENCODE_FFMPEG)
    endif()

    if(ENABLE_AUDIO_PROCESSOR_FFMPEG_DECODE)
        target_compile_definitions(libMoondustAudio PRIVATE -DMOONDUST_DECODE_FFMPEG)
    endif()
endif()

target_sources(libMoondustAudio PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/audio_processor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio_processor.h
    ${CMAKE_CURRENT_LIST_DIR}/audio_detect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio_detect.h
    ${CMAKE_CURRENT_LIST_DIR}/audio_file.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio_file.h
    ${CMAKE_CURRENT_LIST_DIR}/audio_format.h
    ${CMAKE_CURRENT_LIST_DIR}/music_args.cpp
    ${CMAKE_CURRENT_LIST_DIR}/music_args.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_vorbis.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_vorbis.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_flac.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_flac.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_wav.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_wav.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/wav/dr_wav.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_qoa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_qoa.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/qoa/qoa.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_gme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_gme.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_adl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_adl.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_opn.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_opn.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/OPNMIDI/gm_opn_bank.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_edmidi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_edmidi.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_fluidsynth.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_fluidsynth.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/cvt_mus2mid.hpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/cvt_xmi2mid.hpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/file_reader.hpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/fraction.hpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/midi_sequencer.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/midi_sequencer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/midi_sequencer_impl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/mix_midi_seq.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/midi_seq/mix_midi_seq.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_pxtone.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_pxtone.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtn.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnData.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnData.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnDelay.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnDelay.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnError.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnError.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnEvelist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnEvelist.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnMaster.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnMaster.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnMax.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnMem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnMem.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnOverDrive.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnOverDrive.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Frequency.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Frequency.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Noise.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Noise.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_NoiseBuilder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_NoiseBuilder.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Oggv.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Oggv.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Oscillator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_Oscillator.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_PCM.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnPulse_PCM.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnService.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnService.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnService_moo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnText.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnText.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnUnit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnUnit.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtn_version.txt
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnWoice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnWoice.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnWoice_io.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtnWoicePTV.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtoneNoise.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/pxtone/pxtoneNoise.h
)

target_link_libraries(libMoondustAudio PUBLIC
    SDL2 SDL2main
    swresample avformat avcodec avutil
    ogg
    opus opusfile opusenc
    FLAC
    vorbis vorbisfile vorbisenc
    mpg123 mp3lame
    xmp
    ADLMIDI OPNMIDI EDMIDI fluidsynth gme
)

target_compile_options(libMoondustAudio PRIVATE -Wall -Wextra)

target_include_directories(libMoondustAudio PUBLIC ${CMAKE_CURRENT_LIST_DIR})
