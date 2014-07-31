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

#include "wld_scene.h"
#include "../edit_world/world_edit.h"

#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"

#include "../main_window/global_settings.h"

////////////////////////////////////Animator////////////////////////////////
void WldScene::startAnimation()
{
    long q = WldData->tiles.size();
    q+= WldData->levels.size();
    q+= WldData->scenery.size();
    q+= WldData->paths.size();

    if( q > GlobalSettings::animatorItemsLimit )
    {
        WriteToLog(QtWarningMsg,
           QString("Can't start animation: too many items on map: %1").arg(q));
        return;
    }

    foreach(SimpleAnimator * tile, animates_Tiles)
    {
        tile->start();
    }
    foreach(SimpleAnimator * scene, animates_Scenery)
    {
        scene->start();
    }
    foreach(SimpleAnimator * path, animates_Paths)
    {
        path->start();
    }
    foreach(SimpleAnimator * lvl, animates_Levels)
    {
        lvl->start();
    }


}

void WldScene::stopAnimation()
{

    foreach(SimpleAnimator * tile, animates_Tiles)
    {
        tile->stop();
    }
    foreach(SimpleAnimator * scene, animates_Scenery)
    {
        scene->stop();
    }
    foreach(SimpleAnimator * path, animates_Paths)
    {
        path->stop();
    }
    foreach(SimpleAnimator * lvl, animates_Levels)
    {
        lvl->stop();
    }

    update();
}

void WldScene::hideMusicBoxes(bool visible)
{
    foreach (QGraphicsItem* i, items())
    {
        if(i->data(0).toString()=="MUSICBOX"){
            i->setVisible(visible);
        }
    }
}

void WldScene::hidePathAndLevels(bool visible)
{
    foreach (QGraphicsItem* i, items())
    {
        if(i->data(0).toString()=="PATH")
        {
            i->setVisible(visible);
        }
        else
        if(i->data(0).toString()=="LEVEL")
        {
            i->setVisible( visible ||
                        ((ItemLevel)i).levelData.gamestart ||
                        ((ItemLevel)i).levelData.alwaysVisible );
        }
    }
}


//void WldScene::applyLayersVisible()
//{
//    QList<QGraphicsItem*> ItemList = items();
//    QGraphicsItem *tmp;
//    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
//    {
//        if((*it)->data(0)=="Block")
//        {
//            tmp = (*it);
//            foreach(LevelLayers layer, WldData->layers)
//            {
//                if( ((ItemBlock *)tmp)->blockData.layer == layer.name)
//                {
//                    ((ItemBlock *)tmp)->setVisible( !layer.hidden ); break;
//                }
//            }
//        }
//        else
//        if((*it)->data(0)=="BGO")
//        {
//            tmp = (*it);
//            foreach(LevelLayers layer, WldData->layers)
//            {
//                if( ((ItemBGO *)tmp)->bgoData.layer == layer.name)
//                {
//                    ((ItemBGO *)tmp)->setVisible( !layer.hidden ); break;
//                }
//            }
//        }
//        else
//        if((*it)->data(0)=="NPC")
//        {
//            tmp = (*it);
//            foreach(LevelLayers layer, WldData->layers)
//            {
//                if( ((ItemNPC *)tmp)->npcData.layer == layer.name)
//                {
//                    ((ItemNPC *)tmp)->setVisible( !layer.hidden ); break;
//                }
//            }
//        }
//        else
//        if((*it)->data(0)=="Water")
//        {
//            tmp = (*it);
//            foreach(LevelLayers layer, WldData->layers)
//            {
//                if( ((ItemWater *)tmp)->waterData.layer == layer.name)
//                {
//                    ((ItemWater *)tmp)->setVisible( !layer.hidden ); break;
//                }
//            }
//        }
//        else
//        if(((*it)->data(0)=="Door_enter")||((*it)->data(0)=="Door_exit"))
//        {
//            tmp = (*it);
//            foreach(LevelLayers layer, WldData->layers)
//            {
//                if( ((ItemDoor *)tmp)->doorData.layer == layer.name)
//                {
//                    ((ItemDoor *)tmp)->setVisible( !layer.hidden ); break;
//                }
//            }
//        }
//    }
//}


/////////////////////////////////////////////Locks////////////////////////////////
void WldScene::setLocked(int type, bool lock)
{
    QList<QGraphicsItem*> ItemList = items();
    // setLock
    switch(type)
    {
    case 1://Tile
        lock_tile = lock;
        break;
    case 2://Scenery
        lock_scene = lock;
        break;
    case 3://Paths
        lock_path = lock;
        break;
    case 4://Levels
        lock_level = lock;
        break;
    case 5://MusicBoxes
        lock_musbox = lock;
        break;
    default: break;
    }

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        switch(type)
        {
        case 1://Tile
            if((*it)->data(0).toString()=="TILE")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemTile *)(*it))->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemTile *)(*it))->isLocked ) ) );
            }
            break;
        case 2://Scenery
            if((*it)->data(0).toString()=="SCENERY")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemScene *)(*it))->isLocked ) ));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemScene *)(*it))->isLocked ) ));
            }
            break;
        case 3://Paths
            if((*it)->data(0).toString()=="PATH")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemPath *)(*it))->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemPath *)(*it))->isLocked ) ) );
            }
            break;
        case 4://Levels
            if((*it)->data(0).toString()=="LEVEL")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemLevel *)(*it))->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemLevel *)(*it))->isLocked ) ) );
            }
            break;
        case 5://Musicboxes
            if((*it)->data(0).toString()=="MUSICBOX")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemMusic *)(*it))->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemMusic *)(*it))->isLocked ) ) );
            }
            break;
        default: break;
        }
    }

}






void WldScene::SwitchEditingMode(int EdtMode)
{
    //int EditingMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
                     // 3 - drawing water/sand zone, 4 - placing from Buffer
    //bool EraserEnabled;
    //bool PasteFromBuffer;

    //bool DrawMode; //Placing/drawing on map, disable selecting and dragging items

    //bool disableMoveItems;

    //bool contextMenuOpened;
    EraserEnabled=false;
    PasteFromBuffer=false;
    DrawMode=false;
    disableMoveItems=false;

    switch(EdtMode)
    {
    case MODE_PlacingNew:
        DrawMode=true;
        resetResizers();

        break;
    case MODE_DrawSquare:
        resetResizers();
        DrawMode=true;
        break;

    case MODE_Line:
        resetResizers();
        DrawMode=true;
        break;

    case MODE_Resizing:
        resetCursor();
        DrawMode=true;
        disableMoveItems=true;
        break;

    case MODE_PasteFromClip:
        resetCursor();
        resetResizers();
        disableMoveItems=true;
        break;

    case MODE_Erasing:
        resetCursor();
        resetResizers();
        break;

    case MODE_SelectingOnly:
        resetCursor();
        resetResizers();
        disableMoveItems=true;
        break;
    case MODE_Selecting:
    default:
        resetCursor();
        resetResizers();
        break;
    }
    EditingMode = EdtMode;

}
