/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef PGE_EDITOR
#include <common_features/logger.h>
#endif

#include "file_formats.h"
#include "npc_filedata.h"

NPCConfigFile FileFormats::CreateEmpytNpcTXT()
{
    NPCConfigFile FileData;
    FileData.en_gfxoffsetx=false;
    FileData.en_gfxoffsety=false;
    FileData.en_width=false;
    FileData.en_height=false;
    FileData.en_gfxwidth=false;
    FileData.en_gfxheight=false;
    FileData.en_score=false;
    FileData.en_health=false;
    FileData.en_playerblock=false;
    FileData.en_playerblocktop=false;
    FileData.en_npcblock=false;
    FileData.en_npcblocktop=false;
    FileData.en_grabside=false;
    FileData.en_grabtop=false;
    FileData.en_jumphurt=false;
    FileData.en_nohurt=false;
    FileData.en_noblockcollision=false;
    FileData.en_cliffturn=false;
    FileData.en_noyoshi=false;
    FileData.en_foreground=false;
    FileData.en_speed=false;
    FileData.en_nofireball=false;
    FileData.en_nogravity=false;
    FileData.en_frames=false;
    FileData.en_framespeed=false;
    FileData.en_framestyle=false;
    FileData.en_noiceball=false;

    FileData.en_nohammer=false;
    FileData.en_noshell=false;
    FileData.en_name=false;
    FileData.en_image=false;
    FileData.en_script=false;
    return FileData;
}

