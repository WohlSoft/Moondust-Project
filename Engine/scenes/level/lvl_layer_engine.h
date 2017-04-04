/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_LAYER_H
#define LVL_LAYER_H

#include <unordered_map>
#include "lvl_base_object.h"

#define DESTROYED_LAYER_NAME    "Destroyed Blocks"
#define SPAWNED_LAYER_NAME      "Spawned NPCs"

class LevelScene;

struct LVL_SubTree_private;
class LVL_SubTree : public PGE_Phys_Object
{
    public:
        LVL_SubTree(LevelScene *_parent = NULL);
        LVL_SubTree(const LVL_SubTree &st);
        ~LVL_SubTree();

        typedef bool (*t_resultCallback)(PGE_Phys_Object*, void *);
        void query(PGE_RectF &zone, t_resultCallback a_resultCallback, void *context);

    private:
        LVL_SubTree_private *p = nullptr;
};

class LVL_LayerEngine
{
    friend class LevelScene;
    LevelScene * m_scene;
public:
    LVL_LayerEngine(LevelScene *_parent=NULL);
    void spawnSmokeAt(double x, double y);
    void hide(std::string layer, bool smoke=true);
    void show(std::string layer, bool smoke=true);
    void toggle(std::string layer, bool smoke=true);
    void registerItem(std::string layer, PGE_Phys_Object* item);
    void removeRegItem(std::string layer, PGE_Phys_Object* item);
    bool isEmpty(std::string layer);
    void clear();
    struct Layer
    {
        Layer() :
            m_vizible(true),
            m_layerType(T_REGULAR)
        {}
        bool m_vizible;
        typedef std::unordered_map<intptr_t, PGE_Phys_Object* > Members;
        Members m_members;
        enum Type
        {
            T_REGULAR=0,
            T_SPAWNED_NPCs,
            T_DESTROYED_BLOCKS
        };
        Type m_layerType;
    };
    struct MovingLayer
    {
        double m_speedX;
        double m_speedY;
        Layer::Members *m_members;
    };
    void installLayerMotion(std::string layer, double speedX, double speedY);
    typedef std::unordered_map<std::string, Layer> LayersTable;
    LayersTable         m_layers;
    typedef std::unordered_map<std::string, MovingLayer> MovingLayersTable;
    MovingLayersTable   m_movingLayers;
    void processMoving(double tickTime);
};

#endif // LVL_LAYER_H
