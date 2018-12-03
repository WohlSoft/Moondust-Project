/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_SOUND_H
#define OBJ_SOUND_H

#include <string>

#include "obj_sound_roles.h"
struct Mix_Chunk;

struct obj_sound_index
{
    std::string path;
    bool need_reload = false;
    Mix_Chunk *chunk = nullptr;
    int channel = -1;
    void setPath(std::string _path);
    void play(); //!< play sound
};


struct obj_sound
{
    unsigned long id = 0;
    std::string name;
    std::string file;
    std::string absPath;
    int channel = -1;
    bool hidden = false;
};

#endif // OBJ_SOUND_H

