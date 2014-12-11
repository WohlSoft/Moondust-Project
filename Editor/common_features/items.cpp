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

                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_blocks.size())
                {
                    j = scn->index_blocks[ItemID].i;
                    animator = scn->index_blocks[ItemID].ai;
                }

                if(j >= main->configs.main_block.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_Blocks[animator]->wholeImage();
                else
                    return scn->animates_Blocks[animator]->image(main->configs.main_block[j].display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                j = main->configs.getBlockI(ItemID);

                if( confId != NULL)
                    * confId = j;

                if(j>=0)
                {
                    if(j >= main->configs.main_block.size()) j=0;

                    if((!whole)&&(main->configs.main_block[j].animated))
                        tmpI = main->configs.main_block[j].image.copy(0,
                                    (int)round(main->configs.main_block[j].image.height() / main->configs.main_block[j].frames)*main->configs.main_block[j].display_frame,
                                    main->configs.main_block[j].image.width(),
                                    (int)round(main->configs.main_block[j].image.height() / main->configs.main_block[j].frames));
                    else
                        tmpI = main->configs.main_block[j].image;
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

                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_bgo.size())
                {
                    j = scn->index_bgo[ItemID].i;
                    animator = scn->index_bgo[ItemID].ai;
                }

                if(j >= main->configs.main_bgo.size()) j=0;

                if( confId != NULL)
                    * confId = j;

                if(whole)
                    return scn->animates_BGO[animator]->wholeImage();
                else
                    return scn->animates_BGO[animator]->image(main->configs.main_bgo[j].display_frame);
            }
            else
            {
                QPixmap tmpI;
                long j=0;

                //Check for index
                j = main->configs.getBgoI(ItemID);

                if( confId != NULL)
                    * confId = j;

                if(j>=0)
                {
                    if(j >= main->configs.main_bgo.size()) j=0;

                    if((main->configs.main_bgo[j].animated)&&(!whole))
                        tmpI = main->configs.main_bgo[j].image.copy(0,
                                    (int)round(main->configs.main_bgo[j].image.height() / main->configs.main_bgo[j].frames)*main->configs.main_bgo[j].display_frame,
                                    main->configs.main_bgo[j].image.width(),
                                    (int)round(main->configs.main_bgo[j].image.height() / main->configs.main_bgo[j].frames));
                    else
                        tmpI = main->configs.main_bgo[j].image;
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
            }
            else
            {
                int j=0;
                bool noimage=true;
                QPixmap tImg;
                //Check Index exists

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
                    tImg=QPixmap(ApplicationPath + "/" + "data/unknown_npc.png");
                }
                else
                {
                    if(!whole)
                        tImg= tImg.copy(0, main->configs.main_npc[j].gfx_h*main->configs.main_npc[j].display_frame, main->configs.main_npc[j].image.width(), main->configs.main_npc[j].gfx_h );
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
