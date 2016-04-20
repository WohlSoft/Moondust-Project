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

#include "configstatus.h"
#include <ui_configstatus.h>

#ifdef Q_OS_WIN
#include <QtWin>
#include <QSysInfo>
#endif

ConfigStatus::ConfigStatus(dataconfigs &conf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigStatus)
{
    configs = &conf;
    ui->setupUi(this);

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QSysInfo::WindowsVersion>=QSysInfo::WV_VISTA)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1,-1,-1,-1);
            QtWin::enableBlurBehindWindow(this);
            ui->gridLayout->setMargin(0);
        }
        else
        {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }
    #endif

    //Create Statistics
    ui->ItemsStatus->clear();
    QListWidgetItem * item;

    ui->ConfigNameLabel->setText(ConfStatus::configName);
    ui->ConfigPathLabel->setText(configs->config_dir);

    //Level Blocks
    item = new QListWidgetItem;
    item->setText(tr("Level: Blocks (%1/%2)").arg(configs->main_block.stored()).arg(ConfStatus::total_blocks));
    if(configs->main_block.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_block.stored()<ConfStatus::total_blocks)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Level BGO
    item = new QListWidgetItem;
    item->setText(tr("Level: Background objects (%1/%2)").arg(configs->main_bgo.stored()).arg(ConfStatus::total_bgo));
    if(configs->main_bgo.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_bgo.stored()<ConfStatus::total_bgo)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Level BG
    item = new QListWidgetItem;
    item->setText(tr("Level: Background images (%1/%2)").arg(configs->main_bg.stored()).arg(ConfStatus::total_bg));
    if(configs->main_bg.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_bg.stored()<ConfStatus::total_bg)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Level NPC
    item = new QListWidgetItem;
    item->setText(tr("Level: NPCs (%1/%2)").arg(configs->main_npc.stored()).arg(ConfStatus::total_npc));
    if(configs->main_npc.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_npc.stored()<ConfStatus::total_npc)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //MUSIC
    item = new QListWidgetItem;
    item->setText(tr("Music (level) (%1/%2)").arg(configs->main_music_lvl.stored()).arg(ConfStatus::total_music_lvl));
    if(configs->main_music_lvl.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_music_lvl.stored()<ConfStatus::total_music_lvl)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    item = new QListWidgetItem;
    item->setText(tr("Music (world) (%1/%2)").arg(configs->main_music_wld.stored()).arg(ConfStatus::total_music_wld));
    if(configs->main_music_wld.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_music_wld.stored()<ConfStatus::total_music_wld)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    item = new QListWidgetItem;
    item->setText(tr("Music (special) (%1/%2)").arg(configs->main_music_spc.stored()).arg(ConfStatus::total_music_spc));
    if(configs->main_music_spc.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_music_spc.stored()<ConfStatus::total_music_spc)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //SOUND
    item = new QListWidgetItem;
    item->setText(tr("Sounds (%1/%2)").arg(configs->main_sound.stored()).arg(ConfStatus::total_sound));
    if(configs->main_sound.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_sound.stored()<ConfStatus::total_sound)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);


    //World map Terrain tiles
    item = new QListWidgetItem;
    item->setText(tr("World map: Terrain tiles (%1/%2)").arg(configs->main_wtiles.stored()).arg(ConfStatus::total_wtile));
    if(configs->main_wtiles.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_wtiles.stored()<ConfStatus::total_wtile)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //World map Scenery
    item = new QListWidgetItem;
    item->setText(tr("World map: Scenery (%1/%2)").arg(configs->main_wscene.stored()).arg(ConfStatus::total_wscene));
    if(configs->main_wscene.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_wscene.stored()<ConfStatus::total_wscene)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);


    //World Paths
    item = new QListWidgetItem;
    item->setText(tr("World map: Path tiles (%1/%2)").arg(configs->main_wpaths.stored()).arg(ConfStatus::total_wpath));
    if(configs->main_wpaths.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_wpaths.stored()<ConfStatus::total_wpath)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);


    //World Levels
    item = new QListWidgetItem;
    item->setText(tr("World map: Level entrance tiles (%1/%2)").arg(configs->main_wlevels.stored()).arg(ConfStatus::total_wlvl));
    if(configs->main_wlevels.stored()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_wlevels.stored()<ConfStatus::total_wlvl)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Rotation rules table
    item = new QListWidgetItem;
    item->setText(tr("Default rotation rules (%1)").arg(configs->main_rotation_table.size()));
    if(configs->main_rotation_table.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);


    ////////////////////////DirList/////////////////////////
    QTableWidgetItem * itemDir;
    ui->ItemsDirs->insertRow(0);
        itemDir = new QTableWidgetItem(tr("Level data"));
        ui->ItemsDirs->setItem(0, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.glevel);
        ui->ItemsDirs->setItem(0, 1, itemDir);
    ui->ItemsDirs->insertRow(1);
        itemDir = new QTableWidgetItem(tr("World data"));
        ui->ItemsDirs->setItem(1, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.gworld);
        ui->ItemsDirs->setItem(1, 1, itemDir);

    ui->ItemsDirs->insertRow(2);
        itemDir = new QTableWidgetItem(tr("Characters"));
        ui->ItemsDirs->setItem(2, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.gplayble);
        ui->ItemsDirs->setItem(2, 1, itemDir);

    ui->ItemsDirs->insertRow(3);
        itemDir = new QTableWidgetItem(tr("Game worlds"));
        ui->ItemsDirs->setItem(3, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.worlds);
        ui->ItemsDirs->setItem(3, 1, itemDir);

    ui->ItemsDirs->insertRow(4);
        itemDir = new QTableWidgetItem(tr("Music"));
        ui->ItemsDirs->setItem(4, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.music);
        ui->ItemsDirs->setItem(4, 1, itemDir);

    ui->ItemsDirs->insertRow(5);
        itemDir = new QTableWidgetItem(tr("Sounds"));
        ui->ItemsDirs->setItem(5, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.sounds);
        ui->ItemsDirs->setItem(5, 1, itemDir);

    ui->ItemsDirs->insertRow(6);
        itemDir = new QTableWidgetItem(tr("Custom data"));
        ui->ItemsDirs->setItem(6, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.gcustom);
        ui->ItemsDirs->setItem(6, 1, itemDir);

    QTableWidgetItem * itemError;
    if(configs->errorsList.isEmpty())
    {
        QFont font;
        font.setItalic(true);
        ui->ItemsErrors->insertRow(0);
            itemError = new QTableWidgetItem(tr("[Error list is empty]"));
            itemError->setFont(font);
            ui->ItemsErrors->setItem(0, 0, itemError);
    }
    else
    {
        for(long e=0;e<configs->errorsList.size();e++)
        {
            ui->ItemsErrors->insertRow(e);
                itemError = new QTableWidgetItem(configs->errorsList[e]);
                ui->ItemsErrors->setItem(e, 0, itemError);
        }

    }

}

ConfigStatus::~ConfigStatus()
{
    delete ui;
}


void ConfigStatus::on_buttonBox_accepted()
{
    this->close();
}
