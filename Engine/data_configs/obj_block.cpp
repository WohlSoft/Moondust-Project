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

#include "config_manager.h"
#include <QMessageBox>

/*****Level blocks************/
QVector<obj_block >     ConfigManager::lvl_blocks;
QMap<long, obj_block>   ConfigManager::lvl_block_indexes;
CustomDirManager ConfigManager::Dir_Blocks;
QVector<SimpleAnimator > ConfigManager::Animator_Blocks;
/*****Level blocks************/

namespace loadLevelBlocks_fnc
{
    static QString Temp01="";
}

bool ConfigManager::loadLevelBlocks()
{
    using namespace loadLevelBlocks_fnc;

    unsigned int i;

    obj_block sblock;
    unsigned long block_total=0;


    QString block_ini = config_dir + "lvl_blocks.ini";

    if(!QFile::exists(block_ini))
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: file does not exist"), QtCriticalMsg);
        QMessageBox::critical(NULL, "Config error",
                              QString("ERROR LOADING lvl_blocks.ini: file does not exist"),
                              QMessageBox::Ok);
        return false;
    }

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    lvl_blocks.clear();   //Clear old
    lvl_block_indexes.clear();

    blockset.beginGroup("blocks-main");
        block_total = blockset.value("total", "0").toInt();
        total_data +=block_total;
    blockset.endGroup();



    if(block_total==0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), QtCriticalMsg);
        QMessageBox::critical(NULL, "Config error",
                              QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"),
                              QMessageBox::Ok);
        return false;
    }


        for(i=1; i<=block_total; i++)
        {

            sblock.isInit=false;
            sblock.image = NULL;
            sblock.textureArrayId = 0;
            sblock.animator_ID = 0;


                blockset.beginGroup( QString("block-%1").arg(i) );

                sblock.name = blockset.value("name", QString("block %1").arg(i) ).toString();

                if(sblock.name=="")
                {
                    addError(QString("BLOCK-%1 Item name isn't defined").arg(i));
                    goto skipBLOCK;
                }

                sblock.group = blockset.value("group", "_NoGroup").toString();
                sblock.category = blockset.value("category", "_Other").toString();
                //sblock.grid = blockset.value("grid", default_grid).toInt();
                imgFile = blockset.value("image", "").toString();

                sblock.image_n = imgFile;
                if( (imgFile!="") )
                {
                    tmp = imgFile.split(".", QString::SkipEmptyParts);
                    if(tmp.size()==2)
                        imgFileM = tmp[0] + "m." + tmp[1];
                    else
                        imgFileM = "";
                    sblock.mask_n = imgFileM;
                }
                else
                {
                    addError(QString("BLOCK-%1 Image filename isn't defined").arg(i));
                    goto skipBLOCK;
                }

                sblock.sizable = blockset.value("sizable", "0").toBool();
                sblock.danger = blockset.value("danger", "0").toInt();
                sblock.collision = blockset.value("collision", "1").toInt();
                sblock.slopeslide = blockset.value("slope-slide", "0").toBool();
                sblock.phys_shape = blockset.value("fixture-type", "0").toInt();// Leaved for compatibility
                sblock.phys_shape = blockset.value("shape-type", sblock.phys_shape).toInt();//new value-name
                sblock.lava = blockset.value("lava", "0").toBool();
                sblock.destroyable = blockset.value("destroyable", "0").toBool();
                sblock.destroyable_by_bomb = blockset.value("destroyable-by-bomb", "0").toBool();
                sblock.destroyable_by_fireball = blockset.value("destroyable-by-fireball", "0").toBool();

                Temp01 = blockset.value("spawn-on-destroy", "0").toString();
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

                sblock.effect= blockset.value("destroy-effect", "1").toInt();

                sblock.bounce = blockset.value("bounce", "0").toBool();
                sblock.hitable = blockset.value("hitable", "0").toBool();
                sblock.transfororm_on_hit_into = blockset.value("transform-onhit-into", "2").toInt();
                sblock.algorithm= blockset.value("algorithm", "2").toInt();
                sblock.view = (int)(blockset.value("view", "background").toString()=="foreground");
                sblock.animated = blockset.value("animated", "0").toBool();
                sblock.animation_rev = blockset.value("animation-reverse", "0").toBool(); //Reverse animation
                sblock.animation_bid = blockset.value("animation-bidirectional", "0").toBool(); //Bidirectional animation
                sblock.frames = blockset.value("frames", "1").toInt();
                sblock.framespeed = blockset.value("framespeed", "125").toInt();

                //sblock.frame_h = (sblock.animated? qRound(qreal(sblock.image.height())/sblock.frames) : sblock.image.height());

                sblock.display_frame = blockset.value("display-frame", "0").toInt();

                long iTmp;
                iTmp = blockset.value("default-invisible", "-1").toInt();
                sblock.default_invisible = (iTmp>=0);
                sblock.default_invisible_value = (iTmp>=0)?(bool)iTmp:false;

                iTmp = blockset.value("default-slippery", "-1").toInt();
                sblock.default_slippery = (iTmp>=0);
                sblock.default_slippery_value = (iTmp>=0)?(bool)iTmp:false;

                iTmp = blockset.value("default-npc-content", "-1").toInt();
                sblock.default_content = (iTmp>=0);
                sblock.default_content_value = (iTmp>=0) ? (iTmp<1000? iTmp*-1 : iTmp-1000) : 0;

                sblock.id = i;
                lvl_blocks.push_back(sblock);

                //Add to Index
                lvl_block_indexes[lvl_blocks.last().id] = lvl_blocks.last();

            skipBLOCK:
            blockset.endGroup();

          if( blockset.status()!=QSettings::NoError)
          {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i), QtCriticalMsg);
            QMessageBox::critical(NULL, "Config error",
                                  QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i),
                                  QMessageBox::Ok);
            break;
          }
       }

       if((unsigned int)lvl_blocks.size()<block_total)
       {
           addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(lvl_blocks.size()), QtWarningMsg);
       }

       return true;
}



