#ifndef WLD_TILEBOX_H
#define WLD_TILEBOX_H

#include <QHash>
#include <QList>
#include <unordered_map>
#include <PGE_File_Formats/wld_filedata.h>
#include <graphics/graphics.h>
#include <graphics/gl_renderer.h>
#include <data_configs/config_manager.h>

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
        texture.w=0;
        texture.h=0;
        texture.texture=0;
        animatorID=0;
        animated=false;
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
        texture=xx.texture;
        animatorID=xx.animatorID;
        animated=xx.animated;
    }
    virtual void render(float, float) {}
    virtual bool collidePoint(long rx, long ry)
    {
        if(rx<x) return false;
        if(ry<y) return false;
        if(rx>x+w) return false;
        if(ry>y+h) return false;
        return true;
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
    PGE_Texture texture;
    bool animated;
    long animatorID;
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
        setup=x.setup;
    }
    ~WldTileItem()
    {}
    bool init()
    {
        r=1.f;
        g=1.f;
        b=0.f;
        if(!ConfigManager::wld_tiles.contains(data.id))
            return false;

        long tID=ConfigManager::getTileTexture(data.id);
        if(tID<0) return false;
        texture = ConfigManager::world_textures[tID];

        setup = ConfigManager::wld_tiles[data.id];
        animated   = setup.animated;
        animatorID = setup.animator_ID;
        w = texture.w;
        h = texture.h;
        return true;
    }

    void render(float rx, float ry)
    {
        AniPos a(0,1);
        if(animated) //Get current animated frame
            a = ConfigManager::Animator_Tiles[animatorID].image();
        GlRenderer::renderTexture(&texture, rx, ry, w, h, a.first, a.second);
    }
    obj_w_tile setup;
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
        setup=x.setup;
    }
    ~WldSceneryItem()
    {}

    bool init()
    {
        r=1.f;
        g=1.f;
        b=0.f;
        if(!ConfigManager::wld_scenery.contains(data.id))
            return false;
        long tID= ConfigManager::getSceneryTexture(data.id);
        if(tID<0) return false;
        texture = ConfigManager::world_textures[tID];
        setup = ConfigManager::wld_scenery[data.id];

        animated   = setup.animated;
        animatorID = setup.animator_ID;
        w = texture.w;
        h = texture.h;
        return true;
    }

    void render(float rx, float ry)
    {
        AniPos a(0,1);
        if(animated) //Get current animated frame
            a = ConfigManager::Animator_Scenery[animatorID].image();
        GlRenderer::renderTexture(&texture, rx, ry, w, h, a.first, a.second);
    }
    obj_w_scenery setup;
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
        setup=x.setup;
    }
    ~WldPathItem()
    {}

    bool init()
    {
        r=1.f;
        g=1.f;
        b=0.f;
        if(!ConfigManager::wld_paths.contains(data.id))
            return false;
        long tID = ConfigManager::getWldPathTexture(data.id);
        if(tID<0) return false;
        texture = ConfigManager::world_textures[tID];
        setup = ConfigManager::wld_paths[data.id];

        animated   = setup.animated;
        animatorID = setup.animator_ID;
        w = texture.w;
        h = texture.h;
        return true;
    }

    void render(float rx, float ry)
    {
        AniPos a(0,1);
        if(animated) //Get current animated frame
            a = ConfigManager::Animator_WldPaths[animatorID].image();
        GlRenderer::renderTexture(&texture, rx, ry, w, h, a.first, a.second);
    }
    obj_w_path setup;
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
        offset_x=0;
        offset_y=0;
        _path_offset_x=0;
        _path_offset_y=0;
        _path_big_offset_x=0;
        _path_big_offset_y=0;
    }
    WldLevelItem(const WldLevelItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=level;
        setup=x.setup;
        offset_x=x.offset_x;
        offset_y=x.offset_y;
        _path_offset_x=x._path_offset_x;
        _path_offset_y=x._path_offset_y;
        _path_big_offset_x=x._path_big_offset_x;
        _path_big_offset_y=x._path_big_offset_y;
        _path_tex=x._path_tex;
        _path_big_tex=x._path_big_tex;
    }
    ~WldLevelItem()
    {}

    bool init()
    {
        r=1.f;
        g=1.f;
        b=0.f;
        if(!ConfigManager::wld_levels.contains(data.id))
            return false;
        long tID = ConfigManager::getWldLevelTexture(data.id);
        if(tID<0) return false; texture = ConfigManager::world_textures[tID];
            tID = ConfigManager::getWldLevelTexture(ConfigManager::marker_wlvl.path);
        if(tID<0) return false; _path_tex = ConfigManager::world_textures[tID];
            tID = ConfigManager::getWldLevelTexture(ConfigManager::marker_wlvl.bigpath);
        if(tID<0) return false; _path_big_tex = ConfigManager::world_textures[tID];
        setup = ConfigManager::wld_levels[data.id];

        animated   = setup.animated;
        animatorID = setup.animator_ID;
        w = ConfigManager::default_grid;
        h = ConfigManager::default_grid;
        offset_x = (ConfigManager::default_grid/2)-(texture.w/2);
        offset_y = ConfigManager::default_grid-texture.h;
        _path_offset_x = (ConfigManager::default_grid/2)-(_path_tex.w/2);
        _path_offset_y = ConfigManager::default_grid-_path_tex.h;
        _path_big_offset_x = (ConfigManager::default_grid/2)-(_path_big_tex.w/2);
        _path_big_offset_y = ConfigManager::default_grid-_path_big_tex.h+(ConfigManager::default_grid/4);
        return true;
    }

    void render(float rx, float ry)
    {
        AniPos a(0,1);
        if(animated) //Get current animated frame
            a = ConfigManager::Animator_WldLevel[animatorID].image();
        if(data.pathbg)
            GlRenderer::renderTexture(&_path_tex, rx+_path_offset_x, ry+_path_offset_y);
        if(data.bigpathbg)
            GlRenderer::renderTexture(&_path_big_tex, rx+_path_big_offset_x, ry+_path_big_offset_y);

        GlRenderer::renderTexture(&texture, rx+offset_x, ry+offset_y, texture.w, texture.h, a.first, a.second);
    }

    obj_w_level setup;
    float offset_x;
    float offset_y;
    PGE_Texture _path_tex;
    float       _path_offset_x;
    float       _path_offset_y;
    PGE_Texture _path_big_tex;
    float       _path_big_offset_x;
    float       _path_big_offset_y;
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


#endif // WLD_TILEBOX_H

