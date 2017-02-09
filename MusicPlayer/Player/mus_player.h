#ifndef MUS_PLAYER_H
#define MUS_PLAYER_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>

#ifdef MUSPLAY_USE_WINAPI
#define DebugLog(msg)
#include <assert.h>
#include "../defines.h"
#else
#include <QString>
#include <QDebug>
#define DebugLog(msg) qDebug() << msg;
#endif

namespace PGE_MusicPlayer
{
    extern Mix_Music *play_mus;
    extern Mix_MusicType type;
    extern bool reverbEnabled;
    extern void setMainWindow(void *mwp);
    extern const char* musicTypeC();
    extern QString musicType();
    extern void MUS_stopMusic();
    #ifndef MUSPLAY_USE_WINAPI
    extern QString MUS_getMusTitle();
    extern QString MUS_getMusArtist();
    extern QString MUS_getMusAlbum();
    extern QString MUS_getMusCopy();
    #else
    extern const char* MUS_getMusTitle();
    extern const char* MUS_getMusArtist();
    extern const char* MUS_getMusAlbum();
    extern const char* MUS_getMusCopy();
    #endif
    extern void setPlayListMode(bool playList);
    extern void MUS_playMusic();
    extern void MUS_changeVolume(int volume);
    extern bool MUS_openFile(QString musFile);
    extern void startWavRecording(QString target);
    extern void stopWavRecording();
}

#endif // MUS_PLAYER_H
