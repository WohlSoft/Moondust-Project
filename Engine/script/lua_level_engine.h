#ifndef LUALEVELENGINE_H
#define LUALEVELENGINE_H

#include "lua_engine.h"

class LevelScene;
class LVL_Player;
class LVL_Npc;

class LuaLevelEngine : public LuaEngine
{
//    private:
//        Q_DISABLE_COPY(LuaLevelEngine)
    public:
        LuaLevelEngine(LevelScene *scene);
        ~LuaLevelEngine();

        LVL_Player *createLuaPlayer();
        LVL_Npc *createLuaNpc(unsigned long id);
        void destoryLuaNpc(LVL_Npc *npc);
        void destoryLuaPlayer(LVL_Player *plr);

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
