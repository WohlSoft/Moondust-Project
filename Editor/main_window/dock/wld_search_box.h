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
#ifndef WLD_SEARCH_BOX_H
#define WLD_SEARCH_BOX_H

#include <QDockWidget>
#include "mwdock_base.h"
#include <PGE_File_Formats/wld_filedata.h>

class MainWindow;
class QMdiSubWindow;
class WorldEdit;
class QGraphicsItem;

namespace Ui
{
    class WldSearchBox;
}

class WldSearchBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit WldSearchBox(QWidget *parent = nullptr);
    ~WldSearchBox();

public slots:
    void re_translate();

    void toggleNewWindowWLD(QMdiSubWindow *window);
    void resetAllSearchFieldsWLD();
    void resetAllSearchesWLD();
    void selectLevelForSearch();

    void resetTileSearch();
    void resetScenerySearch();
    void resetPathSearch();
    void resetLevelSearch();
    void resetMusicSearch();

private slots:
    void on_WldSearchBox_visibilityChanged(bool visible);
    void on_FindStartLevel_clicked();
    void on_FindStartTile_clicked();
    void on_FindStartScenery_clicked();
    void on_FindStartPath_clicked();
    void on_FindStartMusic_clicked();

    void on_tileSelectAll_clicked();
    void on_scenerySelectAll_clicked();
    void on_pathSelectAll_clicked();
    void on_levelSelectAll_clicked();
    void on_mboxSelectAll_clicked();

    void on_Find_Button_TypeLevel_clicked();
    void on_Find_Button_TypeTile_clicked();
    void on_Find_Button_TypeScenery_clicked();
    void on_Find_Button_TypePath_clicked();
    void on_Find_Button_TypeMusic_clicked();
    void on_Find_Button_ResetLevel_clicked();
    void on_Find_Button_ResetMusic_clicked();
    void on_Find_Button_ResetPath_clicked();
    void on_Find_Button_ResetScenery_clicked();
    void on_Find_Button_ResetTile_clicked();

private:

    struct SearchMeta
    {
        quint64         total = 0;
        unsigned long   index = 0;
        struct Data
        {
            unsigned long id = 0;
            unsigned int index = 0;
        } data;
    };

    SearchMeta m_curTerrain;
    SearchMeta m_curScenery;
    SearchMeta m_curPath;
    SearchMeta m_curLevel;
    SearchMeta m_curMusicBox;

    typedef bool (WldSearchBox::*SearchCriteriaType)(QGraphicsItem *);

    bool checkTileCriteria(QGraphicsItem *gri);
    bool checkSceneryCriteria(QGraphicsItem *gri);
    bool checkPathCriteria(QGraphicsItem *gri);
    bool checkLevelCriteria(QGraphicsItem *gri);
    bool checkMusicBoxCriteria(QGraphicsItem *gri);

    bool doSearch(SearchMeta &meta,
                  SearchCriteriaType checkCriteria,
                  WorldEdit *edit,
                  bool all = false,
                  bool selectionOnly = false);

    Ui::WldSearchBox *ui;
    bool m_lockSettings = false;

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
    int m_currentSearches = 0;
};

#endif // WLD_SEARCH_BOX_H
