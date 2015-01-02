/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SDLMUSIC_PLAYER_H
#define SDLMUSIC_PLAYER_H

#include <QString>
#include <QObject>
#ifdef USE_QMEDIAPLAYER
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QUrl>
#endif

#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class PGE_MusPlayer : public QObject
{
    Q_OBJECT
public:
    static void MUS_stopMusic();

    static void MUS_playMusic();

    static void MUS_changeVolume(int vlm);
    static void MUS_openFile(QString musFile);
    static void setSampleRate(int sampleRate);
    static int sampleRate();
    static int currentVolume();

public slots:
    void setVolume(int volume);

private:
    static Mix_Music *play_mus;
    #ifdef USE_QMEDIAPLAYER
    static bool isMediaPlayer;
    static QMediaPlayer *musicPlayer;
    static QMediaPlaylist *playList;
    #endif
    static int volume;
    static int sRate;
};

class PGE_Sounds : public QObject
{
    Q_OBJECT
public:
    static void SND_PlaySnd(QString sndFile);

private:
    #ifdef USE_QMEDIAPLAYER
    static QMediaPlayer *mp3Play;
    #else
    static Mix_Music *mp3_sound;
    #endif
    static bool isMp3;
    static Mix_Chunk *sound;
    static QString current;
};

#endif // MUSIC_PLAYER_H
