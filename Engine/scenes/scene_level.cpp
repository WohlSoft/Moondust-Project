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

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "common_features/app_path.h"
#include "common_features/graphics_funcs.h"

#include "level/lvl_scene_ptr.h"

#include "../data_configs/config_manager.h"

#include <QtDebug>

#include "../physics/contact_listener.h"

LevelScene::LevelScene()
{
    LvlSceneP::s = this;

    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    isInit=false;

    IsLoaderWorks=false;

    isWarpEntrance=false;

    isPauseMenu=false;
    isTimeStopped=false;

    /**********************/
    isLevelContinues=true;

    doExit = false;
    exitLevelDelay=3000;
    exitLevelCode = EXIT_Closed;
    warpToLevelFile = "";
    /**********************/

    numberOfPlayers=1;

    world=NULL;

    fader_opacity=1.0f;
    target_opacity=1.0f;
    fade_step=0.0f;


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
}

LevelScene::~LevelScene()
{
    LvlSceneP::s = NULL;
    //stop animators

    //desroy animators


    //destroy textures
    qDebug() << "clear textures";
    while(!textures_bank.isEmpty())
    {
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

bool LevelScene::init()
{
    //Load File

    //Set Entrance  (int entr=0)

    //Init Physics
    b2Vec2 gravity(0.0f, 150.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);

    PGEContactListener *contactListener;

    contactListener = new PGEContactListener();
    world->SetContactListener(contactListener);

    int sID = findNearSection(cameraStart.x(), cameraStart.y());

    qDebug()<<"Create cameras";

    loaderStep();

    //Init Cameras
    PGE_LevelCamera* camera;
    camera = new PGE_LevelCamera();
    camera->setWorld(world);


    camera->changeSection(data.sections[sID]);

    camera->isWarp = data.sections[sID].IsWarp;
    camera->section = &(data.sections[sID]);

    camera->init(
                    (float)cameraStart.x(),
                    (float)cameraStart.y(),
                    (float)PGE_Window::Width, (float)PGE_Window::Height
                );
    cameras.push_back(camera);


    LVL_Background * CurBack = new LVL_Background(cameras.last());

    if(ConfigManager::lvl_bg_indexes.contains(camera->BackgroundID))
    {
        CurBack->setBg(ConfigManager::lvl_bg_indexes[camera->BackgroundID]);
        qDebug() << "Backgroubnd ID:" << camera->BackgroundID;
    }
    else
        CurBack->setNone();

    backgrounds.push_back(CurBack);


    //Init data


    qDebug()<<"Init blocks";

    double zCounter = 0;
    //blocks
    for(int i=0; i<data.blocks.size(); i++)
    {
        loaderStep();

        LVL_Block * block;
        block = new LVL_Block();
        if(ConfigManager::lvl_block_indexes.contains(data.blocks[i].id))
            block->setup = &(ConfigManager::lvl_block_indexes[data.blocks[i].id]);
        else
        {
            //Wrong block!
            delete block;
            continue;
        }

        if(block->setup->sizable)
        {
            block->z_index = Z_blockSizable +
                    ((double)data.blocks[i].y/(double)100000000000) + 1 -
                    ((double)data.blocks[i].w * (double)0.0000000000000001);
        }
        else
        {

            if(block->setup->view==1)
                block->z_index = Z_BlockFore;
            else
                block->z_index = Z_Block;
            zCounter += 0.0000000000001;
            block->z_index += zCounter;
        }

        block->worldPtr = world;
        block->data = &(data.blocks[i]);
        long tID = ConfigManager::getBlockTexture(data.blocks[i].id);
        if( tID >= 0 )
        {
            block->texId = ConfigManager::level_textures[tID].texture;
            block->texture = ConfigManager::level_textures[tID];
            block->animated = ConfigManager::lvl_block_indexes[data.blocks[i].id].animated;
            block->animator_ID = ConfigManager::lvl_block_indexes[data.blocks[i].id].animator_ID;
        }

        block->init();

        blocks.push_back(block);
    }

    qDebug()<<"Init BGOs";
    //BGO
    for(int i=0; i<data.bgo.size(); i++)
    {
        loaderStep();

        LVL_Bgo * bgo;
        bgo = new LVL_Bgo();
        if(ConfigManager::lvl_bgo_indexes.contains(data.bgo[i].id))
            bgo->setup = &(ConfigManager::lvl_bgo_indexes[data.bgo[i].id]);
        else
        {
            //Wrong BGO!
            delete bgo;
            continue;
        }

        bgo->worldPtr = world;
        bgo->data = &(data.bgo[i]);

        double targetZ = 0;
        double zOffset = bgo->setup->zOffset;
        int zMode = bgo->data->z_mode;

        if(zMode==LevelBGO::ZDefault)
            zMode = bgo->setup->view;
        switch(zMode)
        {
            case LevelBGO::Background2:
                targetZ = Z_BGOBack2 + zOffset + bgo->data->z_offset; break;
            case LevelBGO::Background1:
                targetZ = Z_BGOBack1 + zOffset + bgo->data->z_offset; break;
            case LevelBGO::Foreground1:
                targetZ = Z_BGOFore1 + zOffset + bgo->data->z_offset; break;
            case LevelBGO::Foreground2:
                targetZ = Z_BGOFore2 + zOffset + bgo->data->z_offset; break;
            default:
                targetZ = Z_BGOBack1 + zOffset + bgo->data->z_offset; break;
        }

        bgo->z_index += targetZ;

        zCounter += 0.0000000000001;
        bgo->z_index += zCounter;

        long tID = ConfigManager::getBgoTexture(data.bgo[i].id);
        if( tID >= 0 )
        {
            bgo->texId = ConfigManager::level_textures[tID].texture;
            bgo->texture = ConfigManager::level_textures[tID];
            bgo->animated = ConfigManager::lvl_bgo_indexes[data.bgo[i].id].animated;
            bgo->animator_ID = ConfigManager::lvl_bgo_indexes[data.bgo[i].id].animator_ID;
        }
        bgo->init();

        bgos.push_back(bgo);
    }

    qDebug()<<"Init Warps";
    //BGO
    for(int i=0; i<data.doors.size(); i++)
    {
        loaderStep();

        LVL_Warp * warpP;
        warpP = new LVL_Warp();
        warpP->worldPtr = world;
        warpP->data = data.doors[i];
        warpP->init();
        warps.push_back(warpP);
    }


    qDebug() << "textures " << ConfigManager::level_textures.size();


    qDebug()<<"Add players";

    int getPlayers = numberOfPlayers;
    int players_count=0;

    if(!isWarpEntrance) //Dont place players if entered through warp
        for(players_count=0; players_count<data.players.size() && getPlayers>0 ; players_count++)
        {
            loaderStep();

            int i = players_count;
            if(data.players[i].w==0 && data.players[i].h==0) continue;

            LVL_Player * player;
            player = new LVL_Player();
            player->camera = cameras[0];
            player->worldPtr = world;
            player->setSize(data.players[i].w, data.players[i].h);
            player->data = &(data.players[i]);
            player->z_index = Z_Player;
            player->init();
            players.push_back(player);
            if(player->playerID==1)
                keyboard1.registerInControl(player);
            getPlayers--;
        }

    if(players_count<0 && !isWarpEntrance)
    {
        qDebug()<<"No defined players!";
        return false;
    }

    //start animation
    for(int i=0; i<ConfigManager::Animator_Blocks.size(); i++)
        ConfigManager::Animator_Blocks[i]->start();

    for(int i=0; i<ConfigManager::Animator_BGO.size(); i++)
        ConfigManager::Animator_BGO[i]->start();

    for(int i=0; i<ConfigManager::Animator_BG.size(); i++)
        ConfigManager::Animator_BG[i]->start();

    stopLoaderAnimation();
    isInit = true;

    return true;
}



bool LevelScene::loadFile(QString filePath)
{
    QFile file(filePath );
    QFileInfo in_1(filePath);

    if (file.open(QIODevice::ReadOnly))
    {
        if(filePath.endsWith(".lvl", Qt::CaseInsensitive))
            data = FileFormats::ReadLevelFile(file);
        else
            data = FileFormats::ReadExtendedLevelFile(file);
        data.filename = in_1.baseName();
        data.path = in_1.absoluteDir().absolutePath();
    }
    return data.ReadFileValid;
}




bool LevelScene::prepareLevel()
{
    return true;
}

int i;

void LevelScene::update(float step)
{
    if(step<=0) step=10.0f;

    if(doExit)
    {
        if(exitLevelDelay>=0)
            exitLevelDelay -= 10;
        else
        {
            if(fader_opacity<=0.0f) setFade(25, 1.0f, 0.02f);
            if(fader_opacity>=1.0)
                isLevelContinues=false;
        }
    }
    else
    if(!isPauseMenu) //Update physics is not pause menu
    {
        //Make world step
        world->Step(1.0f / 100.f, 5, 5);

        //Update controllers
        keyboard1.sendControls();

        //update players
        for(i=0; i<players.size(); i++)
            players[i]->update();


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

}



void LevelScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    //Move to center of the screen
    //glTranslatef( PGE_Window::Width / 2.f, PGE_Window::Height / 2.f, 0.f );

    if(!isInit) goto renderBlack;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);

    foreach(PGE_LevelCamera* cam, cameras)
    {
        backgrounds.last()->draw(cam->posX(), cam->posY());

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

    renderBlack:

    if(fader_opacity>0.0f)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4f( 0.f, 0.f, 0.f, fader_opacity);
        glBegin( GL_QUADS );
            glVertex2f( 0, 0);
            glVertex2f( PGE_Window::Width, 0);
            glVertex2f( PGE_Window::Width, PGE_Window::Height);
            glVertex2f( 0, PGE_Window::Height);
        glEnd();
    }

    if(IsLoaderWorks) drawLoader();
}



int LevelScene::exec()
{
    //Level scene's Loop
    Uint32 start;
    bool running = true;
    int doUpdate=0;
    float doUpdateP=0;
    while(running)
    {

        //UPDATE Events
        if(doUpdate<=0)
        {

            start=SDL_GetTicks();

            render();

            glFlush();
            SDL_GL_SwapWindow(PGE_Window::window);

            if(1000.0/1000>SDL_GetTicks()-start)
                    //SDL_Delay(1000.0/1000-(SDL_GetTicks()-start));
                    doUpdate = 1000.0/1000-(SDL_GetTicks()-start);
        }
        doUpdate-=10;


        start=SDL_GetTicks();

        SDL_Event event; //  Events of SDL
        while ( SDL_PollEvent(&event) )
        {
            keyboard1.update(event);
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        setExiting(0, EXIT_Closed);
                    }   // End work of program
                break;

                case SDL_KEYDOWN: // If pressed key
                  switch(event.key.keysym.sym)
                  { // Check which
                    case SDLK_ESCAPE: // ESC
                            {
                                setExiting(0, EXIT_Closed);
                            }   // End work of program
                        break;
                    case SDLK_RETURN:// Enter
                          isPauseMenu = !isPauseMenu;
                    break;
                    case SDLK_t:
                        PGE_Window::SDL_ToggleFS(PGE_Window::window);
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


        //Update physics
        update();

        if(1000.0/100>SDL_GetTicks()-start)
        {
            doUpdateP = 1000.0/100-(SDL_GetTicks()-start);
            SDL_Delay( doUpdateP );
        }

        if(isExit())
            running = false;
    }

    return exitLevelCode;
}



bool LevelScene::isExit()
{
    return !isLevelContinues;
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
        setExiting(3000, EXIT_PlayerDeath);
    }
}

void LevelScene::setExiting(int delay, int reason)
{
    exitLevelDelay = delay;
    exitLevelCode = reason;
    doExit = true;
}

QString LevelScene::toAnotherLevel()
{
    if(!warpToLevelFile.endsWith(".lvl", Qt::CaseInsensitive) &&
       !warpToLevelFile.endsWith(".lvlx", Qt::CaseInsensitive))
        warpToLevelFile.append(".lvl");

    return warpToLevelFile;
}



/**************************Fader*******************************/

void LevelScene::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;
    fader_timer_id = SDL_AddTimer(speed, &LevelScene::nextOpacity, this);
}

unsigned int LevelScene::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LevelScene *self = reinterpret_cast<LevelScene *>(p);
    self->fadeStep();
    return 0;
}

void LevelScene::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0 || fader_opacity<=0.0)
        SDL_RemoveTimer(fader_timer_id);
    else
        fader_timer_id = SDL_AddTimer(fadeSpeed, &LevelScene::nextOpacity, this);
}
/**************************Fader**end**************************/


/**************************LoadAnimation*******************************/
namespace lvl_scene_loader
{
    SimpleAnimator * loading_Ani = NULL;
    PGE_Texture loading_texture;
}

void LevelScene::drawLoader()
{
    using namespace lvl_scene_loader;

    if(!loading_Ani) return;

    QRectF loadAniG = QRectF(PGE_Window::Width/2 - loading_texture.w/2,
                           PGE_Window::Height/2 - (loading_texture.h/4)/2,
                           loading_texture.w,
                           loading_texture.h/4);

    glEnable(GL_TEXTURE_2D);
    glColor4f( 1.f, 1.f, 1.f, 1.f);

    AniPos x(0,1);
            x = loading_Ani->image();

    glBindTexture( GL_TEXTURE_2D, loading_texture.texture );

    glBegin( GL_QUADS );
        glTexCoord2f( 0, x.first );
        glVertex2f( loadAniG.left(), loadAniG.top());

        glTexCoord2f( 1, x.first );
        glVertex2f(  loadAniG.right(), loadAniG.top());

        glTexCoord2f( 1, x.second );
        glVertex2f(  loadAniG.right(),  loadAniG.bottom());

        glTexCoord2f( 0, x.second );
        glVertex2f( loadAniG.left(),  loadAniG.bottom());
        glEnd();
    glDisable(GL_TEXTURE_2D);
}


void LevelScene::setLoaderAnimation(int speed)
{
    using namespace lvl_scene_loader;
    loaderSpeed = speed;
    loading_texture = GraphicsHelps::loadTexture(loading_texture, ":/images/shell.png");

    loading_Ani = new SimpleAnimator(true,
                                     4,
                                     70,
                                     0, -1, false, false);
    loading_Ani->start();

    loader_timer_id = SDL_AddTimer(speed, &LevelScene::nextLoadAniFrame, this);
    IsLoaderWorks = true;
}

void LevelScene::stopLoaderAnimation()
{
    using namespace lvl_scene_loader;

    doLoaderStep = false;
    IsLoaderWorks = false;
    SDL_RemoveTimer(loader_timer_id);

    if(loading_Ani)
    {
        loading_Ani->stop();
        delete loading_Ani;
        loading_Ani = NULL;
        glDeleteTextures( 1, &(loading_texture.texture) );
    }

}

unsigned int LevelScene::nextLoadAniFrame(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LevelScene *self = reinterpret_cast<LevelScene *>(p);
    self->loaderTick();
    return 0;
}

void LevelScene::loaderTick()
{
    doLoaderStep = true;
}

void LevelScene::loaderStep()
{
    if(!IsLoaderWorks) return;
    if(!doLoaderStep) return;

    SDL_Event event; //  Events of SDL
    while ( SDL_PollEvent(&event) )
    {
        switch(event.type)
        {
            case SDL_QUIT:
            break;
        }
    }

    render();
    glFlush();
    SDL_GL_SwapWindow(PGE_Window::window);

    loader_timer_id = SDL_AddTimer(loaderSpeed, &LevelScene::nextLoadAniFrame, this);
    doLoaderStep = false;
}

QVector<LVL_Background *> *LevelScene::bgList()
{
    return &backgrounds;
}

LevelData *LevelScene::levelData()
{
    return &data;
}
/**************************LoadAnimation**end**************************/

