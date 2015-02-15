/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "common_features/app_path.h"
#include "common_features/graphics_funcs.h"

#include "../graphics/gl_renderer.h"

#include "level/lvl_scene_ptr.h"

#include "../data_configs/config_manager.h"

#include "../fontman/font_manager.h"

#include "../gui/pge_msgbox.h"

#include <QtDebug>

LevelScene::LevelScene()
    : Scene(Level)
{
    LvlSceneP::s = this;

    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    isInit=false;
    isWarpEntrance=false;

    isPauseMenu=false;
    isTimeStopped=false;

    /*********Exit*************/
    isLevelContinues=true;

    doExit = false;
    exitLevelDelay=3000;
    exitLevelCode = EXIT_Closed;
    warpToLevelFile = "";
    warpToArrayID = 0;
    NewPlayerID = 1;
    /**************************/

    /*********Default players number*************/
    numberOfPlayers=1;
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


    /*********Z-Layers*************/
    //Default Z-Indexes
    //set Default Z Indexes
    Z_backImage = -1000; //Background

    //Background-2
    Z_BGOBack2 = -160; // backround BGO

    Z_blockSizable = -150; // sizable blocks

    //Background-1
    Z_BGOBack1 = -100; // backround BGO

    Z_npcBack = -10; // background NPC
    Z_Block = 1; // standart block
    Z_npcStd = 30; // standart NPC
    Z_Player = 35; //player Point

    //Foreground-1
    Z_BGOFore1 = 50; // foreground BGO
    Z_BlockFore = 100; //LavaBlock
    Z_npcFore = 150; // foreground NPC
    //Foreground-2
    Z_BGOFore2 = 160; // foreground BGO

    Z_sys_PhysEnv = 500;
    Z_sys_door = 700;
    Z_sys_interspace1 = 1000; // interSection space layer
    Z_sys_sctBorder = 1020; // section Border
    /*********Z-Layers*************/

    errorMsg = "";
    //qDebug() << 1000.0/(float)PGE_Window::PhysStep;
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

    //destroy textures
    qDebug() << "clear textures";
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
        keyboard1.removeFromControl(tmp);
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


    qDebug() << "Destroy Warps";
    while(!warps.isEmpty())
    {
        LVL_Warp* tmp;
        tmp = warps.first();
        warps.pop_front();
        if(tmp) delete tmp;
    }

    qDebug() << "Destroy world";
    if(world) delete world; //!< Destroy annoying world, mu-ha-ha-ha >:-D
    world = NULL;

    //destroy players
    //destroy blocks
    //destroy NPC's
    //destroy BGO's

    textures_bank.clear();
}





int i;
int delayToEnter = 1000;
Uint32 lastTicks=0;
bool debug_player_jumping=false;
bool debug_player_onground=false;
int  debug_player_foots=0;
int  uTick = 1;

void LevelScene::update()
{
    uTick = abs((1000.0/(float)PGE_Window::PhysStep)-lastTicks);
    if(uTick<=0) uTick=1;

    if(doExit)
    {
        if(exitLevelDelay>=0)
            exitLevelDelay -= uTick;
        else
        {
            if(exitLevelCode==EXIT_Closed)
            {
                fader_opacity=1.0f;
                isLevelContinues=false;
            }
            else
            {
                if(fader_opacity<=0.0f) setFade(25, 1.0f, 0.02f);
                if(fader_opacity>=1.0)
                    isLevelContinues=false;
            }
        }
    }
    else
    if(!isPauseMenu) //Update physics is not pause menu
    {
        //Make world step
        world->Step(1.0f / (float)PGE_Window::PhysStep, 1, 1);

        //Update controllers
        keyboard1.sendControls();

        //update players
        for(i=0; i<players.size(); i++)
        {
            if(PGE_Window::showDebugInfo)
            {
                debug_player_jumping=players[i]->JumpPressed;
                debug_player_onground=players[i]->onGround;
                debug_player_foots=players[i]->foot_contacts;
            }
            players[i]->update();
        }

        //Enter players via warp
        if(isWarpEntrance)
        {
            if(delayToEnter<=0)
            {
                PlayerPoint newPoint;

                    newPoint.id = NewPlayerID;
                    newPoint.x=0;
                    newPoint.y=0;
                    newPoint.w=24;
                    newPoint.h=54;
                    newPoint.direction=1;

                addPlayer(newPoint, true);

                    NewPlayerID++;
                    numberOfPlayers--;
                    delayToEnter = 1000;
                    if(numberOfPlayers<=0)
                        isWarpEntrance = false;
            }
            else
            {
                delayToEnter-= uTick;
            }
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

    //Move to center of the screen
    //glTranslatef( PGE_Window::Width / 2.f, PGE_Window::Height / 2.f, 0.f );

    long cam_x=0, cam_y=0;

    if(!isInit) goto renderBlack;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);

    foreach(PGE_LevelCamera* cam, cameras)
    {
        backgrounds.last()->draw(cam->posX(), cam->posY());

        if(PGE_Window::showDebugInfo)
        {
            cam_x = cam->posX();
            cam_y = cam->posY();
        }

        foreach(PGE_Phys_Object * item, cam->renderObjects())
        {
            switch(item->type)
            {
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLPlayer:
                item->render(cam->posX(), cam->posY());
                break;
            default:
                break;
            }
        }
    }

    //FontManager::printText("Hello world!\nПривет мир!", 10,10);

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(QString("Camera X=%1 Y=%2").arg(cam_x).arg(cam_y), 200,10);

        FontManager::printText(QString("Player J=%1 G=%2 F=%3; TICK-SUB: %4")
                               .arg(debug_player_jumping)
                               .arg(debug_player_onground)
                               .arg(debug_player_foots)
                               .arg(uTick), 10,100);

        if(doExit)
            FontManager::printText(QString("Exit delay %1, %2")
                                   .arg(exitLevelDelay)
                                   .arg(uTick), 10, 140, 10, qRgb(255,0,0));
    }

    renderBlack:

    Scene::render();

    if(IsLoaderWorks) drawLoader();
}



int LevelScene::exec()
{
    bool running = true;
    isLevelContinues=true;
    doExit=false;


    //Level scene's Loop
 Uint32 start_render;
 Uint32 stop_render;
    int doUpdate_render=0;

 Uint32 start_physics;
 Uint32 stop_physics;
  float doUpdate_physics=0;

  float timeFPS = 1000.0 / (float)PGE_Window::MaxFPS;
  float timeStep = 1000.0 / (float)PGE_Window::PhysStep;

    uTick = 1;

    while(running)
    {

        SDL_Event event; //  Events of SDL
        while ( SDL_PollEvent(&event) )
        {
            keyboard1.update(event);
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        if(doExit) break;
                        setExiting(0, EXIT_Closed);
                    }   // End work of program
                break;

                case SDL_KEYDOWN: // If pressed key
                  switch(event.key.keysym.sym)
                  { // Check which
                    case SDLK_ESCAPE: // ESC
                            {
                                setExiting(0, EXIT_MenuExit);
                            }   // End work of program
                        break;
                    case SDLK_RETURN:// Enter
                        {
                            if(doExit) break;
                            isPauseMenu = true;
                        }
                    break;
                    case SDLK_t:
                        PGE_Window::SDL_ToggleFS(PGE_Window::window);
                    break;
                    case SDLK_F3:
                        PGE_Window::showDebugInfo=!PGE_Window::showDebugInfo;
                    break;
                    case SDLK_F12:
                        GlRenderer::makeShot();
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

        start_physics=SDL_GetTicks();
        update();
        stop_physics=SDL_GetTicks();

        if(doUpdate_render<=0)
        {
            start_render = SDL_GetTicks();
            render();
            glFlush();
            SDL_GL_SwapWindow(PGE_Window::window);
            stop_render = SDL_GetTicks();

            if( timeFPS > stop_render-start_render)
                    doUpdate_render = timeFPS - (stop_render-start_render);
        }
        doUpdate_render -= timeStep;

        if(stop_render<start_render)
            {stop_render=doUpdate_render; start_render=0;}

        doUpdate_physics=1;
        lastTicks=1;
        if( timeStep > (stop_physics-start_physics)-(stop_render-start_render))
        {
            doUpdate_physics = timeStep-(stop_physics-start_physics)-(stop_render-start_render);
            if(doUpdate_physics<=0) doUpdate_physics=1;
            lastTicks = doUpdate_physics;
            SDL_Delay( doUpdate_physics );
        }
        stop_render=0;
        start_render=0;

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

    if(!haveLivePlayers)
    {
        setExiting(4000, EXIT_PlayerDeath);
    }
}


void LevelScene::setExiting(int delay, int reason)
{
    exitLevelDelay = delay;
    exitLevelCode = reason;
    doExit = true;
}



