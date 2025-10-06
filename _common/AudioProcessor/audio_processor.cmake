
set(AUDIO_PROCESSOR_SRC
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

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_qoa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_qoa.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/qoa/qoa.h

    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_adl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_adl.h
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_opn.cpp
    ${CMAKE_CURRENT_LIST_DIR}/codec/audio_midi_opn.h

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

set(AUDIO_PROCESSOR_LIBS
    SDL2 SDL2main
    ogg
    opus opusfile
    FLAC
    vorbis vorbisfile vorbisenc
    mpg123 mp3lame
    xmp
    ADLMIDI OPNMIDI EDMIDI gme
)
set(AUDIO_PROCESSOR_INCLUDES ${CMAKE_CURRENT_LIST_DIR})
