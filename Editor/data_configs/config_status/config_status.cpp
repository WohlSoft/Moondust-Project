/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_status.h"
#include <ui_config_status.h>

#include <common_features/util.h>
#ifdef Q_OS_WIN
#include <QtWin>
#include <QSysInfo>
#endif

static void setDirEntry(QTableWidget *w, int row, QString name, QString path)
{
    QTableWidgetItem *itemDir;
    w->insertRow(row);

    itemDir = new QTableWidgetItem(name);
    w->setItem(row, 0, itemDir);

    itemDir = new QTableWidgetItem(path);
    itemDir->setToolTip(path);
    w->setItem(row, 1, itemDir);
}

static void setRequiredStatusEntry(QListWidget *w, long count, long total, QString status)
{
    QListWidgetItem *item = new QListWidgetItem(w);
    item->setText(status.arg(count).arg(total));
    if(count == 0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else if(count < total)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    w->addItem(item);
}

static void setOptionalStatusEntry(QListWidget *w, long count, long total, QString status)
{
    QListWidgetItem *item = new QListWidgetItem(w);
    item->setText(status.arg(count));
    if(count < total)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    w->addItem(item);
}


ConfigStatus::ConfigStatus(dataconfigs &conf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigStatus)
{
    configs = &conf;
    ui->setupUi(this);

    setWindowFlags(Qt::Window |
                   Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint);
    setGeometry(util::alignToScreenCenter(size()));

#ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
#endif
#ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
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

    ui->ConfigNameLabel->setText(ConfStatus::configName);
    ui->ConfigPathLabel->setText(configs->config_dir);

    //Level Blocks
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_block.stored(),   ConfStatus::total_blocks,   tr("Level: Blocks (%1/%2)"));
    //Level BGO
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_bgo.stored(),     ConfStatus::total_bgo,      tr("Level: Background objects (%1/%2)"));
    //Level BG
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_bg.stored(),      ConfStatus::total_bg,       tr("Level: Background images (%1/%2)"));
    //Level NPC
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_npc.stored(),     ConfStatus::total_npc,      tr("Level: NPCs (%1/%2)"));
    //MUSIC
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_music_lvl.stored(), ConfStatus::total_music_lvl, tr("Music (level) (%1/%2)"));
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_music_wld.stored(), ConfStatus::total_music_wld, tr("Music (world map) (%1/%2)"));
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_music_spc.stored(), ConfStatus::total_music_spc, tr("Music (special) (%1/%2)"));
    //SOUND
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_sound.stored(),   ConfStatus::total_sound,    tr("Sounds (%1/%2)"));
    //World map Terrain tiles
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_wtiles.stored(),  ConfStatus::total_wtile,    tr("World map: Terrain tiles (%1/%2)"));
    //World map Scenery
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_wscene.stored(),  ConfStatus::total_wscene,   tr("World map: Scenery (%1/%2)"));
    //World Paths
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_wpaths.stored(),  ConfStatus::total_wpath,    tr("World map: Path tiles (%1/%2)"));
    //World Levels
    setRequiredStatusEntry(ui->ItemsStatus, configs->main_wlevels.stored(), ConfStatus::total_wlvl,     tr("World map: Level entrance tiles (%1/%2)"));
    //Rotation rules table
    setOptionalStatusEntry(ui->ItemsStatus, configs->main_rotation_table.size(), configs->main_rotation_table.size(), tr("Default rotation rules (%1)"));

    ////////////////////////DirList/////////////////////////
    int row = 0;
    setDirEntry(ui->ItemsDirs, row++, tr("Level data"), configs->dirs.glevel);
    setDirEntry(ui->ItemsDirs, row++, tr("World map data"), configs->dirs.gworld);
    setDirEntry(ui->ItemsDirs, row++, tr("Characters"), configs->dirs.gplayble);
    setDirEntry(ui->ItemsDirs, row++, tr("Game worlds"), configs->dirs.worlds);
    setDirEntry(ui->ItemsDirs, row++, tr("Music"),      configs->dirs.music);
    setDirEntry(ui->ItemsDirs, row++, tr("Sounds"),     configs->dirs.sounds);
    setDirEntry(ui->ItemsDirs, row++, tr("Custom data"), configs->dirs.gcustom);

    QTableWidgetItem *itemError;
    if(configs->errorsList[dataconfigs::ERR_GLOBAL].isEmpty())
    {
        QFont font;
        font.setItalic(true);
        ui->ItemsErrors->insertRow(0);
        itemError = new QTableWidgetItem(tr("[Error list is empty, congratulations!]"));
        itemError->setFont(font);
        ui->ItemsErrors->setItem(0, 0, itemError);
    }
    else
    {
        for(long e = 0; e < configs->errorsList[dataconfigs::ERR_GLOBAL].size(); e++)
        {
            ui->ItemsErrors->insertRow(int(e));
            QString erroText = configs->errorsList[dataconfigs::ERR_GLOBAL][int(e)];
            itemError = new QTableWidgetItem(erroText);
            itemError->setToolTip(erroText);
            ui->ItemsErrors->setItem(int(e), 0, itemError);
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
