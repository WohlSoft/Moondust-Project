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

#include <QMessageBox>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionCDATA_clear_unused_triggered()
{
    //QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
    if(activeChildWindow()==1)
    {
        QStringList filesForRemove;
        LevelEdit * box = activeLvlEditWin();
        if(!box) return;
        if(!box->sceneCreated) return;
        if(box->isUntitled)
        {
            QMessageBox::warning(this, tr("File not saved"),
                tr("File doesn't saved on disk."), QMessageBox::Ok);
            return;
        }

        LvlScene * s = box->scene;
        QString levelDirectory(box->LvlData.path);

        QString levelCustomDirectory(box->LvlData.path+"/"+box->LvlData.filename);
        QDir levelCustomDirectory_dir(levelCustomDirectory);

        filesForRemove.append( levelCustomDirectory_dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot) );

        qDebug() << levelCustomDirectory << "Total custom files" << filesForRemove.size();


        QRegExp reg;
        reg.setPatternSyntax(QRegExp::Wildcard);
        reg.setCaseSensitivity(Qt::CaseInsensitive);

        QList<unsigned long > npcContainers;
        npcContainers << 91;
        npcContainers << 96;
        npcContainers << 283;
        npcContainers << 284;

        //SMBX64 entries
        QStringList whiteList;
        whiteList << "lunadll.txt"; //LunaDLL stuff
        whiteList << "lunadll.lua";

        whiteList << "*.tileset.ini"; //PGE Tilesets

        whiteList << "effect-*.gif"; //All SMBX64 effects

        whiteList << "mario-*.gif"; //All SMBX64 playable characters
        whiteList << "peach-*.gif";
        whiteList << "toad-*.gif";
        whiteList << "luigi-*.gif";
        whiteList << "link-*.gif";
        whiteList << "yoshit-*.gif";
        whiteList << "yoshib-*.gif";


        QString npcFile = "npc-%1.gif|npc-%1.txt|npc-%1m.txt";

        typedef QPair<unsigned long, QStringList > bossNPC;
        QList<bossNPC > bossNPCs;

        bossNPCs << bossNPC(15, npcFile.arg(16).split('|') ); //boom-boom

        bossNPCs << bossNPC(39, npcFile.arg(41).split('|') ); //Birdo
        bossNPCs << bossNPC(39, npcFile.arg(40).split('|') );

        //giant coopa
        bossNPCs << bossNPC(72, npcFile.arg(73).split('|') );

        //green parakoopa 1
        bossNPCs << bossNPC(76, npcFile.arg(4).split('|') );
        bossNPCs << bossNPC(76, npcFile.arg(5).split('|') );
        bossNPCs << bossNPC(4, npcFile.arg(5).split('|') );

        //green parakoopa 2
        bossNPCs << bossNPC(176, npcFile.arg(173).split('|') );
        bossNPCs << bossNPC(176, npcFile.arg(172).split('|') );
        bossNPCs << bossNPC(173, npcFile.arg(172).split('|') );

        //red parakoopa 1
        bossNPCs << bossNPC(161, npcFile.arg(6).split('|') );
        bossNPCs << bossNPC(161, npcFile.arg(7).split('|') );
        bossNPCs << bossNPC(6, npcFile.arg(7).split('|') );

        //red parakoopa 2
        bossNPCs << bossNPC(177, npcFile.arg(175).split('|') );
        bossNPCs << bossNPC(177, npcFile.arg(174).split('|') );
        bossNPCs << bossNPC(175, npcFile.arg(174).split('|') );

        //Blue parakoopa 3
        bossNPCs << bossNPC(111, npcFile.arg(115).split('|') );
        bossNPCs << bossNPC(111, npcFile.arg(55).split('|') );
        bossNPCs << bossNPC(111, npcFile.arg(119).split('|') );

        bossNPCs << bossNPC(123, npcFile.arg(111).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(115).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(55).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(119).split('|') );

        //Red parakoopa 3
        bossNPCs << bossNPC(110, npcFile.arg(118).split('|') );
        bossNPCs << bossNPC(110, npcFile.arg(114).split('|') );

        bossNPCs << bossNPC(122, npcFile.arg(110).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(118).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(114).split('|') );

        //Green parakoopa 3
        bossNPCs << bossNPC(109, npcFile.arg(113).split('|') );//shell
        bossNPCs << bossNPC(109, npcFile.arg(117).split('|') );//beach

        bossNPCs << bossNPC(121, npcFile.arg(109).split('|') );//koopa
        bossNPCs << bossNPC(121, npcFile.arg(113).split('|') );//shell
        bossNPCs << bossNPC(121, npcFile.arg(117).split('|') );//beach

        //Yellow parakoopa 3
        bossNPCs << bossNPC(112, npcFile.arg(116).split('|') );//shell
        bossNPCs << bossNPC(112, npcFile.arg(120).split('|') );//beach

        bossNPCs << bossNPC(124, npcFile.arg(112).split('|') );//koopa
        bossNPCs << bossNPC(124, npcFile.arg(116).split('|') );//shell
        bossNPCs << bossNPC(124, npcFile.arg(120).split('|') );//beach

        bossNPCs << bossNPC(124, npcFile.arg(194).split('|') );
        bossNPCs << bossNPC(112, npcFile.arg(194).split('|') );
        bossNPCs << bossNPC(116, npcFile.arg(194).split('|') );

        //rainbow shell
        bossNPCs << bossNPC(194, npcFile.arg(195).split('|') );

        //Parakoopas
        bossNPCs << bossNPC(243, npcFile.arg(242).split('|') );
        bossNPCs << bossNPC(244, npcFile.arg(1).split('|') );
        bossNPCs << bossNPC(3, npcFile.arg(2).split('|') );

        //random vegetable
        bossNPCs << bossNPC(147, npcFile.arg(146).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(140).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(142).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(92).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(139).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(141).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(143).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(144).split('|') );
        bossNPCs << bossNPC(147, npcFile.arg(145).split('|') );

        //random powerup
        bossNPCs << bossNPC(287, npcFile.arg(9).split('|') );
        bossNPCs << bossNPC(287, npcFile.arg(184).split('|') );
        bossNPCs << bossNPC(287, npcFile.arg(185).split('|') );

        bossNPCs << bossNPC(287, npcFile.arg(14).split('|') );
        bossNPCs << bossNPC(287, npcFile.arg(182).split('|') );
        bossNPCs << bossNPC(287, npcFile.arg(183).split('|') );

        bossNPCs << bossNPC(287, npcFile.arg(34).split('|') );
        bossNPCs << bossNPC(287, npcFile.arg(170).split('|') );

        bossNPCs << bossNPC(287, npcFile.arg(264).split('|') );
        bossNPCs << bossNPC(287, npcFile.arg(277).split('|') );

        //bill blasters
        bossNPCs << bossNPC(21, npcFile.arg(17).split('|') );
        bossNPCs << bossNPC(22, npcFile.arg(17).split('|') );

        //Toothy
        bossNPCs << bossNPC(49, npcFile.arg(50).split('|') );

        //Small Lakitu
        bossNPCs << bossNPC(47, npcFile.arg(48).split('|') );

        //Spiny
        bossNPCs << bossNPC(286, npcFile.arg(285).split('|') );

        //Rex
        bossNPCs << bossNPC(162, npcFile.arg(163).split('|') );

        //Magic potion
        bossNPCs << bossNPC(288, npcFile.arg(289).split('|') );

        //vined heads
        bossNPCs << bossNPC(226, npcFile.arg(213).split('|') );
        bossNPCs << bossNPC(225, npcFile.arg(214).split('|') );
        bossNPCs << bossNPC(227, npcFile.arg(224).split('|') );

        //Add into white SMBX64 dependencied of exists ID
        for(int q=0;q<box->LvlData.npc.size();q++)
        {
            bool isConainer=false;

            foreach(unsigned long x, npcContainers)
            {
                if(box->LvlData.npc[q].id == x)
                {
                    qDebug() << "container!" <<  box->LvlData.npc[q].special_data;
                    isConainer=true; break;
                }
            }

            unsigned long npcID = (isConainer ? box->LvlData.npc[q].special_data : box->LvlData.npc[q].id);
            for(int r=0;r<bossNPCs.size();r++)
            {
                if( npcID == bossNPCs[r].first)
                {
                    whiteList.append(bossNPCs[r].second);
                    bossNPCs.removeAt(r);
                    r=-1;//reset loot to 0
                }
            }

            if(bossNPCs.isEmpty()) break;
        }

        //into included into blocks
        for(int q=0;q<box->LvlData.blocks.size();q++)
        {
            for(int r=0;r<bossNPCs.size();r++)
                if( box->LvlData.blocks[q].npc_id == (signed)bossNPCs[r].first)
                {
                    whiteList << bossNPCs[r].second;
                    bossNPCs.removeAt(r);
                    r=-1;//reset loot to 0
                }
            if(bossNPCs.isEmpty()) break;
        }

        //SMBX64 system items
        whiteList << npcFile.arg(10).split('|'); //coin

        whiteList << npcFile.arg(40).split('|'); //bullets
        whiteList << npcFile.arg(85).split('|');
        whiteList << npcFile.arg(87).split('|');
        whiteList << npcFile.arg(159).split('|');
        whiteList << npcFile.arg(13).split('|');
        whiteList << npcFile.arg(30).split('|');
        whiteList << npcFile.arg(171).split('|');
        whiteList << npcFile.arg(263).split('|');
        whiteList << npcFile.arg(237).split('|');
        whiteList << npcFile.arg(265).split('|');
        whiteList << npcFile.arg(269).split('|');
        whiteList << npcFile.arg(368).split('|');
        whiteList << npcFile.arg(266).split('|');
        whiteList << npcFile.arg(282).split('|');
        whiteList << npcFile.arg(281).split('|');
        whiteList << npcFile.arg(191).split('|');
        whiteList << npcFile.arg(210).split('|');
        whiteList << npcFile.arg(133).split('|');
        whiteList << npcFile.arg(292).split('|');
        whiteList << npcFile.arg(50).split('|');
        whiteList << npcFile.arg(246).split('|');
        whiteList << npcFile.arg(276).split('|');
        whiteList << npcFile.arg(202).split('|');
        whiteList << npcFile.arg(108).split('|');

        //shoes
        whiteList << npcFile.arg(35).split('|');
        whiteList << npcFile.arg(191).split('|');
        whiteList << npcFile.arg(193).split('|');

        //Yoshis
        whiteList << npcFile.arg(95).split('|');
        whiteList << npcFile.arg(98).split('|');
        whiteList << npcFile.arg(99).split('|');
        whiteList << npcFile.arg(100).split('|');
        whiteList << npcFile.arg(148).split('|');
        whiteList << npcFile.arg(149).split('|');
        whiteList << npcFile.arg(150).split('|');
        whiteList << npcFile.arg(228).split('|');


        whiteList << "background-98.gif"; //door lock
        whiteList << "background-98m.gif";
        whiteList << "background-160.gif"; //star lock
        whiteList << "background-160m.gif";


        for(int i=0; i < filesForRemove.size(); i++)
        {
            bool removed=false;
            if(QFileInfo(levelCustomDirectory+"/"+filesForRemove[i]).isDir())
                {filesForRemove.removeAt(i); i--; continue;}
            else
            foreach(QString f, whiteList)
            {
                reg.setPattern(f);
                if(reg.exactMatch(filesForRemove[i]))
                    {filesForRemove.removeAt(i); i--; removed=true; break;}
            }

            if(!removed)
                filesForRemove[i] = levelCustomDirectory+"/"+filesForRemove[i];
        }



        //Get custom data list
        foreach(LevelSection y, box->LvlData.sections)
        {
            if(y.music_file.isEmpty()) continue;

            bool musicFileRemoved = !QFileInfo(levelDirectory+"/"+y.music_file).exists();

            for(int i=0; i < filesForRemove.size(); i++)
            {
                if(musicFileRemoved) break;

                if( filesForRemove[i].toLower()==(levelDirectory+"/"+y.music_file).toLower() )
                {filesForRemove.removeAt(i); i--; musicFileRemoved=true;}
            }
        }

        for(QHash<int, obj_BG>::iterator bgIT=s->uBGs.begin(); bgIT!=s->uBGs.end(); bgIT++)
        {
            obj_BG &x=*bgIT;
            foreach(LevelSection y, box->LvlData.sections)
            {
                if(y.background == x.id)
                {
                    QString image;
                    if(configs.main_bg.contains(x.id))
                    {
                        image = levelCustomDirectory + "/" + configs.main_bg[x.id].image_n;
                    }
                    bool imageRemoved = !QFileInfo(image).exists();

                    for(int i=0; i < filesForRemove.size(); i++)
                    {
                        if(imageRemoved) break;

                        if(filesForRemove[i].toLower()==image.toLower())
                        {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                    }
                    break;
                }
            }
        }

        foreach(UserBlocks x, s->uBlocks)
        {
            foreach(LevelBlock y, box->LvlData.blocks)
            {
                if(y.id == x.id)
                {

                    QString image;
                    QString mask;
                    long transformTo=0;
                    QString transformTo_image;
                    QString transformTo_mask;

                    long I = configs.getBlockI(x.id);
                    if(I>=0)
                    {
                        image = levelCustomDirectory + "/" + configs.main_block[I].image_n;
                        mask = levelCustomDirectory + "/" + configs.main_block[I].mask_n;
                        transformTo = configs.main_block[I].transfororm_on_hit_into;
                    }

                    if(transformTo>0)
                    {
                        I = configs.getBlockI(transformTo);
                        if(I>=0)
                        {
                            transformTo_image = levelCustomDirectory + "/" + configs.main_block[I].image_n;
                            transformTo_mask = levelCustomDirectory + "/" + configs.main_block[I].mask_n;
                        }
                    }

                    bool imageRemoved = !QFileInfo(image).exists();
                    bool maskRemoved = !QFileInfo(mask).exists();
                    bool transformTo_img = !QFileInfo(transformTo_image).exists();;
                    bool transformTo_msk = !QFileInfo(transformTo_mask).exists();;

                    for(int i=0; i < filesForRemove.size(); i++)
                    {
                        if(imageRemoved && maskRemoved &&
                           transformTo_img && transformTo_msk) break;

                        if(filesForRemove[i].toLower()==image.toLower())
                        {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                        else
                        if(filesForRemove[i].toLower()==mask.toLower())
                        {filesForRemove.removeAt(i); i--;maskRemoved=true;}
                        else
                        if(filesForRemove[i].toLower()==transformTo_image.toLower())
                        {filesForRemove.removeAt(i); i--;transformTo_img=true;}
                        else
                        if(filesForRemove[i].toLower()==transformTo_mask.toLower())
                        {filesForRemove.removeAt(i); i--;transformTo_msk=true;}
                    }
                    break;
                }
            }

        }

        foreach(UserBGOs x, s->uBGOs)
        {
            foreach(LevelBGO y, box->LvlData.bgo)
            {
                if(y.id == x.id)
                {

                    QString image;
                    QString mask;
                    long I = configs.getBgoI(x.id);
                    if(I>=0)
                    {
                        image = levelCustomDirectory + "/" + configs.main_bgo[I].image_n;
                        mask = levelCustomDirectory + "/" + configs.main_bgo[I].mask_n;
                    }
                    bool imageRemoved = !QFileInfo(image).exists();
                    bool maskRemoved = !QFileInfo(mask).exists();
                    for(int i=0; i < filesForRemove.size(); i++)
                    {
                        if(imageRemoved && maskRemoved) break;

                        if(filesForRemove[i].toLower()==image.toLower())
                        {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                        else
                        if(filesForRemove[i].toLower()==mask.toLower())
                        {filesForRemove.removeAt(i); i--;maskRemoved=true;}
                    }
                    break;
                }
            }

        }



        foreach(UserNPCs x, s->uNPCs)
        {
            QList<unsigned long > usedIDs;

            //Used in level
            foreach(LevelNPC y, box->LvlData.npc)
            {
                if(y.id==x.id)
                {
                    usedIDs.push_back(x.id);
                    break;
                }
            }

            //Used in NPC containers
            foreach(LevelNPC y, box->LvlData.npc)
            {
                bool found=false;
                foreach(unsigned long z, npcContainers)
                {
                    if(y.id == z)
                    {
                        if(y.special_data == (signed)x.id)
                        {
                            usedIDs.push_back(x.id);
                            found=true;
                        }
                        break;
                    }
                }
                if(found) break;
            }

            //included into blocks
            foreach(LevelBlock y, box->LvlData.blocks)
            {
                if((y.npc_id>0) && ((unsigned long)y.npc_id == x.id))
                {
                    usedIDs.push_back(x.id);
                    break;
                }
            }

            foreach(unsigned long npcID, usedIDs)
            {
                QString image;
                QString mask;
                QString textConfig;
                long I = configs.getNpcI(npcID);
                if(I>=0)
                {
                    if(x.withTxt)
                    {
                        image = levelCustomDirectory + "/" + x.merged.image_n;
                        mask = levelCustomDirectory + "/" + x.merged.mask_n;
                    }
                    else
                    {
                        image = levelCustomDirectory + "/" + configs.main_npc[I].image_n;
                        mask = levelCustomDirectory + "/" + configs.main_npc[I].mask_n;
                    }
                    textConfig = levelCustomDirectory + "/npc-"+QString::number(x.id)+".txt";
                }

                bool imageRemoved = !QFileInfo(image).exists();
                bool maskRemoved = !QFileInfo(mask).exists();
                bool textRemoved = !QFileInfo(textConfig).exists();

                for(int i=0; i < filesForRemove.size(); i++)
                {
                    if(imageRemoved && maskRemoved && textRemoved) break;

                    if(filesForRemove[i].toLower()==image.toLower())
                    {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                    else
                    if(filesForRemove[i].toLower()==mask.toLower())
                    {filesForRemove.removeAt(i); i--;maskRemoved=true;}
                    else
                    if(filesForRemove[i].toLower()==textConfig.toLower())
                    {filesForRemove.removeAt(i); i--;textRemoved=true;}
                }
                break;
            }
        }

        if(filesForRemove.isEmpty())
        {
            QMessageBox::information(this, "No unused fules",
                                     "This level haven't unused custom data",
                                     QMessageBox::Ok );
            return;
        }


        //Ask for removing

        /*
         *  Here add dialog with QListWidget where user will check/uncheck data for removing
        */

        if(QMessageBox::question(this, "Files to remove",
                                 "Found "+QString::number(filesForRemove.size())+" unused files"
                                 "\n\nDo you wanna remove them?",
                              QMessageBox::Yes, QMessageBox::No ) != QMessageBox::Yes )
            return;


        QDir unused_dir(levelCustomDirectory+"/_unused");
        if(!unused_dir.exists()) unused_dir.mkdir(levelCustomDirectory+"/_unused");

        //Remove selected
        foreach(QString f, filesForRemove)
        {\
            QFile x(f);
            if(x.exists())
            {
                x.copy(f, levelCustomDirectory+"/_unused/"+QFileInfo(f).fileName());
                x.remove();
            }
        }

        QMessageBox::information(this, "Unused files removed",
                                 "All unused files successfully removed from custom directory!",
                                 QMessageBox::Ok );
    }
}

