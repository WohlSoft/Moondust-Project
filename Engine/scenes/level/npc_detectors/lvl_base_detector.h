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

#ifndef BASICDETECTOR_H
#define BASICDETECTOR_H

class LVL_Npc;
class LevelScene;
class BasicDetector
{
    friend class LVL_Npc;
public:
    BasicDetector();
    BasicDetector(LVL_Npc* parent);
    BasicDetector(const BasicDetector &dtc);
    virtual ~BasicDetector();
    virtual void processDetector();
    virtual bool detected();
protected:
    bool _detected;
    LVL_Npc* _parentNPC;
    LevelScene *_scene;
};

#endif // BASICDETECTOR_H
