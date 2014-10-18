#include "scene_level.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "common_features/app_path.h"
#include "common_features/graphics_funcs.h"

#include "../data_configs/config_manager.h"

#include <QtDebug>

LevelScene::LevelScene()
{
    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    isWarpEntrance=false;

    isPauseMenu=false;
    isTimeStopped=false;
    isLevelContinues=false;
    exitLevelDelay=false;
    exitLevelCode=false;

    numberOfPlayers=1;

    world=NULL;


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
    //stop animators


    //desroy animators


    //destroy textures
    qDebug() << "clear textures";
    while(!textures_bank.isEmpty())
    {
        glDeleteTextures( 1, &(textures_bank[0].texture) );
        textures_bank.pop_front();
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

    qDebug() << "Destroy world";
    if(world) delete world; //!< Destroy annoying world, mu-ha-ha-ha >:-D
    world = NULL;

    //destroy players
    //destroy blocks
    //destroy NPC's
    //destroy BGO's

    textures_bank.clear();
}

void LevelScene::init()
{

    //Load File

    //Set Entrance  (int entr=0)

    //Load configs
    ConfigManager::loadLevelBlocks();

    //Prepare to texture bank creation
    ConfigManager::Dir_Blocks.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBlock() );



    //Create Animators
    //look for necessary textures and load them into bank


    //Generate texture bank
//    textures_bank.push_back( GraphicsHelps::loadTexture(ApplicationPath + "/block-223.bmp") );
//    textures_bank.push_back( GraphicsHelps::loadTexture(ApplicationPath + "/background2-14.png") );
//    textures_bank.push_back( GraphicsHelps::loadTexture(ApplicationPath+"/background-103.gif",
//                                                        ApplicationPath+"/background-103m.gif") );


    //Init Physics
    b2Vec2 gravity(0.0f, 150.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);


    int sID = findNearSection(cameraStart.x(), cameraStart.y());


    qDebug()<<"Create cameras";
    //Init Cameras
    PGE_LevelCamera* camera;
    camera = new PGE_LevelCamera();
    camera->setWorld(world);

    camera->changeSectionBorders(
                data.sections[sID].size_left,
                data.sections[sID].size_top,
                data.sections[sID].size_right,
                data.sections[sID].size_bottom
                );

    camera->isWarp = data.sections[sID].IsWarp;
    camera->section = &(data.sections[sID]);

    camera->init(
                    (float)cameraStart.x(),
                    (float)cameraStart.y(),
                    (float)PGE_Window::Width, (float)PGE_Window::Height
                );
    cameras.push_back(camera);

    //Init data


    qDebug()<<"Init blocks";
    //blocks
    for(int i=0; i<data.blocks.size(); i++)
    {
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
        }

        block->worldPtr = world;
        block->data = &(data.blocks[i]);
        block->init();
        long tID = ConfigManager::getBlockTexture(data.blocks[i].id);
        if( tID >= 0 )
        {
            block->texId = ConfigManager::level_textures[tID].texture;
            block->texture = &(ConfigManager::level_textures[tID]);
            block->animated = ConfigManager::lvl_block_indexes[data.blocks[i].id].animated;
            block->animator_ID = ConfigManager::lvl_block_indexes[data.blocks[i].id].animator_ID;
        }
        blocks.push_back(block);
    }

    qDebug() << "textures " << ConfigManager::level_textures.size();


    qDebug()<<"Add players";

    int getPlayers = numberOfPlayers;
    for(int i=0; i<data.players.size() && getPlayers>0 ; i++)
    {
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




    //start animation
    for(int i=0; i<ConfigManager::Animator_Blocks.size(); i++)
        ConfigManager::Animator_Blocks[i]->start();


    qDebug()<<"done!";
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

    //Make world step
    world->Step(1.0f / 100.f, 1, 1);

    //Update controllers
    keyboard1.sendControls();

    //update players

    if(!isPauseMenu) //Update physics is not pause menu
    {
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

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);


    foreach(PGE_LevelCamera* cam, cameras)
    {
        foreach(PGE_Phys_Object * item, cam->renderObjects())
        {
            switch(item->type)
            {
            case PGE_Phys_Object::LVLBlock:
                {
                    LVL_Block * b = dynamic_cast<LVL_Block*>(item);

                    QRectF blockG = QRectF(b->posX()-cam->posX(),
                                           b->posY()-cam->posY(),
                                           b->width,
                                           b->height);


                    AniPos x(0,1);

                    if(b->animated) //Get current animated frame
                        x = ConfigManager::Animator_Blocks[b->animator_ID]->image();

                    glEnable(GL_TEXTURE_2D);
                    glColor4f( 1.f, 1.f, 1.f, 1.f);

                    glBindTexture( GL_TEXTURE_2D, b->texId );

                    glBegin( GL_QUADS );
                        glTexCoord2f( 0, x.first );
                        glVertex2f( blockG.left(), blockG.top());

                        glTexCoord2f( 1, x.first );
                        glVertex2f(  blockG.right(), blockG.top());

                        glTexCoord2f( 1, x.second );
                        glVertex2f(  blockG.right(),  blockG.bottom());

                        glTexCoord2f( 0, x.second );
                        glVertex2f( blockG.left(),  blockG.bottom());
                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                    //glTranslated(0, 0, -b->z_index);
                }
                break;
            case PGE_Phys_Object::LVLPlayer:
                {
                    LVL_Player * p = static_cast<LVL_Player*>(item);

                    QRectF player = QRectF( p->posX()
                                            -cam->posX(),

                                            p->posY()
                                            -cam->posY(),

                                            p->width, p->height
                                         );

            //        qDebug() << "PlPos" << pl.left() << pl.top() << player.right() << player.bottom();


                    glColor4f( 0.f, 0.f, 1.f, 1.f);
                    glBegin( GL_QUADS );
                        glVertex2f( player.left(), player.top());
                        glVertex2f( player.right(), player.top());
                        glVertex2f( player.right(),  player.bottom());
                        glVertex2f( player.left(),  player.bottom());
                    glEnd();

                }
            default:
                break;
            }
        }
    }

}



bool LevelScene::isExit()
{

    return true;
}


int LevelScene::exitType()
{

    return exitLevelCode;
}
