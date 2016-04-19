/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/mainwinconnect.h>
#include <common_features/themes.h>

#include "items.h"
#include "app_path.h"

/*!
 * \brief Retreives sprite or single frame of the requested element
 * \param itemType Type of the element (Block, BGO, NPC, Terrain tile, World map scenery, Path tile, Level tile)
 * \param ItemID ID of requested element
 * \param whole Retreive whole srite, overwise single default frame will be retured
 * \param scene Pointer to edit scene, if NULL, current active scene will be retreived
 * \return Pixmap which contains requested sprite
 */
QPixmap Items::getItemGFX(int itemType, unsigned long ItemID, bool whole, QGraphicsScene *scene)
{
    MainWindow *main = MainWinConnect::pMainWin;

    switch(itemType)
    {
    case ItemTypes::LVL_Block:
        {
            if(
                    ((main->activeChildWindow()==1)&&(main->activeLvlEditWin()->sceneCreated))||
                    (scene && (QString(scene->metaObject()->className())=="LvlScene"))
                )
            {
                LvlScene *scn;

                if(scene)
                    scn = dynamic_cast<LvlScene *>(scene);
                else
                    scn = main->activeLvlEditWin()->scene;

                obj_block & block= scn->uBlocks[ItemID];
                long animator=block.animator_id;

                if(whole)
                    return scn->animates_Blocks[animator]->wholeImage();
                else
                    return scn->animates_Blocks[animator]->image(block.display_frame);
            }
            else
            {
                QPixmap tmpI;

                //Check for index
                obj_block & block= main->configs.main_block[ItemID];

                if(block.isValid)
                {
                    if((!whole)&&(block.animated))
                        tmpI = block.image.copy(0,
                                    (int)round(block.image.height() / block.frames)*block.display_frame,
                                    block.image.width(),
                                    (int)round(block.image.height() / block.frames));
                    else
                        tmpI = block.image;
                }

                return tmpI;
            }

            break;
        }
    case ItemTypes::LVL_BGO:
        {
            if(
                ((main->activeChildWindow()==1)&&(main->activeLvlEditWin()->sceneCreated))||
                (scene && (QString(scene->metaObject()->className())=="LvlScene"))
            )
            {
                LvlScene *scn;

                if(scene)
                    scn = dynamic_cast<LvlScene *>(scene);
                else
                    scn = main->activeLvlEditWin()->scene;

                obj_bgo & bgo= scn->uBGOs[ItemID];
                long animator=bgo.animator_id;

                if(whole)
                    return scn->animates_BGO[animator]->wholeImage();
                else
                    return scn->animates_BGO[animator]->image(bgo.display_frame);
            }
            else
            {
                QPixmap tmpI;

                obj_bgo & bgo= main->configs.main_bgo[ItemID];

                if(bgo.isValid)
                {
                    if((bgo.animated)&&(!whole))
                        tmpI = bgo.image.copy(0,
                                    (int)round(bgo.image.height() / bgo.frames) * bgo.display_frame,
                                    bgo.image.width(),
                                    (int)round(bgo.image.height() / bgo.frames));
                    else
                        tmpI = bgo.image;
                }

                return tmpI;
            }
            break;
        }
    case ItemTypes::LVL_NPC:
        {
            if(
                ((main->activeChildWindow()==1)&&(main->activeLvlEditWin()->sceneCreated))||
                (scene && (QString(scene->metaObject()->className())=="LvlScene"))
            )
            {
                LvlScene *scn;

                if(scene)
                    scn = dynamic_cast<LvlScene *>(scene);
                else
                    scn = main->activeLvlEditWin()->scene;

                obj_npc & npc= scn->uNPCs[ItemID];
                long animator=npc.animator_id;

                if(whole)
                    return scn->animates_NPC[animator]->wholeImage();
                else
                    return scn->animates_NPC[animator]->image(-1, npc.display_frame);
            }
            else
            {
                QPixmap tImg;

                //Check Index exists
                obj_npc & npc= main->configs.main_npc[ItemID];
                if(npc.isValid)
                {
                    if(!whole)
                        tImg = npc.image.copy(0,
                                    (int)round(npc.gfx_h / npc.frames) * npc.display_frame,
                                    npc.image.width(),
                                    (int)round(npc.gfx_h / npc.frames));
                    else
                        tImg = npc.image;
                }
                return tImg;
            }
            break;
        }
    case ItemTypes::WLD_Tile:
        {
            if(
                    ((main->activeChildWindow()==3)&&(main->activeWldEditWin()->sceneCreated))||
                    (scene && (QString(scene->metaObject()->className())=="WldScene"))
                )
            {
                WldScene *scn;
                if(scene)
                    scn = dynamic_cast<WldScene *>(scene);
                else
                    scn = main->activeWldEditWin()->scene;

                obj_w_tile & tile= scn->uTiles[ItemID];
                long animator=tile.animator_id;

                if(whole)
                    return scn->animates_Tiles[animator]->wholeImage();
                else
                    return scn->animates_Tiles[animator]->image(tile.display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                obj_w_tile &tile = main->configs.main_wtiles[ItemID];

                if(tile.isValid)
                {
                    if(j >= main->configs.main_wtiles.size()) j=0;

                    if((!whole)&&(main->configs.main_wtiles[j].animated))
                        tmpI = tile.image.copy(0,
                                (int)round(tile.image.height() / tile.frames) * tile.display_frame,
                                tile.image.width(),
                                (int)round(tile.image.height() / tile.frames));
                    else
                        tmpI = tile.image;
                }

                return tmpI;
            }

            break;
        }
    case ItemTypes::WLD_Scenery:
        {
            if(
                    ((main->activeChildWindow()==3)&&(main->activeWldEditWin()->sceneCreated))||
                    (scene && (QString(scene->metaObject()->className())=="WldScene"))
                )
            {
                WldScene *scn;
                if(scene)
                    scn = dynamic_cast<WldScene *>(scene);
                else
                    scn = main->activeWldEditWin()->scene;

                obj_w_scenery & scenery= scn->uScenes[ItemID];
                long animator=scenery.animator_id;

                if(whole)
                    return scn->animates_Scenery[animator]->wholeImage();
                else
                    return scn->animates_Scenery[animator]->image(scenery.display_frame);
            }
            else
            {
                QPixmap tmpI;

                obj_w_scenery & scenery= main->configs.main_wscene[ItemID];

                if(scenery.isValid)
                {
                    if((scenery.animated)&&(!whole))
                        tmpI = scenery.image.copy(0,
                                    (int)round(scenery.image.height() / scenery.frames) * scenery.display_frame,
                                    scenery.image.width(),
                                    (int)round(scenery.image.height() / scenery.frames));
                    else
                        tmpI = scenery.image;
                }

                return tmpI;
            }

            break;
        }
    case ItemTypes::WLD_Path:
        {
            if(
                    ((main->activeChildWindow()==3)&&(main->activeWldEditWin()->sceneCreated))||
                    (scene && (QString(scene->metaObject()->className())=="WldScene"))
                )
            {
                WldScene *scn;
                if(scene)
                    scn = dynamic_cast<WldScene *>(scene);
                else
                    scn = main->activeWldEditWin()->scene;

                obj_w_path & wpath= scn->uPaths[ItemID];
                long animator=wpath.animator_id;

                if(whole)
                    return scn->animates_Paths[animator]->wholeImage();
                else
                    return scn->animates_Paths[animator]->image(wpath.display_frame);
            }
            else
            {
                QPixmap tmpI;

                obj_w_path & wpath= main->configs.main_wpaths[ItemID];

                if(wpath.isValid)
                {
                    if((wpath.animated)&&(!whole))
                        tmpI = wpath.image.copy(0,
                                    (int)round(wpath.image.height() / wpath.frames) * wpath.display_frame,
                                    wpath.image.width(),
                                    (int)round(wpath.image.height() / wpath.frames));
                    else
                        tmpI = wpath.image;
                }

                return tmpI;
            }

            break;
        }
    case ItemTypes::WLD_Level:
        {
            if(
                    ((main->activeChildWindow()==3)&&(main->activeWldEditWin()->sceneCreated))||
                    (scene && (QString(scene->metaObject()->className())=="WldScene"))
                )
            {
                WldScene *scn;
                if(scene)
                    scn = dynamic_cast<WldScene *>(scene);
                else
                    scn = main->activeWldEditWin()->scene;

                obj_w_level & level= scn->uLevels[ItemID];
                long animator=level.animator_id;

                if(whole)
                    return scn->animates_Levels[animator]->wholeImage();
                else
                    return scn->animates_Levels[animator]->image(level.display_frame);
            }
            else
            {
                QPixmap tmpI;

                obj_w_level & level= main->configs.main_wlevels[ItemID];

                if(level.isValid)
                {
                    if((level.animated)&&(!whole))
                        tmpI = level.image.copy(0,
                                    (int)round(level.image.height() / level.frames) * level.display_frame,
                                    level.image.width(),
                                    (int)round(level.image.height() / level.frames));
                    else
                        tmpI = level.image;
                }

                return tmpI;
            }
            break;
        }
    default:
        break;
    }
    //return null if wrong item
    return QPixmap();
}



int Items::getItemType(QString type)
{
    int target=0;

    if(type.toLower()=="block")
        target=ItemTypes::LVL_Block;
    else
    if(type.toLower()=="bgo")
        target=ItemTypes::LVL_BGO;
    else
    if(type.toLower()=="npc")
        target=ItemTypes::LVL_NPC;
    else
    if((type.toLower()=="physenv")||(type.toLower()=="water"))
        target=ItemTypes::LVL_PhysEnv;
    else
    if((type.toLower()=="warp")||(type.toLower()=="door"))
        target=ItemTypes::LVL_Door;
    else
    if((type.toLower()=="character")||(type.toLower()=="player"))
        target=ItemTypes::LVL_Player;
    else
    if(type.toLower()=="tile")
        target=ItemTypes::WLD_Tile;
    else
    if(type.toLower()=="scenery")
        target=ItemTypes::WLD_Scenery;
    else
    if(type.toLower()=="path")
        target=ItemTypes::WLD_Path;
    else
    if(type.toLower()=="level")
        target=ItemTypes::WLD_Level;
    else
    if(type.toLower()=="musicbix")
        target=ItemTypes::WLD_MusicBox;
    else
    {
        bool ok=true;
        target = type.toInt(&ok);
        if(!ok) target=-1;
    }
    return target;
}
