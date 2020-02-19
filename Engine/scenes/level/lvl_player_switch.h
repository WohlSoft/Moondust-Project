/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CHARACTERSWITCHERENGINE_H
#define CHARACTERSWITCHERENGINE_H

#include <vector>
#include <common_features/simple_animator.h>
#include <data_configs/obj_block.h>

class LevelScene;

struct CharacterSwitchState
{
    CharacterSwitchState();
    unsigned int CharacterID;
    std::vector<int> sequence_on;
    std::vector<int> sequence_off;
    SimpleAnimator *animator_p;
};

struct CharacterBlockState
{
    CharacterBlockState();
    unsigned int CharacterID;
    std::vector<int> sequence_on;
    std::vector<int> sequence_off;
    SimpleAnimator *animator_p;
};

class CharacterSwitcherEngine
{
    public:
        CharacterSwitcherEngine();
        CharacterSwitcherEngine(LevelScene *scene);
        void setScene(LevelScene *scene);
        void buildSwitch(obj_block &blk);
        void buildBrick(obj_block &blk);
        void refreshState();
        std::vector<CharacterSwitchState>   m_switches;
        std::vector<CharacterBlockState>    m_blocks;
        LevelScene *m_scene;
};

#endif // CHARACTERSWITCHERENGINE_H
