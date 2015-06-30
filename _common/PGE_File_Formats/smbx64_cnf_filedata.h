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

#ifndef SMBX64_CNF_FILEDATA_H
#define SMBX64_CNF_FILEDATA_H

#include "pge_file_lib_globs.h"

struct SMBX64_ConfigPlayer
{
      SMBX64_ConfigPlayer();
      unsigned int id;
      unsigned int controllerType;
      unsigned int k_up;
      unsigned int k_down;
      unsigned int k_left;
      unsigned int k_right;
      unsigned int k_run;
      unsigned int k_jump;
      unsigned int k_drop;
      unsigned int k_pause;
      unsigned int k_altjump;//>=19
      unsigned int k_altrun;//>=19
      unsigned int j_run;
      unsigned int j_jump;
      unsigned int j_drop;
      unsigned int j_pause;
      unsigned int j_altjump;//>=19
      unsigned int j_altrun;//>=19
};

struct SMBX64_ConfigFile
{
    SMBX64_ConfigFile();
    bool fullScreen;//>=16
    PGEVECTOR<SMBX64_ConfigPlayer> players;

    bool ReadFileValid;
};

#endif // SMBX64_CNF_FILEDATA_H

