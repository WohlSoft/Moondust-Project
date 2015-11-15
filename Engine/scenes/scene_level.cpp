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
#include <settings/debugger.h>

#include <graphics/gl_renderer.h>

#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>
#include <data_configs/config_manager.h>
#include <fontman/font_manager.h>

#include <gui/pge_msgbox.h>
#include <gui/pge_textinputbox.h>

#include <networking/intproc.h>
#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>
#include <settings/global_settings.h>

#include "level/lvl_scene_ptr.h"

#include <QApplication>
#include <QtDebug>

#include <common_features/logger.h>

#include <QElapsedTimer>

QElapsedTimer debug_TimeReal;
int           debug_TimeCounted=0;

LevelScene::LevelScene()
    : Scene(Level), luaEngine(this)
{
    LvlSceneP::s = this;

    data = FileFormats::CreateLevelData();
    data.ReadFileValid = false;

    isInit=false;
    isInitFailed=false;
    isWarpEntrance=false;
    cameraStartDirected=false;
    cameraStartDirection=0;

    /*********Physics**********/
    globalGravity=1.0;
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

    /**************************/
    placingMode=false;
    placingMode_item_type=0;
    placingMode_block=FileFormats::CreateLvlBlock();
    placingMode_bgo  =FileFormats::CreateLvlBgo();
    placingMode_npc  =FileFormats::CreateLvlNpc();
    /**************************/

    /*********Default players number*************/
    numberOfPlayers=2;
    /*********Default players number*************/

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
        plr->_syncPosition();
    }
    for(int i=0;i<active_npcs.size();i++)
    {
        active_npcs[i]->iterateStep(ticks);
        active_npcs[i]->_syncPosition();
    }

    //Collide!
    for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
    {
        LVL_Player*plr=(*it);
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

    layers.members.clear();

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
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy BGO";
    while(!bgos.isEmpty())
    {
        LVL_Bgo* tmp;
        tmp = bgos.last();
        bgos.pop_back();
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy NPC";
    npcs.clear();
//    while(!npcs.isEmpty())
//    {
//        LVL_Npc* tmp;
//        tmp = npcs.last();
//        npcs.pop_back();
//    }


    qDebug() << "Destroy Warps";
    while(!warps.isEmpty())
    {
        LVL_Warp* tmp;
        tmp = warps.first();
        warps.pop_front();
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy Physical Environment zones";
    while(!physenvs.isEmpty())
    {
        LVL_PhysEnv* tmp;
        tmp = physenvs.first();
        physenvs.pop_front();
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy sections";
    sections.clear();

    luaEngine.shutdown();

    destroyLoaderTexture();

    delete player1Controller;
    delete player2Controller;
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
    if(luaEngine.shouldShutdown()){
        fader.setFade(10, 1.0f, 1.0f);
        setExiting(0, LvlExit::EXIT_MenuExit);
    }
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
        events.processTimers(uTickf);

        processEffects(uTickf);

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Make world step
            processPhysics(uTickf);
        }

        while(!block_transforms.isEmpty())
        {
            transformTask_block x = block_transforms.first();
            x.block->transformTo_x(x.id);
            block_transforms.pop_front();
        }

        //Update controllers
        player1Controller->sendControls();
        player2Controller->sendControls();

        //update players
        for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
        {
            LVL_Player*plr=(*it);
            plr->update(uTickf);
            if(PGE_Window::showDebugInfo)
            {
                debug_player_jumping = plr->JumpPressed;
                debug_player_onground= plr->onGround();
                debug_player_foots   = plr->foot_contacts_map.size();
            }
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
            if(active_npcs[i]->isKilled())
            {
                active_npcs.removeAt(i); i--;
            }
            else
            if(active_npcs[i]->activationTimeout<=0)
            {
                if(!active_npcs[i]->warpSpawing)
                    active_npcs[i]->deActivate();
                if(active_npcs[i]->wasDeactivated)
                {
                    if(!isVizibleOnScreen(active_npcs[i]->posRect)||!active_npcs[i]->isVisible() || !active_npcs[i]->is_activity)
                    {
                        active_npcs[i]->wasDeactivated=false;
                        active_npcs.removeAt(i); i--;
                    }
                }
            }
        }

        while(!dead_npcs.isEmpty())
        {
            LVL_Npc *corpse = dead_npcs.last();
            dead_npcs.pop_back();
            #if (QT_VERSION >= 0x050400)
            active_npcs.removeAll(corpse);
            npcs.removeAll(corpse);
            layers.removeRegItem(corpse->data.layer, corpse);
            #else
            //He-he, it's a great workaround for a Qt less than 5.4 which has QVector without removeAll() function
            while(1)
            {
                const QVector<LVL_Npc *>::const_iterator ce = active_npcs.cend(), cit = std::find(active_npcs.cbegin(), ce, corpse);
                if (cit == ce)
                    break;
                const QVector<LVL_Npc *>::iterator e = active_npcs.end(), it = std::remove(active_npcs.begin() + (cit - active_npcs.cbegin()), e, corpse);
                active_npcs.erase(it, e);
                break;
            }

            while(1)
            {
                const QVector<LVL_Npc *>::const_iterator ce = npcs.cend(), cit = std::find(npcs.cbegin(), ce, corpse);
                if (cit == ce)
                    break;
                const QVector<LVL_Npc *>::iterator e = npcs.end(), it = std::remove(npcs.begin() + (cit - npcs.cbegin()), e, corpse);
                npcs.erase(it, e);
                break;
            }
            #endif
            luaEngine.destoryLuaNpc(corpse);
        }

        if(!dead_players.isEmpty())
            LvlSceneP::s->checkPlayers();

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

        //Clear garbage (be careful!)
        //luaEngine.runGarbageCollector();
    }

    //Recive external commands!
    if(IntProc::enabled && IntProc::hasCommand())
    {
        switch(IntProc::commandType())
        {
            case IntProc::MessageBox:
            {
                PGE_MsgBox msgBox = PGE_MsgBox(this, IntProc::getCMD(),
                                  PGE_MsgBox::msg_info, PGE_Point(-1, -1),
                                   ConfigManager::setup_message_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
                msgBox.exec(); break;
            }
            case IntProc::Cheat:
                break;
            case IntProc::PlaceItem:
            {
                QString raw = IntProc::getCMD();
                WriteToLog(QtDebugMsg, raw);
                LevelData got=FileFormats::ReadExtendedLvlFile(raw, ".");
                if(!got.ReadFileValid)
                {
                    WriteToLog(QtDebugMsg, FileFormats::errorString);
                    return;
                }

                PGE_Audio::playSoundByRole(obj_sound_role::PlayerGrab2);

                if(raw.startsWith("BLOCK_PLACE", Qt::CaseInsensitive))
                {
                    if(got.blocks.isEmpty()) break;
                    placingMode=true;
                    placingMode_item_type=0;
                    placingMode_block=got.blocks[0];
                } else
                if(raw.startsWith("BGO_PLACE", Qt::CaseInsensitive))
                {
                    if(got.bgo.isEmpty()) break;
                    placingMode=true;
                    placingMode_item_type=1;
                    placingMode_bgo=got.bgo[0];
                } else
                if(raw.startsWith("NPC_PLACE", Qt::CaseInsensitive))
                {
                    if(got.npc.isEmpty()) break;
                    placingMode=true;
                    placingMode_item_type=2;
                    placingMode_npc=got.npc[0];
                }
                else PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);

                break;
            }
        }
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

        const int render_sz = cam->renderObjects().size();
        PGE_Phys_Object** render_obj = cam->renderObjects().data();
        for(int i=0; i<render_sz; i++)
        {
            switch(render_obj[i]->type)
            {
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLNPC:
            case PGE_Phys_Object::LVLPlayer:
                render_obj[i]->render(cam->posX(), cam->posY());
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

        int dpos=100;
        FontManager::printText(QString("Player J=%1 G=%2 F=%3; TICK-SUB: %4\nNPC's: %5, Active %6")
                               .arg(debug_player_jumping)
                               .arg(debug_player_onground)
                               .arg(debug_player_foots)
                               .arg(uTickf)
                               .arg(npcs.size())
                               .arg(active_npcs.size()), 10,dpos);
        dpos+=35;

        FontManager::printText(QString("Delays E=%1 R=%2 P=%3")
                               .arg(debug_event_delay, 3, 10, QChar('0'))
                               .arg(debug_render_delay, 3, 10, QChar('0'))
                               .arg(debug_phys_delay, 3, 10, QChar('0')), 10,dpos);
        dpos+=35;

        FontManager::printText(QString("Time Real:%1\nTime Loop:%2")
                               .arg(debug_TimeReal.elapsed(), 10, 10, QChar('0'))
                               .arg(debug_TimeCounted, 10, 10, QChar('0')), 10,dpos);
        dpos+=35;

        if(!isLevelContinues)
        {
            FontManager::printText(QString("Exit delay %1, %2")
                                   .arg(exitLevelDelay)
                                   .arg(uTickf), 10, dpos, 0, 1.0, 0, 0, 1.0);
            dpos+=35;
        }

        if(placingMode)
            FontManager::printText(QString("Placing! %1")
                        .arg(placingMode_item_type), 10, 10, 0);
        else
            FontManager::printText(QString("%1")
                        .arg(PGE_MusPlayer::MUS_Title()), 10, 10, 0);
    }
    renderBlack:
    Scene::render();
    if(IsLoaderWorks) drawLoader();

    if(isPauseMenu) _pauseMenu.render();
}

bool slowTimeMode=false;

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
      case SDLK_BACKQUOTE:
      {
          PGE_Debugger::executeCommand(this);
          break;
      }
      case SDLK_1:
      {
        if(!players.isEmpty())
        launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 2000, 0, 0, 0);
      }
      break;
      case SDLK_2:
      {
         if(!players.isEmpty())
          launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 2000, 3, -6, 12);
      }
      break;
      case SDLK_3:
      {
         if(!players.isEmpty())
         {
            Scene_Effect_Phys p;
            p.decelerate_x=0.02;
            p.max_vel_y=12;
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, -3, -6, 5, 0, p);
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, -4, -7, 5, 0, p);
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, 3, -6, 5, 0, p);
            launchStaticEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, 4, -7, 5, 0, p);
         }
      }
      break;
      case SDLK_4:
      {
         if(!players.isEmpty())
         {
            Scene_Effect_Phys p;
            p.max_vel_y=12;
            launchStaticEffect(11, players.first()->posX(), players.first()->posY(), 0, 5000, 0, -3, 12, 0, p);
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
                players[0]->setCharacterSafe(1, 1);
        }
        break;
        case SDLK_8:
        {
            if(players.size()>=1)
              players[0]->setCharacterSafe(1, 2);
        }
        break;
        case SDLK_9:
        {
           if(players.size()>=2)
            players[1]->setCharacterSafe(2, 1);
           else if(players.size()>=1)
            players[0]->setCharacterSafe(2, 1);
        }
        break;
        case SDLK_0:
        {
           if(players.size()>=2)
            players[1]->setCharacterSafe(2, 2);
           else if(players.size()>=1)
            players[0]->setCharacterSafe(2, 2);
        }
        break;

      case SDLK_F5:
        {
          PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
          isTimeStopped=!isTimeStopped;
        }
      break;
        case SDLK_F6:
          {
            PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
            slowTimeMode=!slowTimeMode;
          }
        break;
      default:
        break;
    }
}

void LevelScene::onMousePressed(SDL_MouseButtonEvent &mbevent)
{
    if(!placingMode) return;
    PGE_Point mousePos = GlRenderer::MapToScr(mbevent.x, mbevent.y);
    if( mbevent.button==SDL_BUTTON_LEFT ) {
        placeItemByMouse(mousePos.x(), mousePos.y());
    } else if( mbevent.button==SDL_BUTTON_RIGHT ) {
        placingMode=false;
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

    //Level scene's Loop
 Uint32 start_render=0;
 Uint32 stop_render=0;
  float doUpdate_render=0;

 Uint32 start_physics=0;
 Uint32 stop_physics=0;

  Uint32 start_events=0;
  Uint32 stop_events=0;
  //float junkTicks=0.0f;

  //StTimePt start_common=StClock::now();
  Uint32 start_common = SDL_GetTicks();
  //#define PassedTime (((float)std::chrono::duration_cast<std::chrono::nanoseconds>(StClock::now()-start_common).count())/1000000.0f)
  #define PassedTime (SDL_GetTicks()-start_common)

    /****************Initial update***********************/
    //Apply musics and backgrounds
    for(int i=0; i<cameras.size(); i++)
    {
        //Play music from first camera only
        if(i==0) cameras[i].cur_section->playMusic();
        cameras[i].cur_section->initBG();
    }
    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    player1Controller->resetControls();
    player2Controller->resetControls();
    if(running) update();

    debug_TimeCounted=0;
    debug_TimeReal.restart();
    /*****************************************************/

    while(running)
    {
        //start_common = StClock::now();
        start_common = SDL_GetTicks();

        debug_TimeCounted+=uTickf;

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
        if((PGE_Window::vsync)||(doUpdate_render<=0.f))
        {
            start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            glFlush();
            PGE_Window::rePaint();
            stop_render=SDL_GetTicks();
            doUpdate_render = frameSkip? uTickf+(stop_render-start_render) : 0;
            if(PGE_Window::showDebugInfo) debug_render_delay = stop_render-start_render;
        }
        doUpdate_render -= uTickf;
        if(stop_render < start_render) { stop_render=0; start_render=0; }
        /****************************************************************************/

        //printf("U-%08.5f, P-%08.5f, J-%08.5f", uTickf, PassedTime, junkTicks);
        //fflush(stdout);
        if( (!PGE_Window::vsync) && (uTick > (signed)PassedTime) )
        {
            if(!slowTimeMode)
                SDL_Delay( uTick-PassedTime );
            else
                SDL_Delay( uTick-PassedTime+300 );
        } else if(slowTimeMode) SDL_Delay( uTick-PassedTime+300 );
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
    while(!dead_players.isEmpty())
    {
        LVL_Player *corpse = dead_players.last();
        dead_players.pop_back();
        LVL_Player::deathReason reason = corpse->kill_reason;

        #if (QT_VERSION >= 0x050400)
        players.removeAll(corpse);
        #else
        //He-he, it's a great workaround for a Qt less than 5.4 which has QVector without removeAll() function
        while(1)
        {
            const QVector<LVL_Player *>::const_iterator ce = players.cend(), cit = std::find(players.cbegin(), ce, corpse);
            if (cit == ce)
                break;
            const QVector<LVL_Player *>::iterator e = players.end(), it = std::remove(players.begin() + (cit - players.cbegin()), e, corpse);
            players.erase(it, e);
            break;
        }
        #endif
        luaEngine.destoryLuaPlayer(corpse);

        switch(reason)
        {
        case LVL_Player::deathReason::DEAD_burn:
        case LVL_Player::deathReason::DEAD_fall:
        case LVL_Player::deathReason::DEAD_killed:
            if(players.size() > 0)
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerDied);
            else
            {
                Mix_HaltChannel(-1);
                PGE_Audio::playSoundByRole(obj_sound_role::LevelFailed);
            }
            break;
        }
        if(reason==LVL_Player::deathReason::DEAD_burn)
            PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
    }

    if(players.isEmpty())
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

LevelScene::LVL_NpcsArray &LevelScene::getNpcs()
{
    return npcs;
}

LevelScene::LVL_NpcsArray &LevelScene::getActiveNpcs()
{
    return active_npcs;
}


LevelScene::LVL_PlayersArray& LevelScene::getPlayers()
{
    return players;
}


