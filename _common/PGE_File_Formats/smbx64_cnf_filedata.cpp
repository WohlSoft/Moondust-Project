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

#include "smbx64_cnf_filedata.h"

SMBX64_ConfigPlayer::SMBX64_ConfigPlayer()
{
    id=0;
    controllerType=0;
    k_up=0;
    k_down=0;
    k_left=0;
    k_right=0;
    k_run=0;
    k_jump=0;
    k_drop=0;
    k_pause=0;
    k_altjump=0;//>=19
    k_altrun=0;//>=19
    j_run=0;
    j_jump=0;
    j_drop=0;
    j_pause=0;
    j_altjump=0;//>=19
    j_altrun=0;//>=19
}


SMBX64_ConfigFile::SMBX64_ConfigFile()
{
    fullScreen=false;
    ReadFileValid=false;
}
