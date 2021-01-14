/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

static void cleanupRemoveList(QStringList &list, const QStringList &usedFiles)
{
    for(int i = 0; i < list.size(); i++)
    {
        if(usedFiles.isEmpty())
            break;

        for(const QString &f : usedFiles)
        {
            QFileInfo p(list[i]), q(f);
            if(p.fileName().toLower() == q.fileName().toLower())
            {
                list.removeAt(i);
                i = 0;
            }
        }
    }
}

static QString remExt(QString s)
{
    auto d = s.lastIndexOf('.');
    if(d >= 1)
        return s.left(d);
    return s;
}


static void getDumpOfScripts(QString &dump, QString cPath)
{
    dump.clear();
    QDir d(cPath);
    d.setNameFilters({"*.txt", "*.ini", "*.lua"});
    auto l = d.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(auto &f : l)
    {
        QFile ff(cPath + "/" + f);
        if(ff.open(QIODevice::ReadOnly|QIODevice::Text))
            dump += ff.readAll();
    }
}

void MainWindow::on_actionCDATA_clear_unused_triggered()
{
    //QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
    if(activeChildWindow() == WND_Level)
    {
        QStringList filesForRemove;
        auto *box = activeLvlEditWin();

        if(!box || !box->sceneCreated)
            return;

        if(box->isUntitled())
        {
            QMessageBox::warning(this, tr("File not saved"),
                                 tr("File doesn't saved on disk."), QMessageBox::Ok);
            return;
        }

        LvlScene *s = box->scene;
        QString levelDirectory(box->LvlData.meta.path);

        QString levelCustomDirectory(box->LvlData.meta.path + "/" + box->LvlData.meta.filename);
        QDir levelCustomDirectory_dir(levelCustomDirectory);

        filesForRemove.append(levelCustomDirectory_dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot));

        qDebug() << levelCustomDirectory << "Total custom files" << filesForRemove.size();

        QRegExp reg;
        reg.setPatternSyntax(QRegExp::Wildcard);
        reg.setCaseSensitivity(Qt::CaseInsensitive);

        QList<unsigned long > npcContainers;

        for(long i = 1; i < configs.main_npc.size(); i++)
        {
            auto &npcConf = configs.main_npc[i];
            if(npcConf.setup.container)
                npcContainers << npcConf.setup.id;
        }

        QString scriptsDump;
        getDumpOfScripts(scriptsDump, levelCustomDirectory);

        //White list of entries to keep
        QStringList whiteList;

        whiteList << "lunadll.txt"; //LunaDLL stuff
        whiteList << "lunadll.lua";
        whiteList << "luna.lua";

        whiteList << "*.tileset.ini"; //PGE Tilesets

        whiteList << "effect-*.gif"; //All SMBX64 effects
        whiteList << "effect-*.png";

        whiteList << "mario-*.gif"; //All SMBX64 playable characters
        whiteList << "peach-*.gif";
        whiteList << "toad-*.gif";
        whiteList << "luigi-*.gif";
        whiteList << "link-*.gif";
        whiteList << "yoshit-*.gif";
        whiteList << "yoshib-*.gif";
        whiteList << "mario-*.png";
        whiteList << "peach-*.png";
        whiteList << "toad-*.png";
        whiteList << "luigi-*.png";
        whiteList << "link-*.png";
        whiteList << "yoshit-*.png";
        whiteList << "yoshib-*.png";

        QString npcFile = "npc-%1.png|npc-%1.gif|npc-%1m.gif|npc-%1.txt|npc-%1.ini";

        typedef QPair<unsigned long, QStringList > dependNPC;
        QList<dependNPC > dependNPCs;

        dependNPCs << dependNPC(15, npcFile.arg(16).split('|'));  //boom-boom

        dependNPCs << dependNPC(39, npcFile.arg(41).split('|'));  //Birdo
        dependNPCs << dependNPC(39, npcFile.arg(40).split('|'));

        //giant coopa
        dependNPCs << dependNPC(72, npcFile.arg(73).split('|'));

        //green parakoopa 1
        dependNPCs << dependNPC(76, npcFile.arg(4).split('|'));
        dependNPCs << dependNPC(76, npcFile.arg(5).split('|'));
        dependNPCs << dependNPC(4, npcFile.arg(5).split('|'));

        //green parakoopa 2
        dependNPCs << dependNPC(176, npcFile.arg(173).split('|'));
        dependNPCs << dependNPC(176, npcFile.arg(172).split('|'));
        dependNPCs << dependNPC(173, npcFile.arg(172).split('|'));

        //red parakoopa 1
        dependNPCs << dependNPC(161, npcFile.arg(6).split('|'));
        dependNPCs << dependNPC(161, npcFile.arg(7).split('|'));
        dependNPCs << dependNPC(6, npcFile.arg(7).split('|'));

        //red parakoopa 2
        dependNPCs << dependNPC(177, npcFile.arg(175).split('|'));
        dependNPCs << dependNPC(177, npcFile.arg(174).split('|'));
        dependNPCs << dependNPC(175, npcFile.arg(174).split('|'));

        //Blue parakoopa 3
        dependNPCs << dependNPC(111, npcFile.arg(115).split('|'));
        dependNPCs << dependNPC(111, npcFile.arg(55).split('|'));
        dependNPCs << dependNPC(111, npcFile.arg(119).split('|'));

        dependNPCs << dependNPC(123, npcFile.arg(111).split('|'));
        dependNPCs << dependNPC(123, npcFile.arg(115).split('|'));
        dependNPCs << dependNPC(123, npcFile.arg(55).split('|'));
        dependNPCs << dependNPC(123, npcFile.arg(119).split('|'));

        //Red parakoopa 3
        dependNPCs << dependNPC(110, npcFile.arg(118).split('|'));
        dependNPCs << dependNPC(110, npcFile.arg(114).split('|'));

        dependNPCs << dependNPC(122, npcFile.arg(110).split('|'));
        dependNPCs << dependNPC(123, npcFile.arg(118).split('|'));
        dependNPCs << dependNPC(123, npcFile.arg(114).split('|'));

        //Green parakoopa 3
        dependNPCs << dependNPC(109, npcFile.arg(113).split('|')); //shell
        dependNPCs << dependNPC(109, npcFile.arg(117).split('|')); //beech

        dependNPCs << dependNPC(121, npcFile.arg(109).split('|')); //koopa
        dependNPCs << dependNPC(121, npcFile.arg(113).split('|')); //shell
        dependNPCs << dependNPC(121, npcFile.arg(117).split('|')); //beech

        //Yellow parakoopa 3
        dependNPCs << dependNPC(112, npcFile.arg(116).split('|')); //shell
        dependNPCs << dependNPC(112, npcFile.arg(120).split('|')); //beech

        dependNPCs << dependNPC(124, npcFile.arg(112).split('|')); //koopa
        dependNPCs << dependNPC(124, npcFile.arg(116).split('|')); //shell
        dependNPCs << dependNPC(124, npcFile.arg(120).split('|')); //beech

        dependNPCs << dependNPC(124, npcFile.arg(194).split('|'));
        dependNPCs << dependNPC(112, npcFile.arg(194).split('|'));
        dependNPCs << dependNPC(116, npcFile.arg(194).split('|'));

        //rainbow shell
        dependNPCs << dependNPC(194, npcFile.arg(195).split('|'));

        //Parakoopas
        dependNPCs << dependNPC(243, npcFile.arg(242).split('|'));
        dependNPCs << dependNPC(244, npcFile.arg(1).split('|'));
        dependNPCs << dependNPC(3, npcFile.arg(2).split('|'));

        //random vegetable
        dependNPCs << dependNPC(147, npcFile.arg(146).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(140).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(142).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(92).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(139).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(141).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(143).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(144).split('|'));
        dependNPCs << dependNPC(147, npcFile.arg(145).split('|'));

        //random powerup
        dependNPCs << dependNPC(287, npcFile.arg(9).split('|'));
        dependNPCs << dependNPC(287, npcFile.arg(184).split('|'));
        dependNPCs << dependNPC(287, npcFile.arg(185).split('|'));

        dependNPCs << dependNPC(287, npcFile.arg(14).split('|'));
        dependNPCs << dependNPC(287, npcFile.arg(182).split('|'));
        dependNPCs << dependNPC(287, npcFile.arg(183).split('|'));

        dependNPCs << dependNPC(287, npcFile.arg(34).split('|'));
        dependNPCs << dependNPC(287, npcFile.arg(170).split('|'));

        dependNPCs << dependNPC(287, npcFile.arg(264).split('|'));
        dependNPCs << dependNPC(287, npcFile.arg(277).split('|'));

        //bill blasters
        dependNPCs << dependNPC(21, npcFile.arg(17).split('|'));
        dependNPCs << dependNPC(22, npcFile.arg(17).split('|'));

        //Toothy
        dependNPCs << dependNPC(49, npcFile.arg(50).split('|'));

        //Small Lakitu
        dependNPCs << dependNPC(47, npcFile.arg(48).split('|'));

        //Spiny
        dependNPCs << dependNPC(286, npcFile.arg(285).split('|'));

        //Rex
        dependNPCs << dependNPC(162, npcFile.arg(163).split('|'));

        //Magic potion
        dependNPCs << dependNPC(288, npcFile.arg(289).split('|'));

        //vined heads
        dependNPCs << dependNPC(226, npcFile.arg(213).split('|'));
        dependNPCs << dependNPC(225, npcFile.arg(214).split('|'));
        dependNPCs << dependNPC(227, npcFile.arg(224).split('|'));

        //Add into white SMBX64 dependencied of exists ID
        for(int q = 0; q < box->LvlData.npc.size(); q++)
        {
            bool isConainer = false;

            for(const auto &x : npcContainers)
            {
                if(box->LvlData.npc[q].id == x)
                {
                    qDebug() << "container! " <<  box->LvlData.npc[q].contents;
                    isConainer = true;
                    break;
                }
            }

            unsigned long npcID = (isConainer ? box->LvlData.npc[q].contents : box->LvlData.npc[q].id);
            for(int r = 0; r < dependNPCs.size(); r++)
            {
                if(npcID == dependNPCs[r].first)
                {
                    whiteList.append(dependNPCs[r].second);
                    dependNPCs.removeAt(r);
                    r = -1; //reset loot to 0
                }
            }

            if(dependNPCs.isEmpty()) break;
        }

        //into included into blocks
        for(int q = 0; q < box->LvlData.blocks.size(); q++)
        {
            for(int r = 0; r < dependNPCs.size(); r++)
                if(box->LvlData.blocks[q].npc_id == (signed)dependNPCs[r].first)
                {
                    whiteList << dependNPCs[r].second;
                    dependNPCs.removeAt(r);
                    r = -1; //reset loot to 0
                }
            if(dependNPCs.isEmpty()) break;
        }

        //SMBX64 system items
        //coin
        whiteList << npcFile.arg(10).split('|');

        //bullets
        whiteList << npcFile.arg(40).split('|');
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
        whiteList << "background-98.png"; //door lock
        whiteList << "background-160.png"; //star lock

        for(int i = 0; i < filesForRemove.size(); i++)
        {
            bool removed = false;
            if(QFileInfo(levelCustomDirectory + "/" + filesForRemove[i]).isDir())
            {
                filesForRemove.removeAt(i);
                i--;
                continue;
            }
            else
            {
                for(QString &f : whiteList)
                {
                    reg.setPattern(f);
                    if(reg.exactMatch(filesForRemove[i]))
                    {
                        filesForRemove.removeAt(i);
                        i--;
                        removed = true;
                        break;
                    }
                }
            }

            if(!removed)
                filesForRemove[i] = levelCustomDirectory + "/" + filesForRemove[i];
        }


        //Get custom data list
        for(const LevelSection &y : box->LvlData.sections)
        {
            if(y.music_file.isEmpty())
                continue;

            QString musFile = levelDirectory + "/" + y.music_file;
            auto pathArgsAt = musFile.lastIndexOf('|');

            musFile.replace('\\', '/');
            musFile.replace("//", "/");

            if(pathArgsAt >= 0)
                musFile = musFile.left(pathArgsAt);

            bool musicFileRemoved = !QFile::exists(musFile);

            for(int i = 0; i < filesForRemove.size(); i++)
            {
                if(musicFileRemoved)
                    break;

                QFileInfo p(filesForRemove[i]), q(musFile);
                if(p.fileName().toLower() == q.fileName().toLower())
                {
                    filesForRemove.removeAt(i);
                    break;
                }
            }
        }

        for(auto bgIT = s->m_localConfigBackgrounds.begin(); bgIT != s->m_localConfigBackgrounds.end(); bgIT++)
        {
            obj_BG &x = *bgIT;
            for(const auto &y : box->LvlData.sections)
            {
                if(y.background == x.setup.id)
                {
                    QStringList usedFiles;

                    QString image = levelCustomDirectory + "/" + x.setup.image_n;
                    QString image2 = levelCustomDirectory + "/" + x.setup.second_image_n;

                    usedFiles << remExt(image) + ".png" << remExt(image) + ".gif";
                    if(x.setup.second_image_n.isEmpty())
                        usedFiles << remExt(image2) + ".png" << remExt(image2) + ".gif";

                    usedFiles << QString("%2/background2-%1.ini").arg(y.id).arg(levelCustomDirectory)
                              << QString("%2/background2-%1.txt").arg(y.id).arg(levelCustomDirectory);

                    cleanupRemoveList(filesForRemove, usedFiles);
                    break;
                }
            }
        }

        for(obj_block *x : s->m_customBlocks)
        {
            for(const auto &y : box->LvlData.blocks)
            {
                if(y.id == x->setup.id)
                {
                    long transformTo = x->setup.transfororm_on_hit_into;
                    QSet<long> usedTransforms;

                    do
                    {
                        QStringList usedFiles;

                        QString image = levelCustomDirectory + "/" + x->setup.image_n;
                        QString mask = levelCustomDirectory + "/" + x->setup.mask_n;

                        usedFiles << remExt(image) + ".png" << remExt(image) + ".gif";
                        if(!x->setup.mask_n.isEmpty())
                            usedFiles << mask;
                        usedFiles << QString("%2/block-%1.ini").arg(x->setup.id).arg(levelCustomDirectory)
                                  << QString("%2/block-%1.txt").arg(x->setup.id).arg(levelCustomDirectory);

                        cleanupRemoveList(filesForRemove, usedFiles);

                        if(transformTo > 0 && !usedTransforms.contains(transformTo))
                        {
                            obj_block &t = s->m_localConfigBlocks[transformTo];
                            if(t.isValid)
                            {
                                x = &t;
                                usedTransforms.insert(transformTo);
                                transformTo = x->setup.transfororm_on_hit_into;
                            } else {
                                transformTo = 0;
                            }
                        } else {
                            transformTo = 0;
                        }
                    } while(transformTo > 0);
                    break;
                }
            }

        }

        for(obj_bgo *x : s->m_customBGOs)
        {
            for(const auto &y : box->LvlData.bgo)
            {
                if(y.id == x->setup.id)
                {
                    QStringList usedFiles;

                    QString image = levelCustomDirectory + "/" + x->setup.image_n;
                    QString mask = levelCustomDirectory + "/" + x->setup.mask_n;

                    usedFiles << remExt(image) + ".png" << remExt(image) + ".gif";
                    if(!x->setup.mask_n.isEmpty())
                        usedFiles << mask;
                    usedFiles << QString("%2/background-%1.ini").arg(y.id).arg(levelCustomDirectory)
                              << QString("%2/background-%1.txt").arg(y.id).arg(levelCustomDirectory);
                    cleanupRemoveList(filesForRemove, usedFiles);
                    break;
                }
            }

        }


        for(obj_npc *x : s->m_customNPCs)
        {
            QSet<unsigned long > usedIDs;

            //Used in level
            for(const auto &y : box->LvlData.npc)
            {
                if(y.id == x->setup.id)
                {
                    usedIDs.insert(x->setup.id);
                    break;
                }
            }

            //Used in NPC containers
            for(auto y : box->LvlData.npc)
            {
                bool found = false;
                for(unsigned long z : npcContainers)
                {
                    if(y.id == z)
                    {
                        if(y.special_data == (signed)x->setup.id)
                        {
                            usedIDs.insert(x->setup.id);
                            found = true;
                        }
                        break;
                    }
                }

                if(found)
                    break;
            }

            //included into blocks
            for(const auto &y : box->LvlData.blocks)
            {
                if((y.npc_id > 0) && ((unsigned long)y.npc_id == x->setup.id))
                {
                    usedIDs.insert(x->setup.id);
                    break;
                }
            }

            for(unsigned long npcID : usedIDs)
            {
                QStringList usedFiles;

                QString image = levelCustomDirectory + "/" + x->setup.image_n;
                QString mask = levelCustomDirectory + "/" + x->setup.mask_n;

                usedFiles << remExt(image) + ".png" << remExt(image) + ".gif";
                if(!x->setup.mask_n.isEmpty())
                    usedFiles << mask;
                usedFiles << QString("%2/npc-%1.ini").arg(npcID).arg(levelCustomDirectory)
                          << QString("%2/npc-%1.txt").arg(npcID).arg(levelCustomDirectory);
                cleanupRemoveList(filesForRemove, usedFiles);
            }
        }


        // APROXIMATELY, Check for any possible mentions of these files in scripts
        for(int i = 0; i < filesForRemove.size(); i++)
        {
            QString f = filesForRemove[i];
            QFileInfo p(f);
            if(scriptsDump.contains(p.fileName(), Qt::CaseInsensitive))
            {
                filesForRemove.removeAt(i);
                i--;
            }
        }

        if(filesForRemove.isEmpty())
        {
            QMessageBox::information(this,
                                     "No unused files",
                                     "This level has no unused custom data.",
                                     QMessageBox::Ok);
            return;
        }


        //Ask for removing

        /*
         *  Here add dialog with QListWidget where user will check/uncheck data for removing
        */

        if(QMessageBox::question(this,
                                 "Files to remove",
                                 QString("Found %1 unused files"
                                         "\n\nDo you wanna remove them?").arg(filesForRemove.size()),
                                 QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            return;


        QDir unused_dir(levelCustomDirectory + "/_unused");
        if(!unused_dir.exists()) unused_dir.mkdir(levelCustomDirectory + "/_unused");

        //Remove selected
        for(QString &f : filesForRemove)
        {
            QFile x(f);
            if(x.exists())
            {
                x.copy(f, levelCustomDirectory + "/_unused/" + QFileInfo(f).fileName());
                x.remove();
            }
        }

        QMessageBox::information(this,
                                 "Unused files removed",
                                 "All unused files successfully removed from custom directory!",
                                 QMessageBox::Ok);
    }
}

