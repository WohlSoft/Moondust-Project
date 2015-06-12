/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <graphics/gl_renderer.h>

#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>

#include "level/lvl_scene_ptr.h"

#include <data_configs/config_manager.h>

#include <fontman/font_manager.h>

#include <gui/pge_msgbox.h>

#include <networking/intproc.h>
#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>

#include <QtDebug>

#include <physics/phys_debug_draw.h>

#include <settings/global_settings.h>

DebugDraw dbgDraw;

LevelScene::LevelScene()
    : Scene(Level), luaEngine(this)
{
    LvlSceneP::s = this;

    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    isInit=false;
    isWarpEntrance=false;
    cameraStartDirected=false;
    cameraStartDirection=0;

    /*********Physics**********/
    gravity=26;
    /**************************/

    /*********Exit*************/
    isLevelContinues=true;

    doExit = false;
    exitLevelDelay=3000;
    exitLevelCode = LvlExit::EXIT_Closed;
    lastWarpID=0;
    warpToLevelFile = "";
    warpToArrayID = 0;
    warpToWorld=false;
    NewPlayerID = 1;
    /**************************/

    /*********Default players number*************/
    numberOfPlayers=2;
    /*********Default players number*************/

    world=NULL;

    /*********Loader*************/
    IsLoaderWorks=false;
    /*********Loader*************/

    /*********Fader*************/
    fader.setFull();
    /*********Fader*************/

    /*********Controller********/
    player1Controller = AppSettings.openController(1);
    player2Controller = AppSettings.openController(2);
    /*********Controller********/

    /*********Pause menu*************/
    initPauseMenu1();
    /*********Pause menu*************/

    frameSkip=AppSettings.frameSkip;

    errorMsg = "";

    gameState = NULL;

    debug_player_jumping=false;
    debug_player_onground=false;
    debug_player_foots=0;
    debug_render_delay=0;
    debug_phys_delay=0;
    debug_event_delay=0;
}

void LevelScene::initPauseMenu1()
{
    _pauseMenu_opened=false;
    _pauseMenuID=1;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct("Pause", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                         ConfigManager::setup_menu_box.box_padding,
                         ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    QStringList items;
    items<<"Continue";
    items<<"Save and continue";
    items<<"Save and quit";
    items<<"Exit without saving";
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu=false;
    isTimeStopped=false;
}

void LevelScene::initPauseMenu2()
{
    _pauseMenu_opened=false;
    _pauseMenuID=2;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct("Pause", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                         ConfigManager::setup_menu_box.box_padding,
                         ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    QStringList items;
    items<<"Continue";
    items<<"Quit";
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu=false;
    isTimeStopped=false;
}

void LevelScene::processPauseMenu()
{
    if(!_pauseMenu_opened)
    {
        _pauseMenu.restart();
        _pauseMenu_opened=true;
        PGE_Audio::playSoundByRole(obj_sound_role::MenuPause);
    }
    else
    {
        _pauseMenu.update(uTickf);
        if(!_pauseMenu.isRunning())
        {
            if(_pauseMenuID==1)
            {
                switch(_pauseMenu.answer())
                {
                case PAUSE_Continue:
                    //do nothing!!
                break;
                case PAUSE_SaveCont:
                    //Save game state!
                    gameState->save();
                break;
                case PAUSE_SaveQuit:
                    //Save game state! and exit from episode
                    gameState->save();
                    setExiting(0, LvlExit::EXIT_MenuExit);
                    break;
                case PAUSE_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, LvlExit::EXIT_MenuExit);
                break;
                default: break;
                }
            } else {
                switch(_pauseMenu.answer())
                {
                case PAUSE_2_Continue:
                    //do nothing!!
                break;
                case PAUSE_2_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, LvlExit::EXIT_MenuExit);
                break;
                default: break;
                }
            }
            _pauseMenu_opened=false;
            isPauseMenu=false;
        }
    }
}

void LevelScene::processPhysics(float ticks)
{
    //Iterate
    for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
    {
        LVL_Player*plr=(*it);
        plr->iterateStep(ticks);
        plr->_syncBox2dWithPos();
    }
    for(int i=0;i<active_npcs.size();i++)
    {
        active_npcs[i]->iterateStep(ticks);
        active_npcs[i]->_syncBox2dWithPos();
    }

    //Collide!
    for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
    {
        LVL_Player*plr=(*it);
        plr->foot_contacts_map.clear();
        plr->onGround=false;
        plr->foot_sl_contacts_map.clear();
        plr->contactedWarp = NULL;
        plr->contactedWithWarp=false;
        plr->climbable_map.clear();
        plr->environments_map.clear();
        plr->updateCollisions();
    }
    for(int i=0;i<active_npcs.size();i++)
    {
        active_npcs[i]->updateCollisions();
    }

}



LevelScene::~LevelScene()
{

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    LvlSceneP::s = NULL;
    //stop animators

    //desroy animators

    switch_blocks.clear();
    //destroy textures
    qDebug() << "clear level textures";
    while(!textures_bank.isEmpty())
    {
        GlRenderer::deleteTexture( textures_bank[0] );
        textures_bank.pop_front();
    }

    qDebug() << "Destroy cameras";
    cameras.clear();

    qDebug() << "Destroy players";
    while(!players.isEmpty())
    {
        LVL_Player* tmp;
        tmp = players.last();
        players.pop_back();
        player1Controller->removeFromControl(tmp);
        if(tmp)
        {
            if(!tmp->isLuaPlayer)
                delete tmp;
        }
    }

    qDebug() << "Destroy blocks";
    while(!blocks.isEmpty())
    {
        LVL_Block* tmp;
        tmp = blocks.last();
        blocks.pop_back();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy BGO";
    while(!bgos.isEmpty())
    {
        LVL_Bgo* tmp;
        tmp = bgos.last();
        bgos.pop_back();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy NPC";
    while(!npcs.isEmpty())
    {
        LVL_Npc* tmp;
        tmp = npcs.last();
        npcs.pop_back();
        if(tmp) delete tmp;
    }




    qDebug() << "Destroy Warps";
    while(!warps.isEmpty())
    {
        LVL_Warp* tmp;
        tmp = warps.first();
        warps.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy Physical Environment zones";
    while(!physenvs.isEmpty())
    {
        LVL_PhysEnv* tmp;
        tmp = physenvs.first();
        physenvs.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy sections";
    sections.clear();

    luaEngine.shutdown();

    delete player1Controller;
    delete player2Controller;

    qDebug() << "Destroy world";
    if(world) delete world; //!< Destroy annoying world, mu-ha-ha-ha >:-D
    world = NULL;

    //destroy players
    //destroy blocks
    //destroy NPC's
    //destroy BGO's

    textures_bank.clear();

}


void LevelScene::tickAnimations(float ticks)
{
    //tick animation
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_Blocks.begin();
        it!=ConfigManager::Animator_Blocks.end(); it++)
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_BGO.begin();
        it!=ConfigManager::Animator_BGO.end(); it++)
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_BG.begin();
        it!=ConfigManager::Animator_BG.end(); it++)
        it->manualTick(ticks);
    for(QList<AdvNpcAnimator>::iterator it=ConfigManager::Animator_NPC.begin();
        it!=ConfigManager::Animator_NPC.end(); it++)
        it->manualTick(ticks);
}


void LevelScene::update()
{
    Scene::update();
    tickAnimations(uTickf);

    if(!isLevelContinues)
    {
        exitLevelDelay -= uTickf;
        if(exitLevelDelay<=0.f)
        {
            doExit=true;
            if(fader.isNull())
            {
                if(PGE_MusPlayer::MUS_IsPlaying())
                    PGE_MusPlayer::MUS_stopMusicFadeOut(500);
                fader.setFade(10, 1.0f, 0.01f);
            }
        }
    }

    if(doExit)
    {
        if(exitLevelCode==LvlExit::EXIT_Closed)
        {
            fader.setFull();
            running=false;
        }
        else
        {
            if(fader.isFull())
                running=false;
        }
    } else if(isPauseMenu) {
        processPauseMenu();
    } else {//Update physics is not pause menu

        updateLua();//Process LUA code
        system_events.processEvents(uTickf);

        processEffects(uTickf);

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Make world step
            //world->Step(uTickf/1000.f, 1, 1);
        }

        processPhysics(uTickf);

        while(!block_transforms.isEmpty())
        {
            transformTask_block x = block_transforms.first();
            if(ConfigManager::lvl_block_indexes.contains(x.id))
                x.block->setup = &ConfigManager::lvl_block_indexes[x.id];
            else
            {
                block_transforms.pop_front();
                continue;
            }
            x.block->data.id = block_transforms.first().id;
            x.block->transformTo_x(x.id);
            x.block->init();

            block_transforms.pop_front();
        }

        //Update controllers
        player1Controller->sendControls();
        player2Controller->sendControls();

        //update players
        for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
        {
            LVL_Player*plr=(*it);
            if(PGE_Window::showDebugInfo)
            {
                debug_player_jumping = plr->JumpPressed;
                debug_player_onground= plr->onGround;
                debug_player_foots   = plr->foot_contacts_map.size();
            }
            plr->update(uTickf);
        }

        for(int i=0;i<fading_blocks.size();i++)
        {
            if(fading_blocks[i]->tickFader(uTickf))
            {
                fading_blocks.removeAt(i); i--;
            }
        }

        for(int i=0;i<active_npcs.size();i++)
        {
            active_npcs[i]->update(uTickf);
            if(active_npcs[i]->killed)
            {
                active_npcs.removeAt(i); i--;
            }
            else
            if(active_npcs[i]->timeout<=0)
            {
                active_npcs[i]->deActivate();
                active_npcs.removeAt(i); i--;
            }
        }

        while(!dead_npcs.isEmpty())
        {
            LVL_Npc *corpse = dead_npcs.last();
            launchStaticEffect(corpse->setup->effect_1,
                               corpse->posX(), corpse->posY(),
                               1, 0, 0,0,0);
            dead_npcs.pop_back();
            npcs.removeAll(corpse);
            delete corpse;
        }

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //update activated NPC's
                //comming soon

            //udate visible Effects and destroy invisible
                //comming soon
        }

        //update cameras
        for(QList<PGE_LevelCamera>::iterator cam=cameras.begin();cam!=cameras.end(); cam++)
            cam->update(uTickf);
    }

    //Recive external commands!
    if(IntProc::enabled && IntProc::cmd_accepted)
    {
        PGE_MsgBox msgBox = PGE_MsgBox(this, IntProc::getCMD(),
                          PGE_MsgBox::msg_info, PGE_Point(-1, -1),
                           ConfigManager::setup_message_box.box_padding,
                           ConfigManager::setup_message_box.sprite);
        msgBox.exec();
    }

}

void LevelScene::processEvents()
{
    #ifndef __APPLE__
    if(AppSettings.interprocessing)
        qApp->processEvents();
    #endif
    player1Controller->update();
    player2Controller->update();
    Scene::processEvents();
}

void LevelScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    //glLoadIdentity();
    int c=0;

    if(!isInit) goto renderBlack;
    for(QList<PGE_LevelCamera>::iterator it=cameras.begin();it!=cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);

        if(numberOfPlayers>1)
            GlRenderer::setViewport(0, cam->h()*c,cam->w(), cam->h());

        cam->drawBackground();

        for(PGE_RenderList::iterator it=cam->renderObjects().begin();it!=cam->renderObjects().end(); it++ )
        {
            PGE_Phys_Object*&item=(*it);
            switch(item->type)
            {
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLNPC:
            case PGE_Phys_Object::LVLPlayer:
                item->render(cam->posX(), cam->posY());
                break;
            default:
                break;
            }
        }

        for(SceneEffectsArray::iterator it=WorkingEffects.begin();it!=WorkingEffects.end(); it++ )
        {
             Scene_Effect &item=(*it);
             item.render(cam->posX(), cam->posY());
        }

        if(PGE_Window::showPhysicsDebug)
        {
            for(PGE_RenderList::iterator it=cam->renderObjects().begin();it!=cam->renderObjects().end(); it++ )
            {
                PGE_Phys_Object*&item=(*it);
                item->renderDebug(cam->posX(), cam->posY());
            }
        }

        cam->drawForeground();

        if(numberOfPlayers>1)
            GlRenderer::resetViewport();
        c++;
    }

    //Draw camera separators
    for(c=1; c<cameras.size(); c++)
    {
        GlRenderer::renderRect(0, cameras[c].h()*c-1, cameras[c].w(), 2, 0.f, 0.f, 0.f, 1.f);
    }


    if(PGE_Window::showDebugInfo)
    {
        //FontManager::printText(QString("Camera X=%1 Y=%2").arg(cam_x).arg(cam_y), 200,10);

        FontManager::printText(QString("Player J=%1 G=%2 F=%3; TICK-SUB: %4\nNPC's: %5, Active %6")
                               .arg(debug_player_jumping)
                               .arg(debug_player_onground)
                               .arg(debug_player_foots)
                               .arg(uTickf)
                               .arg(npcs.size())
                               .arg(active_npcs.size()), 10,100);

        FontManager::printText(QString("Delays E=%1 R=%2 P=%3")
                               .arg(debug_event_delay, 3, 10, QChar('0'))
                               .arg(debug_render_delay, 3, 10, QChar('0'))
                               .arg(debug_phys_delay, 3, 10, QChar('0')), 10,135);

        if(!isLevelContinues)
            FontManager::printText(QString("Exit delay %1, %2")
                                   .arg(exitLevelDelay)
                                   .arg(uTickf), 10, 155, 0, 1.0, 0, 0, 1.0);
        //world->DrawDebugData();
    }
    renderBlack:
    Scene::render();
    if(IsLoaderWorks) drawLoader();

    if(isPauseMenu) _pauseMenu.render();
}



void LevelScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(doExit || isExit()) return;

    if(isPauseMenu) _pauseMenu.processKeyEvent(sdl_key);

    switch(sdl_key)
    { // Check which
      case SDLK_ESCAPE:
      case SDLK_RETURN:     // Toggle pause mode
          {
              if(doExit || isPauseMenu) break;
              isPauseMenu = true;
          }
      break;
      case SDLK_1:
      {
        if(!players.isEmpty())
        launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 2000, 0, 0, 0);
      }
      break;
      case SDLK_2:
      {
         if(!players.isEmpty())
          launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 2000, 6, -20, 12);
      }
      break;
      case SDLK_3:
      {
         if(!players.isEmpty())
         {
            Scene_Effect_Phys p;
            p.decelerate_x=0.02;
            p.max_vel_y=12;
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, -3, -6, 5, p);
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, -4, -7, 5, p);
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, 3, -6, 5, p);
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, 4, -7, 5, p);
         }
      }
      break;
      case SDLK_4:
      {
         if(!players.isEmpty())
         {
            Scene_Effect_Phys p;
            p.max_vel_y=12;
            launchStaticEffect(11, players.first()->posX(), players.first()->posY(), 0, 5000, 0, -7, 5, p);
         }
      }
      break;
      case SDLK_5:
      {
         if(!players.isEmpty())
         {
            launchStaticEffect(10, players.first()->posX(), players.first()->posY(), 1, 0, 0, 0, 0);
         }
      }
      break;

        case SDLK_7:
        {
            if(players.size()>=1)
                players[0]->setCharacter(1, 1);
        }
        break;
        case SDLK_8:
        {
            if(players.size()>=1)
              players[0]->setCharacter(1, 2);
        }
        break;
        case SDLK_9:
        {
           if(players.size()>=2)
            players[1]->setCharacter(2, 1);
           else if(players.size()>=1)
            players[0]->setCharacter(2, 1);
        }
        break;
        case SDLK_0:
        {
           if(players.size()>=2)
            players[1]->setCharacter(2, 2);
           else if(players.size()>=1)
            players[0]->setCharacter(2, 2);
        }
        break;

      case SDLK_F5:
        {
          PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
          isTimeStopped=!isTimeStopped;
        }
      break;
      default:
        break;
    }
}

LuaEngine *LevelScene::getLuaEngine()
{
    return &luaEngine;
}

int LevelScene::exec()
{
    isLevelContinues=true;
    doExit=false;
    running=true;

    dbgDraw.c = &cameras.first();
    world->SetDebugDraw(&dbgDraw);
    dbgDraw.SetFlags( dbgDraw.e_shapeBit | dbgDraw.e_jointBit );

    //Level scene's Loop
 Uint32 start_render=0;
 Uint32 stop_render=0;
    float doUpdate_render=0;

 Uint32 start_physics=0;
 Uint32 stop_physics=0;

  Uint32 start_events=0;
  Uint32 stop_events=0;

  Uint32 start_common=0;

    /****************Initial update***********************/
    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    player1Controller->resetControls();
    player2Controller->resetControls();
    if(running) update();
    /*****************************************************/

    while(running)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo) start_events = SDL_GetTicks();
        /**********************Update common events and controllers******************/
        processEvents();
        /****************************************************************************/
        if(PGE_Window::showDebugInfo) stop_events = SDL_GetTicks();
        if(PGE_Window::showDebugInfo) debug_event_delay = (stop_events-start_events);

        start_physics=SDL_GetTicks();
        /**********************Update physics and game progess***********************/
        update();
        /****************************************************************************/
        stop_physics=SDL_GetTicks();
        if(PGE_Window::showDebugInfo) debug_phys_delay  = (stop_physics-start_physics);

        stop_render=0;
        start_render=0;
        /**********************Process rendering of stuff****************************/
        if(doUpdate_render<=0.f)
        {
            start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            glFlush();
            stop_render=SDL_GetTicks();
            doUpdate_render = frameSkip? (stop_render-start_render) : 0;
            if(PGE_Window::showDebugInfo) debug_render_delay = stop_render-start_render;
        }
        doUpdate_render -= uTickf;
        if(stop_render < start_render) { stop_render=0; start_render=0; }
        /****************************************************************************/

        PGE_Window::rePaint();

        if( uTickf > (float)(SDL_GetTicks()-start_common) )
        {
            wait( uTickf-(float)(SDL_GetTicks()-start_common) );
        }
    }
    return exitLevelCode;
}



QString LevelScene::getLastError()
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
    if(isLevelContinues)
    {
        long snd=0;
        switch(exitLevelCode)
        {
            case  1: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit01); break;
            case  2: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit02); break;
            case  3: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit03); break;
            case  4: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit04); break;
            case  5: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit05); break;
            case  6: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit06); break;
            case  7: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit07); break;
            case  8: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit08); break;
            case  9: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit09); break;
            case 10: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit10); break;
            default: break;
        }
        if(snd>0)
        {
            PGE_MusPlayer::MUS_stopMusic();
            PGE_Audio::playSound(snd);
        }
    }
    isLevelContinues = false;
}

QString LevelScene::toAnotherLevel()
{
    if(!warpToLevelFile.isEmpty())
    if(!warpToLevelFile.endsWith(".lvl", Qt::CaseInsensitive) &&
       !warpToLevelFile.endsWith(".lvlx", Qt::CaseInsensitive))
        warpToLevelFile.append(".lvl");

    return warpToLevelFile;
}

int LevelScene::toAnotherEntrance()
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

void LevelScene::checkPlayers()
{
    bool haveLivePlayers=false;

    for(int i=0; i<players.size(); i++)
    {
        if(players[i]->isLive)
            haveLivePlayers = true;
    }

    for(int i=0; i<players.size(); i++)
    {
        if((!players[i]->isLive) && (!players[i]->locked()))
        {
            switch(players[i]->kill_reason)
            {
            case LVL_Player::deathReason::DEAD_burn:
                PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
                if(!haveLivePlayers)
                    PGE_Audio::playSoundByRole(obj_sound_role::LevelFailed);
                break;
            case LVL_Player::deathReason::DEAD_fall:
            case LVL_Player::deathReason::DEAD_killed:
                if(haveLivePlayers)
                    PGE_Audio::playSoundByRole(obj_sound_role::PlayerDied);
                else
                    PGE_Audio::playSoundByRole(obj_sound_role::LevelFailed);
                break;
            }
            players[i]->setLocked(true);
        }
    }

    if(!haveLivePlayers)
    {
        PGE_MusPlayer::MUS_stopMusic();
        setExiting(4000, LvlExit::EXIT_PlayerDeath);
    }
}


void LevelScene::setGameState(EpisodeState *_gameState)
{
    gameState = _gameState;
    numberOfPlayers = gameState->numOfPlayers;
    if(gameState)
    {
        if(gameState->isEpisode && !gameState->isHubLevel)
            initPauseMenu2();
        else
            initPauseMenu1();
    }
}

