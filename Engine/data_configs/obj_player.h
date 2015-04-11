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

#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include "../graphics/graphics.h"

#include <QString>
#include <QPixmap>
#include <QMap>
#include <QList>
class QSettings;

/**********************Player's calibration**********************/
struct AniFrame
{
    int x;
    int y;
};

struct AniFrameSet
{
    QVector<AniFrame > L;
    QVector<AniFrame > R;
    QString name;
};

struct FrameSets
{
    QVector<AniFrameSet > set;
};

struct frameOpts
{
    unsigned int H;
    unsigned int W;
    int offsetX;
    int offsetY;
    bool used;
    bool isDuck;
    bool isRightDir;
    bool showGrabItem;
};

struct obj_player_calibration
{
    int frameWidth;
    int frameHeight;
    int frameHeightDuck;
    int frameGrabOffsetX;
    int frameGrabOffsetY;
    int frameOverTopGrab;
    QList<QList<frameOpts > > framesX; //!< Collision boxes settings
    FrameSets AniFrames;               //!< Animation settings
    void init(int x, int y);
    bool load(QString fileName);
private:
    void getSpriteAniData(QSettings &set, QString name);
};
/**********************Player's calibration**********************/


/****************Definition of playable character state*******************/
struct obj_player_physics
{
    inline void make() {} //!< Dummy function
    float walk_force; //!< Move force
    float slippery_c; //!< Slippery coefficien
    float gravity_scale; //!< Gravity scale
    float velocity_jump; //!< Jump velocity
    float velocity_climb; //!< Climbing velocity
    float MaxSpeed_walk; //!< Max walk speed
    float MaxSpeed_run; //!< Max run speed
    float MaxSpeed_up; //!< Fly UP Max fall speed
    float MaxSpeed_down; //!< Max fall down speed
    float damping;
    bool    zero_speed_y_on_enter;
    bool    slow_speed_x_on_enter;
};

struct obj_player_state
{
    inline void make() {} //!< Dummy function
    int width;
    int height;
   bool duck_allow;
    int duck_height;
    bool allow_floating;
    int floating_max_time;
    QHash<int, obj_player_physics > phys;
    QString event_script;//!< LUA-Script with events

    obj_player_calibration sprite_setup;

    QString image_n;
    QString mask_n;
    /*   OpenGL    */
    bool isInit;
    PGE_Texture *image;
    GLuint textureID;
    long textureArrayId;
    long animator_ID;
    /*   OpenGL    */
};


/******************Definition of playable character*********************/
struct obj_player
{
    inline void make();//!< Dummy function
    unsigned long id;
        QString image_wld_n;
        QString mask_wld_n;

    int matrix_width;
    int matrix_height;

    //Size of one frame (will be calculated automatically!)
    int frame_width;
    int frame_height;

    /*   OpenGL    */
        //for world map
    bool isInit_wld;
    PGE_Texture *image_wld;
    GLuint textureID_wld;
    long textureArrayId_wld;
    long animator_ID_wld;
    /*   OpenGL    */

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
    QList<int > wld_frames_up;
    QList<int > wld_frames_right;
    QList<int > wld_frames_down;
    QList<int > wld_frames_left;
/* World map */

    QHash<int, obj_player_state > states;
    QHash<int, obj_player_physics > phys_default;

    bool allowFloating;
};

#endif // OBJ_PLAYER_H

