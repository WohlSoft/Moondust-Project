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
#ifndef ITEMSELECTDIALOG_H
#define ITEMSELECTDIALOG_H

#include <QList>
#include <QDialog>
#include <QMenu>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMdiSubWindow>
#include <QRadioButton>
#include <QSpinBox>

#include <data_configs/data_configs.h>

namespace Ui
{
    class ItemSelectDialog;
}

class ItemBoxListModel;

class ItemSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemSelectDialog(DataConfig *configs, int tabs, int npcExtraData = 0,
                              int curSelIDBlock = 0, int curSelIDBGO = 0, int curSelIDNPC = 0,
                              int curSelIDTile = 0, int curSelIDScenery = 0, int curSelIDPath = 0,
                              int curSelIDLevel = 0, int curSelIDMusic = 0, QWidget *parent = nullptr,
                              int noEmptyTypes = 0);
    ~ItemSelectDialog();

    int currentTab;
    int blockID;    QList<int> blockIDs;
    int bgoID;      QList<int> bgoIDs;
    int npcID;      QList<int> npcIDs;
    int tileID;     QList<int> tileIDs;
    int sceneryID;  QList<int> sceneryIDs;
    int pathID;     QList<int> pathIDs;
    int levelID;    QList<int> levelIDs;
    int musicID;    QList<int> musicIDs;
    QString musicFile;

    bool isCoin;

    enum Tabs
    {
        TAB_BLOCK = 1 << 0,
        TAB_BGO = 1 << 1,
        TAB_NPC = 1 << 2,
        TAB_TILE = 1 << 3,
        TAB_SCENERY = 1 << 4,
        TAB_PATH = 1 << 5,
        TAB_LEVEL = 1 << 6,
        TAB_MUSIC = 1 << 7
    };

    enum NpcExtraFlags
    {
        NPCEXTRA_WITHCOINS = 1 << 0,
        NPCEXTRA_ISCOINSELECTED = 1 << 1
    };

    void setTabsOrder(QVector<int> tabIds);

    void setMultiSelect(bool _multiselect);

    void setWorldMapRootDir(QString dir);

private slots:

    void on_Sel_TabCon_ItemType_currentChanged(int index);
    void npcTypeChange(bool toggled);

    void on_Sel_DiaButtonBox_accepted();
    void on_Sel_Combo_GroupsBlock_currentIndexChanged(int index);
    void on_Sel_Combo_CategoryBlock_currentIndexChanged(int index);
    void on_Sel_Combo_GroupsBGO_currentIndexChanged(int index);
    void on_Sel_Combo_CategoryBGO_currentIndexChanged(int index);
    void on_Sel_Combo_GroupsNPC_currentIndexChanged(int index);
    void on_Sel_Combo_CategoryNPC_currentIndexChanged(int index);

    void SelListNPC_itemDoubleClicked(const QModelIndex &index);
    void SelListBlock_itemDoubleClicked(const QModelIndex &index);
    void SelListBGO_itemDoubleClicked(const QModelIndex &index);
    void SelListTile_itemDoubleClicked(const QModelIndex &index);
    void SelListScenery_itemDoubleClicked(const QModelIndex &index);
    void SelListPath_itemDoubleClicked(const QModelIndex &index);
    void SelListLevel_itemDoubleClicked(const QModelIndex &index);
    void SelListMusic_itemDoubleClicked(const QModelIndex &index);

private:
    void selectListItem(QListView *w, ItemBoxListModel *m, int itemId);
    void selectListItem(QTableView *w, ItemBoxListModel *m, int itemId);
    void selectListItem(QListView *w, int array_id);
    void selectListItem(QTableWidget *w, int array_id);

    void checkExtraDataVis(QList<QWidget *> &l, QWidget *t);
    bool updateLabelVis(QList<QWidget *> &l, QWidget *t);

    int extractID(QListView *w);
    int extractID(QTableView *w);
    QList<int> extractIDs(QListView *w);
    QList<int> extractIDs(QTableView *w);

    bool isMultiSelect;

    QRadioButton *npcFromList;
    QRadioButton *npcCoins;
    QSpinBox     *npcCoinsSel;

    void addExtraDataControl(QWidget *control);

    ItemBoxListModel *m_blockModel = nullptr;
    ItemBoxListModel *m_bgoModel = nullptr;
    ItemBoxListModel *m_npcModel = nullptr;

    ItemBoxListModel *m_tileModel = nullptr;
    ItemBoxListModel *m_sceneModel = nullptr;
    ItemBoxListModel *m_pathModel = nullptr;
    ItemBoxListModel *m_levelModel = nullptr;
    ItemBoxListModel *m_musboxModel = nullptr;

    QString m_allLabel = "[all]";

    QMenu   m_blockFilterSetup;
    QMenu   m_bgoFilterSetup;
    QMenu   m_npcFilterSetup;

    QMenu   m_tilesFilterSetup;
    QMenu   m_sceneFilterSetup;
    QMenu   m_pathFilterSetup;
    QMenu   m_levelFilterSetup;
    QMenu   m_musicFilterSetup;

    QList<QWidget *> extraBlockWid;
    QList<QWidget *> extraBGOWid;
    QList<QWidget *> extraNPCWid;
    QList<QWidget *> extraTileWid;
    QList<QWidget *> extraSceneryWid;
    QList<QWidget *> extraPathWid;
    QList<QWidget *> extraLevelWid;
    QList<QWidget *> extraMusicWid;

    QString worldMapRoot;

    DataConfig *conf;
    Ui::ItemSelectDialog *ui;
};

inline ItemSelectDialog::Tabs operator|(ItemSelectDialog::Tabs a, ItemSelectDialog::Tabs b)
{
    return static_cast<ItemSelectDialog::Tabs>(static_cast<int>(a) | static_cast<int>(b));
}

inline ItemSelectDialog::NpcExtraFlags operator|(ItemSelectDialog::NpcExtraFlags a, ItemSelectDialog::NpcExtraFlags b)
{
    return static_cast<ItemSelectDialog::NpcExtraFlags>(static_cast<int>(a) | static_cast<int>(b));
}

#endif // ITEMSELECTDIALOG_H
