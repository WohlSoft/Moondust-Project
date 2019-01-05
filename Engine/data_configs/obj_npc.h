/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_NPC_H
#define OBJ_NPC_H

#include <string>
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
    uint64_t    bubble;   //    bubble=283    ; NPC-Container for packed in bubble
    uint64_t    egg;      //    egg=96        ; NPC-Container for packed in egg
    uint64_t    lakitu;   //    lakitu=284    ; NPC-Container for spawn by lakitu
    uint64_t    buried;   //    burred=91 ; NPC-Container for packed in herb

    uint64_t    ice_cube; //    icecube=263   ; NPC-Container for frozen NPCs

    //    ;markers
    uint64_t    iceball;  //    iceball=265
    uint64_t    fireball; //    fireball=13
    uint64_t    hammer;   //    hammer=171
    uint64_t    boomerang;//    boomerang=292
    uint64_t    coin_in_block; //    coin-in-block=10

    // some physics settings
    double      phs_gravity_accel;
    double      phs_max_fall_speed;
    //effects
    uint64_t    eff_lava_burn; //Lava burn effect [Effect to spawn on contact with lava]

    //projectile properties
    SpawnEffectDef projectile_effect;
    uint64_t    projectile_sound_id;
    double      projectile_speed;

    //Talking NPC's properties
    std::string talking_sign_img;
};

#endif // OBJ_NPC_H
