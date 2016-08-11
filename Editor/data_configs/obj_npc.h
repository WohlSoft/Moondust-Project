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
#ifndef OBJ_NPC_H
#define OBJ_NPC_H

#include <QString>
#include <QPixmap>
#include <ConfigPackManager/level/config_npc.h>

// //Defines:// //
//  obj_npc     //
//  npc_Markers //
// //////////// //

struct obj_npc
{
    obj_npc();
    bool            isValid;
    unsigned long   animator_id;
    QPixmap *       cur_image;
    QPixmap         image;

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_npc &npc);
    NpcSetup setup;
};

struct npc_Markers
{
    //    ;Defines for SMBX64
    unsigned long bubble;   //    bubble=283	; NPC-Container for packed in bubble
    unsigned long egg;      //    egg=96		; NPC-Container for packed in egg
    unsigned long lakitu;   //    lakitu=284	; NPC-Container for spawn by lakitu
    unsigned long buried;   //    burred=91	; NPC-Container for packed in herb

    unsigned long ice_cube; //    icecube=263	; NPC-Container for frozen NPCs

    //    ;markers
    unsigned long iceball;  //    iceball=265
    unsigned long fireball; //    fireball=13
    unsigned long hammer;   //    hammer=171
    unsigned long boomerang;//    boomerang=292
    unsigned long coin_in_block; //    coin-in-block=10
};


#endif // OBJ_NPC_H
