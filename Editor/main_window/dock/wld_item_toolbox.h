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

#ifndef WLD_ITEM_TOOLBOX_H
#define WLD_ITEM_TOOLBOX_H

#include <QDockWidget>
#include "mwdock_base.h"

class MainWindow;
class QTabWidget;
class QTableWidgetItem;
class QListWidgetItem;

namespace Ui {
class WorldToolBox;
}

class WorldToolBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT
    friend class MainWindow;

public:
    explicit WorldToolBox(QWidget *parent = 0);
    ~WorldToolBox();
    QTabWidget *tabWidget();

public slots:
    void re_translate();
    void setWldItemBoxes(bool setGrp=false, bool setCat=false);

private slots:
    void on_WorldToolBox_visibilityChanged(bool visible);

    void on_WLD_TilesList_itemClicked(QTableWidgetItem *item);
    void on_WLD_SceneList_itemClicked(QListWidgetItem *item);
    void on_WLD_PathsList_itemClicked(QTableWidgetItem *item);
    void on_WLD_LevelList_itemClicked(QListWidgetItem *item);
    void on_WLD_MusicList_itemClicked(QListWidgetItem *item);

private:
    Ui::WorldToolBox *ui;

    QString allWLabel;
    QString customWLabel;

    bool lock_Wgrp;
    bool lock_Wcat;

    QString grp_tiles;
    QString grp_paths;
    QString grp_scenes;
};

#endif // WLD_ITEM_TOOLBOX_H
