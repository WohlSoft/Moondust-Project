/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_LAYER_H
#define LVL_LAYER_H

#include <string> // on Emscripten: Must be here, or "type does not provide a call operator" error will appear
#include <unordered_set>
#include <unordered_map>
#include "lvl_base_object.h"
#include "lvl_subtree.h"

#define DESTROYED_LAYER_NAME    "Destroyed Blocks"
#define SPAWNED_LAYER_NAME      "Spawned NPCs"

class LevelScene;
class LVL_LayerEngine
{
    friend class LevelScene;
    LevelScene  *m_scene;
public:
    LVL_LayerEngine(LevelScene *_parent=NULL);
    void spawnSmokeAt(double x, double y);

    struct Layer
    {
        bool m_visible = true;
        typedef std::unordered_set<PGE_Phys_Object* > Members;
        Members     m_members;
        //! Count of destroyed objects are
        size_t      m_destroyedObjects = 0;
        //! Sub-tree of statical objects
        LVL_SubTree m_subtree;
        enum Type
        {
            T_REGULAR = 0,
            T_SPAWNED_NPCs,
            T_DESTROYED_BLOCKS
        };
        Type m_layerType = T_REGULAR;
    };

    Layer &getLayer(const std::string &lyr);

    void hide(std::string layer, bool smoke=true);
    void show(std::string layer, bool smoke=true);
    void toggle(std::string layer, bool smoke=true);
    void registerItem(std::string layer, PGE_Phys_Object* item, bool keepAbsPos = true);
    void removeRegItem(std::string layer, PGE_Phys_Object* item, bool keepAbsPos = true);
    void moveToAnotherLayerItem(std::string oldLayer, std::string newLayer, PGE_Phys_Object* item, bool keepAbsPos = true);

    /**
     * @brief Register item to layer's subtree as movable object and unregister from the scene's tree
     * @param lyr Layer to attach
     * @param item Item to re-register
     * @param enabled if true - register as movable, false - register as free item (will not move together with layer)
     * @param keepAbsPos - keep absolute position of the object and apply current layer's offsets when toggling the state
     */
    void setItemMovable(Layer& lyr, PGE_Phys_Object *item, bool enabled, bool keepAbsPos = true);

    bool isEmpty(std::string layer);
    void clear();

    struct MovingLayer
    {
        double m_speedX;
        double m_speedY;
        Layer::Members  *m_members;
        LVL_SubTree     *m_subtree;
    };

    void installLayerMotion(std::string layer, double speedX, double speedY);
    typedef std::unordered_map<std::string, Layer> LayersTable;
    LayersTable         m_layers;
    typedef std::unordered_map<std::string, MovingLayer> MovingLayersTable;
    MovingLayersTable   m_movingLayers;
    void processMoving(double tickTime);
};

#endif // LVL_LAYER_H
