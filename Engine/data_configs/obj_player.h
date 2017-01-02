/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include <graphics/graphics.h>
#include <common_features/player_calibration.h>
#include <common_features/data_array.h>
#include "spawn_effect_def.h"

#include <string>
#include <QString>
#include <QPixmap>
#include <QMap>
#include <QList>

/****************Definition of playable character state*******************/
struct obj_player_physics
{
    obj_player_physics();
    inline void make() {} //!< Dummy function
    double walk_force; //!< Move force
    double run_force;  //!< Running force

    double decelerate_stop; //!< Deceleration while stopping
    double decelerate_run;  //!< Deceleration running while speed higher than walking
    double decelerate_turn; //!< Deceleration while turning
    double decelerate_air;  //!< Decelerate in air

    double ground_c_max; //!< On-Ground max speed     coefficient
    double ground_c;     //!< On-Ground accelerations coefficient
    double slippery_c;   //!< Slippery accelerations coefficien

    double gravity_accel; //!< Gravity acceleration
    double gravity_scale; //!< Gravity scale
    double velocity_jump; //!< Jump velocity
    double velocity_jump_bounce; //!< Boubce velocity
    double velocity_jump_spring; //!< Jump velocity on spring
    double velocity_jump_c; //!< Jump coefficient which provides increzed jump height dependent to speed
    int   jump_time;     //!< Time to jump
    int   jump_time_bounce;//!< Time to bounce
    int   jump_time_spring;     //!< Time to jump

    double velocity_climb_x; //!< Climbing velocity
    double velocity_climb_y_up; //!< Climbing velocity
    double velocity_climb_y_down; //!< Climbing velocity

    double MaxSpeed_walk; //!< Max walk speed
    double MaxSpeed_run;  //!< Max run speed

    double MaxSpeed_up;   //!< Fly UP Max fall speed
    double MaxSpeed_down; //!< Max fall down speed

    bool  strict_max_speed_on_ground;//!< reduce speed to max if faster than allowed on ground

    bool    zero_speed_y_on_enter;
    double   slow_up_speed_y_coeff; //!< Coefficient to slow speed if it going up
    bool    slow_speed_x_on_enter;
    double   slow_speed_x_coeff; //!< Coefficient to slow speed
};

struct obj_player_state
{
    std::string name;
    int     width;
    int     height;
    bool    duck_allow;
    int     duck_height;
    bool    allow_floating;
    int     floating_max_time;
    double  floating_amplitude;

    PGE_DataArray<obj_player_physics > phys;
    std::string event_script;   //!< LUA-Script with events

    obj_player_calibration  sprite_setup;

    QString image_n;
    QString mask_n;
    /*   OpenGL    */
    bool            isInit;
    PGE_Texture    *image;
    GLuint          textureID;
    int             textureArrayId;
    int             animator_ID;
    /*   OpenGL    */
};


/******************Definition of playable character*********************/
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

    SpawnEffectDef fail_effect;
    SpawnEffectDef slide_effect;

    /*   OpenGL    */
    //for world map
    bool isInit_wld;
    PGE_Texture *image_wld;
    GLuint textureID_wld;
    int textureArrayId_wld;
    int animator_ID_wld;
    /*   OpenGL    */

    //! LUA-Script of playable character
    QString script;

    std::string name;
    std::string sprite_folder;
    enum StateTypes
    {
        powerup = 0,
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

    PGE_DataArray<obj_player_state > states;
    PGE_DataArray<obj_player_physics > phys_default;

    bool allowFloating;
};

#endif // OBJ_PLAYER_H
