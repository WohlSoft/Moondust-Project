qt5_wrap_ui(UIS_HDRS
    MainWindow/mainwindow.ui
    MainWindow/multi_music_test.ui
    MainWindow/multi_music_item.ui
    MainWindow/multi_sfx_test.ui
    MainWindow/multi_sfx_item.ui
    MainWindow/musicfx.ui
    MainWindow/sfx_tester.ui
    MainWindow/setup_audio.ui
    MainWindow/setup_midi.ui
    MainWindow/track_muter.ui
    MainWindow/echo_tune.ui
    MainWindow/reverb_tune.ui
    AssocFiles/assoc_files.ui
)

set(RESOURCE ${SDLMixerMusPlayer_SOURCE_DIR}/_resources/musicplayer.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})


list(APPEND SDLMixer_MusPlay_SRC
    AssocFiles/assoc_files.cpp
    Effects/reverb.cpp
    Effects/spc_echo.cpp
    MainWindow/flowlayout.cpp
    MainWindow/label_marquee.cpp
    MainWindow/multi_music_test.cpp
    MainWindow/multi_music_item.cpp
    MainWindow/multi_sfx_test.cpp
    MainWindow/multi_sfx_item.cpp
    MainWindow/musplayer_base.cpp
    MainWindow/musplayer_qt.cpp
    MainWindow/musicfx.cpp
    MainWindow/seek_bar.cpp
    MainWindow/setup_audio.cpp
    MainWindow/setup_midi.cpp
    MainWindow/sfx_tester.cpp
    MainWindow/track_muter.cpp
    MainWindow/echo_tune.cpp
    MainWindow/reverb_tune.cpp
    Player/mus_player.cpp
    SingleApplication/localserver.cpp
    SingleApplication/pge_application.cpp
    SingleApplication/singleapplication.cpp
    main.cpp
    wave_writer.c

    MainWindow/snes_spc/dsp.cpp
    MainWindow/snes_spc/SNES_SPC.cpp
    MainWindow/snes_spc/SNES_SPC_misc.cpp
    MainWindow/snes_spc/SNES_SPC_state.cpp
    MainWindow/snes_spc/spc.cpp
    MainWindow/snes_spc/SPC_DSP.cpp
    MainWindow/snes_spc/SPC_Filter.cpp
)

if(APPLE)
    add_definitions(-DQ_OS_MACX) # Workaround for MOC
    set(PGE_FILE_ICONS "${SDLMixerMusPlayer_SOURCE_DIR}/_resources/file_musplay.icns")
    list(APPEND SDLMixer_MusPlay_SRC
        _resources/cat_musplay.icns
        ${PGE_FILE_ICONS}
    )
endif()

if(WIN32)
    list(APPEND SDLMixer_MusPlay_SRC _resources/musicplayer.rc)
endif()
