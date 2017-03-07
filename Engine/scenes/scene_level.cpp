/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "scene_level.h"

#include <common_features/app_path.h>
#include <common_features/graphics_funcs.h>
#include <settings/debugger.h>

#include <graphics/gl_renderer.h>

#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>
#include <data_configs/config_manager.h>
#include <fontman/font_manager.h>

#include <gui/pge_msgbox.h>
#include <gui/pge_textinputbox.h>

#include <audio/pge_audio.h>
#include <audio/play_music.h>
#include <settings/global_settings.h>

#include <algorithm>

#include <QtDebug>

#include <common_features/logger.h>

#include <script/lua_event.h>
#include <script/bindings/core/events/luaevents_core_engine.h>
#include <fmt/fmt_format.h>
#include <Utils/files.h>

#include <QElapsedTimer>

static QElapsedTimer debug_TimeReal;
static int           debug_TimeCounted = 0;

LevelScene::LevelScene()
    : Scene(Level),
      isInit(false),
      isInitFailed(false),
      isWarpEntrance(false),
      cameraStartDirected(false),
      cameraStartDirection(0),
      NewPlayerID(1),
      /*********Exit*************/
      isLevelContinues(true),
      warpToLevelFile(""),
      lastWarpID(0),
      warpToArrayID(0),
      warpToWorld(false),
      exitLevelDelay(3000),
      exitLevelCode(LvlExit::EXIT_Closed),
      /**************************/
      character_switchers(this),
      data(FileFormats::CreateLevelData()),
      /*********Physics**********/
      globalGravity(1.0),
      /**************************/
      luaEngine(this)
{
    tree.RemoveAll();
    layers.m_scene = this;
    events.m_scene = this;
    data.meta.ReadFileValid = false;
    zCounter = 0.0L;
    /**************************/
    placingMode = false;
    placingMode_item_type = 0;
    placingMode_block = FileFormats::CreateLvlBlock();
    placingMode_bgo  = FileFormats::CreateLvlBgo();
    placingMode_npc  = FileFormats::CreateLvlNpc();
    placingMode_animatorID = 0;
    placingMode_animated = false;
    placingMode_sizableBlock = false;
    placingMode_rect_draw = false;
    /**************************/
    /*********Default players number*************/
    numberOfPlayers = 2;
    /*********Default players number*************/
    /*********Loader*************/
    IsLoaderWorks = false;
    /*********Loader*************/
    /*********Fader*************/
    m_fader.setFull();
    /*********Fader*************/
    /*********Controller********/
    player1Controller = g_AppSettings.openController(1);
    player2Controller = g_AppSettings.openController(2);
    /*********Controller********/
    /*********Pause menu*************/
    initPauseMenu1();
    /*********Pause menu*************/
    frameSkip = g_AppSettings.frameSkip;
    m_messages.m_scene = this;
    errorMsg = "";
    gameState = NULL;
    debug_player_jumping = false;
    debug_player_onground = false;
    debug_player_foots = 0;
    debug_render_delay = 0;
    debug_phys_delay = 0;
    debug_event_delay = 0;
}

void LevelScene::processPhysics(double ticks)
{
    //Iterate layer movement
    layers.processMoving(uTickf);

    //Iterate playable characters
    for(LVL_PlayersArray::iterator it = players.begin(); it != players.end(); it++)
    {
        LVL_Player *plr = (*it);
        plr->iterateStep(ticks);
    }

    //Iterate activated NPCs
    for(size_t i = 0; i < active_npcs.size(); i++)
        active_npcs[i]->iterateStep(ticks);
}

static bool comparePosY(PGE_Phys_Object *i, PGE_Phys_Object *j)
{
    return (i->m_momentum.y > j->m_momentum.y);
}

void LevelScene::processAllCollisions()
{
    std::vector<PGE_Phys_Object *> toCheck;

    //Reset events first
    for(LVL_PlayersArray::iterator it = players.begin(); it != players.end(); it++)
    {
        LVL_Player *plr = (*it);
        plr->resetEvents();
        toCheck.push_back(plr);
    }

    //Process collision check and resolving for activated NPC's
    for(size_t i = 0; i < active_npcs.size(); i++)
    {
        active_npcs[i]->resetEvents();
        toCheck.push_back(active_npcs[i]);
    }

    std::stable_sort(toCheck.begin(), toCheck.end(), comparePosY);

    for(std::vector<PGE_Phys_Object *>::iterator it = toCheck.begin(); it != toCheck.end(); it++)
    {
        PGE_Phys_Object *obj = *it;
        obj->updateCollisions();
    }
}




LevelScene::~LevelScene()
{
    GlRenderer::clearScreen();
    layers.clear();
    switch_blocks.clear();
    //destroy textures
    size_t i = 0;
    D_pLogDebug("clear level textures");

    for(i = 0; i < textures_bank.size(); i++)
        GlRenderer::deleteTexture(textures_bank[i]);

    textures_bank.clear();
    D_pLogDebug("Destroy cameras");
    cameras.clear();
    D_pLogDebug("Destroy players");

    for(i = 0; i < players.size(); i++)
    {
        LVL_Player *tmp;
        tmp = players[i];
        player1Controller->removeFromControl(tmp);

        if(tmp)
        {
            if(player1Controller)
                player1Controller->removeFromControl(tmp);
            if(player2Controller)
                player2Controller->removeFromControl(tmp);
            tmp->unregisterFromTree();
            if(!tmp->isLuaPlayer)
                delete tmp;
        }
    }

    D_pLogDebug("Destroy blocks");

    for(i = 0; i < blocks.size(); i++)
    {
        LVL_Block *tmp;
        tmp = blocks[i];
        layers.removeRegItem(tmp->data.layer, tmp);

        if(tmp)
        {
            tmp->unregisterFromTree();
            delete tmp;
        }
    }

    D_pLogDebug("Destroy BGO");

    for(i = 0; i < bgos.size(); i++)
    {
        LVL_Bgo *tmp;
        tmp = bgos[i];
        layers.removeRegItem(tmp->data.layer, tmp);

        if(tmp)
        {
            tmp->unregisterFromTree();
            delete tmp;
        }
    }

    D_pLogDebug("Destroy NPC");

    for(i = 0; i < npcs.size(); i++)
    {
        LVL_Npc *tmp;
        tmp = npcs[i];

        if(tmp)
        {
            tmp->unregisterFromTree();

            if(!tmp->isLuaNPC)
                delete tmp;
        }
    }

    D_pLogDebug("Destroy Warps");

    for(i = 0; i < warps.size(); i++)
    {
        LVL_Warp *tmp;
        tmp = warps[i];
        layers.removeRegItem(tmp->data.layer, tmp);

        if(tmp)
        {
            tmp->unregisterFromTree();
            delete tmp;
        }
    }

    D_pLogDebug("Destroy Physical Environment zones");

    for(i = 0; i < physenvs.size(); i++)
    {
        LVL_PhysEnv *tmp;
        tmp = physenvs[i];
        layers.removeRegItem(tmp->data.layer, tmp);

        if(tmp)
        {
            tmp->unregisterFromTree();
            delete tmp;
        }
    }

    D_pLogDebug("Destroy sections");
    sections.clear();
    luaEngine.shutdown();
    destroyLoaderTexture();
    delete player1Controller;
    delete player2Controller;
    textures_bank.clear();
}


void LevelScene::tickAnimations(double ticks)
{
    //tick animation
    for(std::vector<SimpleAnimator>::iterator it = ConfigManager::Animator_Blocks.begin();
        it != ConfigManager::Animator_Blocks.end(); it++)
        it->manualTick(ticks);

    for(std::vector<SimpleAnimator>::iterator it = ConfigManager::Animator_BGO.begin();
        it != ConfigManager::Animator_BGO.end(); it++)
        it->manualTick(ticks);

    for(std::vector<SimpleAnimator>::iterator it = ConfigManager::Animator_BG.begin();
        it != ConfigManager::Animator_BG.end(); it++)
        it->manualTick(ticks);

    for(std::vector<AdvNpcAnimator>::iterator it = ConfigManager::Animator_NPC.begin();
        it != ConfigManager::Animator_NPC.end(); it++)
        it->manualTick(ticks);
}


void LevelScene::update()
{
    if(luaEngine.shouldShutdown())
    {
        m_fader.setFade(10, 1.0, 1.0);
        setExiting(0, LvlExit::EXIT_Error);
    }

    Scene::update();
    tickAnimations(uTickf);

    if(!isLevelContinues)
    {
        //Level exit timeout
        exitLevelDelay -= uTickf;

        if(exitLevelDelay <= 0.0)
        {
            m_doExit = true;

            if(m_fader.isNull())
            {
                if(PGE_MusPlayer::isPlaying())
                    PGE_MusPlayer::fadeOut(500);

                m_fader.setFade(10, 1.0, 0.01);
            }
        }
    }

    if(m_doExit)
    {
        if(exitLevelCode == LvlExit::EXIT_Closed)
        {
            m_fader.setFull();
            m_isRunning = false;
        }
        else
        {
            if(m_fader.isFull())
                m_isRunning = false;
        }
    }
    else if(m_isPauseMenu)
        processPauseMenu();
    else
    {
        //Update physics is not pause menu
        updateLua();//Process LUA code
        system_events.processEvents(uTickf);
        events.processTimers(uTickf);

        //update cameras
        for(PGE_LevelCamera &cam : cameras)
            cam.updatePre(uTickf);

        processEffects(uTickf);

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Make world step
            processPhysics(uTickf);
        }

        while(!block_transforms.empty())
        {
            transformTask_block x = block_transforms.front();
            x.block->transformTo_x(x.id);
            block_transforms.pop_front();
        }

        //Update controllers
        player1Controller->sendControls();
        player2Controller->sendControls();

        //update players
        for(LVL_Player *plr : players)
        {
            plr->update(uTickf);

            if(PGE_Window::showDebugInfo)
            {
                debug_player_jumping = plr->JumpPressed;
                debug_player_onground = plr->onGround();
                debug_player_foots   = plr->l_contactB.size();
            }
        }

        for(size_t i = 0; i < fading_blocks.size(); i++)
        {
            if(fading_blocks[i]->tickFader(uTickf))
            {
                fading_blocks.erase(fading_blocks.begin() + i);
                i--;
            }
        }

        //Process activated NPCs
        for(size_t i = 0; i < active_npcs.size(); i++)
        {
            active_npcs[i]->update(uTickf);

            if(active_npcs[i]->isKilled())
            {
                active_npcs.erase(active_npcs.begin() + i);
                i--;
            }
            else if(active_npcs[i]->activationTimeout <= 0)
            {
                if(!active_npcs[i]->warpSpawing)
                    active_npcs[i]->deActivate();

                if(active_npcs[i]->wasDeactivated)
                {
                    if(!isVizibleOnScreen(active_npcs[i]->m_momentum) || !active_npcs[i]->isVisible() || !active_npcs[i]->is_activity)
                    {
                        active_npcs[i]->wasDeactivated = false;
                        active_npcs.erase(active_npcs.begin() + i);
                        i--;
                    }
                }
            }
        }

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Process and resolve collisions
            processAllCollisions();
        }

        /***************Collect garbage****************/
        if(!dead_npcs.empty())
            collectGarbageNPCs();

        if(!dead_players.empty())
            collectGarbagePlayers();

        /**********************************************/

        //update cameras
        for(PGE_LevelCamera &cam : cameras)
        {
            cam.updatePost(uTickf);
            //! --------------DRAW HUD--------------------------------------
            LuaEngine *sceneLuaEngine = getLuaEngine();

            if(sceneLuaEngine)
            {
                if(sceneLuaEngine->isValid() && !sceneLuaEngine->shouldShutdown())
                {
                    LuaEvent drawHUDEvent = BindingCore_Events_Engine::createDrawLevelHUDEvent(sceneLuaEngine,
                                            &cam,
                                            &player_states[cam.playerID - 1]);
                    sceneLuaEngine->dispatchEvent(drawHUDEvent);
                }
            }

            //! ------------------------------------------------------------
        }

        //Add effects into the render table
        for(Scene_Effect &item : WorkingEffects)
        {
            renderArrayAddFunction([this, &item](double camPosX, double camPosY)
            {
                item.render(camPosX, camPosY);
            },  item.m_zIndex);
        }

        //Clear garbage (be careful!)
        //luaEngine.runGarbageCollector();
    }

    //Process interprocessing commands cache
    process_InterprocessCommands();
    //Process Z-sort of the render functions
    renderArrayPrepare();
    //Process message boxes
    m_messages.process();
}

void LevelScene::processEvents()
{
    Scene::processEvents();
    player1Controller->update();
    player2Controller->update();
}

void LevelScene::render()
{
    GlRenderer::clearScreen();
    size_t c = 0;

    if(!isInit)
        goto renderBlack;

    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);

    for(PGE_LevelCamera &cam : cameras)
    {
        if(numberOfPlayers > 1)
            GlRenderer::setViewport(static_cast<int>(cam.renderX()),
                                    static_cast<int>(cam.renderY()),
                                    static_cast<int>(cam.w()),
                                    static_cast<int>(cam.h()));

        cam.drawBackground();
        double camPosX = cam.posX();
        double camPosY = cam.posY();
        //Size of array with objects catched by camera
        int render_sz = cam.renderObjects_count();
        //Pointer to array with objects catched by camera
        PGE_Phys_Object **render_obj = cam.renderObjects_arr();
        //Index of array with lua-drawn elements
        size_t currentLuaRenderObj = 0;
        //Size of array with lua-drawn elements
        size_t currentLuaRenderSz = luaRenders.size();

        for(int i = 0; i < render_sz; i++)
        {
            switch(render_obj[i]->type)
            {
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLNPC:
            case PGE_Phys_Object::LVLPlayer:
            {
                PGE_Phys_Object *obj = render_obj[i];
                //Get Z-index of current element
                long double Z = obj->zIndex();

                //Draw lua-drew elements which are less than current Z-index
                while(currentLuaRenderObj < currentLuaRenderSz)
                {
                    RenderFuncs &r = luaRenders[currentLuaRenderObj];

                    if(r.z_index > Z)
                        break;

                    r.render(camPosX, camPosY);
                    currentLuaRenderObj++;
                }

                //Draw element itself
                obj->render(camPosX, camPosY);
                break;
            }

            default:
                break;
            }
        }

        //Draw elements left after all camera objects are drawn
        while(currentLuaRenderObj < currentLuaRenderSz)
        {
            RenderFuncs &r = luaRenders[currentLuaRenderObj];
            r.render(camPosX, camPosY);
            currentLuaRenderObj++;
        }

        if(PGE_Window::showPhysicsDebug)
        {
            for(int i = 0; i < render_sz; i++)
            {
                PGE_Phys_Object *obj = render_obj[i];
                obj->renderDebug(camPosX, camPosY);

                if(obj->type == PGE_Phys_Object::LVLNPC)
                {
                    LVL_Npc *npc = static_cast<LVL_Npc *>(obj);
                    for(size_t i = 0; i < npc->detectors_inarea.size(); i++)
                    {
                        PGE_RectF trapZone = npc->detectors_inarea[i].trapZone();
                        GlRenderer::renderRect(static_cast<float>(trapZone.x() - camPosX),
                                               static_cast<float>(trapZone.y() - camPosY),
                                               static_cast<float>(trapZone.width()),
                                               static_cast<float>(trapZone.height()),
                                               1.0f, 0.0, 0.0f, 1.0f, false);
                    }
                }
            }
        }

        cam.drawForeground();

        if(numberOfPlayers > 1)
            GlRenderer::resetViewport();

        c++;
    }

    //Draw camera separators
    for(c = 1; c < cameras.size(); c++)
        GlRenderer::renderRect(0, cameras[c].h() * c - 1, cameras[c].w(), 2, 0.f, 0.f, 0.f, 1.f);

    if(PGE_Window::showDebugInfo)
    {
        //FontManager::printText(fmt::format("Camera X={0} Y={1}", cam_x, cam_y), 200,10);
        int dpos = 60;
        FontManager::printText(fmt::format("Player J={0} G={1} F={2}; TICK-SUB: {3}\n"
                                       "NPC's: {4}, Active {5}; BLOCKS: {6}",
                                        debug_player_jumping,
                                        debug_player_onground,
                                        debug_player_foots,
                                        uTickf,
                                        npcs.size(),
                                        active_npcs.size(),
                                        blocks.size()), 10, dpos);
        dpos += 35;
        FontManager::printText(fmt::format("Vizible objects: {0}", !cameras.empty() ? cameras[0].renderObjects_count() : 0), 10, dpos);
        dpos += 35;
        FontManager::printText(fmt::format("Delays E=%03d R=%03d P=%03d",
                                            debug_event_delay,
                                            debug_render_delay,
                                            debug_phys_delay), 10, dpos);
        dpos += 35;
        FontManager::printText(fmt::format("Time Real:%010d\nTime Loop:%010d",
                                            debug_TimeReal.elapsed(),
                                            debug_TimeCounted), 10, dpos);
        dpos += 35;

        if(!isLevelContinues)
        {
            FontManager::printText(fmt::format("Exit delay {0}, {1}",
                                                exitLevelDelay,
                                                uTickf), 10, dpos, 0, 1.0, 0, 0, 1.0);
            dpos += 35;
        }

        if(placingMode)
            FontManager::printText(fmt::format("Placing! {0} X={1} Y={2}",
                                                placingMode_item_type,
                                                placingMode_renderAt.x(),
                                                placingMode_renderAt.y()), 10, 10, 0);
        else
            FontManager::printText(fmt::format("{0}", PGE_MusPlayer::getTitle()), 10, 10, 0);
    }

renderBlack:
    //Scene::render();
    {
        if(!m_fader.isNull())
            GlRenderer::renderRect(0.f,
                                   0.f,
                                   static_cast<float>(PGE_Window::Width),
                                   static_cast<float>(PGE_Window::Height),
                                   0.f, 0.f, 0.f,
                                   static_cast<float>(m_fader.fadeRatio()));
    }

    if(placingMode) drawPlacingItem();

    if(IsLoaderWorks) drawLoader();

    if(m_isPauseMenu) m_pauseMenu.render();
}

static bool slowTimeMode = false;
static bool OneStepMode = false;
static bool OneStepMode_doStep = false;

void LevelScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(m_doExit || isExit()) return;

    if(m_isPauseMenu)
        m_pauseMenu.processKeyEvent(sdl_key);

    switch(sdl_key)
    {
    // Check which
    case SDLK_ESCAPE:
    case SDLK_RETURN:   // Toggle pause mode
    {
        if(m_doExit || m_isPauseMenu) break;

        m_isPauseMenu = true;
        break;
    }

    case SDLK_BACKQUOTE:
    {
        PGE_Debugger::executeCommand(this);
        break;
    }

    default:

        //Keys allowed only in debug mode!
        if(PGE_Debugger::cheat_debugkeys)
        {
            switch(sdl_key)
            {
            case SDLK_1:
            {
                if(!players.empty())
                    launchEffect(1, players.front()->posX(), players.front()->posY(), 0, 2000, 0, 0, 0);
            }
            break;

            case SDLK_2:
            {
                if(!players.empty())
                    launchEffect(1, players.front()->posX(), players.front()->posY(), 0, 2000, 3, -6, 12);

                break;
            }

            case SDLK_3:
            {
                if(!players.empty())
                {
                    Scene_Effect_Phys p;
                    p.decelerate_x = 0.02;
                    p.max_vel_y = 12;
                    LVL_Player *pl = players.front();
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, -3, -6, 5, 0, p);
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, -4, -7, 5, 0, p);
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, 3, -6, 5, 0, p);
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, 4, -7, 5, 0, p);
                }

                break;
            }

            case SDLK_4:
            {
                if(!players.empty())
                {
                    Scene_Effect_Phys p;
                    p.max_vel_y = 12;
                    LVL_Player *pl = players.front();
                    launchEffect(11, pl->posX(), pl->posY(), 0, 5000, 0, -3, 12, 0, p);
                }

                break;
            }

            case SDLK_5:
            {
                if(!players.empty())
                    launchEffect(10, players.front()->posX(), players.front()->posY(), 1, 0, 0, 0, 0);
                break;
            }

            case SDLK_7:
            {
                if(players.size() >= 1)
                    players[0]->setCharacterSafe(players[0]->characterID - 1, players[0]->stateID);

                break;
            }

            case SDLK_8:
            {
                if(players.size() >= 1)
                    players[0]->setCharacterSafe(players[0]->characterID + 1, players[0]->stateID);

                break;
            }

            case SDLK_9:
            {
                if(players.size() >= 2)
                    players[1]->setCharacterSafe(2, 1);

                if(players.size() >= 1)
                    players[0]->setCharacterSafe(players[0]->characterID, players[0]->stateID - 1);

                break;
            }

            case SDLK_0:
            {
                if(players.size() >= 2)
                    players[1]->setCharacterSafe(2, 2);
                else if(players.size() >= 1)
                    players[0]->setCharacterSafe(players[0]->characterID, players[0]->stateID + 1);

                break;
            }

            case SDLK_F5:
            {
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
                isTimeStopped = !isTimeStopped;
                break;
            }

            case SDLK_F6:
            {
                PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
                slowTimeMode = !slowTimeMode;
                break;
            }

            case SDLK_F7:
            {
                PGE_Audio::playSoundByRole(obj_sound_role::WorldOpenPath);
                OneStepMode = !OneStepMode;
                break;
            }

            case SDLK_F8:
            {
                if(OneStepMode)
                {
                    PGE_Audio::playSoundByRole(obj_sound_role::WorldMove);
                    OneStepMode_doStep = true;
                }

                break;
            }

            default:
                break;
            }//switch(sdl_key)
        }//if(PGE_Debugger::cheat_debugkeys)
    }//switch(sdl_key)
}

LuaEngine *LevelScene::getLuaEngine()
{
    return &luaEngine;
}

int LevelScene::exec()
{
    isLevelContinues = true;
    m_doExit = false;
    m_isRunning = true;
    LoopTiming times;
    times.start_common = SDL_GetTicks();
    //Set black color clearer
    GlRenderer::setClearColor(0.f, 0.f, 0.f, 1.0f);

    /****************Initial update***********************/
    //Apply musics and backgrounds
    for(size_t i = 0; i < cameras.size(); i++)
    {
        //Play music from first camera only
        if(i == 0)
            cameras[i].cur_section->playMusic();
        cameras[i].cur_section->initBG();
    }

    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    player1Controller->resetControls();
    player2Controller->resetControls();

    if(m_isRunning) update();

    debug_TimeCounted = 0;
    debug_TimeReal.restart();
    /*****************************************************/

    while(m_isRunning)
    {
        times.start_common = SDL_GetTicks();
        debug_TimeCounted += uTickf;

        if(PGE_Window::showDebugInfo) times.start_events = SDL_GetTicks();

        /**********************Update common events and controllers******************/
        processEvents();

        /****************************************************************************/
        if(PGE_Window::showDebugInfo) times.stop_events = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
            debug_event_delay = static_cast<int>(times.stop_events - times.start_events);

        times.start_physics = SDL_GetTicks();

        /**********************Update physics and game progess***********************/
        if(!OneStepMode || OneStepMode_doStep)
        {
            update();
            OneStepMode_doStep = false;
        }

        /****************************************************************************/
        times.stop_physics = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
            debug_phys_delay  = static_cast<int>(times.stop_physics - times.start_physics);

        times.stop_render = 0;
        times.start_render = 0;

        /**********************Process rendering of stuff****************************/
        if((PGE_Window::vsync) || (times.doUpdate_render <= 0.0))
        {
            times.start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            GlRenderer::flush();
            GlRenderer::repaint();
            times.stop_render = SDL_GetTicks();
            times.doUpdate_render = frameSkip ? uTickf + (times.stop_render - times.start_render) : 0;

            if(PGE_Window::showDebugInfo)
                debug_render_delay = static_cast<int>(times.stop_render - times.start_render);
        }

        times.doUpdate_render -= uTickf;

        if(times.stop_render < times.start_render)
        {
            times.stop_render = 0;
            times.start_render = 0;
        }

        /****************************************************************************/

        if((!PGE_Window::vsync) && (uTick > times.passedCommonTime()))
        {
            if(!slowTimeMode)
                SDL_Delay(uTick - times.passedCommonTime());
            else
                SDL_Delay(uTick - times.passedCommonTime() + 300);
        }
        else if(slowTimeMode) SDL_Delay(uTick - times.passedCommonTime() + 300);
    }

    return exitLevelCode;
}



std::string LevelScene::getLastError()
{
    return errorMsg;
}



bool LevelScene::isExit()
{
    return !isLevelContinues;
}

void LevelScene::setExiting(int delay, int reason)
{
    exitLevelDelay   = delay;
    exitLevelCode    = reason;
    isLevelContinues = false;
}

std::string LevelScene::toAnotherLevel()
{
    if(!warpToLevelFile.empty())
        if(!Files::hasSuffix(warpToLevelFile, ".lvl") &&
           !Files::hasSuffix(warpToLevelFile, ".lvlx"))
            warpToLevelFile.append(".lvl");
    return warpToLevelFile;
}

unsigned long LevelScene::toAnotherEntrance()
{
    return warpToArrayID;
}

PGE_Point LevelScene::toWorldXY()
{
    return warpToWorldXY;
}

int LevelScene::exitType()
{
    return exitLevelCode;
}



void LevelScene::setGameState(EpisodeState *_gameState)
{
    gameState = _gameState;
    numberOfPlayers = 1;

    if(gameState)
    {
        numberOfPlayers = gameState->numOfPlayers;

        if((gameState->isEpisode) && (!gameState->isHubLevel))
            initPauseMenu2();
        else if(gameState->isTestingModeL)
            initPauseMenu3();
        else
            initPauseMenu1();
    }
}

LevelScene::LVL_NpcsArray &LevelScene::getNpcs()
{
    return npcs;
}

LevelScene::LVL_NpcsArray &LevelScene::getActiveNpcs()
{
    return active_npcs;
}

LevelScene::LVL_BlocksArray &LevelScene::getBlocks()
{
    return blocks;
}

LevelScene::LVL_BgosArray &LevelScene::getBGOs()
{
    return bgos;
}

LevelScene::LVL_PlayersArray &LevelScene::getPlayers()
{
    return players;
}
