/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_PHYSENV_H
#define LVL_PHYSENV_H

#include "lvl_base_object.h"
#include <PGE_File_Formats/file_formats.h>

class LVL_PhysEnv : public PGE_Phys_Object
{
    public:
        LVL_PhysEnv(LevelScene *_parent = NULL);
        ~LVL_PhysEnv();

        void init();
        static const unsigned long numOfEnvironments;
        enum EnvType
        {
            Env_SameAsAround = -1,
            Env_Air = 0,
            Env_Water,
            Env_Quicksand
        };
        int env_type;

        LevelPhysEnv data;
};

#endif // LVL_PHYSENV_H
