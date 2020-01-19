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

#include "../scene_level.h"
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>
#include <common_features/logger.h>
#include <common_features/tr.h>
#include <Utils/maths.h>
#include <gui/pge_msgbox.h>

#include <Utils/files.h>
#include <functional>
#include <algorithm>

bool LevelScene::setEntrance(unsigned long entr)
{
    player_defs.clear();
    unsigned int plr = 1;

    auto &gameState = m_gameState->m_gameSave;

    for(unsigned long &xxx : gameState.currentCharacter)
    {
        LVL_PlayerDef def;
        def.setPlayerID(static_cast<int>(plr));
        def.setCharacterID(xxx);

        for(size_t j = 0; j < gameState.characterStates.size(); j++)
        {
            if(gameState.characterStates[j].id == xxx)
            {
                def.setState(gameState.characterStates[j].state);
                break;
            }
        }

        player_defs[plr] = def;
        plr++;
    }

    if((entr == 0) || (entr > static_cast<unsigned long>(m_data.doors.size())))
    {
        m_isWarpEntrance = false;
        bool found = false;

        for(size_t i = 0, j = 0; (i < m_data.players.size()) && (int32_t(j) < m_numberOfPlayers); i++)
        {
            if(m_data.players[i].w == 0 && m_data.players[i].h == 0)
                continue; //Skip empty points

            LVL_PlayerDef d = player_defs[m_data.players[i].id];
            m_cameraStart.setX(m_data.players[i].x + (m_data.players[i].w / 2) - (d.width() / 2));
            m_cameraStart.setY(m_data.players[i].y +  m_data.players[i].h    -  d.height());
            found = true;
            j++;
            break;
        }

        if(!found)
        {
            m_exitLevelCode = LvlExit::EXIT_Error;
            PGE_MsgBox msgBox(NULL, "ERROR:\nCan't start level without player's start point.\nPlease set a player's start point and start level again.",
                              PGE_MsgBox::msg_error);
            msgBox.exec();
        }

        //Find available start points
        return found;
    }
    else
    {
        for(size_t i = 0; i < m_data.doors.size(); i++)
        {
            if(m_data.doors[i].meta.array_id == static_cast<unsigned int>(entr))
            {
                m_isWarpEntrance = true;
                m_warpInitial = m_data.doors[i];
                m_cameraStartDirected = (m_warpInitial.type == 1);
                m_cameraStartDirection = m_warpInitial.odirect;

                for(int i = 1; i <= m_numberOfPlayers; i++)
                {
                    LVL_PlayerDef d = player_defs[static_cast<unsigned int>(i)];
                    int w = d.width(), h = d.height();
                    EventQueueEntry<LevelScene >event3;
                    event3.makeCaller([this, i, w, h]()->void
                    {
                        PlayerPoint newPoint = getStartLocation(i);
                        newPoint.id = static_cast<unsigned>(i);
                        newPoint.x = m_warpInitial.ox;
                        newPoint.y = m_warpInitial.oy;
                        newPoint.w = w;
                        newPoint.h = h;
                        newPoint.direction = 1;
                        this->addPlayer(newPoint, true,
                        m_warpInitial.type, m_warpInitial.odirect,
                        m_warpInitial.cannon_exit, m_warpInitial.cannon_exit_speed);
                        m_isWarpEntrance = false;
                    }, 1000);
                    m_systemEvents.events.push_back(event3);
                }

                return true;
            }
        }
    }

    m_isWarpEntrance = false;
    PGE_MsgBox msgBox(NULL, "ERROR:\nTarget section is not found.\nMayby level is empty.",
                      PGE_MsgBox::msg_error);
    msgBox.exec();
    //Error, sections is not found
    m_exitLevelCode = LvlExit::EXIT_Error;
    return false;
}

PlayerPoint LevelScene::getStartLocation(int playerID)
{
    //If no placed player star points
    if(m_data.players.empty())
    {
        PlayerPoint point;

        if(this->m_isWarpEntrance)
        {
            //Construct spawn point with basing on camera position
            point.x = static_cast<long>(m_cameraStart.x());
            point.y = static_cast<long>(m_cameraStart.x());
            point.w = 20;
            point.h = 60;
        }
        else if(!m_data.sections.empty())
        {
            //Construct point based on first section boundary coordinates
            point.x = m_data.sections[0].size_left + 20;
            point.y = m_data.sections[0].size_top + 60;
            point.w = 20;
            point.h = 60;
        }
        else
        {
            //Construct null player point
            point.x = 0;
            point.y = 0;
            point.w = 0;
            point.h = 0;
        }

        point.direction = 1;
        point.id = static_cast<unsigned int>(playerID);
        return point;
    }

    for(PlayerPoint &p : m_data.players)
    {
        //Return player ID specific spawn point
        if(p.id == unsigned(playerID))
        {
            //Must not be null!
            if((p.w != 0) && (p.h != 0))
                return p;
        }
    }

    if(playerID <= int32_t(m_data.players.size()))
    {
        PlayerPoint p = m_data.players[static_cast<size_t>(playerID) - 1];

        //Return spawn point by array index if not out of range [Not null]
        if((p.w != 0) && (p.h != 0))
        {
            p.id = static_cast<unsigned int>(playerID);
            return m_data.players[static_cast<size_t>(playerID) - 1];
        }
    }

    for(PlayerPoint &p : m_data.players)
    {
        //Return first not null point
        if((p.w != 0) && (p.h != 0))
        {
            p.id = static_cast<unsigned int>(playerID);
            return p;
        }
    }

    //Return first presented point entry even if null
    return m_data.players.front();
}





bool LevelScene::loadConfigs()
{
    bool success = true;
    std::string musIni = m_data.meta.path + "/music.ini";
    std::string sndIni = m_data.meta.path + "/sounds.ini";

    if(ConfigManager::music_lastIniFile != musIni)
    {
        ConfigManager::loadDefaultMusics();
        ConfigManager::loadMusic(m_data.meta.path + "/", musIni, true);
    }

    if(ConfigManager::sound_lastIniFile != sndIni)
    {
        ConfigManager::loadDefaultSounds();
        ConfigManager::loadSound(m_data.meta.path + "/", sndIni, true);

        if(ConfigManager::soundIniChanged())
            ConfigManager::buildSoundIndex();
    }

    //Set paths
    std::string metaPath  = m_data.meta.path;
    std::string metaFName = m_data.meta.filename;
    std::vector<std::string> extraPaths;
    ConfigManager::loadExtraFoldersList(metaPath, extraPaths);
    ConfigManager::Dir_Blocks.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelBlock(), extraPaths);
    ConfigManager::Dir_BGO.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelBGO(), extraPaths);
    ConfigManager::Dir_NPC.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelNPC(), extraPaths);
    ConfigManager::Dir_NPCScript.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelNPCScript(), extraPaths);
    ConfigManager::Dir_PlayerScript.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelPlayerScript(), extraPaths);
    ConfigManager::Dir_PlayerCalibrations.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelPlayerCalibrations(), extraPaths);
    ConfigManager::Dir_PlayerLvl.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelPlayable(), extraPaths);
    ConfigManager::Dir_BG.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelBG(), extraPaths);
    ConfigManager::Dir_EFFECT.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelEffect(), extraPaths);

    //Load INI-files
    success = ConfigManager::loadLevelBlocks(); //!< Blocks

    if(!success)
    {
        m_exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelBGO();    //!< BGO
    if(!success)
    {
        m_exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelNPC();  //!< NPC
    if(!success)
    {
        m_exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelBackG();  //!< Backgrounds
    if(!success)
    {
        m_exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadPlayableCharacters();  //!< Playalbe Characters
    if(!success)
    {
        m_exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelEffects();  //!< Effects
    if(!success)
    {
        m_exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    //Validate all playable characters until use game state!
    if(m_gameState)
    {
        for(int i = 1; i <= m_numberOfPlayers; i++)
        {
            PlayerState st = m_gameState->getPlayerState(i);

            if(!ConfigManager::playable_characters.contains(st.characterID))
            {
                //% "Invalid playable character ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_CHARACTER") + " "
                               + std::to_string(st.characterID);
                m_errorMsg = _errorString;
                success = false;
                break;
            }
            else if(!ConfigManager::playable_characters[st.characterID].states.contains(st.stateID))
            {
                //% "Invalid playable character state ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_STATE") + " "
                               + std::to_string(st.stateID);
                m_errorMsg = _errorString;
                success = false;
                break;
            }
        }
    }

    if(!success) m_exitLevelCode = LvlExit::EXIT_Error;

abortInit:
    return success;
}

bool LevelScene::init_items()
{
    //Global script path
    m_luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    //Episode path
    m_luaEngine.appendLuaScriptPath(m_data.meta.path);
    //Level custom path
    m_luaEngine.appendLuaScriptPath(m_data.meta.path + "/" + m_data.meta.filename);
    m_luaEngine.setFileSearchPath(m_data.meta.path + "/" + m_data.meta.filename);
    m_luaEngine.setCoreFile(":/script/maincore_level.lua");
    m_luaEngine.setUserFile(ConfigManager::setup_Level.luaFile);
    m_luaEngine.setNpcBaseClassPath(":/script/npcs/maincore_npc.lua");
    m_luaEngine.setPlayerBaseClassPath(":/script/player/maincore_player.lua");
    m_luaEngine.setErrorReporterFunc([this](const std::string &errorMessage, const std::string &stacktrace)->void
    {
        pLogWarning("Lua-Error: ");
        pLogWarning("Error Message: %s", errorMessage.data());
        pLogWarning("Stacktrace: \n%s", stacktrace.data());
        _errorString = std::string("A lua error has been thrown: \n") + errorMessage + "\n\nMore details in the log!";
        //return false;
    });
    m_luaEngine.init();

    if(m_luaEngine.shouldShutdown())
        return false;

    for(unsigned long i = 1; i < ConfigManager::lvl_npc_indexes.size(); i++)
    {
        obj_npc &npc = ConfigManager::lvl_npc_indexes[i];
        std::string scriptPath = ConfigManager::Dir_NPCScript.getCustomFile(npc.setup.algorithm_script);

        if((!scriptPath.empty()) && (Files::fileExists(scriptPath)))
            m_luaEngine.loadNPCClass(npc.setup.id, scriptPath);
    }

    for(unsigned long i = 1; i < ConfigManager::playable_characters.size(); i++)
    {
        obj_player &player = ConfigManager::playable_characters[i];
        std::string scriptPath = ConfigManager::Dir_PlayerScript.getCustomFile(player.script);

        if((!scriptPath.empty()) && (Files::fileExists(scriptPath)))
            m_luaEngine.loadPlayerClass(player.id, scriptPath);
    }

    m_zCounter = 0.0L;
    D_pLogDebugNA("Build sections");

    for(size_t i = 0; i < m_data.sections.size(); i++)
    {
        LVL_Section sct;
        m_sections.push_back(sct);
        m_sections.back().setScene(this);
        m_sections.back().setData(m_data.sections[i]);
        m_sections.back().setMusicRoot(m_data.meta.path);
    }

    D_pLogDebugNA("Create cameras");

    //quit from game if window was closed
    if(!m_isLevelContinues) return false;

    for(int i = 0; i < m_numberOfPlayers; i++)
    {
        int width  = PGE_Window::Width;
        int height = PGE_Window::Height / m_numberOfPlayers;
        LVL_PlayerDef d = player_defs[static_cast<unsigned int>(i + 1)];

        if(m_isWarpEntrance)
        {
            m_cameraStart.setX(m_warpInitial.ox + 16 - (d.width() / 2));
            m_cameraStart.setY(m_warpInitial.oy + 32 - d.height());

            if(m_cameraStartDirected)
            {
                switch(m_warpInitial.odirect)
                {
                case 2://right
                    m_cameraStart.setX(m_warpInitial.ox);
                    m_cameraStart.setY(m_warpInitial.oy + 32 - d.height());
                    break;

                case 1://down
                    m_cameraStart.setX(m_warpInitial.ox + 16 - d.width() / 2);
                    m_cameraStart.setY(m_warpInitial.oy);
                    break;

                case 4://left
                    m_cameraStart.setX(m_warpInitial.ox + 32 - d.width());
                    m_cameraStart.setY(m_warpInitial.oy + 32 - d.height());
                    break;

                case 3://up
                    m_cameraStart.setX(m_warpInitial.ox + 16 - d.width() / 2);
                    m_cameraStart.setY(m_warpInitial.oy + 32 - d.height());
                    break;

                default:
                    break;
                }
            }
        }

        int sID = findNearestSection(Maths::lRound(m_cameraStart.x()),
                                     Maths::lRound(m_cameraStart.y()));
        LVL_Section *t_sct = getSection(sID);

        if(!t_sct)
        {
            /*% "Fatal error: Impossible to find start section.\n"
                "Did you placed player start point (or entrance warp point) too far off of the section(s)?" */
            _errorString = qtTrId("LVL_ERROR_NOSECTIONS");
            m_errorMsg = _errorString;
            return false;
        }

        double x = m_cameraStart.x();
        double y = m_cameraStart.y();
        //Init Cameras
        PGE_LevelCamera camera(this);
        camera.init(x, y, width, height);
        camera.playerID = (i + 1);
        camera.setRenderPos(0.0, (height * i));
        camera.setRenderObjectsCacheEnabled(i == 0);
        camera.changeSection(t_sct, true);
        camera.setPos(x - camera.w() / 2 + d.width() / 2,
                      y - camera.h() / 2 + d.height() / 2);
        m_cameras.push_back(camera);
        lua_LevelPlayerState luaPlState(this, (i + 1));
        m_playerStates.push_back(luaPlState);
    }

    //Init data
    //blocks
    for(size_t i = 0; i < m_data.blocks.size(); i++)
    {
        if(!m_isLevelContinues)
            return false;//!< quit from game if window was closed
        placeBlock(m_data.blocks[i]);
    }

    //Build character switchers and configure switches and filters
    m_characterSwitchers.refreshState();

    //BGO
    for(size_t i = 0; i < m_data.bgo.size(); i++)
    {
        if(!m_isLevelContinues)
            return false;//!< quit from game if window was closed
        placeBGO(m_data.bgo[i]);
    }

    //NPC
    for(size_t i = 0; i < m_data.npc.size(); i++)
    {
        if(!m_isLevelContinues)
            return false;//!< quit from game if window was closed
        placeNPC(m_data.npc[i]);
    }

    //BGO
    for(size_t i = 0; i < m_data.doors.size(); i++)
    {
        if(!m_isLevelContinues)
            return false;//!< quit from game if window was closed
        //Don't put contactable points for "level entrance" points
        if(m_data.doors[i].lvl_i)
            continue;

        LVL_Warp *warpP;
        LevelDoor door = m_data.doors[i];
        bool two_way_opposite = false;
place_door_again:
        warpP = new LVL_Warp(this);
        warpP->data = door;
        warpP->init();
        int sID = findNearestSection(Maths::lRound(warpP->posX()),
                                     Maths::lRound(warpP->posY()));
        LVL_Section *sct = getSection(sID);

        if(sct)
            warpP->setParentSection(sct);

        warpP->m_treemap.updatePos();
        m_itemsWarps.insert(warpP);

        if(!two_way_opposite && door.two_way)//Place opposite entrance point
        {
            two_way_opposite = true;
            std::swap(door.idirect,  door.odirect);
            std::swap(door.ix,       door.ox);
            std::swap(door.iy,       door.oy);
            std::swap(door.length_i, door.length_o);
            goto place_door_again;
        }
    }

    //BGO
    for(size_t i = 0; i < m_data.physez.size(); i++)
    {
        if(!m_isLevelContinues)
            return false;//!< quit from game if window was closed

        LVL_PhysEnv *physesP;
        physesP = new LVL_PhysEnv(this);
        physesP->data = m_data.physez[i];
        physesP->init();
        physesP->m_treemap.updatePos();
        m_itemsPhysEnvs.insert(physesP);
    }

    D_pLogDebug("Total textures loaded: %d", ConfigManager::level_textures.size());
    int added_players = 0;

    if(!m_isWarpEntrance) //Dont place players if entered through warp
        for(int i = 1; i <= m_numberOfPlayers; i++)
        {
            if(!m_isLevelContinues) return false;//!< quit from game if window was closed

            PlayerPoint startPoint = getStartLocation(i);
            startPoint.id = static_cast<unsigned int>(i);

            //Don't place player if point is null!
            if(startPoint.w == 0 && startPoint.h == 0) continue;

            addPlayer(startPoint);
            added_players++;
        }

    if(added_players <= 0 && !m_isWarpEntrance)
    {
        _errorString = "No defined players!";
        m_errorMsg = _errorString;
        return false;
    }

    D_pLogDebugNA("Apply layers");

    for(size_t i = 0; i < m_data.layers.size(); i++)
    {
        if(m_data.layers[i].hidden)
            m_layers.hide(m_data.layers[i].name, false);
    }

    D_pLogDebugNA("Apply Events");

    for(size_t i = 0; i < m_data.events.size(); i++)
        m_events.addSMBX64Event(m_data.events[i]);

    m_isInit = true;
    return true;
}


bool LevelScene::init()
{
    m_isInitFinished = false;
    m_isInitFailed = false;
    #if 0
    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext_background);
    initializer_thread = SDL_CreateThread(init_thread, "LevelInitializer", this);
    setLoaderAnimation(62);

    while(!isInitFinished)
    {
        drawLoader();
        PGE_Window::rePaint();
        //SDL_PumpEvents();
        SDL_Delay(20);
    }

    stopLoaderAnimation();
    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext);
    #else
    //Load everything without loading animation, in the main thread
    //(because in the threaded loading some issues are appearence)
    init_thread(this);
    #endif

    if(m_isInitFailed)
        PGE_MsgBox::error(_errorString);

    return !m_isInitFailed;
}


int LevelScene::init_thread(void *self)
{
    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext);
    LevelScene *_self = static_cast<LevelScene *>(self);
    SDL_assert(_self);
    if(!_self)
        return -1;
//  _self->isInitFailed = true;
    if(!_self->loadConfigs())
        _self->m_isInitFailed = true;
    else if(!_self->init_items())
        _self->m_isInitFailed = true;

    _self->m_isInitFinished = true;
    return 0;
}
