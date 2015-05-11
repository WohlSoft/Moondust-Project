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
#include <audio/SdlMusPlayer.h>
#include <audio/pge_audio.h>
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <common_features/event_queue.h>
#include <common_features/maths.h>
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>

#include <QHash>
#include <unordered_map>

class WorldNode
{
public:
    enum nodeType
    {
        unknown=0,
        tile,
        scenery,
        path,
        level,
        musicbox
    };

    WorldNode()
    {
        x=0;
        y=0;
        w=32;
        h=32;
        r=0.f;
        g=0.f;
        b=1.f;
        Z=0.0;
        type=unknown;
    }

    virtual ~WorldNode() {}

    WorldNode(const WorldNode &xx)
    {
        x=xx.x;
        y=xx.y;
        w=xx.w;
        h=xx.h;
        r=xx.r;
        g=xx.g;
        b=xx.b;
        Z=xx.Z;
        type=xx.type;
    }
    int type;
    long x;
    long y;
    long w;
    long h;
    float r;
    float g;
    float b;
    double Z;
};

class WldTileItem: public WorldNode
{
public:
    WldTileItem(WorldTiles _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=0.0+(double(data.array_id)*0.0000001);
        type=tile;
    }
    WldTileItem(const WldTileItem &x): WorldNode(x)
    {
        data = x.data;
        type=tile;
    }
    ~WldTileItem()
    {}

    WorldTiles data;
};

class WldSceneryItem: public WorldNode
{
public:
    WldSceneryItem(WorldScenery _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        w = 16;
        h = 16;
        Z=10.0+(double(data.array_id)*0.0000001);
        vizible=true;
        type=scenery;
    }
    WldSceneryItem(const WldSceneryItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=scenery;
    }
    ~WldSceneryItem()
    {}
    WorldScenery data;
    bool vizible;
};

class WldPathItem: public WorldNode
{
public:
    WldPathItem(WorldPaths _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=20.0+(double(data.array_id)*0.0000001);
        vizible=true;
        type=path;
    }
    WldPathItem(const WldPathItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=path;
    }
    ~WldPathItem()
    {}
    WorldPaths data;
    bool vizible;
};

class WldLevelItem: public WorldNode
{
public:
    WldLevelItem(WorldLevels _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=30.0+(double(data.array_id)*0.0000001);
        vizible=true;
        type=level;
    }
    WldLevelItem(const WldLevelItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=level;
    }
    ~WldLevelItem()
    {}
    WorldLevels data;
    bool vizible;
};

class WldMusicBoxItem: public WorldNode
{
public:
    WldMusicBoxItem(WorldMusic _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=-10.0;
        type=musicbox;
    }
    WldMusicBoxItem(const WldMusicBoxItem &x): WorldNode(x)
    {
        data = x.data;
        type=musicbox;
    }
    ~WldMusicBoxItem()
    {}
    WorldMusic data;
};

class TileBox
{
public:
    TileBox()
    {
        gridSize=32;
        gridSize_h=16;
    }

    TileBox(unsigned long size)
    {
        gridSize=size;
        gridSize_h=size/2;
    }

    ~TileBox()
    {
        clean();
    }

    void addNode(long X, long Y, long W, long H, WorldNode* item)
    {
        for(long i=X; i<X+W; i+=gridSize)
        {
            for(long j=Y; j<Y+H; j+=gridSize)
            {
                QPoint t = applyGrid(i,j);
                map[t.x()][t.y()].push_back(item);
            }
        }
    }

    QList<WorldNode* > query(long X, long Y)
    {
        QList<WorldNode * > list;

        QPoint t = applyGrid(X,Y);
        long listI = t.x();
        long listJ = t.y();

        const typename std::unordered_map<long, std::unordered_map<long, QList<WorldNode* > > >::const_iterator got = map.find(listI);
        if(got != map.end())
        {
            const typename std::unordered_map<long, QList<WorldNode* > >::const_iterator got2 = map[listI].find(listJ);
            if(got2 != map[listI].end())
                list.append(map[listI][listJ]);
        }
        return list;
    }

    QList<WorldNode* > query(long Left, long Top, long Right, long Bottom, bool z_sort=false)
    {
        QList<WorldNode * > list;
        for(long i=Left-gridSize; i<Right+gridSize*2; i+=gridSize)
        {
            for(long j=Top-gridSize; j<Bottom+gridSize*2; j+=gridSize)
            {
                QPoint t = applyGrid(i,j);
                long listI = t.x();
                long listJ = t.y();

                const typename std::unordered_map<long, std::unordered_map<long, QList<WorldNode* > > >::const_iterator got = map.find(listI);
                if(got != map.end())
                {
                    const typename std::unordered_map<long, QList<WorldNode* > >::const_iterator got2 = map[listI].find(listJ);
                    if(got2 != map[listI].end())
                        list.append(map[listI][listJ]);
                }
            }
        }

        if(z_sort)
        {
            //Sort array
            int total = list.size();
            long i;
            double ymin;
            long ymini;
            long sorted = 0;

            while(sorted < list.size())
            {
                ymin = list[sorted]->Z;
                ymini = sorted;

                for(i = sorted; i < total; i++)
                {
                    if( list[i]->Z < ymin )
                    {
                        ymin = list[i]->Z; ymini = i;
                    }
                }
                list.swap(ymini, sorted);
                sorted++;
            }
        }

        return list;
    }

    void clean()
    {
        map.clear();
    }

    QPoint applyGrid(long x, long y)
    {
        QPoint source;
        source.setX(x);
        source.setY(y);
        int gridX, gridY;
        if(gridSize>0)
        { //ATTACH TO GRID

            if((int)source.x()<0)
            {
                gridX=(int)source.x()+1+(abs((int)source.x()+1)%gridSize)-gridSize;
            }
            else
            {
                gridX=((int)source.x()-(int)source.x() % gridSize);
            }

            if((int)source.y()<0)
            {
                gridY = (int)source.y()+1+(abs((int)source.y()+1) % gridSize)-gridSize;
            }
            else
            {
                gridY = ((int)source.y() - (int)source.y() % gridSize);
            }

            return QPoint(gridX, gridY);
        }
        else
            return source;
    }

    std::unordered_map<long, std::unordered_map<long, QList<WorldNode* > > > map;
    long gridSize;
    long gridSize_h;
};



TileBox worldMap;
QList<WldTileItem >     wld_tiles;
QList<WldSceneryItem >  wld_sceneries;
QList<WldPathItem >     wld_paths;
QList<WldLevelItem >    wld_levels;
QList<WldMusicBoxItem > wld_musicboxes;
EventQueue<WorldScene > wld_events;

QList<WorldNode > wldItems;
QList<WorldNode * > toRender;

WorldScene::WorldScene()
{
    wld_events.abort();

    exitWorldCode=WldExit::EXIT_error;
    exitWorldDelay=2000;
    worldIsContinues=true;
    doExit=false;
    isPauseMenu=false;

    gameState=NULL;

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

    keyboard1.setKeyMap(AppSettings.player1_keyboard);

    uTick = 1;
    move_speed = 115/(float)PGE_Window::PhysStep;
    move_steps_count=0;

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
    coins  = 0;

    jumpTo=false;

    dir=0;
    lock_controls=false;
    ignore_paths=false;
    allow_left=false;
    allow_up=false;
    allow_right=false;
    allow_down=false;
    _playStopSnd=false;

    data = FileFormats::dummyWldDataArray();
}

WorldScene::~WorldScene()
{
    PGE_MusPlayer::MUS_stopMusic();
    wld_events.abort();
    worldMap.clean();
    wldItems.clear();
    toRender.clear();

    wld_tiles.clear();
    wld_sceneries.clear();
    wld_paths.clear();
    wld_levels.clear();
    wld_musicboxes.clear();

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

void WorldScene::setGameState(EpisodeState *_state)
{
    if(!_state) return;
    gameState = _state;

    gameState->replay_on_fail = data.restartlevel;
    if(gameState->episodeIsStarted && !data.HubStyledWorld)
    {
        posX = gameState->game_state.worldPosX;
        posY = gameState->game_state.worldPosY;
        updateAvailablePaths();
        updateCenter();
    }
    else
    {
        gameState->episodeIsStarted=true;
        gameState->WorldPath = data.path;

        //Detect gamestart and set position on them
        for(long i=0; i<data.levels.size(); i++)
        {
            if(data.levels[i].gamestart)
            {
                posX=data.levels[i].x;
                posY=data.levels[i].y;
                gameState->game_state.worldPosX=posX;
                gameState->game_state.worldPosY=posY;
                break;
            }
        }

        //open Intro level
        if(!data.IntroLevel_file.isEmpty())
        {
            //Fix file extension
            if((!data.IntroLevel_file.endsWith(".lvlx", Qt::CaseInsensitive))&&
               (!data.IntroLevel_file.endsWith(".lvl", Qt::CaseInsensitive)))
                    data.IntroLevel_file.append(".lvl");

            QString introLevelFile = gameState->WorldPath+"/"+data.IntroLevel_file;
            WriteToLog(QtDebugMsg, "Opening intro level: "+introLevelFile);

            if(QFileInfo(introLevelFile).exists())
            {
                LevelData checking = FileFormats::OpenLevelFile(introLevelFile);
                if(checking.ReadFileValid)
                {
                    WriteToLog(QtDebugMsg, "File valid, do exit!");
                    gameState->LevelFile = introLevelFile;
                    gameState->LevelPath = checking.path;
                    if(data.HubStyledWorld)
                    {
                        gameState->LevelFile_hub = checking.path;
                        gameState->LevelTargetWarp = gameState->game_state.last_hub_warp ;
                    }
                    else
                    {
                        gameState->LevelTargetWarp = 0;
                    }
                    gameState->isHubLevel = data.HubStyledWorld;

                    //Jump to the intro/hub level
                    doExit=true;
                    exitWorldCode=WldExit::EXIT_beginLevel;
                    return;
                }
                else
                {
                    WriteToLog(QtDebugMsg, "File invalid");
                }
            }
        }
    }
}

bool WorldScene::init()
{
    worldMap.clean();
    wldItems.clear();
    toRender.clear();

    wld_tiles.clear();
    wld_sceneries.clear();
    wld_paths.clear();
    wld_levels.clear();
    wld_musicboxes.clear();

    if(doExit) return true;


    for(int i=0; i<data.tiles.size(); i++)
    {
        WldTileItem path(data.tiles[i]);
        path.r=1.f;
        path.g=1.f;
        path.b=0.f;
        wld_tiles << path;
        worldMap.addNode(path.x, path.y, path.w, path.h, &(wld_tiles.last()));
    }

    for(int i=0; i<data.scenery.size(); i++)
    {
        WldSceneryItem path(data.scenery[i]);
        path.r=1.f;
        path.g=0.f;
        path.b=1.f;
        wld_sceneries << path;
        worldMap.addNode(path.x, path.y, path.w, path.h, &(wld_sceneries.last()));
    }

    for(int i=0; i<data.paths.size(); i++)
    {
        WldPathItem path(data.paths[i]);
        path.r=0.f;
        path.g=0.f;
        path.b=1.f;
        wld_paths << path;
        worldMap.addNode(path.x, path.y, path.w, path.h, &(wld_paths.last()));
    }

    for(int i=0; i<data.levels.size(); i++)
    {
        WldLevelItem path(data.levels[i]);
        path.r=1.f;
        path.g=0.f;
        path.b=0.f;
        wld_levels << path;
        worldMap.addNode(path.x, path.y, path.w, path.h, &(wld_levels.last()));
    }

    for(int i=0; i<data.music.size(); i++)
    {
        WldMusicBoxItem path(data.music[i]);
        path.r=0.5f;
        path.g=0.5f;
        path.b=1.f;
        wld_musicboxes << path;
        worldMap.addNode(path.x, path.y, path.w, path.h, &(wld_musicboxes.last()));
    }

    updateAvailablePaths();
    updateCenter();

    if(gameState)
        playMusic(gameState->game_state.musicID, gameState->game_state.musicFile, true, 200);
    isInit=true;

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
            if(exitWorldCode==WldExit::EXIT_close)
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
    else
    {
        wld_events.processEvents(uTick);

        if(dir==0)
        {
            if(!lock_controls)
            {
                if(keyboard1.keys.left && (allow_left || ignore_paths))
                    dir=1;
                if(keyboard1.keys.right && (allow_right || ignore_paths))
                    dir=2;
                if(keyboard1.keys.up && (allow_up || ignore_paths))
                    dir=3;
                if(keyboard1.keys.down && (allow_down || ignore_paths))
                    dir=4;
            }
            if(dir!=0)
            {
                _playStopSnd=false;
                levelTitle.clear();
                gameState->LevelFile.clear();
                jumpTo=false;
            }

            if(_playStopSnd) { PGE_Audio::playSoundByRole(obj_sound_role::WorldMove); _playStopSnd=false; }
        }
        else
        {
            #define setDir(dr) {dir=dr;\
                    move_steps_count=ConfigManager::default_grid-move_steps_count;}
            switch(dir)
            {
            case 1://left
                if(keyboard1.keys.right)
                setDir(2);
                break;
            case 2://right
                if(keyboard1.keys.left)
                setDir(1);
                break;
            case 3://up
                if(keyboard1.keys.down)
                setDir(4);
                break;
            case 4://down
                if(keyboard1.keys.up)
                setDir(3);
                break;
            }
        }

        #define doMoveStep(posVal)  \
            move_steps_count+=move_speed;\
            if(move_steps_count>=ConfigManager::default_grid)\
            {\
                move_steps_count=0;\
                posVal=Maths::roundTo(posVal, ConfigManager::default_grid);\
            }\
            if((long(posVal)==posVal)&&(long(posVal)%ConfigManager::default_grid==0)) {dir=0; _playStopSnd=true; updateAvailablePaths(); updateCenter();}

        switch(dir)
        {
            case 1://left
                posX-=move_speed;
                doMoveStep(posX);
                break;
            case 2://right
                posX+=move_speed;
                doMoveStep(posX);
                break;
            case 3://up
                posY-=move_speed;
                doMoveStep(posY);
                break;
            case 4://down
                posY+=move_speed;
                doMoveStep(posY);
                break;
        }

        toRender = worldMap.query(posX-(viewportRect.width()/2), posY-(viewportRect.height()/2), posX+(viewportRect.width()/2), posY+(viewportRect.height()/2), true);

        if(isPauseMenu)
        {
            PGE_MsgBox msgBox(this, "Hi guys!\nThis is a dumym world map. I think, it works fine!",
                              PGE_MsgBox::msg_info);

            if(!ConfigManager::setup_message_box.sprite.isEmpty())
                msgBox.loadTexture(ConfigManager::setup_message_box.sprite);
            msgBox.exec();
            isPauseMenu=false;
        }
    }

    Scene::update();
}

void fetchSideNodes(bool &side, QList<WorldNode* > &nodes)
{
    side=false;
    foreach (WorldNode* x, nodes)
    {
        if(x->type==WorldNode::path)
        {
            side=true; break;
        }

        if(x->type==WorldNode::level)
        {
            side=true; break;
        }
    }
}

void WorldScene::updateAvailablePaths()
{
    QList<WorldNode* > nodes;

    //left
    nodes=worldMap.query(posX+worldMap.gridSize_h-worldMap.gridSize,    posY+worldMap.gridSize_h);
    fetchSideNodes(allow_left, nodes);

    //Right
    nodes=worldMap.query(posX+worldMap.gridSize_h+worldMap.gridSize,    posY+worldMap.gridSize_h);
    fetchSideNodes(allow_right, nodes);


    //Top
    nodes=worldMap.query(posX+worldMap.gridSize_h,      posY+worldMap.gridSize_h-worldMap.gridSize);
    fetchSideNodes(allow_up, nodes);

    //Bottom
    nodes=worldMap.query(posX+worldMap.gridSize_h,      posY+worldMap.gridSize_h+worldMap.gridSize);
    fetchSideNodes(allow_down, nodes);
}

void WorldScene::updateCenter()
{
    if(gameState)
    {
        gameState->LevelFile.clear();
        gameState->LevelTargetWarp=0;
    }

    levelTitle.clear();
    jumpTo=false;

    QList<WorldNode* > nodes;
    nodes=worldMap.query(posX+worldMap.gridSize_h, posY+worldMap.gridSize_h);
    foreach (WorldNode* x, nodes)
    {
        if(x->type==WorldNode::musicbox)
        {
            WldMusicBoxItem *y = dynamic_cast<WldMusicBoxItem*>(x);
            if(y)
            {
                if(isInit)
                    playMusic(y->data.id, y->data.music_file);
                else
                if(gameState)
                {
                    gameState->game_state.musicID   = y->data.id;
                    gameState->game_state.musicFile = y->data.music_file;
                }
            }
        }

        if(x->type==WorldNode::level)
        {
            WldLevelItem *y = dynamic_cast<WldLevelItem*>(x);
            if(y)
            {
                levelTitle = y->data.title;
                if(!y->data.lvlfile.isEmpty())
                {
                    QString lvlPath=data.path+"/"+y->data.lvlfile;
                    LevelData head = FileFormats::OpenLevelFileHeader(lvlPath);
                    if(head.ReadFileValid)
                    {
                        if(!y->data.title.isEmpty())
                        {
                            levelTitle = y->data.title;
                        }
                        else if(!head.LevelName.isEmpty())
                        {
                            levelTitle = head.LevelName;
                        }
                        else if(!y->data.lvlfile.isEmpty())
                        {
                            levelTitle = y->data.lvlfile;
                        }

                        if(gameState)
                        {
                            gameState->LevelFile = lvlPath;
                            gameState->LevelTargetWarp = y->data.entertowarp;
                        }
                    }
                }
                else
                {
                    if( (y->data.gotox!=-1)&&(y->data.gotoy!=-1))
                    {
                        jumpToXY.setX(y->data.gotox);
                        jumpToXY.setY(y->data.gotoy);
                        jumpTo=true;
                    }
                }
            }
            else
            {
                levelTitle = "!!!FAIL!!!";
            }
        }
    }
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

    //Viewport zone black background
    glDisable(GL_TEXTURE_2D);
    glColor4f( 0.f, 0.f, 0.f, 1.0f);
    glBegin( GL_QUADS );
        glVertex2f( viewportRect.left(), viewportRect.top());
        glVertex2f( viewportRect.right()+1, viewportRect.top());
        glVertex2f( viewportRect.right()+1, viewportRect.bottom()+1);
        glVertex2f( viewportRect.left(), viewportRect.bottom()+1);
    glEnd();

    {
        //Set small viewport
        glViewport( viewportRect.left(), PGE_Window::Height-(viewportRect.bottom()+1), viewportRect.width(), viewportRect.height());
        double renderX = posX+16-(viewportRect.width()/2);
        double renderY = posY+16-(viewportRect.height()/2);
        double ratioX = double(PGE_Window::Width)/double(viewportRect.width());
        double ratioY = double(PGE_Window::Height)/double(viewportRect.height());

        //Render items
        foreach(WorldNode * it, toRender)
        {
            glDisable(GL_TEXTURE_2D);
            glColor4f( it->r, it->g, it->b, 1.0f);
            glBegin( GL_QUADS );
                glVertex2f( ratioX*((it->x)-renderX),       ratioY*((it->y)-renderY));
                glVertex2f( ratioX*((it->x+it->w)-renderX), ratioY*((it->y)-renderY));
                glVertex2f( ratioX*((it->x+it->w)-renderX), ratioY*((it->y+it->h)-renderY));
                glVertex2f( ratioX*((it->x)-renderX),       ratioY*((it->y+it->h)-renderY));
            glEnd();
        }

        glDisable(GL_TEXTURE_2D);
        glColor4f( 1.f, 1.f, 1.f, 1.0f);
        glBegin( GL_QUADS );
            glVertex2f( ratioX*(posX-renderX),       ratioY*(posY-renderY));
            glVertex2f( ratioX*((posX+32)-renderX), ratioY*(posY-renderY));
            glVertex2f( ratioX*((posX+32)-renderX), ratioY*((posY+32)-renderY));
            glVertex2f( ratioX*(posX-renderX),       ratioY*((posY+32)-renderY));
        glEnd();

        //Restore viewport
        glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height );
    }

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

    if(common_setup.coin_en)
    {
        FontManager::printText(QString("%1")
                               .arg(coins),
                               common_setup.coin_x,
                               common_setup.coin_y);
    }

    if(common_setup.star_en)
    {
        FontManager::printText(QString("%1")
                               .arg(stars),
                               common_setup.star_x,
                               common_setup.star_y);
    }

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(QString("X=%1 Y=%2")
                               .arg(posX)
                               .arg(posY), 300,10);

        {
        QPoint grid = worldMap.applyGrid(posX, posY);
        FontManager::printText(QString("TILE X=%1 Y=%2")
                               .arg(grid.x())
                               .arg(grid.y()), 300,45);
        }

        FontManager::printText(QString("Player J=%1 G=%2,%5 F=%3; TICK-SUB: %4")
                               .arg(debug_player_jumping)
                               .arg(debug_player_onground)
                               .arg(debug_player_foots)
                               .arg(uTick)
                               .arg(toRender.size()), 10,100);

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
    glClearColor(0.0, 0.0, 0.0, 1.0f);

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

    bool running = !doExit;
    while(running)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
        {
            start_events = SDL_GetTicks();
        }

        keyboard1.update();

        SDL_Event event; //  Events of SDL
        while ( SDL_PollEvent(&event) )
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        if(doExit) break;
                        setExiting(0, WldExit::EXIT_close);
                    }   // End work of program
                break;

                case SDL_KEYDOWN: // If pressed key
                  switch(event.key.keysym.sym)
                  { // Check which
                    case SDLK_ESCAPE: // ESC
                            {
                                setExiting(0, WldExit::EXIT_exitNoSave);
                            }   // End work of program
                        break;
                    case SDLK_RETURN:// Enter
                        {
                            if( doExit || lock_controls) break;
                            isPauseMenu = true;
                        }
                    break;
                    case SDLK_t:
                        PGE_Window::SDL_ToggleFS(PGE_Window::window);
                    break;
                    case SDLK_i:
                        ignore_paths= !ignore_paths;
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

        if(keyboard1.keys.jump)
        {
            if((!lock_controls) && (gameState))
            {
                if(!gameState->LevelFile.isEmpty())
                {
                    gameState->game_state.worldPosX=posX;
                    gameState->game_state.worldPosY=posY;
                    PGE_Audio::playSoundByRole(obj_sound_role::WorldEnterLevel);
                    stopMusic(true, 300);
                    lock_controls=true;
                    setExiting(0, WldExit::EXIT_beginLevel);
                }
                else if(jumpTo)
                {
                    //Create events
                    EventQueueEntry<WorldScene >event1;
                    event1.makeWaiterFlagT(this, &WorldScene::isOpacityFadding, true, 100);
                    wld_events.events.push_back(event1);

                    EventQueueEntry<WorldScene >event2;
                    event2.makeCallerT(this, &WorldScene::jump, 100);
                    wld_events.events.push_back(event2);

                    EventQueueEntry<WorldScene >event3;
                    event3.makeCaller([this]()->void{
                                          this->setFade(25, 0.0, 0.08);
                                          this->lock_controls=false;
                                      }, 0);
                    wld_events.events.push_back(event3);

                    this->lock_controls=true;
                    PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                    this->setFade(25, 1.0f, 0.08);
                }
            }
        }

        /**********************Update physics and game progess***********************/

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
            doUpdate_render = stop_render-start_render;//-timeStep;
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

void WorldScene::jump()
{
    posX = jumpToXY.x();
    posY = jumpToXY.y();
    if(gameState)
    {
        gameState->game_state.worldPosX = posX;
        gameState->game_state.worldPosY = posY;
    }
    updateAvailablePaths();
    updateCenter();
}



void WorldScene::stopMusic(bool fade, int fadeLen)
{
    if(fade)
        PGE_MusPlayer::MUS_stopMusicFadeOut(fadeLen);
    else
        PGE_MusPlayer::MUS_stopMusic();
}

void WorldScene::playMusic(long musicID, QString customMusicFile, bool fade, int fadeLen)
{
    QString musPath = ConfigManager::getWldMusic(musicID, data.path+"/"+customMusicFile);
    if(musPath.isEmpty()) return;

    PGE_MusPlayer::MUS_openFile(musPath);
    if(fade)
        PGE_MusPlayer::MUS_playMusicFadeIn(fadeLen);
    else
        PGE_MusPlayer::MUS_playMusic();
    if(gameState)
    {
        gameState->game_state.musicID = musicID;
        gameState->game_state.musicFile = customMusicFile;
    }
}

