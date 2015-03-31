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

#include "scene_world.h"
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/graphics_funcs.h>
#include <gui/pge_msgbox.h>

WorldScene::WorldScene()
{
    exitWorldCode=EXIT_error;
    exitWorldDelay=2000;
    worldIsContinues=true;
    doExit=false;
    isPauseMenu=false;

    isInit=false;

    i=0;
    delayToEnter = 1000;
    lastTicks=0;
    debug_player_jumping=false;
    debug_player_onground=false;
    debug_player_foots=0;
    debug_render_delay=0;
    debug_phys_delay=0;
    debug_event_delay=0;
    uTick = 1;

    common_setup = ConfigManager::setup_WorldMap;

    if(ConfigManager::setup_WorldMap.backgroundImg.isEmpty())
        backgroundTex.w=0;
    else
        backgroundTex = GraphicsHelps::loadTexture(backgroundTex, ConfigManager::setup_WorldMap.backgroundImg);

    viewportRect.setX(ConfigManager::setup_WorldMap.viewport_x);
    viewportRect.setY(ConfigManager::setup_WorldMap.viewport_y);
    viewportRect.setWidth(ConfigManager::setup_WorldMap.viewport_w);
    viewportRect.setHeight(ConfigManager::setup_WorldMap.viewport_h);

    posX=0;
    posY=0;
    levelTitle = "Hello!";
    health = 3;
    points = 0;
    stars  = 0;

    data = FileFormats::dummyWldDataArray();
}

WorldScene::~WorldScene()
{
    if(backgroundTex.w>0)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(backgroundTex.texture) );
    }
    //destroy textures
    qDebug() << "clear world textures";
    while(!textures_bank.isEmpty())
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(textures_bank[0].texture) );
        textures_bank.pop_front();
    }
}


bool WorldScene::init()
{
    isInit=true;

    //Detect gamestart and set position on them
    for(long i=0; i<data.levels.size(); i++)
    {
        if(data.levels[i].gamestart)
        {
            posX=data.levels[i].x;
            posY=data.levels[i].y;
            break;
        }
    }



    return true;
}

void WorldScene::update()
{
    uTick = (1000.0/(float)PGE_Window::PhysStep);//-lastTicks;
    if(uTick<=0) uTick=1;

    if(doExit)
    {
        if(exitWorldDelay>=0)
            exitWorldDelay -= uTick;
        else
        {
            if(exitWorldCode==EXIT_close)
            {
                fader_opacity=1.0f;
                worldIsContinues=false;
            }
            else
            {
                if(fader_opacity<=0.0f) setFade(25, 1.0f, 0.02f);
                if(fader_opacity>=1.0)
                    worldIsContinues=false;
            }
        }
    }

    if(keyboard1.keys.left)
        posX-=4;
    if(keyboard1.keys.right)
        posX+=4;
    if(keyboard1.keys.up)
        posY-=4;
    if(keyboard1.keys.down)
        posY+=4;

    if(isPauseMenu)
    {
        PGE_MsgBox msgBox(this, "Hi guys!\nThis is a dumym world map. I think, it works fine!",
                          PGE_MsgBox::msg_info);

        if(!ConfigManager::setup_message_box.sprite.isEmpty())
            msgBox.loadTexture(ConfigManager::setup_message_box.sprite);
        msgBox.exec();
        isPauseMenu=false;
    }

    Scene::update();
}

void WorldScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();
    if(!isInit)
        goto renderBlack;

    if(backgroundTex.w>0)
    {
        glEnable(GL_TEXTURE_2D);
        glColor4f( 1.f, 1.f, 1.f, 1.f);
        glBindTexture(GL_TEXTURE_2D, backgroundTex.texture);
        glBegin( GL_QUADS );
            glTexCoord2f( 0, 0 );
            glVertex2f( 0,             0);
            glTexCoord2f( 1, 0 );
            glVertex2f( backgroundTex.w, 0);
            glTexCoord2f( 1, 1 );
            glVertex2f( backgroundTex.w, backgroundTex.h);
            glTexCoord2f( 0, 1 );
            glVertex2f( 0,             backgroundTex.h);
        glEnd();
    }

    //Dummy "Viewport" zone probe
    glDisable(GL_TEXTURE_2D);
    glColor4f( 0.f, 1.f, 0.f, 1.0f);
    glBegin( GL_QUADS );
        glVertex2f( viewportRect.left(), viewportRect.top());
        glVertex2f( viewportRect.right()+1, viewportRect.top());
        glVertex2f( viewportRect.right()+1, viewportRect.bottom()+1);
        glVertex2f( viewportRect.left(), viewportRect.bottom()+1);
    glEnd();

    if(common_setup.points_en)
    {
        FontManager::printText(QString("%1")
                               .arg(points),
                               common_setup.points_x,
                               common_setup.points_y);
    }

    if(common_setup.health_en)
    {
        FontManager::printText(QString("%1")
                               .arg(health),
                               common_setup.health_x,
                               common_setup.health_y);
    }

    FontManager::printText(QString("%1")
                           .arg(levelTitle),
                           common_setup.title_x,
                           common_setup.title_y);

    if(common_setup.star_en)
    {
        FontManager::printText(QString("%1")
                               .arg(stars),
                               common_setup.star_x,
                               common_setup.star_y);
    }


    FontManager::printText(QString("X=%1 Y=%2")
                           .arg(posX)
                           .arg(posY), 300,10);


    if(PGE_Window::showDebugInfo)
    {
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
                                   .arg(exitWorldDelay)
                                   .arg(uTick), 10, 140, 10, qRgb(255,0,0));
    }
    renderBlack:
    Scene::render();
}

int WorldScene::exec()
{
    worldIsContinues=true;
    doExit=false;

    //Greeeeeeeeeeeeeeeeen! :D
    glClearColor(0.0, 1.0, 0.0, 1.0f);

    //World scene's Loop
 Uint32 start_render=0;
 Uint32 stop_render=0;
    int doUpdate_render=0;

 Uint32 start_physics=0;
 Uint32 stop_physics=0;

  Uint32 start_events=0;
  Uint32 stop_events=0;

  Uint32 start_common=0;
     int wait_delay=0;

  float timeStep = 1000.0 / (float)PGE_Window::PhysStep;

    bool running = true;
    while(running)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
        {
            start_events = SDL_GetTicks();
        }

        SDL_Event event; //  Events of SDL
        while ( SDL_PollEvent(&event) )
        {
            keyboard1.update(event);
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        if(doExit) break;
                        setExiting(0, EXIT_close);
                    }   // End work of program
                break;

                case SDL_KEYDOWN: // If pressed key
                  switch(event.key.keysym.sym)
                  { // Check which
                    case SDLK_ESCAPE: // ESC
                            {
                                setExiting(0, EXIT_exitNoSave);
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
            doUpdate_render = stop_render-start_render-timeStep;
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

        stop_render=0;
        start_render=0;

        if(isExit())
            running = false;

    }
    return exitWorldCode;
}

bool WorldScene::isExit()
{
    return !worldIsContinues;
}

void WorldScene::setExiting(int delay, int reason)
{
    exitWorldDelay = delay;
    exitWorldCode = reason;
    doExit = true;
}

bool WorldScene::loadFile(QString filePath)
{
    data.ReadFileValid = false;
    if(!QFileInfo(filePath).exists())
    {
        errorMsg += "File not exist\n\n";
        errorMsg += filePath;
        return false;
    }

    data = FileFormats::OpenWorldFile(filePath);
    if(!data.ReadFileValid)
        errorMsg += "Bad file format\n";
    return data.ReadFileValid;
}

QString WorldScene::getLastError()
{
    return errorMsg;
}
