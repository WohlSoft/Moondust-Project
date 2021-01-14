/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_EFFECT_H
#define OBJ_EFFECT_H

#include "../graphics/graphics.h"
#include "spawn_effect_def.h"

#include <string>

struct obj_effect
{
    unsigned long id;
    std::string image_n;
    std::string mask_n;
    //QPixmap image;
    //QPixmap mask;

    /*   OpenGL    */
    bool isInit;
    PGE_Texture *image;
    GLuint textureID;
    int textureArrayId;
    /*   OpenGL    */

    std::string name;

    unsigned int frames;
    int framespeed;
    int framestyle;
    int frame_h; //!< Captured frame height
};

struct Effects_GlobalSetup
{
    /* Common */
    //!Default effect for unknown, damaged, or unconfigured effects
    SpawnEffectDef m_default;
    //!Smoke effect on layer showing/hidding
    SpawnEffectDef m_smoke;
    //!Water splash effect
    SpawnEffectDef m_waterSplash;

    /* Playable character */
    //!Playable characters transform
    SpawnEffectDef m_playerTransform;
    //!Smoke effect on ground sliding
    SpawnEffectDef m_groundSlide;
    //!Enemy stomp effect
    SpawnEffectDef m_stomp;
    //!Enemy smash effect
    SpawnEffectDef m_smash;

    /* Blocks */
    //!Block destroyed effect
    SpawnEffectDef m_blockDestroy;
    //!Coin kicked out from a block
    SpawnEffectDef m_blockCoinHit;

};
#endif // OBJ_EFFECT_H
