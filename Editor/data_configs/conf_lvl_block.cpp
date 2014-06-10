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

#include "data_configs.h"

#include "../main_window/global_settings.h"

void dataconfigs::loadLevelBlocks()
{
    unsigned int i;

    obj_block sblock;
    unsigned long block_total=0;


    QString block_ini = config_dir + "lvl_blocks.ini";

    if(!QFile::exists(block_ini))
    {
        WriteToLog(QtCriticalMsg, QString("ERROR LOADING OF lvl_blocks.ini: file not exist"));
          return;
    }

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    main_block.clear();   //Clear old

    blockset.beginGroup("blocks-main");
        block_total = blockset.value("total", "0").toInt();
        total_data +=block_total;
    blockset.endGroup();

    ConfStatus::total_blocks = block_total;

    //creation of empty indexes of arrayElements
    blocksIndexes blockIndex;
    for(i=0;i<=block_total; i++)
    {
        blockIndex.i=i;
        blockIndex.type=0;
        index_blocks.push_back(blockIndex);
    }


        for(i=1; i<=block_total; i++)
        {
            blockset.beginGroup( QString("block-%1").arg(i) );

                sblock.name = blockset.value("name", QString("block %1").arg(i) ).toString();
                sblock.type = blockset.value("type", "Other").toString();
                sblock.grid = blockset.value("grid", "32").toInt();
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
                    if(tmp.size()==2) mask = QBitmap(blockPath + imgFileM);
                    sblock.mask = mask;
                    sblock.image = QPixmap(blockPath + imgFile);
                    if(tmp.size()==2) sblock.image.setMask(mask);
                }
                else
                {
                    sblock.image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
                    sblock.mask_n = "";
                }

                sblock.sizable = blockset.value("sizable", "0").toBool();
                sblock.danger = blockset.value("danger", "0").toInt();
                sblock.collision = blockset.value("collision", "1").toInt();
                sblock.slopeslide = blockset.value("slope-slide", "0").toBool();
                sblock.fixture = blockset.value("fixture-type", "0").toInt();
                sblock.lava = blockset.value("lava", "0").toBool();
                sblock.destruct = blockset.value("destruct", "0").toBool();
                sblock.dest_bomb = blockset.value("destruct-bomb", "0").toBool();
                sblock.dest_fire = blockset.value("destruct-fireball", "0").toBool();

                imgFile = blockset.value("spawn-on-destroy", "0").toString();
                if(imgFile!="0")
                {
                    tmp =  imgFile.split("-", QString::SkipEmptyParts);
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

                sblock.effect= blockset.value("destruct-effect", "1").toInt();

                sblock.bounce = blockset.value("bounce", "0").toBool();
                sblock.hitable = blockset.value("hitable", "0").toBool();
                sblock.onhit = blockset.value("hitable", "0").toBool();
                sblock.onhit_block= blockset.value("onhit-block", "2").toInt();
                sblock.algorithm= blockset.value("algorithm", "2").toInt();
                sblock.view = (int)(blockset.value("view", "background").toString()=="foreground");
                sblock.animated = (blockset.value("animated", "0").toString()=="1");
                sblock.frames = blockset.value("frames", "1").toInt();
                sblock.framespeed = blockset.value("framespeed", "125").toInt();
                sblock.id = i;
                main_block.push_back(sblock);

                //Add to Index
                if(i < (unsigned int)index_blocks.size())
                    index_blocks[i].i = i-1;

            blockset.endGroup();

          if( blockset.status()!=QSettings::NoError)
          {
            WriteToLog(QtCriticalMsg, QString("ERROR LOADING OF lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i));
            break;
          }
       }

       if((unsigned int)main_block.size()<block_total)
       {
           WriteToLog(QtWarningMsg, QString("Not all blocks loaded: total:%1, loaded: %2)").arg(block_total).arg(main_block.size()));
       }

}
