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

/*!
 * \file npc_filedata.h
 * \brief Contains defition of the SMBX64-NPC.txt config data structure
 */

#ifndef NPC_FILEDATA_H
#define NPC_FILEDATA_H

#include "pge_file_lib_globs.h"

/*!
 * \brief SMBX64-NPC.txt File Data structure
 */
struct NPCConfigFile
{
    bool ReadFileValid;

/*
 * SMBX64 Standard
 */
    //! Is GFX-Offset X field using?
    bool en_gfxoffsetx;
    //! GFX-Offset X field
    int gfxoffsetx;

    //! Is GFX-Offset Y field using?
    bool en_gfxoffsety;
    //! GFX-Offset Y field
    int gfxoffsety;

    //! Is Width field using?
    bool en_width;
    //! Width field
    unsigned int width;

    //! Is height field using?
    bool en_height;
    //! Height field
    unsigned int height;

    //! Is GFX-Width field using?
    bool en_gfxwidth;
    //! GFX-Width field
    unsigned int gfxwidth;

    //! Is GFX-Height field using?
    bool en_gfxheight;
    //! GFX-Height field
    unsigned int gfxheight;

    //! Is Score field using?
    bool en_score;
    //! Score rewards field (from 0 to 13)
    unsigned int score;

    //! Is "Block player at side" flag using?
    bool en_playerblock;
    //! "Block player at side" flag
    bool playerblock;

    //! Is "Player can stay on top" flag using?
    bool en_playerblocktop;
    //! "Player can stay on top" flag
    bool playerblocktop;

    //! Is "Block other NPC at side" flag using?
    bool en_npcblock;
    //! "Block other NPC at side" flag
    bool npcblock;

    //! Is "NPC Can stay on top" flag using?
    bool en_npcblocktop;
    //! "NPC Can stay on top" flag
    bool npcblocktop;

    //! Is "Allow to grab at side" flag using?
    bool en_grabside;
    //! "Allow to grab at side" flag
    bool grabside;

    //! Is "Allow to grab at top" flag using?
    bool en_grabtop;
    //! "Allow to grab at top" flag
    bool grabtop;

    //! Is "Hurt player on stomp attempt" flag using?
    bool en_jumphurt;
    //! "Hurt player on stomp attempt" flag
    bool jumphurt;

    //! Is "Safe for playable character" flag using?
    bool en_nohurt;
    //! "Safe for playable character" flag
    bool nohurt;

    //! Is "Don't collide with blocks" flag using?
    bool en_noblockcollision;
    //! "Don't collide with blocks" flag
    bool noblockcollision;

    //! Is "Turn on cliff" flag using?
    bool en_cliffturn;
    //! "Turn on cliff" flag
    bool cliffturn;

    //! Is "Not eatable" flag using?
    bool en_noyoshi;
    //! "Not eatable" flag
    bool noyoshi;

    //! Is "Foreground render" flag using?
    bool en_foreground;
    //! "Foreground render" flag
    bool foreground;

    //! Is Speed modifier field us using?
    bool en_speed;
    //! Speed modifier field
    float speed;

    //! Is "Immune to fire" flag using?
    bool en_nofireball;
    //! "Immune to fire" flag
    bool nofireball;

    //! Is "Zero gravity" flag using?
    bool en_nogravity;
    //! Zero gravity flag
    bool nogravity;

    //! Is Frames number field using?
    bool en_frames;
    //! Frames number field
    unsigned int frames;

    //! Is Frame speed modifier field using?
    bool en_framespeed;
    //! Frame speed modifier field
    unsigned int framespeed;

    //! Is frame style field using?
    bool en_framestyle;
    //! Frame style field
    unsigned int framestyle;

    //! Is "Immune to ice" flag using?
    bool en_noiceball;
    //! "Immune to ice" flag
    bool noiceball;


/*
 * Extended
 */
    //! Is "Immune to hammers/axes/knifes" flag using?
    bool en_nohammer;
    //! "Immune to hammers/axes/knifes" flag
    bool nohammer;

    //! Is "Immune to thrown NPC's" flag using?
    bool en_noshell;
    //! "Immune to thrown NPC's" flag
    bool noshell;

    //! Is NPC's Custom name field using?
    bool en_name;
    //! NPC's Custom name
    PGESTRING name;

    //! Is custom health level field using?
    bool en_health;
    //! Custom health level field
    unsigned int health;

    //! Is custom image file name field using?
    bool en_image;
    //! Is custom image file name field using?
    PGESTRING image;

    //! Is custom AI script filename field using?
    bool en_script;
    //! Custom AI Script filename field
    PGESTRING script;

    //! Is custom aligning grid size field using?
    bool en_grid;
    //! Custom aligning grid size field
    unsigned int grid;

    //! Is custom grid offset X field using?
    bool en_grid_offset_x;
    //! Custom grid offset X field
    unsigned int grid_offset_x;

    //! Is custom grid offset Y field using?
    bool en_grid_offset_y;
    //! Custom grid offset Y field
    unsigned int grid_offset_y;

    //! Is custom grid align style field using?
    bool en_grid_align;
    //! Custom grid align style field
    unsigned int grid_align;

};

#endif // NPC_FILEDATA_H
