/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef WLD_TILEBOX_H
#define WLD_TILEBOX_H

#include <vector>
#include <unordered_map>
#include <common_features/point.h>
#include <PGE_File_Formats/wld_filedata.h>
#include <graphics/gl_renderer.h>
#include <data_configs/config_manager.h>
#include <common_features/RTree/RTree.h>

class WorldNode
{
    public:
        enum nodeType
        {
            unknown = 0,
            tile,
            scenery,
            path,
            level,
            musicbox
        };

        WorldNode();
        virtual ~WorldNode();
        WorldNode(const WorldNode &xx);
        virtual void render(double, double) {}
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
        int animatorID;
        bool vizible;
};

class WldTerrainItem: public WorldNode
{
    public:
        WldTerrainItem(const WorldTerrainTile &_data);
        WldTerrainItem(const WldTerrainItem &x);
        ~WldTerrainItem();
        bool init();
        void render(double rx, double ry);
        obj_w_tile setup;
        WorldTerrainTile data;
};

class WldSceneryItem: public WorldNode
{
    public:
        WldSceneryItem(const WorldScenery &_data);
        WldSceneryItem(const WldSceneryItem &x);
        ~WldSceneryItem();
        bool init();
        void render(double rx, double ry);
        obj_w_scenery setup;
        WorldScenery data;
};

class WldPathItem: public WorldNode
{
    public:
        WldPathItem(const WorldPathTile &_data);
        WldPathItem(const WldPathItem &x);
        ~WldPathItem();
        bool init();
        void render(double rx, double ry);
        obj_w_path setup;
        WorldPathTile data;
};

class WldLevelItem: public WorldNode
{
    public:
        WldLevelItem(const WorldLevelTile &_data);
        WldLevelItem(const WldLevelItem &x);
        ~WldLevelItem();
        bool init();
        void render(double rx, double ry);

        obj_w_level setup;
        double      offset_x;
        double      offset_y;
        PGE_Texture _path_tex;
        double      _path_offset_x;
        double      _path_offset_y;
        PGE_Texture _path_big_tex;
        double      _path_big_offset_x;
        double      _path_big_offset_y;
        WorldLevelTile data;
};

class WldMusicBoxItem: public WorldNode
{
    public:
        WldMusicBoxItem(WorldMusicBox _data);
        WldMusicBoxItem(const WldMusicBoxItem &x);
        ~WldMusicBoxItem();
        WorldMusicBox data;
};


class TileBox
{
    public:
        TileBox();
        TileBox(unsigned long size);
        ~TileBox();
        void addNode(long X, long Y, long W, long H, WorldNode *item);
        void query(long X, long Y, std::vector<WorldNode *> &list);
        void query(long Left, long Top, long Right, long Bottom, std::vector<WorldNode *> &list, bool z_sort = false);
        PGE_Point applyGrid(long x, long y);
        void clean();

        const long &grid();
        const long &grid_half();

        void registerElement(WorldNode *item);
        void registerElement(WorldNode *item, long X, long Y, long W, long H);
        void unregisterElement(WorldNode *item);
        typedef long RPoint[2];

    private:
        void sortElements(std::vector<WorldNode *> &list);
        typedef RTree<WorldNode *, long, 2, long > IndexTree;
        IndexTree tree;
        long gridSize;
        long gridSize_h;
};


#endif // WLD_TILEBOX_H
