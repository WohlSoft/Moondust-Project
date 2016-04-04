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

QPixmap Items::getItemGFX(int itemType, unsigned long ItemID, bool whole, long  *confId, QGraphicsScene *scene)
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

                if( confId != NULL)
                    * confId = ItemID;

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

                if( confId != NULL)
                    * confId = ItemID;

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

                if( confId != NULL)
                    * confId = bgo.id;

                if(whole)
                    return scn->animates_BGO[animator]->wholeImage();
                else
                    return scn->animates_BGO[animator]->image(bgo.display_frame);
            }
            else
            {
                QPixmap tmpI;

                obj_bgo & bgo= main->configs.main_bgo[ItemID];

                if( confId != NULL)
                    * confId = bgo.id;

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
                if( confId != NULL)
                    * confId = npc.id;

                if(whole)
                    return scn->animates_NPC[animator]->wholeImage();
                else
                    return scn->animates_NPC[animator]->image(-1, npc.display_frame);
                /*
                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_npc.size())
                {
                    j = scn->index_npc[ItemID].gi-1;
                    animator = scn->index_npc[ItemID].ai;
                }

                if(j >= main->configs.main_npc.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_NPC[animator]->wholeImage();
                else
                    return scn->animates_NPC[animator]->image(-1, main->configs.main_npc[j].display_frame);
                 */
            }
            else
            {
                /*int j=0;
                bool noimage=true;*/
                QPixmap tImg;
                //Check Index exists
                obj_npc & npc= main->configs.main_npc[ItemID];

                if( confId != NULL)
                     *confId = npc.id;

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

                /*
                j= main->configs.getNpcI(ItemID);

                if(j>=0)
                {   //get neccesary element directly
                    tImg = main->configs.main_npc[j].image;
                    noimage=false;
                }

                if( confId != NULL)
                    * confId = j;

                if((noimage)||(tImg.isNull()))
                {
                    tImg = Themes::Image(Themes::dummy_npc);
                }
                else
                {
                    if(!whole)
                        tImg= tImg.copy(0, main->configs.main_npc[j].gfx_h*main->configs.main_npc[j].display_frame, main->configs.main_npc[j].image.width(), main->configs.main_npc[j].gfx_h );
                }*/
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

                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_tiles.size())
                {
                    j = scn->index_tiles[ItemID].i;
                    animator = scn->index_tiles[ItemID].ai;
                }

                if(j >= main->configs.main_wtiles.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_Tiles[animator]->wholeImage();
                else
                    return scn->animates_Tiles[animator]->image(main->configs.main_wtiles[j].display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                j = main->configs.getTileI(ItemID);

                if( confId != NULL)
                    * confId = j;

                if(j>=0)
                {
                    if(j >= main->configs.main_wtiles.size()) j=0;

                    if((!whole)&&(main->configs.main_wtiles[j].animated))
                        tmpI = main->configs.main_wtiles[j].image.copy(0,
                                    (int)round(main->configs.main_wtiles[j].image.height() / main->configs.main_wtiles[j].frames)*main->configs.main_wtiles[j].display_frame,
                                    main->configs.main_wtiles[j].image.width(),
                                    (int)round(main->configs.main_wtiles[j].image.height() / main->configs.main_wtiles[j].frames));
                    else
                        tmpI = main->configs.main_wtiles[j].image;
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

                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_scenes.size())
                {
                    j = scn->index_scenes[ItemID].i;
                    animator = scn->index_scenes[ItemID].ai;
                }

                if(j >= main->configs.main_wscene.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_Scenery[animator]->wholeImage();
                else
                    return scn->animates_Scenery[animator]->image(main->configs.main_wscene[j].display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                j = main->configs.getSceneI(ItemID);

                if( confId != NULL)
                    * confId = j;

                if(j>=0)
                {
                    if(j >= main->configs.main_wscene.size()) j=0;

                    if((!whole)&&(main->configs.main_wscene[j].animated))
                        tmpI = main->configs.main_wscene[j].image.copy(0,
                                    (int)round(main->configs.main_wscene[j].image.height() / main->configs.main_wscene[j].frames)*main->configs.main_wscene[j].display_frame,
                                    main->configs.main_wscene[j].image.width(),
                                    (int)round(main->configs.main_wscene[j].image.height() / main->configs.main_wscene[j].frames));
                    else
                        tmpI = main->configs.main_wscene[j].image;
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

                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_paths.size())
                {
                    j = scn->index_paths[ItemID].i;
                    animator = scn->index_paths[ItemID].ai;
                }

                if(j >= main->configs.main_wpaths.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_Paths[animator]->wholeImage();
                else
                    return scn->animates_Paths[animator]->image(main->configs.main_wpaths[j].display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                j = main->configs.getPathI(ItemID);

                if( confId != NULL)
                    * confId = j;

                if(j>=0)
                {
                    if(j >= main->configs.main_wpaths.size()) j=0;

                    if((!whole)&&(main->configs.main_wpaths[j].animated))
                        tmpI = main->configs.main_wpaths[j].image.copy(0,
                                    (int)round(main->configs.main_wpaths[j].image.height() / main->configs.main_wpaths[j].frames)*main->configs.main_wpaths[j].display_frame,
                                    main->configs.main_wpaths[j].image.width(),
                                    (int)round(main->configs.main_wpaths[j].image.height() / main->configs.main_wpaths[j].frames));
                    else
                        tmpI = main->configs.main_wpaths[j].image;
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

                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_levels.size())
                {
                    j = scn->index_levels[ItemID].i;
                    animator = scn->index_levels[ItemID].ai;
                }

                if(j >= main->configs.main_wlevels.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_Levels[animator]->wholeImage();
                else
                    return scn->animates_Levels[animator]->image(main->configs.main_wlevels[j].display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                j = main->configs.getWLevelI(ItemID);

                if( confId != NULL)
                    * confId = j;

                if(j>=0)
                {
                    if(j >= main->configs.main_wlevels.size()) j=0;

                    if((!whole)&&(main->configs.main_wlevels[j].animated))
                        tmpI = main->configs.main_wlevels[j].image.copy(0,
                                    (int)round(main->configs.main_wlevels[j].image.height() / main->configs.main_wlevels[j].frames)*main->configs.main_wlevels[j].display_frame,
                                    main->configs.main_wlevels[j].image.width(),
                                    (int)round(main->configs.main_wlevels[j].image.height() / main->configs.main_wlevels[j].frames));
                    else
                        tmpI = main->configs.main_wlevels[j].image;
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
