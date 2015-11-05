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

#ifndef WLD_TILEBOX_H
#define WLD_TILEBOX_H

#include <QVector>
#include <unordered_map>
#include <common_features/point.h>
#include <PGE_File_Formats/wld_filedata.h>
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

    WorldNode();
    virtual ~WorldNode();
    WorldNode(const WorldNode &xx);
    virtual void render(float, float) {}
    virtual bool collidePoint(long rx, long ry);
    virtual bool collideWith(WorldNode *it);
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
    bool vizible;
};

class WldTileItem: public WorldNode
{
public:
    WldTileItem(WorldTiles _data);
    WldTileItem(const WldTileItem &x);
    ~WldTileItem();
    bool init();
    void render(float rx, float ry);
    obj_w_tile setup;
    WorldTiles data;
};

class WldSceneryItem: public WorldNode
{
public:
    WldSceneryItem(WorldScenery _data);
    WldSceneryItem(const WldSceneryItem &x);
    ~WldSceneryItem();
    bool init();
    void render(float rx, float ry);
    obj_w_scenery setup;
    WorldScenery data;
};

class WldPathItem: public WorldNode
{
public:
    WldPathItem(WorldPaths _data);
    WldPathItem(const WldPathItem &x);
    ~WldPathItem();
    bool init();
    void render(float rx, float ry);
    obj_w_path setup;
    WorldPaths data;
};

class WldLevelItem: public WorldNode
{
public:
    WldLevelItem(WorldLevels _data);
    WldLevelItem(const WldLevelItem &x);
    ~WldLevelItem();
    bool init();
    void render(float rx, float ry);

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
};

class WldMusicBoxItem: public WorldNode
{
public:
    WldMusicBoxItem(WorldMusic _data);
    WldMusicBoxItem(const WldMusicBoxItem &x);
    ~WldMusicBoxItem();
    WorldMusic data;
};




class TileBox
{
public:
    TileBox();
    TileBox(unsigned long size);
    ~TileBox();
    void addNode(long X, long Y, long W, long H, WorldNode* item);
    QVector<WorldNode* > query(long X, long Y, QVector<WorldNode * > &list);
    QVector<WorldNode* > query(long Left, long Top, long Right, long Bottom, QVector<WorldNode * > &list, bool z_sort=false);
    PGE_Point applyGrid(long x, long y);
    void clean();

    const long &grid();
    const long &grid_half();
private:
    void sortElements(QVector<WorldNode * > &list);
    std::unordered_map<long, std::unordered_map<long, QVector<WorldNode* > > > map;
    long gridSize;
    long gridSize_h;
};


#endif // WLD_TILEBOX_H

