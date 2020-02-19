/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef WLD_MUSICBOX_ITEMSET_H
#define WLD_MUSICBOX_ITEMSET_H

#include <QDockWidget>
#include "mwdock_base.h"

class MainWindow;
class QTabWidget;
class ItemBoxListModel;

namespace Ui {
class WorldMusicBoxItemBox;
}

class WorldMusicBoxItemBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT
    friend class MainWindow;
private:
    explicit WorldMusicBoxItemBox(QWidget *parent = nullptr);
    ~WorldMusicBoxItemBox();

public:
    QTabWidget *tabWidget();

public slots:
    void re_translate();
    /**
     * @brief Initialize the item box
     */
    void initItemLists();

private slots:
    void MusicList_itemClicked(const QModelIndex &item);

private:
    Ui::WorldMusicBoxItemBox *ui;

    ItemBoxListModel *m_musicBoxModel = nullptr;
};

#endif // WLD_MUSICBOX_ITEMSET_H
