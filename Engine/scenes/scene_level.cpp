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

#include <common_features/logger.h>

#include <script/lua_event.h>
#include <script/bindings/core/events/luaevents_core_engine.h>
#include <fmt/fmt_format.h>
#include <Utils/files.h>
#include <Utils/elapsed_timer.h>

static ElapsedTimer     debug_TimeReal;
static int              debug_TimeCounted = 0;

LevelScene::LevelScene()
    : Scene(Level),
      m_isInit(false),
      m_isInitFailed(false),
      m_isWarpEntrance(false),
      m_cameraStartDirected(false),
      m_cameraStartDirection(0),
      m_newPlayerID(1),
      /*********Exit*************/
      m_isLevelContinues(true),
      m_warpToLevelFile(""),
      m_lastWarpID(0),
      m_warpToArrayID(0),
      m_warpToWorld(false),
      m_exitLevelDelay(3000),
      m_exitLevelCode(LvlExit::EXIT_Closed),
      /**************************/
      m_characterSwitchers(this),
      m_data(FileFormats::CreateLevelData()),
      /*********Physics**********/
      m_globalGravity(1.0),
      /**************************/
      m_luaEngine(this)
{
    //m_tree.RemoveAll();
    m_qtree.clear();

    m_layers.m_scene = this;
    m_events.m_scene = this;
    m_data.meta.ReadFileValid = false;
    m_zCounter = 0.0L;
    /**************************/
    m_placingMode = false;
    m_placingMode_item_type = 0;
    m_placingMode_block = FileFormats::CreateLvlBlock();
    m_placingMode_bgo  = FileFormats::CreateLvlBgo();
    m_placingMode_npc  = FileFormats::CreateLvlNpc();
    m_placingMode_animatorID = 0;
    m_placingMode_animated = false;
    m_placingMode_sizableBlock = false;
    m_placingMode_rect_draw = false;
    /**************************/
    /*********Default players number*************/
    m_numberOfPlayers = 2;
    /*********Default players number*************/
    /*********Loader*************/
    m_loaderIsWorks = false;
    /*********Loader*************/
    /*********Fader*************/
    m_fader.setFull();
    /*********Fader*************/
    /*********Controller********/
    m_player1Controller = g_AppSettings.openController(1);
    m_player2Controller = g_AppSettings.openController(2);
    /*********Controller********/
    /*********Pause menu*************/
    initPauseMenu1();
    /*********Pause menu*************/
    m_frameSkip = g_AppSettings.frameSkip;
    m_messages.m_scene = this;
    m_errorMsg = "";
    m_gameState = NULL;
    m_debug_player_jumping = false;
    m_debug_player_onground = false;
    m_debug_player_foots = 0;
    m_debug_render_delay = 0;
    m_debug_phys_delay = 0;
    m_debug_event_delay = 0;
}

void LevelScene::processPhysics(double ticks)
{
    //Iterate layer movement
    m_layers.processMoving(uTickf);

    //Iterate playable characters
    for(LVL_PlayersArray::iterator it = m_itemsPlayers.begin(); it != m_itemsPlayers.end(); it++)
    {
        LVL_Player *plr = (*it);
        plr->iterateStep(ticks);
    }

    //Iterate activated NPCs
    for(LVL_NpcActiveSet::iterator i = m_npcActive.begin(); i != m_npcActive.end(); ++i)
    {
        LVL_Npc* n = *i;
        n->iterateStep(ticks);
    }
}

static bool comparePosY(PGE_Phys_Object *i, PGE_Phys_Object *j)
{
    return (i->m_momentum.y > j->m_momentum.y);
}

void LevelScene::processAllCollisions()
{
    std::vector<PGE_Phys_Object *> toCheck;

    //Reset events first
    for(LVL_PlayersArray::iterator it = m_itemsPlayers.begin(); it != m_itemsPlayers.end(); it++)
    {
        LVL_Player *plr = (*it);
        plr->resetEvents();
        toCheck.push_back(plr);
    }

    //Process collision check and resolving for activated NPC's
    for(LVL_NpcActiveSet::iterator i = m_npcActive.begin(); i != m_npcActive.end(); ++i)
    {
        LVL_Npc* n = *i;
        n->resetEvents();
        toCheck.push_back(n);
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
    m_layers.clear();
    m_switchBlocks.clear();
    //destroy textures
    size_t i = 0;
    D_pLogDebug("clear level textures");

    for(i = 0; i < m_texturesBank.size(); i++)
        GlRenderer::deleteTexture(m_texturesBank[i]);

    m_texturesBank.clear();
    D_pLogDebug("Destroy cameras");
    m_cameras.clear();

    D_pLogDebug("Destroy players");
    for(i = 0; i < m_itemsPlayers.size(); i++)
    {
        LVL_Player *tmp = m_itemsPlayers[i];
        if(tmp)
        {
            if(m_player1Controller)
                m_player1Controller->removeFromControl(tmp);
            if(m_player2Controller)
                m_player2Controller->removeFromControl(tmp);
            tmp->unregisterFromTree();
            if(!tmp->m_isLuaPlayer)
                delete tmp;
        }
    }

    D_pLogDebug("Destroy blocks");
    for(LVL_BlocksArray::iterator i = m_itemsBlocks.begin(); i != m_itemsBlocks.end(); ++i)
    {
        LVL_Block *tmp = *i;
        if(tmp)
        {
            m_layers.removeRegItem(tmp->data.layer, tmp);
            tmp->unregisterFromTree();
            delete tmp;
        }
    }
    m_itemsBlocks.clear();

    D_pLogDebug("Destroy BGO");
    for(LVL_BgosArray::iterator i = m_itemsBgo.begin(); i != m_itemsBgo.end(); ++i)
    {
        LVL_Bgo *tmp = *i;
        if(tmp)
        {
            m_layers.removeRegItem(tmp->data.layer, tmp);
            tmp->unregisterFromTree();
            delete tmp;
        }
    }
    m_itemsBlocks.clear();

    D_pLogDebug("Destroy NPC");
    for(LVL_NpcsArray::iterator i = m_itemsNpc.begin(); i != m_itemsNpc.end(); ++i)
    {
        LVL_Npc *tmp = *i;
        if(tmp)
        {
            tmp->unregisterFromTree();
            if(!tmp->isLuaNPC)
                delete tmp;
        }
    }
    m_itemsNpc.clear();

    D_pLogDebug("Destroy Warps");
    for(LVL_WarpsArray::iterator i = m_itemsWarps.begin(); i != m_itemsWarps.end(); ++i)
    {
        LVL_Warp *tmp = *i;
        if(tmp)
        {
            m_layers.removeRegItem(tmp->data.layer, tmp);
            tmp->unregisterFromTree();
            delete tmp;
        }
    }
    m_itemsWarps.clear();

    D_pLogDebug("Destroy Physical Environment zones");
    for(LVL_PhysEnvsArray::iterator i = m_itemsPhysEnvs.begin(); i != m_itemsPhysEnvs.end(); ++i)
    {
        LVL_PhysEnv *tmp = *i;
        if(tmp)
        {
            m_layers.removeRegItem(tmp->data.layer, tmp);
            tmp->unregisterFromTree();
            delete tmp;
        }
    }
    m_itemsPhysEnvs.clear();

    D_pLogDebug("Destroy sections");
    m_sections.clear();
    m_luaEngine.shutdown();
    destroyLoaderTexture();
    delete m_player1Controller;
    delete m_player2Controller;
    m_texturesBank.clear();
}


void LevelScene::tickAnimations(double ticks)
{
    //tick animation
    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_Blocks.begin();
        it != ConfigManager::Animator_Blocks.end(); it++)
        it->manualTick(ticks);

    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_BGO.begin();
        it != ConfigManager::Animator_BGO.end(); it++)
        it->manualTick(ticks);

    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_BG.begin();
        it != ConfigManager::Animator_BG.end(); it++)
        it->manualTick(ticks);

    for(VPtrList<AdvNpcAnimator>::iterator it = ConfigManager::Animator_NPC.begin();
        it != ConfigManager::Animator_NPC.end(); it++)
        it->manualTick(ticks);
}


void LevelScene::update()
{
    if(m_luaEngine.shouldShutdown())
    {
        m_fader.setFade(10, 1.0, 1.0);
        setExiting(0, LvlExit::EXIT_Error);
    }

    Scene::update();
    tickAnimations(uTickf);

    if(!m_isLevelContinues)
    {
        //Level exit timeout
        m_exitLevelDelay -= uTickf;

        if(m_exitLevelDelay <= 0.0)
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
        if(m_exitLevelCode == LvlExit::EXIT_Closed)
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
        m_systemEvents.processEvents(uTickf);
        m_events.processTimers(uTickf);

        //update cameras
        for(PGE_LevelCamera &cam : m_cameras)
            cam.updatePre(uTickf);

        processEffects(uTickf);

        if(!m_isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Make world step
            processPhysics(uTickf);
        }

        while(!m_blockTransforms.empty())
        {
            transformTask_block x = m_blockTransforms.front();
            x.block->transformTo_x(x.id);
            m_blockTransforms.pop_front();
        }

        //Update controllers
        m_player1Controller->sendControls();
        m_player2Controller->sendControls();

        //update players
        for(LVL_Player *plr : m_itemsPlayers)
        {
            plr->update(uTickf);

            if(PGE_Window::showDebugInfo)
            {
                m_debug_player_jumping    = plr->m_jumpPressed;
                m_debug_player_onground   = plr->onGround();
                m_debug_player_foots      = plr->l_contactB.size();
            }
        }

        for(size_t i = 0; i < m_blocksInFade.size(); i++)
        {
            if(m_blocksInFade[i]->tickFader(uTickf))
            {
                m_blocksInFade.erase(m_blocksInFade.begin() + i);
                i--;
            }
        }

        //Process activated NPCs
        //for(size_t i = 0; i < m_npcActive.size(); i++)
        for(LVL_NpcActiveSet::iterator i = m_npcActive.begin(); i != m_npcActive.end(); )
        {
            LVL_Npc *n = *i;
            n->update(uTickf);
            if(n->isKilled())
            {
                i = m_npcActive.erase(i);
                continue;
            }
            else if(n->activationTimeout <= 0)
            {
                if(!n->warpSpawing)
                    n->deActivate();
                if(n->wasDeactivated)
                {
                    if(!isVizibleOnScreen(n->m_momentum) || !n->isVisible() || !n->is_activity)
                    {
                        n->wasDeactivated = false;
                        i = m_npcActive.erase(i);
                        continue;
                    }
                }
            }
            ++i;
        }

        if(!m_isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Process and resolve collisions
            processAllCollisions();
        }

        /***************Collect garbage****************/
        if(!m_npcDead.empty())
            collectGarbageNPCs();

        if(!m_playersDead.empty())
            collectGarbagePlayers();

        if(!m_blocksToDelete.empty())
            collectGarbageBlocks();

        /**********************************************/

        //update cameras
        for(PGE_LevelCamera &cam : m_cameras)
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
                                            &m_playerStates[cam.playerID - 1]);
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
    m_player1Controller->update();
    m_player2Controller->update();
}

void LevelScene::render()
{
    GlRenderer::clearScreen();
    size_t c = 0;

    if(!m_isInit)
        goto renderBlack;

    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);

    for(PGE_LevelCamera &cam : m_cameras)
    {
        if(m_numberOfPlayers > 1)
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

        if(m_numberOfPlayers > 1)
            GlRenderer::resetViewport();

        c++;
    }

    //Draw camera separators
    for(c = 1; c < m_cameras.size(); c++)
        GlRenderer::renderRect(0, float(m_cameras[c].h()) * c - 1, m_cameras[c].w(), 2, 0.f, 0.f, 0.f, 1.f);

    if(PGE_Window::showDebugInfo)
    {
        //FontManager::printText(fmt::format("Camera X={0} Y={1}", cam_x, cam_y), 200,10);
        int dpos = 60;
        FontManager::printText(fmt::format("Player J={0} G={1} F={2}; TICK-SUB: {3}\n"
                                       "NPC's: {4}, Active {5}; BLOCKS: {6}",
                                        int(m_debug_player_jumping),
                                        int(m_debug_player_onground),
                                        int(m_debug_player_foots),
                                        uTickf,
                                        m_itemsNpc.size(),
                                        m_npcActive.size(),
                                        m_itemsBlocks.size()), 10, dpos);
        dpos += 35;
        FontManager::printText(fmt::format("Visible objects: {0}", !m_cameras.empty() ? m_cameras[0].renderObjects_count() : 0), 10, dpos);
        dpos += 35;
        FontManager::printText(fmt::format("Delays E={0} R={1} P={2}",
                                            m_debug_event_delay,
                                            m_debug_render_delay,
                                            m_debug_phys_delay), 10, dpos);
        dpos += 35;
        FontManager::printText(fmt::format("Time Real:{0}\nTime Loop:{1}",
                                            debug_TimeReal.elapsed(),
                                            debug_TimeCounted), 10, dpos);
        dpos += 35;

        if(!m_isLevelContinues)
        {
            FontManager::printText(fmt::format("Exit delay {0}, {1}",
                                                m_exitLevelDelay,
                                                uTickf), 10, dpos, 0, 1.0, 0, 0, 1.0);
            dpos += 35;
        }

        if(m_placingMode)
            FontManager::printText(fmt::format("Placing! {0} X={1} Y={2}",
                                                m_placingMode_item_type,
                                                m_placingMode_renderAt.x(),
                                                m_placingMode_renderAt.y()), 10, 10, 0);
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

    if(m_placingMode)
        drawPlacingItem();
    if(m_loaderIsWorks)
        drawLoader();
    if(m_isPauseMenu)
        m_pauseMenu.render();
    // Flip the syncronious blinker flag
    m_blinkStateFlag = !m_blinkStateFlag;
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
                if(!m_itemsPlayers.empty())
                    launchEffect(1, m_itemsPlayers.front()->posX(), m_itemsPlayers.front()->posY(), 0, 2000, 0, 0, 0);
            }
            break;

            case SDLK_2:
            {
                if(!m_itemsPlayers.empty())
                    launchEffect(1, m_itemsPlayers.front()->posX(), m_itemsPlayers.front()->posY(), 0, 2000, 3, -6, 12);

                break;
            }

            case SDLK_3:
            {
                if(!m_itemsPlayers.empty())
                {
                    Scene_Effect_Phys p;
                    p.decelerate_x = 0.02;
                    p.max_vel_y = 12;
                    LVL_Player *pl = m_itemsPlayers.front();
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, -3, -6, 5, 0, p);
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, -4, -7, 5, 0, p);
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, 3, -6, 5, 0, p);
                    launchEffect(1, pl->posX(), pl->posY(), 0, 5000, 4, -7, 5, 0, p);
                }

                break;
            }

            case SDLK_4:
            {
                if(!m_itemsPlayers.empty())
                {
                    Scene_Effect_Phys p;
                    p.max_vel_y = 12;
                    LVL_Player *pl = m_itemsPlayers.front();
                    launchEffect(11, pl->posX(), pl->posY(), 0, 5000, 0, -3, 12, 0, p);
                }

                break;
            }

            case SDLK_5:
            {
                if(!m_itemsPlayers.empty())
                    launchEffect(10, m_itemsPlayers.front()->posX(), m_itemsPlayers.front()->posY(), 1, 0, 0, 0, 0);
                break;
            }

            case SDLK_7:
            {
                if(m_itemsPlayers.size() >= 1)
                    m_itemsPlayers[0]->setCharacterSafe(m_itemsPlayers[0]->characterID - 1, m_itemsPlayers[0]->stateID);

                break;
            }

            case SDLK_8:
            {
                if(m_itemsPlayers.size() >= 1)
                    m_itemsPlayers[0]->setCharacterSafe(m_itemsPlayers[0]->characterID + 1, m_itemsPlayers[0]->stateID);

                break;
            }

            case SDLK_9:
            {
                if(m_itemsPlayers.size() >= 2)
                    m_itemsPlayers[1]->setCharacterSafe(2, 1);

                if(m_itemsPlayers.size() >= 1)
                    m_itemsPlayers[0]->setCharacterSafe(m_itemsPlayers[0]->characterID, m_itemsPlayers[0]->stateID - 1);

                break;
            }

            case SDLK_0:
            {
                if(m_itemsPlayers.size() >= 2)
                    m_itemsPlayers[1]->setCharacterSafe(2, 2);
                else if(m_itemsPlayers.size() >= 1)
                    m_itemsPlayers[0]->setCharacterSafe(m_itemsPlayers[0]->characterID, m_itemsPlayers[0]->stateID + 1);

                break;
            }

            case SDLK_F5:
            {
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
                m_isTimeStopped = !m_isTimeStopped;
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
    return &m_luaEngine;
}

int LevelScene::exec()
{
    m_isLevelContinues = true;
    m_doExit = false;
    m_isRunning = true;
    LoopTiming times;
    times.start_common = SDL_GetTicks();
    //Set black color clearer
    GlRenderer::setClearColor(0.f, 0.f, 0.f, 1.0f);

    /****************Initial update***********************/
    //Apply musics and backgrounds
    for(size_t i = 0; i < m_cameras.size(); i++)
    {
        //Play music from first camera only
        if(i == 0)
            m_cameras[i].cur_section->playMusic();
        m_cameras[i].cur_section->initBG();
    }

    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    m_player1Controller->resetControls();
    m_player2Controller->resetControls();

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
            m_debug_event_delay = static_cast<int>(times.stop_events - times.start_events);

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
            m_debug_phys_delay  = static_cast<int>(times.stop_physics - times.start_physics);

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
            times.doUpdate_render = m_frameSkip ? uTickf + (times.stop_render - times.start_render) : 0;

            if(PGE_Window::showDebugInfo)
                m_debug_render_delay = static_cast<int>(times.stop_render - times.start_render);
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

    return m_exitLevelCode;
}



std::string LevelScene::getLastError()
{
    return m_errorMsg;
}



bool LevelScene::isExit()
{
    return !m_isLevelContinues;
}

void LevelScene::setExiting(int delay, int reason)
{
    m_exitLevelDelay   = delay;
    m_exitLevelCode    = reason;
    m_isLevelContinues = false;
}

std::string LevelScene::toAnotherLevel()
{
    if(!m_warpToLevelFile.empty())
        if(!Files::hasSuffix(m_warpToLevelFile, ".lvl") &&
           !Files::hasSuffix(m_warpToLevelFile, ".lvlx"))
            m_warpToLevelFile.append(".lvl");
    return m_warpToLevelFile;
}

unsigned long LevelScene::toAnotherEntrance()
{
    return m_warpToArrayID;
}

PGE_Point LevelScene::toWorldXY()
{
    return m_warpToWorldXY;
}

int LevelScene::exitType()
{
    return m_exitLevelCode;
}



void LevelScene::setGameState(EpisodeState *_gameState)
{
    m_gameState = _gameState;
    m_numberOfPlayers = 1;

    if(m_gameState)
    {
        m_numberOfPlayers = m_gameState->numOfPlayers;

        if((m_gameState->isEpisode) && (!m_gameState->isHubLevel))
            initPauseMenu2();
        else if(m_gameState->isTestingModeL)
            initPauseMenu3();
        else
            initPauseMenu1();
    }
}

LevelScene::LVL_NpcsArray &LevelScene::getNpcs()
{
    return m_itemsNpc;
}

LevelScene::LVL_NpcActiveSet &LevelScene::getActiveNpcs()
{
    return m_npcActive;
}

LevelScene::LVL_BlocksArray &LevelScene::getBlocks()
{
    return m_itemsBlocks;
}

LevelScene::LVL_BgosArray &LevelScene::getBGOs()
{
    return m_itemsBgo;
}

LevelScene::LVL_PlayersArray &LevelScene::getPlayers()
{
    return m_itemsPlayers;
}
