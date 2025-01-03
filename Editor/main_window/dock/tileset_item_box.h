/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef TILESET_ITEM_BOX_H
#define TILESET_ITEM_BOX_H

#include <QDockWidget>
#include <QTabWidget>
#include "mwdock_base.h"

#include <data_configs/obj_tilesets.h>

class MainWindow;
class QScrollArea;
class QComboBox;
class QTabBar;

namespace Ui {
class TilesetItemBox;
}

class TilesetItemBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit TilesetItemBox(QWidget *parent = nullptr);
    ~TilesetItemBox();

public slots:
    void re_translate();

    void setTileSetBox(bool force = false);

    void on_TilesetItemBox_visibilityChanged(bool visible);
    void on_newTileset_clicked();
    void on_tilesetGroup_currentIndexChanged(int index);

    void setTabPosition(QTabWidget::TabPosition pos);

    void editSelectedTileset();
    QScrollArea *getFrameTilesetOfTab(QWidget *catTab);
    QTabBar *getGroupComboboxOfTab(QWidget *catTab);
    QWidget *findTabWidget(const QString &categoryItem);
    QWidget *makeCategory(const QString &categoryItem);
    void prepareCategoriesAndGroups();
    void prepareTilesetGroup(const SimpleTilesetGroup &tilesetGroups);
    void clearTilesetGroups();
    void makeSelectedTileset(int tabIndex);
    QVector<SimpleTileset> loadCustomTilesets();
    void makeCurrentTileset();
    void makeAllTilesets();

protected:
    virtual void focusInEvent(QFocusEvent * ev);

private:
    Ui::TilesetItemBox *ui;
    bool m_lockSettings = false;
};

#endif // TILESET_ITEM_BOX_H
