/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CHARACTERSWITCHERENGINE_H
#define CHARACTERSWITCHERENGINE_H

#include <QList>
#include <common_features/simple_animator.h>
#include <data_configs/obj_block.h>

class LevelScene;

struct CharacterSwitchState
{
    CharacterSwitchState();
    int CharacterID;
    QList<int> sequence_on;
    QList<int> sequence_off;
    SimpleAnimator *animator_p;
};

struct CharacterBlockState
{
    CharacterBlockState();
    int CharacterID;
    QList<int> sequence_on;
    QList<int> sequence_off;
    SimpleAnimator *animator_p;
};

class CharacterSwitcherEngine
{
public:
    CharacterSwitcherEngine();
    CharacterSwitcherEngine(LevelScene * scene);
    void setScene(LevelScene * scene);
    void buildSwitch(obj_block &blk);
    void buildBrick(obj_block &blk);
    void refreshState();
    QList<CharacterSwitchState> m_switches;
    QList<CharacterBlockState> m_blocks;
    LevelScene * m_scene;
};

#endif // CHARACTERSWITCHERENGINE_H
