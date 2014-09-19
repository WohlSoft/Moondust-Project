/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class PGE_MusPlayer : public QObject
{
    Q_OBJECT
public slots:
    void setVolume(int volume);

};

namespace PGE_MusicPlayer
{
    void MUS_stopMusic();

    void MUS_playMusic();

    void MUS_changeVolume(int volume);
    void MUS_openFile(QString musFile);
}


#endif // MUSIC_PLAYER_H
