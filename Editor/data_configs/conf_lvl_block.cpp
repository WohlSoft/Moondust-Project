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
#include "../common_features/graphics_funcs.h"

static QString Temp01="";

void dataconfigs::loadLevelBlocks(QProgressDialog *prgs)
{
    unsigned int i;

    obj_block sblock;
    unsigned long block_total=0;


    QString block_ini = config_dir + "lvl_blocks.ini";

    if(!QFile::exists(block_ini))
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: file does not exist"), QtCriticalMsg);
          return;
    }

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    main_block.clear();   //Clear old

    blockset.beginGroup("blocks-main");
        block_total = blockset.value("total", "0").toInt();
        total_data +=block_total;
    blockset.endGroup();

    if(prgs) prgs->setMaximum(block_total);
    if(prgs) prgs->setLabelText(QApplication::tr("Loading Blocks..."));

    ConfStatus::total_blocks = block_total;

    //creation of empty indexes of arrayElements
    blocksIndexes blockIndex;
    for(i=0;i<=block_total; i++)
    {
        blockIndex.i=i;
        blockIndex.type=0;
        index_blocks.push_back(blockIndex);
    }


    if(ConfStatus::total_blocks==0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), QtCriticalMsg);
        return;
    }


        for(i=1; i<=block_total; i++)
        {
            qApp->processEvents();
            if(prgs)
            {
                if(!prgs->wasCanceled()) prgs->setValue(i);
            }

            blockset.beginGroup( QString("block-%1").arg(i) );

                sblock.name = blockset.value("name", QString("block %1").arg(i) ).toString();

                if(sblock.name=="")
                {
                    addError(QString("BLOCK-%1 Item name isn't defined").arg(i));
                    goto skipBLOCK;
                }

                sblock.group = blockset.value("group", "_NoGroup").toString();
                sblock.category = blockset.value("category", "_Other").toString();
                sblock.grid = blockset.value("grid", default_grid).toInt();
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
                    mask = QPixmap();
                    if(tmp.size()==2) mask = QPixmap(blockPath + imgFileM);
                    sblock.mask = mask;
                    sblock.image = GraphicsHelps::setAlphaMask(QPixmap(blockPath + imgFile), sblock.mask);
                    if(sblock.image.isNull())
                    {
                        addError(QString("LoadConfig -> BLOCK-%1 Brocken image file").arg(i));
                        goto skipBLOCK;
                    }
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
                sblock.fixture = blockset.value("fixture-type", "0").toInt();
                sblock.lava = blockset.value("lava", "0").toBool();
                sblock.destruct = blockset.value("destruct", "0").toBool();
                sblock.dest_bomb = blockset.value("destruct-bomb", "0").toBool();
                sblock.dest_fire = blockset.value("destruct-fireball", "0").toBool();

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

                sblock.effect= blockset.value("destruct-effect", "1").toInt();

                sblock.bounce = blockset.value("bounce", "0").toBool();
                sblock.hitable = blockset.value("hitable", "0").toBool();
                sblock.onhit = blockset.value("hitable", "0").toBool();
                sblock.onhit_block= blockset.value("onhit-block", "2").toInt();
                sblock.algorithm= blockset.value("algorithm", "2").toInt();
                sblock.view = (int)(blockset.value("view", "background").toString()=="foreground");
                sblock.animated = blockset.value("animated", "0").toBool();
                sblock.animation_rev = blockset.value("animation-reverse", "0").toBool(); //Reverse animation
                sblock.animation_bid = blockset.value("animation-bidirectional", "0").toBool(); //Bidirectional animation
                sblock.frames = blockset.value("frames", "1").toInt();
                sblock.framespeed = blockset.value("framespeed", "125").toInt();
                sblock.id = i;
                main_block.push_back(sblock);

                //Add to Index
                if(i < (unsigned int)index_blocks.size())
                    index_blocks[i].i = i-1;

            skipBLOCK:
            blockset.endGroup();

          if( blockset.status()!=QSettings::NoError)
          {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i), QtCriticalMsg);
            break;
          }
       }

       if((unsigned int)main_block.size()<block_total)
       {
           addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(main_block.size()), QtWarningMsg);
       }

}
