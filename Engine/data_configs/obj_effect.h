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

#ifndef OBJ_EFFECT_H
#define OBJ_EFFECT_H

#include "../graphics/graphics.h"
#include "spawn_effect_def.h"

#include <QString>
#include <QPixmap>

struct obj_effect
{
    unsigned long id;
    QString image_n;
    QString mask_n;
    //QPixmap image;
    //QPixmap mask;

    /*   OpenGL    */
    bool isInit;
    PGE_Texture *image;
    GLuint textureID;
    int textureArrayId;
    /*   OpenGL    */

    QString name;

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
