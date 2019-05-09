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

#include "lvl_base_detector.h"
#include "../lvl_npc.h"

BasicDetector::BasicDetector()
{
    _scene = NULL;
    _parentNPC = NULL;
    _detected = false;
}

BasicDetector::BasicDetector(LVL_Npc *parent)
{
    _parentNPC = parent;
    _scene = parent->m_scene;
    _detected=false;
}

BasicDetector::BasicDetector(const BasicDetector &dtc)
{
    _scene     = dtc._scene;
    _parentNPC = dtc._parentNPC;
    _detected  = dtc._detected;
}

BasicDetector::~BasicDetector()
{}

void BasicDetector::processDetector()
{}

bool BasicDetector::detected()
{ return _detected; }

