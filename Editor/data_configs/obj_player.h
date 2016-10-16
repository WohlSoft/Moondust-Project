/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include <QString>
#include <QList>

struct obj_player_state
{
    QString name;
};

struct obj_player
{
    unsigned long id;
    QString image_wld_n;
    QString mask_wld_n;

    int matrix_width;
    int matrix_height;

    //Size of one frame (will be calculated automatically!)
    int frame_width;
    int frame_height;

    //! LUA-Script of playable character
    QString script;

    QString name;
    QString sprite_folder;
    enum StateTypes
    {
        powerup=0,
        suites
    };
    int state_type;

    /* World map */
    int wld_framespeed;
    int wld_frames;
    int wld_offset_y;
    QList<int > wld_frames_up;
    QList<int > wld_frames_right;
    QList<int > wld_frames_down;
    QList<int > wld_frames_left;
    /* World map */

    int statesCount;
    QList<obj_player_state> states;
};

#endif // OBJ_PLAYER_H
