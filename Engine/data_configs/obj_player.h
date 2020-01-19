/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include <graphics/graphics.h>
#include <common_features/player_calibration.h>
#include <common_features/data_array.h>
#include "spawn_effect_def.h"

#include <string>
#include <vector>

/****************Definition of playable character state*******************/
struct obj_player_physics
{
    /**
     * @brief Dummy function used to poke hash table initialize entry
     */
    inline void make() {}
    //! Move force
    double walk_force = 6.5;
    //! Running force
    double run_force  = 3.25;

    //! Deceleration while stopping
    double decelerate_stop = 4.55;
    //! Deceleration running while speed higher than walking
    double decelerate_run  = 10.88;
    //! Deceleration while turning
    double decelerate_turn = 18.2;
    //! Decelerate in air
    double decelerate_air  = 0.0;

    //! On-Ground max speed     coefficient
    double ground_c_max = 1.0;
    //! On-Ground accelerations coefficient
    double ground_c     = 1.0;
    //! Slippery accelerations coefficien
    double slippery_c   = 4.0;

    //! Gravity acceleration
    double gravity_accel = 26.0;
    //! Gravity scale
    double gravity_scale = 1.0;
    //! Jump velocity
    double velocity_jump = 5.2;
    //! Boubce velocity
    double velocity_jump_bounce = 5.3;
    //! Jump velocity on spring
    double velocity_jump_spring = 9.3;
    //! Jump coefficient which provides increzed jump height dependent to speed
    double velocity_jump_c = 5.8;
    //! Time to jump
    int   jump_time = 260;
    //! Time to bounce
    int   jump_time_bounce = 370;
    //! Time to jump
    int   jump_time_spring = 530;

    //! Climbing velocity
    double velocity_climb_x      = 1.5;
    //! Climbing velocity
    double velocity_climb_y_up   = 2.0;
    //! Climbing velocity
    double velocity_climb_y_down = 3.0;

    //! Max walk speed
    double MaxSpeed_walk     = 3.0;
    //! Max run speed
    double MaxSpeed_run      = 6.0;

    //! Fly UP Max fall speed
    double MaxSpeed_up       = 74.0;
    //! Max fall down speed
    double MaxSpeed_down     = 12.0;

    //! reduce speed to max if faster than allowed on ground
    bool  strict_max_speed_on_ground = false;

    bool    zero_speed_y_on_enter = false;
    //! Coefficient to slow speed if it going up
    double   slow_up_speed_y_coeff = 0.325;
    bool    slow_speed_x_on_enter = false;
    //! Coefficient to slow speed
    double   slow_speed_x_coeff = 0.125;
};

struct obj_player_state
{
    std::string name;
    int     width = 32;
    int     height = 60;
    bool    duck_allow = true;
    int     duck_height = 30;
    bool    allow_floating = false;
    int     floating_max_time = 0;
    double  floating_amplitude = 0.0;

    PGE_DataArray<obj_player_physics > phys;
    std::string event_script;   //!< LUA-Script with events

    obj_player_calibration  sprite_setup;

    std::string image_n;
    std::string mask_n;
    /*   OpenGL    */
    bool            isInit = false;
    PGE_Texture    *image = nullptr;
    GLuint          textureID = 0;
    int             textureArrayId = 0;
    int             animator_ID = 0;
    /*   OpenGL    */
};


/******************Definition of playable character*********************/
struct obj_player
{
    unsigned long id = 0;
    std::string image_wld_n;
    std::string mask_wld_n;

    int matrix_width    = 10;
    int matrix_height   = 10;

    //Size of one frame (will be calculated automatically!)
    int frame_width     = 100;
    int frame_height    = 100;

    SpawnEffectDef fail_effect;
    SpawnEffectDef slide_effect;

    /*   OpenGL    */
    //for world map
    bool            isInit_wld = false;
    PGE_Texture     *image_wld = nullptr;
    GLuint          textureID_wld = 0;
    int             textureArrayId_wld = 0;
    int             animator_ID_wld = 0;
    /*   OpenGL    */

    //! LUA-Script of playable character
    std::string     script;

    std::string     name;
    std::string     sprite_folder;

    enum StateTypes
    {
        powerup = 0,
        suites
    };
    int             state_type = 0;

    /* World map */
    int     wld_framespeed = 128;
    int     wld_frames = 1;
    int     wld_offset_y = 0;
    std::vector<int> wld_frames_up;
    std::vector<int> wld_frames_right;
    std::vector<int> wld_frames_down;
    std::vector<int> wld_frames_left;
    /* World map */

    PGE_DataArray<obj_player_state >    states;
    PGE_DataArray<obj_player_physics >  phys_default;

    bool allowFloating = false;
};

#endif // OBJ_PLAYER_H
