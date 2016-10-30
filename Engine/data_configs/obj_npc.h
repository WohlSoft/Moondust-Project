/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_NPC_H
#define OBJ_NPC_H

#include <QString>
#include <QStringList>
#include <common_features/size.h>
#include <ConfigPackManager/level/config_npc.h>

#include "../graphics/graphics.h"
#include "spawn_effect_def.h"

// //Defines:// //
//  obj_npc     //
//  npc_Markers //
// //////////// //

struct obj_npc
{
    /*   OpenGL    */
    bool isInit;
    PGE_Texture *image;
    GLuint textureID;
    int textureArrayId;
    int animator_ID;
    PGE_Size      image_size;
    /*   OpenGL    */

    //! Generic NPC settings and parameters
    NpcSetup setup;

    SpawnEffectDef effect_1_def;
    SpawnEffectDef effect_2_def;

    enum blockSpawn
    {
        spawn_warp = 0,
        spawn_bump
    };
    //!Type of NPC spawn from block
    unsigned int block_spawn_type;
    //!NPC's initial Y Velocity after spawn from block
    double       block_spawn_speed;
    //!Play sound on spawn from block (if false - spawn without 'radish' sound)
    bool         block_spawn_sound;
};



struct NPC_GlobalSetup
{
    //    ;Defines for SMBX64
    unsigned long bubble;   //    bubble=283    ; NPC-Container for packed in bubble
    unsigned long egg;      //    egg=96        ; NPC-Container for packed in egg
    unsigned long lakitu;   //    lakitu=284    ; NPC-Container for spawn by lakitu
    unsigned long buried;   //    burred=91 ; NPC-Container for packed in herb

    unsigned long ice_cube; //    icecube=263   ; NPC-Container for frozen NPCs

    //    ;markers
    unsigned long iceball;  //    iceball=265
    unsigned long fireball; //    fireball=13
    unsigned long hammer;   //    hammer=171
    unsigned long boomerang;//    boomerang=292
    unsigned long coin_in_block; //    coin-in-block=10

    // some physics settings
    double phs_gravity_accel;
    double phs_max_fall_speed;
    //effects
    unsigned long eff_lava_burn; //Lava burn effect [Effect to spawn on contact with lava]

    //projectile properties
    SpawnEffectDef projectile_effect;
    long    projectile_sound_id;
    double  projectile_speed;

    //Talking NPC's properties
    QString talking_sign_img;
};

#endif // OBJ_NPC_H
