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

#include <common_features/main_window_ptr.h>
#include <common_features/themes.h>
#include <common_features/graphics_funcs.h>

#include "items.h"
#include "app_path.h"

void Items::getItemGFX(const obj_npc *inObj, QPixmap &outImg, bool whole, QSize targetSize)
{
    if(inObj->isValid)
    {
        const QPixmap *srcImage = inObj->cur_image ? inObj->cur_image : &inObj->image;
        if( !whole )
        {
            outImg = srcImage->copy(0,
                                   inObj->setup.gfx_h*inObj->setup.display_frame,
                                   inObj->setup.gfx_w,
                                   inObj->setup.gfx_h);
        }
        else
        {
            outImg = *srcImage;
        }
        if( !targetSize.isNull() )
        {
            GraphicsHelps::squareImageR(outImg, targetSize);
        }
    }
    else
    {
        outImg = Themes::Image(Themes::dummy_terrain);
    }
}

template<class OBJ_ITEM>
inline void TPL_getItemGFX(const OBJ_ITEM *inObj, QPixmap &outImg, bool &whole, QSize &targetSize, Themes::Images imgType)
{
    if(inObj->isValid)
    {
        const QPixmap *srcImage = inObj->cur_image ? inObj->cur_image : &inObj->image;
        if( (!whole) && (inObj->setup.animated) )
        {
            outImg =  srcImage->copy(0,
                                    (int)round(srcImage->height()/inObj->setup.frames) * inObj->setup.display_frame,
                                    srcImage->width(),
                                    (int)round(srcImage->height() / inObj->setup.frames));
        }
        else
        {
            outImg = *srcImage;
        }
        if(!targetSize.isNull())
        {
            GraphicsHelps::squareImageR(outImg, targetSize);
        }
    }
    else
    {
        outImg = Themes::Image(imgType);
    }
}

void Items::getItemGFX(const obj_block *inObj, QPixmap &outImg, bool whole, QSize targetSize)
{
    TPL_getItemGFX(inObj, outImg, whole, targetSize, Themes::dummy_block);
}

void Items::getItemGFX(const obj_bgo *inObj, QPixmap &outImg, bool whole, QSize targetSize)
{
    TPL_getItemGFX(inObj, outImg, whole, targetSize, Themes::dummy_bgo);
}

void Items::getItemGFX(const obj_wld_generic *inObj, QPixmap &outImg, bool whole, QSize targetSize)
{
    Themes::Images imgType = Themes::dummy_terrain;
    switch(inObj->m_itemType)
    {
        case ItemTypes::WLD_Tile:
            imgType = Themes::dummy_terrain; break;
        case ItemTypes::WLD_Scenery:
            imgType = Themes::dummy_scenery; break;
        case ItemTypes::WLD_Path:
            imgType = Themes::dummy_path; break;
        case ItemTypes::WLD_Level:
            imgType = Themes::dummy_wlevel; break;
        default:;
    }
    TPL_getItemGFX(inObj, outImg, whole, targetSize, imgType);
}

void Items::getItemGFX(int itemType, unsigned long ItemID, QPixmap &outImg, QGraphicsScene *scene, bool whole, QSize targetSize)
{
    LvlScene *scene_lvl = dynamic_cast<LvlScene*>(scene);
    WldScene *scene_wld = dynamic_cast<WldScene*>(scene);
    dataconfigs &config = MainWinConnect::pMainWin->configs;
    switch(itemType)
    {
    case ItemTypes::LVL_Block:
        {
            PGE_DataArray<obj_block>* array = scene_lvl ? &scene_lvl->m_localConfigBlocks : &config.main_block;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    case ItemTypes::LVL_BGO:
        {
            PGE_DataArray<obj_bgo>* array = scene_lvl ? &scene_lvl->m_localConfigBGOs : &config.main_bgo;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    case ItemTypes::LVL_NPC:
        {
            PGE_DataArray<obj_npc>* array = scene_lvl ? &scene_lvl->m_localConfigNPCs : &config.main_npc;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    case ItemTypes::WLD_Tile:
        {
            PGE_DataArray<obj_w_tile>* array = scene_wld ? &scene_wld->m_localConfigTerrain : &config.main_wtiles;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    case ItemTypes::WLD_Scenery:
        {
            PGE_DataArray<obj_w_scenery>* array = scene_wld ? &scene_wld->m_localConfigScenery : &config.main_wscene;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    case ItemTypes::WLD_Path:
        {
            PGE_DataArray<obj_w_path>* array = scene_wld ? &scene_wld->m_localConfigPaths : &config.main_wpaths;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    case ItemTypes::WLD_Level:
        {
            PGE_DataArray<obj_w_level>* array = scene_wld ? &scene_wld->m_localConfigLevels : &config.main_wlevels;
            getItemGFX(&(*array)[ItemID], outImg, whole, targetSize);
        }
        break;
    default:
        break;
    }
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
    if(type.toLower()=="musicbox")
        target=ItemTypes::WLD_MusicBox;
    else
    {
        bool ok=true;
        target = type.toInt(&ok);
        if(!ok) target=-1;
    }
    return target;
}
