/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"


obj_block::obj_block()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = NULL;
}


void obj_block::copyTo(obj_block &block)
{
    /* for internal usage */
    block.isValid         = isValid;
    block.animator_id     = animator_id;
    block.cur_image       = cur_image;
    if(cur_image==NULL)
        block.cur_image   = &image;
    block.frame_h         = frame_h;
    /* for internal usage */

    block.id=id;
    block.image_n=image_n;
    block.mask_n=mask_n;
    block.name=name;
    //    grid=32				; 32 | 16 Default="32"
    block.grid=grid;
    block.group=group;
    block.category=category;
    block.sizable=sizable;
    block.danger=danger;
    block.collision=collision;
    block.slopeslide=slopeslide;
    block.phys_shape=phys_shape;
    block.lava=lava;
    block.destroyable=destroyable;
    block.destroyable_by_bomb=destroyable_by_bomb;
    block.destroyable_by_fireball=destroyable_by_fireball;
    block.spawn=spawn; //split string by "-" in != "0"
    block.spawn_obj=spawn_obj; // 1 - NPC, 2 - block, 3 - BGO
    block.spawn_obj_id=spawn_obj_id;
    block.effect=effect;
    block.bounce=bounce;
    block.hitable=hitable;
    block.transfororm_on_hit_into=transfororm_on_hit_into;
    block.algorithm=algorithm;

    block.view=view;
    block.animated=animated;
    block.animation_rev=animation_rev; //Reverse animation
    block.animation_bid=animation_bid; //Bidirectional animation
    block.frames=frames;
    block.framespeed=framespeed;

    block.frame_h=frame_h; //Hegth of the frame. Calculating automatically

    block.display_frame=display_frame;

    //Editor defaults
    block.default_slippery=default_slippery; //Slippery flag
    block.default_slippery_value=default_slippery_value;

    block.default_invisible=default_invisible; //Invisible flag
    block.default_invisible_value=default_invisible_value;

    block.default_content=default_content; //Content value
    block.default_content_value=default_content_value;

}


//long dataconfigs::getBlockI(unsigned long itemID)
//{
//    long j;
//    bool found=false;

//    if(itemID < (unsigned int)index_blocks.size())
//    {
//        j = index_blocks[itemID].i;

//        if(j < main_block.size())
//        {
//            if( main_block[j].id == itemID)
//                found=true;
//        }
//    }

//    if(!found)
//    {
//        for(j=0; j < main_block.size(); j++)
//        {
//            if(main_block[j].id==itemID)
//            {
//                found=true;
//                break;
//            }
//        }
//    }

//    if(!found) j=-1;
//    return j;
//}


static QString Temp01="";

void dataconfigs::loadLevelBlocks()
{
    unsigned int i;

    obj_block sblock;
    unsigned long block_total=0;


    QString block_ini = config_dir + "lvl_blocks.ini";

    if(!QFile::exists(block_ini))
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: file does not exist"), PGE_LogLevel::Critical);
          return;
    }

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    main_block.clear();   //Clear old
    //index_blocks.clear();

    blockset.beginGroup("blocks-main");
        block_total = blockset.value("total", "0").toInt();
        total_data +=block_total;
    blockset.endGroup();

    emit progressPartNumber(2);
    emit progressMax(block_total);
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Blocks..."));

    ConfStatus::total_blocks = block_total;

    //creation of empty indexes of arrayElements
//    blocksIndexes blockIndex;
//    for(i=0;i<=block_total; i++)
//    {
//        blockIndex.i=i;
//        blockIndex.type=0;
//        index_blocks.push_back(blockIndex);
//    }

    main_block.allocateSlots(block_total);

    if(ConfStatus::total_blocks==0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

        for(i=1; i<=block_total; i++)
        {
            emit progressValue(i);
            QString errStr;

            blockset.beginGroup( QString("block-%1").arg(i) );

                sblock.name =       blockset.value("name", QString("block %1").arg(i) ).toString();

                if(sblock.name=="")
                {
                    addError(QString("BLOCK-%1 Item name isn't defined").arg(i));
                    goto skipBLOCK;
                }

                sblock.group =      blockset.value("group", "_NoGroup").toString();
                sblock.category =   blockset.value("category", "_Other").toString();
                sblock.grid =       blockset.value("grid", default_grid).toInt();

                sblock.image_n =           blockset.value("image", "").toString();
                /***************Load image*******************/
                GraphicsHelps::loadMaskedImage(blockPath,
                   sblock.image_n, sblock.mask_n,
                   sblock.image,
                   errStr);

                if(!errStr.isEmpty())
                {
                    addError(QString("BLOCK-%1 %2").arg(i).arg(errStr));
                    goto skipBLOCK;
                }
                /***************Load image*end***************/

                sblock.sizable =                blockset.value("sizable", "0").toBool();
                sblock.danger =                 blockset.value("danger", "0").toInt();
                sblock.collision =              blockset.value("collision", "1").toInt();
                sblock.slopeslide =             blockset.value("slope-slide", "0").toBool();
                sblock.phys_shape =             blockset.value("fixture-type", "0").toInt();// Leaved for compatibility
                sblock.phys_shape =             blockset.value("shape-type", sblock.phys_shape).toInt();//new value-name
                sblock.lava =                   blockset.value("lava", "0").toBool();
                sblock.destroyable =            blockset.value("destroyable", "0").toBool();
                sblock.destroyable_by_bomb =    blockset.value("destroyable-by-bomb", "0").toBool();
                sblock.destroyable_by_fireball= blockset.value("destroyable-by-fireball", "0").toBool();

                Temp01 =     blockset.value("spawn-on-destroy", "0").toString();
                if(Temp01!="0")
                {
                    tmp =  Temp01.split("-", QString::SkipEmptyParts);
                    if(tmp.size()==2)
                    {
                        if(tmp[0]=="npc")
                            sblock.spawn_obj = 1;
                        else
                        if(tmp[0]=="block")
                             sblock.spawn_obj = 2;
                        else
                        if(tmp[0]=="bgo")
                             sblock.spawn_obj = 3;
                        // 1 - NPC, 2 - block, 3 - BGO
                        sblock.spawn_obj_id = tmp[1].toInt();
                    }
                    else // if syntax error in config
                    {
                        sblock.spawn = false;
                        sblock.spawn_obj = 0;
                        sblock.spawn_obj_id = 0;
                    }
                }
                else
                {
                    sblock.spawn = false;
                    sblock.spawn_obj = 0;
                    sblock.spawn_obj_id = 0;
                }

                sblock.effect=                  blockset.value("destroy-effect", "1").toInt();

                sblock.bounce =                 blockset.value("bounce", "0").toBool();
                sblock.hitable =                blockset.value("hitable", "0").toBool();
                sblock.transfororm_on_hit_into= blockset.value("transform-onhit-into", "2").toInt();
                sblock.algorithm =              blockset.value("algorithm", "2").toInt();
                sblock.view =             (int)(blockset.value("view", "background").toString()=="foreground");
                sblock.animated =               blockset.value("animated", "0").toBool();
                sblock.animation_rev =          blockset.value("animation-reverse", "0").toBool(); //Reverse animation
                sblock.animation_bid =          blockset.value("animation-bidirectional", "0").toBool(); //Bidirectional animation
                sblock.frames =                 blockset.value("frames", "1").toInt();
                sblock.framespeed =             blockset.value("framespeed", "125").toInt();

                sblock.frame_h =               (sblock.animated?
                                                    qRound(
                                                        qreal(sblock.image.height())
                                                        /sblock.frames)
                                                  : sblock.image.height());

                sblock.display_frame =          blockset.value("display-frame", "0").toInt();

                long iTmp;
                iTmp =  blockset.value("default-invisible", "-1").toInt();
                    sblock.default_invisible = (iTmp>=0);
                    sblock.default_invisible_value = (iTmp>=0)?(bool)iTmp:false;

                iTmp = blockset.value("default-slippery", "-1").toInt();
                    sblock.default_slippery = (iTmp>=0);
                    sblock.default_slippery_value = (iTmp>=0)?(bool)iTmp:false;

                iTmp = blockset.value("default-npc-content", "-1").toInt();
                    sblock.default_content = (iTmp>=0);
                    sblock.default_content_value = (iTmp>=0) ? (iTmp<1000? iTmp*-1 : iTmp-1000) : 0;

                sblock.isValid = true;
                sblock.id = i;
                main_block.storeElement(i, sblock);

                /************Add to Index***************
                if(i < (unsigned int)index_blocks.size())
                    index_blocks[i].i = i-1;
                ************Add to Index***************/

            skipBLOCK:
            blockset.endGroup();

          if( blockset.status()!=QSettings::NoError)
          {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i), PGE_LogLevel::Critical);
            break;
          }
       }

       if((unsigned int)main_block.stored()<block_total)
       {
           addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(main_block.size()), PGE_LogLevel::Warning);
       }

}
