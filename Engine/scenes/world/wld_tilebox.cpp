/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wld_tilebox.h"
#include <common_features/QuadTree/LooseQuadtree.h>
#include <stack>


WorldNode::WorldNode()
{
    x = 0;
    y = 0;
    w = ConfigManager::default_grid;
    h = ConfigManager::default_grid;
    r = 0.f;
    g = 0.f;
    b = 1.f;
    Z = 0.0;
    type = unknown;
    texture.w = 0;
    texture.h = 0;
    texture.frame_w = 0;
    texture.frame_h = 0;
    texture.texture = 0;
    animatorID = 0;
    animated = false;
    vizible = true;
}

WorldNode::~WorldNode() {}

WorldNode::WorldNode(const WorldNode &xx)
{
    x = xx.x;
    y = xx.y;
    w = xx.w;
    h = xx.h;
    r = xx.r;
    g = xx.g;
    b = xx.b;
    Z = xx.Z;
    type = xx.type;
    texture = xx.texture;
    animatorID = xx.animatorID;
    animated = xx.animated;
    vizible = xx.vizible;
}

bool WorldNode::collidePoint(long rx, long ry)
{
    if(rx < x)
        return false;

    if(ry < y)
        return false;

    if(rx > x + w)
        return false;

    if(ry > y + h)
        return false;

    return true;
}

bool WorldNode::collideWith(WorldNode *it)
{
    if((it->x + it->w) <= x) return false;

    if((it->y + it->h) <= y) return false;

    if((x + w) <= it->x) return false;

    if((y + h) <= it->y) return false;

    return true;
}

long WorldNode::finalX() const
{
    return x;
}

long WorldNode::finalY() const
{
    return y;
}

long WorldNode::finalW() const
{
    return w;
}

long WorldNode::finalH() const
{
    return h;
}




WldTerrainItem::WldTerrainItem(const WorldTerrainTile &_data): WorldNode()
{
    data = _data;
    x = data.x;
    y = data.y;
    Z = 0.0 + (double(data.meta.array_id) * 0.0000001);
    type = tile;
}

WldTerrainItem::WldTerrainItem(const WldTerrainItem &x): WorldNode(x)
{
    data = x.data;
    type = tile;
    setup = x.setup;
}

WldTerrainItem::~WldTerrainItem()
{}

bool WldTerrainItem::init()
{
    r = 1.f;
    g = 1.f;
    b = 0.f;

    if(!ConfigManager::wld_tiles.contains(data.id))
        return false;

    int tID = ConfigManager::getTileTexture(data.id);

    if(tID < 0) return false;

    texture = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_tiles[data.id];
    animated   = setup.setup.animated;
    animatorID = setup.animator_ID;
    w = texture.frame_w;
    h = texture.frame_h;
    return true;
}

void WldTerrainItem::render(double rx, double ry)
{
    AniPos a(0, 1);

    if(animated) //Get current animated frame
        a = ConfigManager::Animator_Tiles[animatorID].image();

    GlRenderer::renderTexture(&texture,
                              static_cast<float>(rx),
                              static_cast<float>(ry),
                              static_cast<float>(w),
                              static_cast<float>(h),
                              static_cast<float>(a.first),
                              static_cast<float>(a.second));
}








WldSceneryItem::WldSceneryItem(const WorldScenery &_data): WorldNode()
{
    data = _data;
    x = data.x;
    y = data.y;
    w = 16;
    h = 16;
    Z = 10.0 + (double(data.meta.array_id) * 0.0000001);
    vizible = true;
    type = scenery;
}

WldSceneryItem::WldSceneryItem(const WldSceneryItem &x): WorldNode(x)
{
    data = x.data;
    vizible = x.vizible;
    type = scenery;
    setup = x.setup;
}

WldSceneryItem::~WldSceneryItem()
{}

bool WldSceneryItem::init()
{
    r = 1.f;
    g = 1.f;
    b = 0.f;

    if(!ConfigManager::wld_scenery.contains(data.id))
        return false;

    int tID = ConfigManager::getSceneryTexture(data.id);

    if(tID < 0) return false;

    texture = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_scenery[data.id];
    animated   = setup.setup.animated;
    animatorID = setup.animator_ID;
    w = texture.frame_w;
    h = texture.frame_h;
    return true;
}

void WldSceneryItem::render(double rx, double ry)
{
    AniPos a(0, 1);

    if(animated) //Get current animated frame
        a = ConfigManager::Animator_Scenery[animatorID].image();

    GlRenderer::renderTexture(&texture,
                              static_cast<float>(rx),
                              static_cast<float>(ry),
                              static_cast<float>(w),
                              static_cast<float>(h),
                              static_cast<float>(a.first),
                              static_cast<float>(a.second));
}







WldPathItem::WldPathItem(const WorldPathTile &_data): WorldNode()
{
    data = _data;
    x = data.x;
    y = data.y;
    Z = 20.0 + (double(data.meta.array_id) * 0.0000001);
    vizible = true;
    type = path;
}

WldPathItem::WldPathItem(const WldPathItem &x): WorldNode(x)
{
    data = x.data;
    vizible = x.vizible;
    type = path;
    setup = x.setup;
}

WldPathItem::~WldPathItem()
{}

bool WldPathItem::init()
{
    r = 1.f;
    g = 1.f;
    b = 0.f;

    if(!ConfigManager::wld_paths.contains(data.id))
        return false;

    int tID = ConfigManager::getWldPathTexture(data.id);

    if(tID < 0) return false;

    texture = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_paths[data.id];
    animated   = setup.setup.animated;
    animatorID = setup.animator_ID;
    w = texture.frame_w;
    h = texture.frame_h;
    return true;
}

void WldPathItem::render(double rx, double ry)
{
    AniPos a(0, 1);

    if(animated) //Get current animated frame
        a = ConfigManager::Animator_WldPaths[static_cast<int>(animatorID)].image();

    GlRenderer::renderTexture(&texture,
                              static_cast<float>(rx),
                              static_cast<float>(ry),
                              static_cast<float>(w),
                              static_cast<float>(h),
                              static_cast<float>(a.first),
                              static_cast<float>(a.second));
}










WldLevelItem::WldLevelItem(const WorldLevelTile &_data): WorldNode()
{
    data = _data;
    x = data.x;
    y = data.y;
    Z = 30.0 + (static_cast<double>(data.meta.array_id) * 0.0000001);
    vizible = true;
    type = level;
    offset_x = 0.0;
    offset_y = 0.0;
    _path_offset_x = 0.0;
    _path_offset_y = 0.0;
    _path_big_offset_x = 0.0;
    _path_big_offset_y = 0.0;
}

WldLevelItem::WldLevelItem(const WldLevelItem &x): WorldNode(x)
{
    data = x.data;
    vizible = x.vizible;
    type = level;
    setup = x.setup;
    offset_x = x.offset_x;
    offset_y = x.offset_y;
    _path_offset_x = x._path_offset_x;
    _path_offset_y = x._path_offset_y;
    _path_big_offset_x = x._path_big_offset_x;
    _path_big_offset_y = x._path_big_offset_y;
    _path_tex = x._path_tex;
    _path_big_tex = x._path_big_tex;
}

WldLevelItem::~WldLevelItem()
{}

bool WldLevelItem::init()
{
    r = 1.f;
    g = 1.f;
    b = 0.f;

    if(!ConfigManager::wld_levels.contains(data.id))
        return false;

    int tID = ConfigManager::getWldLevelTexture(data.id);

    if(tID < 0) return false;

    texture = ConfigManager::world_textures[tID];
    tID = ConfigManager::getWldLevelTexture(ConfigManager::marker_wlvl.path);

    if(tID < 0) return false;

    _path_tex = ConfigManager::world_textures[tID];
    tID = ConfigManager::getWldLevelTexture(ConfigManager::marker_wlvl.bigpath);

    if(tID < 0) return false;

    _path_big_tex = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_levels[data.id];
    animated   = setup.setup.animated;
    animatorID = setup.animator_ID;
    w = ConfigManager::default_grid;
    h = ConfigManager::default_grid;
    int defGrid = static_cast<int>(ConfigManager::default_grid);
    offset_x = (defGrid / 2) - (texture.frame_w / 2);
    offset_y = defGrid - texture.frame_h;
    _path_offset_x = (defGrid / 2) - (_path_tex.frame_w / 2);
    _path_offset_y = defGrid - _path_tex.frame_h;
    _path_big_offset_x = (defGrid / 2) - (_path_big_tex.frame_w / 2);
    _path_big_offset_y = defGrid - _path_big_tex.frame_h + (defGrid / 4);
    return true;
}

void WldLevelItem::render(double rx, double ry)
{
    AniPos a(0, 1);

    if(animated) //Get current animated frame
        a = ConfigManager::Animator_WldLevel[animatorID].image();

    if(data.pathbg)
        GlRenderer::renderTexture(&_path_tex,
                                  static_cast<float>(rx + _path_offset_x),
                                  static_cast<float>(ry + _path_offset_y));

    if(data.bigpathbg)
        GlRenderer::renderTexture(&_path_big_tex,
                                  static_cast<float>(rx + _path_big_offset_x),
                                  static_cast<float>(ry + _path_big_offset_y));

    GlRenderer::renderTexture(&texture,
                              static_cast<float>(rx + offset_x),
                              static_cast<float>(ry + offset_y),
                              static_cast<float>(texture.frame_w),
                              static_cast<float>(texture.frame_h),
                              static_cast<float>(a.first),
                              static_cast<float>(a.second));
}

long WldLevelItem::finalX() const
{
    return x + static_cast<long>(offset_x);
}

long WldLevelItem::finalY() const
{
    return y + static_cast<long>(offset_y);
}

long WldLevelItem::finalW() const
{
    return texture.w;
}

long WldLevelItem::finalH() const
{
    return texture.h;
}








WldMusicBoxItem::WldMusicBoxItem(WorldMusicBox _data): WorldNode()
{
    data = _data;
    x = data.x;
    y = data.y;
    Z = -10.0;
    type = musicbox;
}

WldMusicBoxItem::WldMusicBoxItem(const WldMusicBoxItem &x): WorldNode(x)
{
    data = x.data;
    type = musicbox;
}

WldMusicBoxItem::~WldMusicBoxItem()
{}





class QTreePGE_WorldNode_Extractor
{
public:
    static void ExtractBoundingBox(const WorldNode *object, loose_quadtree::BoundingBox<long> *bbox)
    {
        bbox->left      = object->finalX();
        bbox->top       = object->finalY();
        bbox->width     = object->finalW();
        bbox->height    = object->finalH();
    }
};

struct WldQuadTree_private
{
    typedef loose_quadtree::LooseQuadtree<long, WorldNode, QTreePGE_WorldNode_Extractor> IndexTreeQ;
    IndexTreeQ tree;
};



TileBox::TileBox() :
    p(new WldQuadTree_private)
{
    gridSize = ConfigManager::default_grid;
    gridSize_h = ConfigManager::default_grid / 2;
}

TileBox::TileBox(unsigned long size)
{
    gridSize = static_cast<long>(size);
    gridSize_h = size / 2;
}

TileBox::~TileBox()
{
    clean();
}

void TileBox::addNode(WorldNode *item)
{
    p->tree.Insert(item);
}

void TileBox::removeNode(WorldNode *item)
{
    p->tree.Remove(item);
}

void TileBox::updateElement(WorldNode *item)
{
    p->tree.Update(item);
}

void TileBox::query(long X, long Y, std::vector<WorldNode *> &list)
{
    //PGE_Point t = applyGrid(X,Y);
    long margin = gridSize_h - 2l;
    WldQuadTree_private::IndexTreeQ::Query q = p->tree.QueryIntersectsRegion(loose_quadtree::BoundingBox<long>(X - margin, Y - margin, margin * 2, margin * 2));
    while(!q.EndOfQuery())
    {
        auto *item = q.GetCurrent();
        if(item)
            list.push_back(q.GetCurrent());
        q.Next();
    }
}

void TileBox::query(long Left, long Top, long Right, long Bottom, std::vector<WorldNode *> &list, bool z_sort)
{
    WldQuadTree_private::IndexTreeQ::Query q = p->tree.QueryIntersectsRegion(loose_quadtree::BoundingBox<long>(Left - gridSize_h, Top - gridSize_h, (Right - Left) + gridSize_h * 2, (Bottom - Top) +  + gridSize_h * 2));
    while(!q.EndOfQuery())
    {
        auto *item = q.GetCurrent();
        if(item)
        {
            if((z_sort && item->vizible) || !z_sort)
                list.push_back(q.GetCurrent());
        }
        q.Next();
    }

    if(z_sort)
        sortElements(list);
}

void TileBox::sortElements(std::vector<WorldNode *> &list)
{
    if(list.size() <= 1)
        return; //Nothing to sort!

    std::vector<size_t> beg;
    std::vector<size_t> end;
    beg.reserve(list.size());
    end.reserve(list.size());

    WorldNode *piv;
    ssize_t i = 0;
    ssize_t L, R, swapv;
    beg.push_back(0);
    end.push_back(list.size());

    while(i >= 0)
    {
        L = beg[static_cast<size_t>(i)];
        R = end[static_cast<size_t>(i)] - 1;

        if(L < R)
        {
            piv = list[L];
            while(L < R)
            {
                while((list[R]->Z >= piv->Z) && (L < R)) R--;
                if(L < R) list[L++] = std::move(list[R]);

                while((list[L]->Z <= piv->Z) && (L < R)) L++;
                if(L < R) list[R--] = std::move(list[L]);
            }

            list[L] = piv;
            beg.push_back(L + 1);
            end.push_back(end[i]);
            end[i++] = (L);

            if((end[i] - beg[i]) > (end[i - 1] - beg[i - 1]))
            {
                swapv = beg[i];
                beg[i] = beg[i - 1];
                beg[i - 1] = swapv;
                swapv = end[i];
                end[i] = end[i - 1];
                end[i - 1] = swapv;
            }
        }
        else
        {
            i--;
            beg.pop_back();
            end.pop_back();
        }
    }
}

void TileBox::clean()
{
    p->tree.Clear();
}

const long &TileBox::grid()
{
    return gridSize;
}

const long &TileBox::grid_half()
{
    return gridSize_h;
}


PGE_Point TileBox::applyGrid(long x, long y)
{
    PGE_Point source;
    source.setX(static_cast<int>(x));
    source.setY(static_cast<int>(y));
    int gridX, gridY;
    int gridSz = static_cast<int>(gridSize);

    if(gridSz > 0)
    {
        //ATTACH TO GRID
        if(source.x() < 0)
            gridX = source.x() + 1 + (abs(source.x() + 1) % gridSz) - gridSz;
        else
            gridX = (source.x() - source.x() % gridSz);

        if(source.y() < 0)
            gridY = source.y() + 1 + (abs(source.y() + 1) % gridSz) - gridSz;
        else
            gridY = (source.y() - source.y() % gridSz);

        return PGE_Point(gridX, gridY);
    }
    else
        return source;
}
