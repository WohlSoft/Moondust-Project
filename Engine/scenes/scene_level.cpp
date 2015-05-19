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
    : Scene(Level)
{
    LvlSceneP::s = this;

    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    uTick = (1000.0/(float)PGE_Window::PhysStep);//-lastTicks;
    if(uTick<=0) uTick=1;

    isInit=false;
    isWarpEntrance=false;
    cameraStartDirected=false;
    cameraStartDirection=0;

    isPauseMenu=false;
    isTimeStopped=false;

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
    fader_opacity=1.0f;
    target_opacity=1.0f;
    fade_step=0.0f;
    fadeSpeed=25;
    /*********Fader*************/

    /*********Controller********/
    player1Controller = AppSettings.openController(1);
    player2Controller = AppSettings.openController(2);
    /*********Controller********/

    errorMsg = "";

    gameState = NULL;
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
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(textures_bank[0].texture) );
        textures_bank.pop_front();
    }


    qDebug() << "Destroy backgrounds";
    while(!backgrounds.isEmpty())
    {
        LVL_Background* tmp;
        tmp = backgrounds.first();
        backgrounds.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy cameras";
    while(!cameras.isEmpty())
    {
        PGE_LevelCamera* tmp;
        tmp = cameras.first();
        cameras.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy players";
    while(!players.isEmpty())
    {
        LVL_Player* tmp;
        tmp = players.first();
        players.pop_front();
        player1Controller->removeFromControl(tmp);
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy blocks";
    while(!blocks.isEmpty())
    {
        LVL_Block* tmp;
        tmp = blocks.first();
        blocks.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy BGO";
    while(!bgos.isEmpty())
    {
        LVL_Bgo* tmp;
        tmp = bgos.first();
        bgos.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy NPC";
    while(!npcs.isEmpty())
    {
        LVL_Npc* tmp;
        tmp = npcs.first();
        npcs.pop_front();
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


void LevelScene::tickAnimations(int ticks)
{
    //tick animation
    for(int i=0; i<ConfigManager::Animator_Blocks.size(); i++)
        ConfigManager::Animator_Blocks[i].manualTick(ticks);
    for(int i=0; i<ConfigManager::Animator_BGO.size(); i++)
        ConfigManager::Animator_BGO[i].manualTick(ticks);
    for(int i=0; i<ConfigManager::Animator_BG.size(); i++)
        ConfigManager::Animator_BG[i].manualTick(ticks);
}



int i;
int delayToEnter = 1000;
Uint32 lastTicks=0;
bool debug_player_jumping=false;
bool debug_player_onground=false;
int  debug_player_foots=0;
int  debug_render_delay=0;
int  debug_phys_delay=0;
int  debug_event_delay=0;

void LevelScene::update()
{
    tickAnimations(uTick);

    if(doExit)
    {
        if(exitLevelDelay>=0)
            exitLevelDelay -= uTick;
        else
        {
            if(exitLevelCode==LvlExit::EXIT_Closed)
            {
                fader_opacity=1.0f;
                isLevelContinues=false;
            }
            else
            {
                if(fader_opacity<=0.0f)
                {
                    if(PGE_MusPlayer::MUS_IsPlaying())
                        PGE_MusPlayer::MUS_stopMusicFadeOut(500);
                    setFade(25, 1.0f, 0.02f);
                }
                if(fader_opacity>=1.0)
                    isLevelContinues=false;
            }
        }
    }
    else
    if(!isPauseMenu) //Update physics is not pause menu
    {
        system_events.processEvents(uTick);

        //Make world step
        world->Step(1.0f / (float)PGE_Window::PhysStep, 1, 1);

        while(!block_transfors.isEmpty())
        {
            transformTask_block x = block_transfors.first();
            if(ConfigManager::lvl_block_indexes.contains(x.id))
                x.block->setup = &ConfigManager::lvl_block_indexes[x.id];
            else
            {
                block_transfors.pop_front();
                continue;
            }
            x.block->data.id = block_transfors.first().id;
            x.block->transformTo_x(x.id);
            x.block->init();

            block_transfors.pop_front();
        }

        //Update controllers
        player1Controller->sendControls();
        player2Controller->sendControls();

        //update players
        for(i=0; i<players.size(); i++)
        {
            if(PGE_Window::showDebugInfo)
            {
                debug_player_jumping=players[i]->JumpPressed;
                debug_player_onground=players[i]->onGround;
                debug_player_foots=players[i]->foot_contacts_map.size();
            }
            players[i]->update(uTick);
        }

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //update activated NPC's
                //comming soon

            //udate visible Effects and destroy invisible
                //comming soon
        }

        //update cameras
        for(i=0; i<cameras.size(); i++)
            cameras[i]->update();
    }

    if(IntProc::enabled && IntProc::cmd_accepted)
    {
        PGE_MsgBox msgBox(this, IntProc::getCMD(),
                          PGE_MsgBox::msg_info);

        if(!ConfigManager::setup_message_box.sprite.isEmpty())
            msgBox.loadTexture(ConfigManager::setup_message_box.sprite);
        msgBox.exec();
    }

    if(isPauseMenu)
    {
        PGE_MsgBox msgBox(this, "This is a dummy pause menu\nJust, for message box test\n\nHello! :D :D :D\n\nXXxxXXXxxxXxxXXXxxXXXxXXxxXXxxXXXxxxXxxXXXxxXXXxXXxxXXxxXXXxxxXxxXXXxxXXXxXXxx",
                          PGE_MsgBox::msg_info);

        if(!ConfigManager::setup_message_box.sprite.isEmpty())
            msgBox.loadTexture(ConfigManager::setup_message_box.sprite);
        msgBox.exec();
        isPauseMenu=false;
    }
}



void LevelScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    if(!isInit) goto renderBlack;
    for(int c=0;c<cameras.size();c++)
    {
        PGE_LevelCamera* cam=cameras[c];

        if(numberOfPlayers>1)
            GlRenderer::setViewport(0, cam->h()*c,cam->w(), cam->h());

        cam->drawBackground();

        foreach(PGE_Phys_Object * item, cam->renderObjects())
        {
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
        if(numberOfPlayers>1)
            GlRenderer::resetViewport();
    }

    if(PGE_Window::showDebugInfo)
    {
        //FontManager::printText(QString("Camera X=%1 Y=%2").arg(cam_x).arg(cam_y), 200,10);

        FontManager::printText(QString("Player J=%1 G=%2 F=%3; TICK-SUB: %4")
                               .arg(debug_player_jumping)
                               .arg(debug_player_onground)
                               .arg(debug_player_foots)
                               .arg(uTick), 10,100);

        FontManager::printText(QString("Delays E=%1 R=%2 P=%3")
                               .arg(debug_event_delay, 3, 10, QChar('0'))
                               .arg(debug_render_delay, 3, 10, QChar('0'))
                               .arg(debug_phys_delay, 3, 10, QChar('0')), 10,120);

        if(doExit)
            FontManager::printText(QString("Exit delay %1, %2")
                                   .arg(exitLevelDelay)
                                   .arg(uTick), 10, 140, 0, 1.0, 0, 0, 1.0);
        //world->DrawDebugData();
    }
    renderBlack:
    Scene::render();
    if(IsLoaderWorks) drawLoader();
}



int LevelScene::exec()
{
    isLevelContinues=true;
    doExit=false;

    dbgDraw.c = cameras.first();

    world->SetDebugDraw(&dbgDraw);

    dbgDraw.SetFlags( dbgDraw.e_shapeBit | dbgDraw.e_jointBit );

    //Level scene's Loop
 Uint32 start_render=0;
 Uint32 stop_render=0;
    int doUpdate_render=0;

 Uint32 start_physics=0;
 Uint32 stop_physics=0;

  Uint32 start_events=0;
  Uint32 stop_events=0;

  Uint32 start_common=0;
     int wait_delay=0;

  //float timeFPS = 1000.0 / (float)PGE_Window::MaxFPS;
  float timeStep = 1000.0 / (float)PGE_Window::PhysStep;

    bool running = true;
    while(running)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
        {
            start_events = SDL_GetTicks();
        }

        #ifndef __APPLE__
        if(AppSettings.interprocessing)
            qApp->processEvents();
        #endif

        player1Controller->update();
        player2Controller->update();

        SDL_Event event; //  Events of SDL
        while ( SDL_PollEvent(&event) )
        {
            if(PGE_Window::processEvents(event)!=0) continue;
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        if(doExit) break;
                        setExiting(0, LvlExit::EXIT_Closed);
                    }   // End work of program
                break;
                case SDL_KEYDOWN: // If pressed key
                  switch(event.key.keysym.sym)
                  { // Check which
                    case SDLK_ESCAPE: // ESC
                            {
                                setExiting(0, LvlExit::EXIT_MenuExit);
                            }   // End work of program
                        break;
                    case SDLK_RETURN:// Enter
                        {
                            if(doExit) break;
                            isPauseMenu = true;
                        }
                    break;
                    default:
                      break;

                  }
                break;

                case SDL_KEYUP:
                switch(event.key.keysym.sym)
                {
                case SDLK_RETURN:// Enter
                    break;
                default:
                    break;
                }
                break;
            }
        }

        if(PGE_Window::showDebugInfo)
        {
            stop_events = SDL_GetTicks();
            start_physics=SDL_GetTicks();
        }

        /**********************Update physics and game progess***********************/
        update();

        if(PGE_Window::showDebugInfo)
        {
            stop_physics=SDL_GetTicks();
        }

        stop_render=0;
        start_render=0;
        if(doUpdate_render<=0)
        {
            start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            glFlush();
            SDL_GL_SwapWindow(PGE_Window::window);
            stop_render = SDL_GetTicks();
            doUpdate_render = stop_render-start_render;
            debug_render_delay = stop_render-start_render;
        }
        doUpdate_render -= timeStep;

        if(stop_render < start_render)
            {stop_render=0; start_render=0;}

        wait_delay=timeStep;
        lastTicks=1;
        if( timeStep > (timeStep-(int)(SDL_GetTicks()-start_common)) )
        {
            wait_delay = timeStep-(int)(SDL_GetTicks()-start_common);
            lastTicks = (stop_physics-start_physics)+(stop_render-start_render)+(stop_events-start_events);
        }
        debug_phys_delay=(stop_physics-start_physics);
        debug_event_delay=(stop_events-start_events);

        if(wait_delay>0)
            SDL_Delay( wait_delay );

        if(isExit())
            running = false;

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

QPoint LevelScene::toWorldXY()
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


void LevelScene::setExiting(int delay, int reason)
{
    exitLevelDelay = delay;
    exitLevelCode = reason;
    doExit = true;
}


void LevelScene::setGameState(EpisodeState *_gameState)
{
    gameState = _gameState;
    numberOfPlayers = gameState->numOfPlayers;
}
