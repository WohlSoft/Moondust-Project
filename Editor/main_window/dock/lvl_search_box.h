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

#pragma once
#ifndef LVL_SEARCH_BOX_H
#define LVL_SEARCH_BOX_H

#include <QDockWidget>
#include "mwdock_base.h"
#include <PGE_File_Formats/lvl_filedata.h>

class MainWindow;
class QMdiSubWindow;
class LevelEdit;
class QComboBox;
class QGraphicsItem;

namespace Ui
{
    class LvlSearchBox;
}

class LvlSearchBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
    explicit LvlSearchBox(QWidget *parent = 0);
    ~LvlSearchBox();
public:
    QComboBox *cbox_layer_block();
    QComboBox *cbox_layer_bgo();
    QComboBox *cbox_layer_npc();
    QComboBox *cbox_layer_attached_npc();

    QComboBox *cbox_event_block_dest();
    QComboBox *cbox_event_block_hit();
    QComboBox *cbox_event_block_le();

    QComboBox *cbox_event_npc_activate();
    QComboBox *cbox_event_npc_death();
    QComboBox *cbox_event_npc_talk();
    QComboBox *cbox_event_npc_empty_layer();

public slots:
    void re_translate();
    void resetAllSearches();
    void toggleNewWindowLVL(QMdiSubWindow *window);
    void resetBlockSearch();
    void resetBGOSearch();
    void resetNPCSearch();

private slots:
    void on_FindStartBlock_clicked();
    void on_FindStartBGO_clicked();
    void on_FindStartNPC_clicked();

    void on_blockSelectAll_clicked();
    void on_npcSelectAll_clicked();
    void on_bgoSelectAll_clicked();

    void on_Find_Button_TypeBlock_clicked();
    void on_Find_Button_TypeBGO_clicked();
    void on_Find_Button_TypeNPC_clicked();
    void on_Find_Button_ResetBlock_clicked();
    void on_Find_Button_ResetBGO_clicked();
    void on_Find_Button_ResetNPC_clicked();
    void on_Find_Button_ContainsNPCBlock_clicked();

private:
    Ui::LvlSearchBox *ui;
    bool m_lockReset;

    struct SearchMeta
    {
        quint64         total = 0;
        unsigned long   index = 0;
        struct Data
        {
            unsigned long id = 0;
            unsigned int index = 0;
            long npc_id = 0;
        } data;
    };

    void resetAllSearchFields();
    SearchMeta m_curBlock;
    SearchMeta m_curBgo;
    SearchMeta m_curNpc;

    typedef bool (LvlSearchBox::*SearchCriteriaType)(QGraphicsItem *);

    bool checkBlockCriteria(QGraphicsItem *gri);
    bool checkBgoCriteria(QGraphicsItem *gri);
    bool checkNpcCriteria(QGraphicsItem *gri);

    bool doSearch(SearchMeta &meta,
                  SearchCriteriaType checkCriteria,
                  LevelEdit *edit,
                  bool curSectionOnly = false,
                  bool all = false,
                  bool selectionOnly = false);

    enum currentSearch
    {
        SEARCH_BLOCK = 1 << 0,
        SEARCH_BGO = 1 << 1,
        SEARCH_NPC = 1 << 2,
        SEARCH_TILE = 1 << 3,
        SEARCH_SCENERY = 1 << 4,
        SEARCH_PATH = 1 << 5,
        SEARCH_LEVEL = 1 << 6,
        SEARCH_MUSICBOX = 1 << 7
    };
    int m_currentSearches;
};

#endif // LVL_SEARCH_BOX_H
