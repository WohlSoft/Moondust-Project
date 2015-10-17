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

#include "npctxt_manager.h"

#if defined(PGE_EDITOR)
#include <common_features/logger.h>
#endif

obj_npc mergeNPCConfigs(obj_npc &global, NPCConfigFile &local, QSize captured)
{
    obj_npc merged;
    merged = global;

#if defined(PGE_EDITOR)
    merged.image = QPixmap();   //Clear image values
    merged.mask = QPixmap();
#endif

    merged.name = (local.en_name)?local.name:global.name;

    merged.image_n = (local.en_image)?local.image:global.image_n;

    merged.algorithm_script = (local.en_script)?local.script:global.algorithm_script;

    merged.gfx_offset_x = (local.en_gfxoffsetx)?local.gfxoffsetx:global.gfx_offset_x;
    merged.gfx_offset_y = (local.en_gfxoffsety)?local.gfxoffsety:global.gfx_offset_y;

    merged.width = (local.en_width)?local.width:global.width;
    merged.height = (local.en_height)?local.height:global.height;

    merged.foreground = (local.en_foreground)?local.foreground:global.foreground;

    merged.framespeed = (local.en_framespeed)? qRound( qreal(global.framespeed) / (qreal(8) / qreal(local.framespeed)) ) : global.framespeed;
    merged.framestyle = (local.en_framestyle)?local.framestyle:global.framestyle;

    //Copy physical size to GFX size
    if( (local.en_width) && (merged.custom_physics_to_gfx) )
        merged.gfx_w = merged.width;
    else
    {
        if((!local.en_gfxwidth)&&(captured.width()!=0)&&(global.gfx_w!=captured.width()))
            merged.width = captured.width();

        merged.gfx_w = ( (captured.width()!=0) ? captured.width() : global.gfx_w );
    }

    //Copy physical size to GFX size
    if( (local.en_height) && (merged.custom_physics_to_gfx) )
        merged.gfx_h = merged.height;
    else
        merged.gfx_h = global.gfx_h;


    if((!local.en_gfxwidth)&&(captured.width()!=0)&&(global.gfx_w!=captured.width()))
        merged.gfx_w = captured.width();
    else
        merged.gfx_w = (local.en_gfxwidth) ? (local.gfxwidth>0 ? local.gfxwidth : 1 ) : merged.gfx_w;

    merged.gfx_h = (local.en_gfxheight) ? (local.gfxheight>0 ? local.gfxheight : 1 ) : merged.gfx_h;


    if(((int)merged.width>=(int)merged.grid))
        merged.grid_offset_x = -1 * qRound( qreal((int)merged.width % merged.grid)/2 );
    else
        merged.grid_offset_x = qRound( qreal( merged.grid - (int)merged.width )/2 );

    if(merged.grid_attach_style==1) merged.grid_offset_x += 16;

    merged.grid_offset_y = -merged.height % merged.grid;


    if((merged.framestyle==0)&&((local.en_gfxheight)||(local.en_height))&&(!local.en_frames))
    {
        merged.frames = qRound(qreal(captured.height())/qreal(merged.gfx_h));
        //merged.custom_animate = false;
    }
    else
        merged.frames = (local.en_frames)?local.frames:global.frames;

    if((local.en_frames)||(local.en_framestyle))
    {
        merged.ani_bidir = false; //Disable bidirectional animation
        if((local.en_frames)) merged.custom_animate = false; //Disable custom animation
    }

    // Convert out of range frames by framestyle into animation with controlable diraction
    if((merged.framestyle>0)&&(merged.gfx_h*merged.frames >= (unsigned int)captured.height()))
    {
        merged.framestyle = 0;
        merged.ani_direct = false;
        merged.ani_directed_direct = true;
    }
    if((global.framestyle==0)&&(local.framestyle>0))
    {
        merged.ani_direct = false;
        merged.ani_directed_direct = false;
    }

    merged.score = (local.en_score)?local.score:global.score;
    merged.health = (local.en_health)?local.health:global.health;
    merged.block_player = (local.en_playerblock)?local.playerblock:global.block_player;
    merged.block_player_top = (local.en_playerblocktop)?local.playerblocktop:global.block_player_top;
    merged.block_npc = (local.en_npcblock)?local.npcblock:global.block_npc;
    merged.block_npc_top = (local.en_npcblocktop)?local.npcblocktop:global.block_npc_top;
    merged.grab_side = (local.en_grabside)?local.grabside:global.grab_side;
    merged.grab_top = (local.en_grabtop)?local.grabtop:global.grab_top;
    merged.kill_on_jump = (local.en_jumphurt)? (!local.jumphurt) : global.kill_on_jump ;
    merged.hurt_player = (local.en_nohurt)?!local.nohurt:global.hurt_player;
    merged.collision_with_blocks = (local.en_noblockcollision)?(!local.noblockcollision):global.collision_with_blocks;
    merged.turn_on_cliff_detect = (local.en_cliffturn)?local.cliffturn:global.turn_on_cliff_detect;
    merged.can_be_eaten = (local.en_noyoshi)?(!local.noyoshi):global.can_be_eaten;
    merged.speed = (local.en_speed) ? global.speed * local.speed : global.speed;
    merged.kill_by_fireball = (local.en_nofireball)?(!local.nofireball):global.kill_by_fireball;
    merged.gravity = (local.en_nogravity)?(!local.nogravity):global.gravity;
    merged.freeze_by_iceball = (local.en_noiceball)?(!local.noiceball):global.freeze_by_iceball;
    merged.kill_hammer = (local.en_nohammer)?(!local.nohammer):global.kill_hammer;
    merged.kill_by_npc = (local.en_noshell)?(!local.noshell):global.kill_by_npc;

    #ifdef PGE_EDITOR
    WriteToLog(QtDebugMsg, QString("-------------------------------------"));
    WriteToLog(QtDebugMsg, QString("NPC-Merge for NPC-ID=%1").arg(merged.id));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Height:   %1").arg(merged.height));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Width:    %1").arg(merged.width));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX h:    %1").arg(merged.gfx_h));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX w:    %1").arg(merged.gfx_w));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Grid size %1").arg(merged.grid));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Offset x: %1").arg(merged.grid_offset_x));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Offset y: %1").arg(merged.grid_offset_y));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GridStl:  %1").arg(merged.grid_attach_style));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX offX: %1").arg(merged.gfx_offset_x));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX offY: %1").arg(merged.gfx_offset_y));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> FrStyle:  %1").arg(merged.framestyle));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Frames:   %1").arg((int)merged.frames));
    WriteToLog(QtDebugMsg, QString("-------------------------------------"));
    #endif

    return merged;
}
