/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/themes.h>
#include <editing/edit_world/world_edit.h>
#include <main_window/global_settings.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"

void WldScene::SwitchEditingMode(int EdtMode)
{
    m_eraserIsEnabled = false; //All just selected items will be removed
    m_pastingMode = false;
    m_busyMode = false; //Placing/drawing on map, disable selecting and dragging items
    m_disableMoveItems = false; // You can do anything with items, but can't move them

    switch(EdtMode)
    {
    case MODE_PlacingNew:
        switchMode("Placing");
        break;

    case MODE_DrawRect:
        switchMode("Square");
        break;

    case MODE_DrawCircle:
        switchMode("Circle");
        break;

    case MODE_Line:
        switchMode("Line");
        break;

    case MODE_SetPoint:
        switchMode("SetPoint");
        break;

    case MODE_Resizing:
        switchMode("Resize");
        break;

    case MODE_PasteFromClip:
        switchMode("Select");
        clearSelection();
        m_disableMoveItems = true;
        m_viewPort->setInteractive(true);
        m_viewPort->setCursor(Themes::Cursor(Themes::cursor_pasting));
        m_viewPort->setDragMode(QGraphicsView::NoDrag);
        break;

    case MODE_Erasing:
        switchMode("Erase");
        break;

    case MODE_SelectingOnly:
        switchMode("Select");
        m_disableMoveItems = true;
        break;

    case MODE_HandScroll:
        switchMode("HandScroll");
        break;

    case MODE_Fill:
        switchMode("Fill");
        break;

    case MODE_Selecting:
    default:
        switchMode("Select");
        break;

    }
    m_editMode = EdtMode;

}

void WldScene::switchMode(QString title)
{
    for(int i = 0; i < m_editModes.size(); i++)
    {
        if(m_editModes[i]->name() == title)
        {
            m_editModeObj = m_editModes[i];
            m_editModeObj->set();
            break;
        }
    }
}



void WldScene::hideMusicBoxes(bool visible)
{
    foreach(QGraphicsItem *i, items())
    {
        if(i->data(0).toString() == "MUSICBOX")
            i->setVisible(visible);
    }
}

void WldScene::setSemiTransparentPaths(bool semiTransparent)
{
    foreach(QGraphicsItem *i, items())
    {
        if(i->data(0).toString() == "PATH")
            i->setOpacity(semiTransparent ? 0.5 : 1);
    }
    update();
}

void WldScene::hidePathAndLevels(bool visible)
{
    foreach(QGraphicsItem *i, items())
    {
        if(i->data(0).toString() == "PATH")
            i->setVisible(visible);
        else if(i->data(0).toString() == "LEVEL")
        {
            i->setVisible(visible ||
                          ((ItemLevel *)i)->m_data.gamestart ||
                          ((ItemLevel *)i)->m_data.alwaysVisible);
        }
    }
}

/////////////////////////////////////////////Locks////////////////////////////////
void WldScene::setLocked(int type, bool lock)
{
    QList<QGraphicsItem *> ItemList = items();
    // setLock
    switch(type)
    {
    case 1://Tile
        m_lockTerrain = lock;
        break;
    case 2://Scenery
        m_lockScenery = lock;
        break;
    case 3://Paths
        m_lockPath = lock;
        break;
    case 4://Levels
        m_lockLevel = lock;
        break;
    case 5://MusicBoxes
        m_lockMusicBox = lock;
        break;
    default:
        break;
    }

    for(QList<QGraphicsItem *>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        switch(type)
        {
        case 1://Tile
            if((*it)->data(0).toString() == "TILE")
            {
                ItemTile *gi = qgraphicsitem_cast<ItemTile *>(*it);
                if(!gi)continue;
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!((lock) || gi->isLocked())));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!((lock) || gi->isLocked())));
            }
            break;
        case 2://Scenery
            if((*it)->data(0).toString() == "SCENERY")
            {
                ItemScene *gi = qgraphicsitem_cast<ItemScene *>(*it);
                if(!gi)continue;
                gi->setFlag(QGraphicsItem::ItemIsSelectable, (!((lock) || ((ItemScene *)(*it))->isLocked())));
                gi->setFlag(QGraphicsItem::ItemIsMovable, (!((lock) || ((ItemScene *)(*it))->isLocked())));
            }
            break;
        case 3://Paths
            if((*it)->data(0).toString() == "PATH")
            {
                ItemPath *gi = qgraphicsitem_cast<ItemPath *>(*it);
                if(!gi)continue;
                gi->setFlag(QGraphicsItem::ItemIsSelectable, (!((lock) || gi->isLocked())));
                gi->setFlag(QGraphicsItem::ItemIsMovable, (!((lock) || gi->isLocked())));
            }
            break;
        case 4://Levels
            if((*it)->data(0).toString() == "LEVEL")
            {
                ItemLevel *gi = qgraphicsitem_cast<ItemLevel *>(*it);
                if(!gi)continue;
                gi->setFlag(QGraphicsItem::ItemIsSelectable, (!((lock) || gi->isLocked())));
                gi->setFlag(QGraphicsItem::ItemIsMovable, (!((lock) || gi->isLocked())));
            }
            break;
        case 5://Musicboxes
            if((*it)->data(0).toString() == "MUSICBOX")
            {
                ItemMusic *gi = qgraphicsitem_cast<ItemMusic *>(*it);
                if(!gi)continue;
                gi->setFlag(QGraphicsItem::ItemIsSelectable, (!((lock) || gi->isLocked())));
                gi->setFlag(QGraphicsItem::ItemIsMovable, (!((lock) || gi->isLocked())));
            }
            break;
        default:
            break;
        }
    }

}




