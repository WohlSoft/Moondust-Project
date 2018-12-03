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

#include "../lvl_npc.h"
#include "../../scene_level.h"
#include <Utils/maths.h>

void LVL_Npc::updateGenerator(double tickTime)
{
    if(!m_isGenerator)
        return;

    generatorTimeLeft -= tickTime;
    if(generatorTimeLeft <= 0)
    {
        generatorTimeLeft += data.generator_period * 100.0;
        if(!contacted_npc.empty())
            return;
        if(!contacted_players.empty())
            return;
        LevelNPC def = data;
        def.x = Maths::lRound(posX());
        def.y = Maths::lRound(posY());
        def.generator = false;
        def.layer = "Spawned NPCs";
        m_scene->spawnNPC(def,
                         static_cast<LevelScene::NpcSpawnType>(generatorType),
                         static_cast<LevelScene::NpcSpawnDirection>(generatorDirection), false);
    }
}
