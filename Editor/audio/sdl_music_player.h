/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef SDLMUSIC_PLAYER_H
#define SDLMUSIC_PLAYER_H

#include <QString>
#include <QObject>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>

namespace MixerX
{
extern void initAudio(int frequency, Uint16 format = AUDIO_F32, int channels = 2, int chunksize = 512);
extern int  sampleRate();
extern void closeAudio();
}

class PGE_MusPlayer : public QObject
{
    Q_OBJECT
    friend void MixerX::initAudio(int,Uint16,int,int);

public:
    static void changeVolume(int vlm);
    static int  currentVolume();

    static void openFile(QString musFile);
    static void closeFile();
    static void stop();
    static void play();

    static void setTempo(double tempo);

public slots:
    void setVolume(int volume);

private:
    static Mix_Music *play_mus;
    static int volume;
    static int sRate;
    static QString currentMusic;
};

class PGE_SfxPlayer : public QObject
{
    Q_OBJECT
public:
    static void playFile(QString sndFile);
    static void freeBuffer();

private:
    static Mix_Chunk *sound;
    static QString currentSound;
};

#endif // MUSIC_PLAYER_H
