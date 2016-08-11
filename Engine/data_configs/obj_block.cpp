/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_manager.h"
#include <gui/pge_msgbox.h>
#include <common_features/graphics_funcs.h>
#include <common_features/number_limiter.h>
#include <common_features/util.h>

/*****Level blocks************/
PGE_DataArray<obj_block>   ConfigManager::lvl_block_indexes;
CustomDirManager        ConfigManager::Dir_Blocks;
QList<SimpleAnimator >  ConfigManager::Animator_Blocks;
/*****Level blocks************/

bool ConfigManager::loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal = !setup;
    QString errStr;
    if(internal)
        setup=new QSettings(iniFile, QSettings::IniFormat);

    sblock.isInit = false;
    sblock.image = NULL;
    sblock.textureArrayId = 0;
    sblock.animator_ID = -1;

    setup->beginGroup( section );
        if(sblock.setup.parse(setup, blockPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        {
            valid=true;
        } else {
            addError(errStr);
            valid=false;
        }
    setup->endGroup();
    if(internal) delete setup;
    return valid;
}

bool ConfigManager::loadLevelBlocks()
{
    QString Temp01="";
    unsigned int i;

    obj_block sblock;
    unsigned int block_total=0;


    QString block_ini = config_dir + "lvl_blocks.ini";

    if(!QFile::exists(block_ini))
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal(QString("ERROR LOADING lvl_blocks.ini: file does not exist"));
        return false;
    }

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    lvl_block_indexes.clear();//Clear old

    blockset.beginGroup("blocks-main");
        block_total = blockset.value("total", 0).toUInt();
    blockset.endGroup();



    if(block_total==0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"));

        return false;
    }

    lvl_block_indexes.allocateSlots(signed(block_total));


        for(i=1; i<=block_total; i++)
        {
            #if 0
            sblock.isInit=false;
            sblock.image = NULL;
            sblock.textureArrayId = 0;
            sblock.animator_ID = 0;


                blockset.beginGroup( QString("block-%1").arg(i) );

                sblock.setup.name = blockset.value("name", QString("block %1").arg(i) ).toString();

                if(sblock.setup.name=="")
                {
                    addError(QString("BLOCK-%1 Item name isn't defined").arg(i));
                    goto skipBLOCK;
                }

                sblock.setup.group = blockset.value("group", "_NoGroup").toString();
                sblock.setup.category = blockset.value("category", "_Other").toString();
                //sblock.grid = blockset.value("grid", default_grid).toInt();
                imgFile = blockset.value("image", "").toString();
                GraphicsHelps::getMaskedImageInfo(blockPath, imgFile, sblock.setup.mask_n, Temp01);
                sblock.setup.image_n = imgFile;
                if( imgFile=="" )
                {
                    addError(QString("BLOCK-%1 Image filename isn't defined.\n%2").arg(i).arg(Temp01));
                    goto skipBLOCK;
                }

                sblock.setup.sizable = blockset.value("sizable", "0").toBool();
                sblock.setup.danger = blockset.value("danger", "0").toInt();
                sblock.setup.collision = blockset.value("collision", "1").toInt();
                sblock.setup.slopeslide = blockset.value("slope-slide", "0").toBool();
                sblock.setup.phys_shape = blockset.value("fixture-type", "0").toInt();// Leaved for compatibility
                sblock.setup.phys_shape = blockset.value("shape-type", sblock.setup.phys_shape).toInt();//new value-name
                sblock.setup.lava = blockset.value("lava", "0").toBool();
                sblock.setup.destroyable = blockset.value("destroyable", "0").toBool();
                sblock.setup.destroyable_by_bomb = blockset.value("destroyable-by-bomb", "0").toBool();
                sblock.setup.destroyable_by_fireball = blockset.value("destroyable-by-fireball", "0").toBool();

                Temp01 = blockset.value("spawn-on-destroy", "0").toString();
                if(Temp01!="0")
                {
                    tmp =  Temp01.split("-", QString::SkipEmptyParts);
                    if(tmp.size()==2)
                    {
                        if(tmp[0]=="npc")
                            sblock.setup.spawn_obj = 1;
                        else
                        if(tmp[0]=="block")
                             sblock.setup.spawn_obj = 2;
                        else
                        if(tmp[0]=="bgo")
                             sblock.setup.spawn_obj = 3;
                        // 1 - NPC, 2 - block, 3 - BGO
                        sblock.setup.spawn_obj_id = tmp[1].toUInt();
                    }
                    else // if syntax error in config
                    {
                        sblock.setup.spawn = false;
                        sblock.setup.spawn_obj = 0;
                        sblock.setup.spawn_obj_id = 0;
                    }
                }
                else
                {
                    sblock.setup.spawn = false;
                    sblock.setup.spawn_obj = 0;
                    sblock.setup.spawn_obj_id = 0;
                }

                sblock.setup.effect = blockset.value("destroy-effect", 1).toUInt();

                sblock.setup.bounce = blockset.value("bounce", "0").toBool();
                sblock.setup.hitable = blockset.value("hitable", "0").toBool();
                sblock.setup.transfororm_on_hit_into = blockset.value("transform-onhit-into", 2).toUInt();
                sblock.setup.view = uint(blockset.value("view", "background").toString()=="foreground");
                sblock.setup.animated = blockset.value("animated", "0").toBool();
                sblock.setup.animation_rev = blockset.value("animation-reverse", "0").toBool(); //Reverse animation
                sblock.setup.animation_bid = blockset.value("animation-bidirectional", "0").toBool(); //Bidirectional animation
                sblock.setup.frames = blockset.value("frames", 1).toUInt();
                    NumberLimiter::apply(sblock.setup.frames, 1u);
                sblock.setup.framespeed = blockset.value("framespeed", "125").toInt();
                    NumberLimiter::apply(sblock.setup.framespeed, 1);
                sblock.setup.hit_sound_id = blockset.value("hit-sound-id", "0").toInt();
                    NumberLimiter::apply(sblock.setup.hit_sound_id, 0);
                sblock.setup.destroy_sound_id = blockset.value("destroy-sound-id", "0").toInt();
                    NumberLimiter::apply(sblock.setup.destroy_sound_id, 0);

                static int switchID=0;
                sblock.setup.switch_Button    = blockset.value("switch-button", false).toBool();
                sblock.setup.switch_Block     = blockset.value("switch-block", false).toBool();
                sblock.setup.switch_ID        = blockset.value("switch-id", switchID++).toInt();;
                sblock.setup.switch_transform = blockset.value("switch-transform", "1").toInt();;

                sblock.setup.display_frame = blockset.value("display-frame", 0).toUInt();

                sblock.setup.plSwitch_Button = blockset.value("player-switch-button", false).toBool();
                sblock.setup.plSwitch_Button_id = blockset.value("player-switch-button-id", 0).toInt();
                    NumberLimiter::apply(sblock.setup.plSwitch_Button_id, 0);
                sblock.setup.plSwitch_frames_true.clear();
                sblock.setup.plSwitch_frames_false.clear();
                if(sblock.setup.plSwitch_Button)
                {
                    util::CSV2IntArr(blockset.value("player-switch-frames-true", "").toString(), sblock.setup.plSwitch_frames_true);
                    util::CSV2IntArr(blockset.value("player-switch-frames-false", "").toString(), sblock.setup.plSwitch_frames_false);
                }

                sblock.setup.plFilter_Block= blockset.value("player-filter-block", false).toBool();
                sblock.setup.plFilter_Block_id= blockset.value("player-filter-block-id", 0).toInt();
                    NumberLimiter::apply(sblock.setup.plFilter_Block_id, 0);
                sblock.setup.plFilter_frames_true.clear();
                sblock.setup.plFilter_frames_false.clear();
                if(sblock.setup.plFilter_Block)
                {
                    util::CSV2IntArr(blockset.value("player-filter-frames-true", "").toString(), sblock.setup.plFilter_frames_true);
                    util::CSV2IntArr(blockset.value("player-filter-frames-false", "").toString(), sblock.setup.plFilter_frames_false);
                }

                long iTmp;
                iTmp = blockset.value("default-invisible", "-1").toInt();
                sblock.setup.default_invisible = (iTmp>=0);
                sblock.setup.default_invisible_value = (iTmp>=0)?bool(iTmp):false;

                iTmp = blockset.value("default-slippery", "-1").toInt();
                sblock.setup.default_slippery = (iTmp>=0);
                sblock.setup.default_slippery_value = (iTmp>=0)?bool(iTmp):false;

                iTmp = blockset.value("default-npc-content", "-1").toInt();
                sblock.setup.default_content = (iTmp>=0);
                sblock.setup.default_content_value = (iTmp>=0) ? (iTmp<1000? iTmp*-1 : iTmp-1000) : 0;

                sblock.setup.id = i;

                //Add to Index
                lvl_block_indexes.storeElement(signed(sblock.setup.id), sblock);

            skipBLOCK:
            blockset.endGroup();
            #endif

            if(!loadLevelBlock(sblock, QString("block-%1").arg(i), nullptr, "", &blockset))
                return false;

            sblock.setup.id = i;
            lvl_block_indexes.storeElement(sblock.setup.id, sblock);

          if( blockset.status()!=QSettings::NoError)
          {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i), QtCriticalMsg);

            PGE_MsgBox::error(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i));
            break;
          }
       }

       if(uint(lvl_block_indexes.stored()) < block_total)
       {
           addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(lvl_block_indexes.stored()), QtWarningMsg);
       }

       return true;
}



