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






#ifndef RENDER_H
#define RENDER_H
#include <Box2D/Box2D.h>
struct b2AABB;
// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class DebugDraw : public b2Draw
{
public:
DebugDraw() { c=NULL; }
void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
void DrawTransform(const b2Transform& xf);
void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
void DrawString(int x, int y, const char* string, ...);
void DrawString(const b2Vec2& p, const char* string, ...);
void DrawAABB(b2AABB* aabb, const b2Color& cl);
PGE_LevelCamera *c;
};
#endif

#include <stdio.h>
#include <stdarg.h>

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < vertexCount; ++i)
    {
        glVertex2f(vertices[i].x*10 -c->posX(), vertices[i].y*10-c->posY());
    }
    glEnd();
}
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    for (int32 i = 0; i < vertexCount; ++i)
    {
        glVertex2f(vertices[i].x*10-c->posX(), vertices[i].y*10 - c->posY());
    }
    glEnd();
    glDisable(GL_BLEND);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < vertexCount; ++i)
    {
        glVertex2f(vertices[i].x*10-c->posX(), vertices[i].y*10-c->posY());
    }
    glEnd();
    glEnable(GL_BLEND);
}
void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
}
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
    glDisable(GL_BLEND);
    theta = 0.0f;
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
    b2Vec2 p = center + radius * axis;
    glBegin(GL_LINES);
    glVertex2f(center.x, center.y);
    glVertex2f(p.x, p.y);
    glEnd();
}
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(p1.x*10-c->posX(), p1.y*10-c->posY());
    glVertex2f(p2.x*10-c->posX(), p2.y*10-c->posY());
    glEnd();
}
void DebugDraw::DrawTransform(const b2Transform& xf)
{
    b2Vec2 p1 = xf.p, p2;
    const float32 k_axisScale = 0.4f;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glVertex2f(p1.x*10-c->posX(), p1.y*10-c->posY());
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    glVertex2f(p2.x*10-c->posX(), p2.y*10-c->posY());
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertex2f(p1.x*10-c->posX(), p1.y*10-c->posY());
    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    glVertex2f(p2.x*10-c->posX(), p2.y*10-c->posY());
    glEnd();
}
void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glPointSize(size);
    glBegin(GL_POINTS);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glVertex2f(p.x*10-c->posX(), p.y*10-c->posY());
    glEnd();
    glPointSize(1.0f);
}
void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
    char buffer[128];
    va_list arg;
    va_start(arg, string);
    vsprintf(buffer, string, arg);
    va_end(arg);
    FontManager::printText(buffer, x*10-c->posX(), y*10-c->posY());
    //imguiDrawText(x, h - y, IMGUI_ALIGN_LEFT, buffer, imguiRGBA(230, 153, 153, 255));
}

void DebugDraw::DrawString(const b2Vec2& p, const char *string, ...)
{
    char buffer[128];
    va_list arg;
    va_start(arg, string);
    vsprintf(buffer, string, arg);
    va_end(arg);
    FontManager::printText(buffer, (int)p.x*10-c->posX(), (int)p.y*10-c->posY());
    //imguiDrawText((int)p.x, h - (int)p.y, IMGUI_ALIGN_LEFT, buffer, imguiRGBA(230, 153, 153, 255));
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& cl)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(cl.r, cl.g, cl.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(aabb->lowerBound.x*10-c->posX(), aabb->lowerBound.y*10-c->posY());
    glVertex2f(aabb->upperBound.x*10-c->posX(), aabb->lowerBound.y*10-c->posY());
    glVertex2f(aabb->upperBound.x*10-c->posX(), aabb->upperBound.y*10-c->posY());
    glVertex2f(aabb->lowerBound.x*10-c->posX(), aabb->upperBound.y*10-c->posY());
    glEnd();
}



DebugDraw dbgDraw;


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
                debug_player_foots=players[i]->foot_contacts_map.size();
            }
            players[i]->update(uTick);
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

        world->DrawDebugData();
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


    dbgDraw.c = cameras.first();

    world->SetDebugDraw(&dbgDraw);

    dbgDraw.SetFlags( dbgDraw.e_shapeBit | dbgDraw.e_jointBit );

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



