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

#ifndef LUALEVELENGINE_H
#define LUALEVELENGINE_H

#include "lua_engine.h"

class LevelScene;
class LVL_Player;
class LVL_Npc;

class LuaLevelEngine : public LuaEngine
{
public:
    LuaLevelEngine(LevelScene *scene);
    LuaLevelEngine(const LuaLevelEngine&) = delete;
    ~LuaLevelEngine();

    LVL_Player *createLuaPlayer();
    LVL_Npc *createLuaNpc(unsigned long id);
    void destoryLuaNpc(LVL_Npc *npc);
    void destoryLuaPlayer(LVL_Player *plr);

    void initNPCClassTable();
    void loadNPCClass(unsigned long id, const std::string &path);
    void loadPlayerClass(unsigned long id, const std::string &path);

    LevelScene *getScene();

    std::string getNpcBaseClassPath() const;
    void setNpcBaseClassPath(const std::string &npcBaseClassPath);

    std::string getPlayerBaseClassPath() const;
    void setPlayerBaseClassPath(const std::string &playerBaseClassPath);

protected:
    std::string m_npcBaseClassPath;
    std::string m_playerBaseClassPath;

    void onBindAll();
};

#endif // LUALEVELENGINE_H
