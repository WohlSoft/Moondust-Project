/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/npc_filedata.h>

#include "npcedit.h"
#include <ui_npcedit.h>

void NpcEdit::setDataBoxes()
{

    //Apply NPC data
    ui->en_GFXOffsetX->setChecked(m_npcDataBackup.en_gfxoffsetx);
    ui->offsetx_label->setEnabled(m_npcDataBackup.en_gfxoffsetx);
    ui->GFXOffSetX->setEnabled(m_npcDataBackup.en_gfxoffsetx);
    if(!m_npcDataBackup.en_gfxoffsetx)
    {
        ui->GFXOffSetX->setValue(m_npcDataDefault.gfxoffsetx);
        NpcData.gfxoffsetx = m_npcDataDefault.gfxoffsetx;
    }
    else
        ui->GFXOffSetX->setValue(m_npcDataBackup.gfxoffsetx);


    ui->en_GFXOffsetY->setChecked(m_npcDataBackup.en_gfxoffsety);
    ui->offsety_label->setEnabled(m_npcDataBackup.en_gfxoffsety);
    ui->GFXOffSetY->setEnabled(m_npcDataBackup.en_gfxoffsety);
    if(!m_npcDataBackup.en_gfxoffsety)
    {
        ui->GFXOffSetY->setValue(m_npcDataDefault.gfxoffsety);
        NpcData.gfxoffsety = m_npcDataDefault.gfxoffsety;
    }
    else
        ui->GFXOffSetY->setValue(m_npcDataBackup.gfxoffsety);


    ui->En_IsForeground->setChecked(m_npcDataBackup.en_foreground);
    ui->IsForeground->setEnabled(m_npcDataBackup.en_foreground);
    if(!m_npcDataBackup.en_foreground)
    {
        ui->IsForeground->setChecked(m_npcDataDefault.foreground);
        NpcData.foreground = m_npcDataDefault.foreground;
    }
    else
        ui->IsForeground->setChecked(m_npcDataBackup.foreground);


    ui->En_GFXh->setChecked(m_npcDataBackup.en_gfxheight);
    ui->gheight_label->setEnabled(m_npcDataBackup.en_gfxheight);
    ui->GFXh->setEnabled(m_npcDataBackup.en_gfxheight);
    if(!m_npcDataBackup.en_gfxheight)
    {
        ui->GFXh->setValue(m_npcDataDefault.gfxheight);
        NpcData.gfxheight = m_npcDataDefault.gfxheight;
    }
    else
        ui->GFXh->setValue(m_npcDataBackup.gfxheight);


    ui->En_GFXw->setChecked(m_npcDataBackup.en_gfxwidth);
    ui->gwidth_label->setEnabled(m_npcDataBackup.en_gfxwidth);
    ui->GFXw->setEnabled(m_npcDataBackup.en_gfxwidth);
    if(!m_npcDataBackup.en_gfxwidth)
    {
        ui->GFXw->setValue(m_npcDataDefault.gfxwidth);
        NpcData.gfxwidth = m_npcDataDefault.gfxwidth;
    }
    else
        ui->GFXw->setValue(m_npcDataBackup.gfxwidth);


    ui->En_Framestyle->setChecked(m_npcDataBackup.en_framestyle);
    ui->framestyle_label->setEnabled(m_npcDataBackup.en_framestyle);
    ui->FrameStyle->setEnabled(m_npcDataBackup.en_framestyle);
    if(!m_npcDataBackup.en_framestyle)
    {
        ui->FrameStyle->setCurrentIndex(m_npcDataDefault.framestyle);
        NpcData.framestyle = m_npcDataDefault.framestyle;
    }
    else
        ui->FrameStyle->setCurrentIndex(m_npcDataBackup.framestyle);


    ui->En_Framespeed->setChecked(m_npcDataBackup.en_framespeed);
    ui->framespeed_label->setEnabled(m_npcDataBackup.en_framespeed);
    ui->Framespeed->setEnabled(m_npcDataBackup.en_framespeed);
    if(!m_npcDataBackup.en_framespeed)
    {
        ui->Framespeed->setValue(m_npcDataDefault.framespeed);
        NpcData.framespeed = m_npcDataDefault.framespeed;
    }
    else
        ui->Framespeed->setValue(m_npcDataBackup.framespeed);


    ui->En_Frames->setChecked(m_npcDataBackup.en_frames);
    ui->frames_label->setEnabled(m_npcDataBackup.en_frames);
    ui->Frames->setEnabled(m_npcDataBackup.en_frames);
    if(!m_npcDataBackup.en_frames)
    {
        ui->Frames->setValue(m_npcDataDefault.frames);
        NpcData.frames = m_npcDataDefault.frames;
    }
    else
        ui->Frames->setValue(m_npcDataBackup.frames);


    ui->En_Score->setChecked(m_npcDataBackup.en_score);
    ui->score_label->setEnabled(m_npcDataBackup.en_score);
    ui->Score->setEnabled(m_npcDataBackup.en_score);
    if(!m_npcDataBackup.en_score)
    {
        ui->Score->setCurrentIndex(m_npcDataDefault.score);
        NpcData.score = m_npcDataDefault.score;
    }
    else
        ui->Score->setCurrentIndex(m_npcDataBackup.score);


    ui->En_Health->setChecked(m_npcDataBackup.en_health);
    ui->health_label->setEnabled(m_npcDataBackup.en_health);
    ui->Health->setEnabled(m_npcDataBackup.en_health);
    if(!m_npcDataBackup.en_health)
    {
        ui->Health->setValue(m_npcDataDefault.health);
        NpcData.health = m_npcDataDefault.health;
    }
    else
        ui->Health->setValue(m_npcDataBackup.health);


    ui->En_JumpHurt->setChecked(m_npcDataBackup.en_jumphurt);
    ui->JumpHurt->setEnabled(m_npcDataBackup.en_jumphurt);
    if(!m_npcDataBackup.en_jumphurt)
    {
        ui->JumpHurt->setChecked(m_npcDataDefault.jumphurt);
        NpcData.jumphurt = m_npcDataDefault.jumphurt;
    }
    else
        ui->JumpHurt->setChecked(m_npcDataBackup.jumphurt);


    ui->En_GrabTop->setChecked(m_npcDataBackup.en_grabtop);
    ui->GrabTop->setEnabled(m_npcDataBackup.en_grabtop);
    if(!m_npcDataBackup.en_grabtop)
    {
        ui->GrabTop->setChecked(m_npcDataDefault.grabtop);
        NpcData.grabtop = m_npcDataDefault.grabtop;
    }
    else
        ui->GrabTop->setChecked(m_npcDataBackup.grabtop);


    ui->En_GrabSide->setChecked(m_npcDataBackup.en_grabside);
    ui->GrabSide->setEnabled(m_npcDataBackup.en_grabside);
    if(!m_npcDataBackup.en_grabside)
    {
        ui->GrabSide->setChecked(m_npcDataDefault.grabside);
        NpcData.grabside = m_npcDataDefault.grabside;
    }
    else
        ui->GrabSide->setChecked(m_npcDataBackup.grabside);


    ui->En_NoFireball->setChecked(m_npcDataBackup.en_nofireball);
    ui->NoFireball->setEnabled(m_npcDataBackup.en_nofireball);
    if(!m_npcDataBackup.en_nofireball)
    {
        ui->NoFireball->setChecked(m_npcDataDefault.nofireball);
        NpcData.nofireball = m_npcDataDefault.nofireball;
    }
    else
        ui->NoFireball->setChecked(m_npcDataBackup.nofireball);


    ui->En_DontHurt->setChecked(m_npcDataBackup.en_nohurt);
    ui->DontHurt->setEnabled(m_npcDataBackup.en_nohurt);
    if(!m_npcDataBackup.en_nohurt)
    {
        ui->DontHurt->setChecked(m_npcDataDefault.nohurt);
        NpcData.nohurt = m_npcDataDefault.nohurt;
    }
    else
        ui->DontHurt->setChecked(m_npcDataBackup.nohurt);


    ui->En_NoEat->setChecked(m_npcDataBackup.en_noyoshi);
    ui->NoEat->setEnabled(m_npcDataBackup.en_noyoshi);
    if(!m_npcDataBackup.en_noyoshi)
    {
        ui->NoEat->setChecked(m_npcDataDefault.noyoshi);
        NpcData.noyoshi = m_npcDataDefault.noyoshi;
    }
    else
        ui->NoEat->setChecked(m_npcDataBackup.noyoshi);


    ui->En_NoIceball->setChecked(m_npcDataBackup.en_noiceball);
    ui->NoIceball->setEnabled(m_npcDataBackup.en_noiceball);
    if(!m_npcDataBackup.en_noiceball)
    {
        ui->NoIceball->setChecked(m_npcDataDefault.noiceball);
        NpcData.noiceball = m_npcDataDefault.noiceball;
    }
    else
        ui->NoIceball->setChecked(m_npcDataBackup.noiceball);


    ui->En_Height->setChecked(m_npcDataBackup.en_height);
    ui->height_label->setEnabled(m_npcDataBackup.en_height);
    ui->Height->setEnabled(m_npcDataBackup.en_height);
    if(!m_npcDataBackup.en_height)
    {
        ui->Height->setValue(m_npcDataDefault.height);
        NpcData.height = m_npcDataDefault.height;
    }
    else
        ui->Height->setValue(m_npcDataBackup.height);


    ui->En_Width->setChecked(m_npcDataBackup.en_width);
    ui->width_label->setEnabled(m_npcDataBackup.en_width);
    ui->Width->setEnabled(m_npcDataBackup.en_width);
    if(!m_npcDataBackup.en_width)
    {
        ui->Width->setValue(m_npcDataDefault.width);
        NpcData.width = m_npcDataDefault.width;
    }
    else
        ui->Width->setValue(m_npcDataBackup.width);


    ui->En_NPCBlock->setChecked(m_npcDataBackup.en_npcblock);
    ui->NPCBlock->setEnabled(m_npcDataBackup.en_npcblock);
    if(!m_npcDataBackup.en_npcblock)
    {
        ui->NPCBlock->setChecked(m_npcDataDefault.npcblock);
        NpcData.npcblock = m_npcDataDefault.npcblock;
    }
    else
        ui->NPCBlock->setChecked(m_npcDataBackup.npcblock);


    ui->En_NPCBlockTop->setChecked(m_npcDataBackup.en_npcblocktop);
    ui->NPCBlockTop->setEnabled(m_npcDataBackup.en_npcblocktop);
    if(!m_npcDataBackup.en_npcblocktop)
    {
        ui->NPCBlockTop->setChecked(m_npcDataDefault.npcblocktop);
        NpcData.npcblocktop = m_npcDataDefault.npcblocktop;
    }
    else
        ui->NPCBlockTop->setChecked(m_npcDataBackup.npcblocktop);


    ui->En_Speed->setChecked(m_npcDataBackup.en_speed);
    ui->speed_label->setEnabled(m_npcDataBackup.en_speed);
    ui->Speed->setEnabled(m_npcDataBackup.en_speed);
    if(!m_npcDataBackup.en_speed)
    {
        NpcData.speed = m_npcDataDefault.speed;
        ui->Speed->setValue(m_npcDataDefault.speed);
    }
    else
        ui->Speed->setValue(m_npcDataBackup.speed);


    ui->En_PlayerBlock->setChecked(m_npcDataBackup.en_playerblock);
    ui->PlayerBlock->setEnabled(m_npcDataBackup.en_playerblock);
    if(!m_npcDataBackup.en_playerblock)
    {
        ui->PlayerBlock->setChecked(m_npcDataDefault.playerblock);
        NpcData.playerblock = m_npcDataDefault.playerblock;
    }
    else
        ui->PlayerBlock->setChecked(m_npcDataBackup.playerblock);


    ui->En_PlayerBlockTop->setChecked(m_npcDataBackup.en_playerblocktop);
    ui->PlayerBlockTop->setEnabled(m_npcDataBackup.en_playerblocktop);
    if(!m_npcDataBackup.en_playerblocktop)
    {
        ui->PlayerBlockTop->setChecked(m_npcDataDefault.playerblocktop);
        NpcData.playerblocktop = m_npcDataDefault.playerblocktop;
    }
    else
        ui->PlayerBlockTop->setChecked(m_npcDataBackup.playerblocktop);


    ui->En_NoBlockCollision->setChecked(m_npcDataBackup.en_noblockcollision);
    ui->NoBlockCollision->setEnabled(m_npcDataBackup.en_noblockcollision);
    if(!m_npcDataBackup.en_noblockcollision)
    {
        ui->NoBlockCollision->setChecked(m_npcDataDefault.noblockcollision);
        NpcData.noblockcollision = m_npcDataDefault.noblockcollision;
    }
    else
        ui->NoBlockCollision->setChecked(m_npcDataBackup.noblockcollision);


    ui->En_NoGravity->setChecked(m_npcDataBackup.en_nogravity);
    ui->NoGravity->setEnabled(m_npcDataBackup.en_nogravity);
    if(!m_npcDataBackup.en_nogravity)
    {
        ui->NoGravity->setChecked(m_npcDataDefault.nogravity);
        NpcData.nogravity = m_npcDataDefault.nogravity;
    }
    else
        ui->NoGravity->setChecked(m_npcDataBackup.nogravity);


    ui->En_TurnCliff->setChecked(m_npcDataBackup.en_cliffturn);
    ui->TurnCliff->setEnabled(m_npcDataBackup.en_cliffturn);
    if(!m_npcDataBackup.en_cliffturn)
    {
        ui->TurnCliff->setChecked(m_npcDataDefault.cliffturn);
        NpcData.cliffturn = m_npcDataDefault.cliffturn;
    }
    else
        ui->TurnCliff->setChecked(m_npcDataBackup.cliffturn);

    //Extended

    ui->En_NoHammer->setChecked(m_npcDataBackup.en_nohammer);
    ui->NoHammer->setEnabled(m_npcDataBackup.en_nohammer);
    if(!m_npcDataBackup.en_nohammer)
    {
        ui->NoHammer->setChecked(m_npcDataDefault.nohammer);
        NpcData.nohammer = m_npcDataDefault.nohammer;
    }
    else
        ui->NoHammer->setChecked(m_npcDataBackup.nohammer);

    ui->En_NoShell->setChecked(m_npcDataBackup.en_noshell);
    ui->NoShell->setEnabled(m_npcDataBackup.en_noshell);
    if(!m_npcDataBackup.en_noshell)
    {
        ui->NoShell->setChecked(m_npcDataDefault.noshell);
        NpcData.noshell = m_npcDataDefault.noshell;
    }
    else
        ui->NoShell->setChecked(m_npcDataBackup.noshell);


    ui->En_Name->setChecked(m_npcDataBackup.en_name);
    ui->Name->setEnabled(m_npcDataBackup.en_name);
    if(!m_npcDataBackup.en_name)
    {
        ui->Name->setText(m_npcDataDefault.name);
        NpcData.name = m_npcDataDefault.name;
    }
    else
        ui->Name->setText(m_npcDataBackup.name);

    ui->En_Health->setChecked(m_npcDataBackup.en_health);
    ui->Health->setEnabled(m_npcDataBackup.en_health);
    if(!m_npcDataBackup.en_health)
    {
        ui->Health->setValue(m_npcDataDefault.health);
        NpcData.health = m_npcDataDefault.health;
    }
    else
        ui->Health->setValue(m_npcDataBackup.health);

    ui->En_GridSize->setChecked(m_npcDataBackup.en_grid);
    ui->GridSize->setEnabled(m_npcDataBackup.en_grid);
    if(!m_npcDataBackup.en_grid)
    {
        ui->GridSize->setValue(m_npcDataDefault.grid);
        NpcData.grid = m_npcDataDefault.grid;
    }
    else
        ui->GridSize->setValue(m_npcDataBackup.grid);

    int offsetMin = -1 * (abs(ui->GridSize->value()) / 2) + 1;
    int offsetMax = (abs(ui->GridSize->value()) / 2) - 1;
    ui->GridOffsetX->setMinimum(offsetMin);
    ui->GridOffsetX->setMaximum(offsetMax);
    ui->GridOffsetY->setMinimum(offsetMin);
    ui->GridOffsetY->setMaximum(offsetMax);

    ui->En_GridOffsetX->setChecked(m_npcDataBackup.en_gridoffsetx);
    ui->GridOffsetX->setEnabled(m_npcDataBackup.en_gridoffsetx);
    if(!m_npcDataBackup.en_gridoffsetx)
    {
        ui->GridOffsetX->setValue(m_npcDataDefault.gridoffsetx);
        NpcData.gridoffsetx = m_npcDataDefault.gridoffsetx;
    }
    else
        ui->GridOffsetX->setValue(m_npcDataBackup.gridoffsetx);

    ui->En_GridOffsetY->setChecked(m_npcDataBackup.en_gridoffsety);
    ui->GridOffsetY->setEnabled(m_npcDataBackup.en_gridoffsety);
    if(!m_npcDataBackup.en_gridoffsety)
    {
        ui->GridOffsetY->setValue(m_npcDataDefault.gridoffsety);
        NpcData.gridoffsety = m_npcDataDefault.gridoffsety;
    }
    else
        ui->GridOffsetY->setValue(m_npcDataBackup.gridoffsety);

    ui->En_AlignAt->setChecked(m_npcDataBackup.en_gridalign);
    ui->AlignAt->setEnabled(m_npcDataBackup.en_gridalign);
    if(!m_npcDataBackup.en_gridalign)
    {
        ui->AlignAt->setCurrentIndex(m_npcDataDefault.gridalign);
        NpcData.gridalign = m_npcDataDefault.gridalign;
    }
    else
        ui->AlignAt->setCurrentIndex(m_npcDataBackup.gridalign);
}

void NpcEdit::setDefaultData(unsigned long npc_id)
{

    m_npcDataDefault = FileFormats::CreateEmpytNpcTXT();

    if((npc_id == 0) || (npc_id > (unsigned long)m_configPack->main_npc.stored()))
    {
        m_npcDataDefault.gfxoffsetx = 0;
        m_npcDataDefault.gfxoffsety = 0;
        m_npcDataDefault.width = 0;
        m_npcDataDefault.height = 0;
        m_npcDataDefault.gfxwidth = 0;
        m_npcDataDefault.gfxheight = 0;
        m_npcDataDefault.score = 0;
        m_npcDataDefault.playerblock = 0;
        m_npcDataDefault.playerblocktop = 0;
        m_npcDataDefault.npcblock = 0;
        m_npcDataDefault.npcblocktop = 0;
        m_npcDataDefault.grabside = 0;
        m_npcDataDefault.grabtop = 0;
        m_npcDataDefault.jumphurt = 0;
        m_npcDataDefault.nohurt = 0;
        m_npcDataDefault.noblockcollision = 0;
        m_npcDataDefault.cliffturn = 0;
        m_npcDataDefault.noyoshi = 0;
        m_npcDataDefault.foreground = 0;
        m_npcDataDefault.speed = 1.0;
        m_npcDataDefault.nofireball = 0;
        m_npcDataDefault.nogravity = 0;
        m_npcDataDefault.frames = 0;
        m_npcDataDefault.framespeed = 8;
        m_npcDataDefault.framestyle = 0;
        m_npcDataDefault.noiceball = 0;
        m_npcDataDefault.nohammer = 0;
        m_npcDataDefault.name = "";
        m_npcDataDefault.health = 1;
        m_npcDataDefault.script = "";
        m_npcDataDefault.image = "";
        m_npcDataDefault.grid = 32;
        m_npcDataDefault.gridoffsetx = 0;
        m_npcDataDefault.gridoffsety = 0;
        m_npcDataDefault.gridalign = 0;
    }
    else
    {
        obj_npc &t_npc = m_configPack->main_npc[npc_id];

        m_npcDataDefault.gfxoffsetx = t_npc.setup.gfx_offset_x;
        m_npcDataDefault.gfxoffsety = t_npc.setup.gfx_offset_y;
        m_npcDataDefault.width = t_npc.setup.width;
        m_npcDataDefault.height = t_npc.setup.height;
        m_npcDataDefault.gfxwidth = t_npc.setup.gfx_w;
        m_npcDataDefault.gfxheight = t_npc.setup.gfx_h;
        m_npcDataDefault.score = t_npc.setup.score;
        m_npcDataDefault.playerblock = (int)t_npc.setup.block_player;
        m_npcDataDefault.playerblocktop = (int)t_npc.setup.block_player_top;
        m_npcDataDefault.npcblock = (int)t_npc.setup.block_npc;
        m_npcDataDefault.npcblocktop = (int)t_npc.setup.block_player_top;
        m_npcDataDefault.grabside = (int)t_npc.setup.grab_side;
        m_npcDataDefault.grabtop = (int)t_npc.setup.grab_top;
        m_npcDataDefault.jumphurt = (int)(
                                        (t_npc.setup.hurt_player)
                                        &&
                                        (!t_npc.setup.kill_on_jump));
        m_npcDataDefault.nohurt = (int)(!t_npc.setup.hurt_player);
        m_npcDataDefault.noblockcollision = (int)(!t_npc.setup.collision_with_blocks);
        m_npcDataDefault.cliffturn = (int)t_npc.setup.turn_on_cliff_detect;
        m_npcDataDefault.noyoshi = (int)(!t_npc.setup.can_be_eaten);
        m_npcDataDefault.foreground = (int)t_npc.setup.foreground;
        m_npcDataDefault.speed = 1.0;
        m_npcDataDefault.nofireball = (int)(!t_npc.setup.kill_by_fireball);
        m_npcDataDefault.nogravity = (int)(!t_npc.setup.gravity);
        m_npcDataDefault.frames = t_npc.setup.frames;
        m_npcDataDefault.framespeed = 8;
        m_npcDataDefault.framestyle = t_npc.setup.framestyle;
        m_npcDataDefault.noiceball = (int)(!t_npc.setup.freeze_by_iceball);
        //Extended
        m_npcDataDefault.nohammer = (int)(!t_npc.setup.kill_hammer);
        m_npcDataDefault.noshell = (int)(!t_npc.setup.kill_by_npc);
        m_npcDataDefault.name = t_npc.setup.name;
        m_npcDataDefault.health = t_npc.setup.health;
        m_npcDataDefault.script = t_npc.setup.algorithm_script;
        m_npcDataDefault.image = t_npc.setup.image_n;
        m_npcDataDefault.grid = t_npc.setup.grid;
        m_npcDataDefault.gridoffsetx = 0;
        m_npcDataDefault.gridoffsety = 0;
        m_npcDataDefault.gridalign = t_npc.setup.grid_attach_style;
    }
}


