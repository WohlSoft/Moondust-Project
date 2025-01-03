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

#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/world/items/item_level.h>
#include <editing/_scenes/world/items/item_music.h>
#include <editing/_scenes/world/items/item_path.h>
#include <editing/_scenes/world/items/item_scene.h>
#include <editing/_scenes/world/items/item_tile.h>
#include <PGE_File_Formats/wld_filedata.h>
#include "file_list_browser/levelfilelist.h"

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "wld_search_box.h"
#include "ui_wld_search_box.h"

WldSearchBox::WldSearchBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WldSearchBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    m_lockSettings = false;

    m_currentSearches = 0;

    m_curTerrain.data.id = 0;
    m_curTerrain.data.index = 0;

    m_curScenery.data.id = 0;
    m_curScenery.data.index = 0;

    m_curPath.data.id = 0;
    m_curPath.data.index = 0;

    m_curLevel.data.id = 0;
    m_curLevel.data.index = 0;

    m_curMusicBox.data.id = 0;
    m_curMusicBox.data.index = 0;

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );

    m_lastVisibilityState = isVisible();
    mw()->docks_world.
    addState(this, &m_lastVisibilityState);

    //enable & disable
    connect(ui->Find_Check_TypeLevel, SIGNAL(toggled(bool)), ui->Find_Button_TypeLevel, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeTile, SIGNAL(toggled(bool)), ui->Find_Button_TypeTile, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeScenery, SIGNAL(toggled(bool)), ui->Find_Button_TypeScenery, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypePath, SIGNAL(toggled(bool)), ui->Find_Button_TypePath, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeMusic, SIGNAL(toggled(bool)), ui->Find_Button_TypeMusic, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_PathBackground, SIGNAL(toggled(bool)), ui->Find_Check_PathBackgroundActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_BigPathBackground, SIGNAL(toggled(bool)), ui->Find_Check_BigPathBackgroundActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_AlwaysVisible, SIGNAL(toggled(bool)), ui->Find_Check_AlwaysVisibleActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_GameStartPoint, SIGNAL(toggled(bool)), ui->Find_Check_GameStartPointActive, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LevelFile, SIGNAL(toggled(bool)), ui->Find_Edit_LevelFile, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LevelFile, SIGNAL(toggled(bool)), ui->Find_Button_LevelFile, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_ContainsTitle, SIGNAL(toggled(bool)), ui->Find_Edit_ContainsTitle, SLOT(setEnabled(bool)));

    //reset if modify
    connect(ui->Find_Button_TypeTile, SIGNAL(clicked()), this, SLOT(resetTileSearch()));
    connect(ui->Find_Button_TypeScenery, SIGNAL(clicked()), this, SLOT(resetScenerySearch()));
    connect(ui->Find_Button_TypePath, SIGNAL(clicked()), this, SLOT(resetPathSearch()));
    connect(ui->Find_Button_TypeLevel, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Button_TypeMusic, SIGNAL(clicked()), this, SLOT(resetMusicSearch()));
    connect(ui->Find_Check_PathBackgroundActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_BigPathBackgroundActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_AlwaysVisibleActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_GameStartPointActive, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Edit_LevelFile, SIGNAL(textEdited(QString)), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Button_LevelFile, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Edit_ContainsTitle, SIGNAL(textEdited(QString)), this, SLOT(resetLevelSearch()));

    //also checkboxes
    connect(ui->Find_Check_TypeTile, SIGNAL(clicked()), this, SLOT(resetTileSearch()));
    connect(ui->Find_Check_TypeScenery, SIGNAL(clicked()), this, SLOT(resetScenerySearch()));
    connect(ui->Find_Check_TypePath, SIGNAL(clicked()), this, SLOT(resetPathSearch()));
    connect(ui->Find_Check_TypeLevel, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_TypeMusic, SIGNAL(clicked()), this, SLOT(resetMusicSearch()));
    connect(ui->Find_Check_PathBackground, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_BigPathBackground, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_AlwaysVisible, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_GameStartPoint, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_LevelFile, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));
    connect(ui->Find_Check_ContainsTitle, SIGNAL(clicked()), this, SLOT(resetLevelSearch()));

    connect(ui->Find_Button_LevelFile, SIGNAL(clicked()), this, SLOT(selectLevelForSearch()));

    connect(mw()->ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(toggleNewWindowWLD(QMdiSubWindow *)));
}

WldSearchBox::~WldSearchBox()
{
    delete ui;
}

void WldSearchBox::re_translate()
{
    ui->retranslateUi(this);
}




void MainWindow::on_actionWLD_SearchBox_triggered(bool checked)
{
    dock_WldSearchBox->setVisible(checked);
    if(checked)dock_WldSearchBox->raise();
}

void WldSearchBox::on_WldSearchBox_visibilityChanged(bool visible)
{
    mw()->ui->actionWLD_SearchBox->setChecked(visible);
}

void WldSearchBox::on_Find_Button_ResetLevel_clicked()
{
    if(!(m_currentSearches & SEARCH_LEVEL))
    {
        ui->Find_Check_TypeLevel->setChecked(true);
        ui->Find_Button_TypeLevel->setText(tr("[empty]"));
        m_curLevel.data.id = 0;
        ui->Find_Check_PathBackground->setChecked(false);
        ui->Find_Check_PathBackgroundActive->setChecked(false);
        ui->Find_Check_BigPathBackground->setChecked(false);
        ui->Find_Check_BigPathBackgroundActive->setChecked(false);
        ui->Find_Check_AlwaysVisible->setChecked(false);
        ui->Find_Check_AlwaysVisibleActive->setChecked(false);
        ui->Find_Check_GameStartPoint->setChecked(false);
        ui->Find_Check_GameStartPointActive->setChecked(false);
        ui->Find_Check_LevelFile->setChecked(false);
        ui->Find_Edit_LevelFile->setText("");
        ui->Find_Check_ContainsTitle->setChecked(false);
        ui->Find_Edit_ContainsTitle->setText("");
    }
    else
    {
        m_currentSearches ^= SEARCH_LEVEL;
        ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
        ui->FindStartLevel->setText(tr("Search Level"));
        m_curLevel.index = 0;
        m_curLevel.total = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetMusic_clicked()
{
    if(!(m_currentSearches & SEARCH_MUSICBOX))
    {
        ui->Find_Check_TypeMusic->setChecked(true);
        ui->Find_Button_TypeMusic->setText(tr("[empty]"));
        m_curMusicBox.data.id = 0;
    }
    else
    {
        m_currentSearches ^= SEARCH_MUSICBOX;
        ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
        ui->FindStartMusic->setText(tr("Search Music"));
        m_curMusicBox.index = 0;
        m_curMusicBox.total = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetPath_clicked()
{
    if(!(m_currentSearches & SEARCH_PATH))
    {
        ui->Find_Check_TypePath->setChecked(true);
        ui->Find_Button_TypePath->setText(tr("[empty]"));
        m_curPath.data.id = 0;
    }
    else
    {
        m_currentSearches ^= SEARCH_PATH;
        ui->Find_Button_ResetPath->setText(tr("Reset Search Fields"));
        ui->FindStartPath->setText(tr("Search Path"));
        m_curPath.index = 0;
        m_curPath.total = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetScenery_clicked()
{
    if(!(m_currentSearches & SEARCH_SCENERY))
    {
        ui->Find_Check_TypeScenery->setChecked(true);
        ui->Find_Button_TypeScenery->setText(tr("[empty]"));
        m_curScenery.data.id = 0;
    }
    else
    {
        m_currentSearches ^= SEARCH_SCENERY;
        ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
        ui->FindStartScenery->setText(tr("Search Scenery"));
        m_curScenery.index = 0;
        m_curScenery.total = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetTile_clicked()
{
    if(!(m_currentSearches & SEARCH_TILE))
    {
        ui->Find_Check_TypeTile->setChecked(true);
        ui->Find_Button_TypeTile->setText(tr("[empty]"));
        m_curTerrain.data.id = 0;
    }
    else
    {
        m_currentSearches ^= SEARCH_TILE;
        ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
        ui->FindStartTile->setText(tr("Search Tile"));
        m_curTerrain.index = 0;
        m_curTerrain.total = 0;
    }
}


void WldSearchBox::on_Find_Button_TypeLevel_clicked()
{
    std::unique_ptr<ItemSelectDialog> selLevel(new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_LEVEL, 0, 0, 0, 0, 0, 0, 0, m_curLevel.data.id, 0, this));
    if(selLevel->exec() == QDialog::Accepted)
    {
        int selected = selLevel->levelID;
        m_curLevel.data.id = selected;
        ui->Find_Button_TypeLevel->setText(((selected > 0) ? QString("Level-%1").arg(selected) : tr("[empty]")));
    }
}

void WldSearchBox::on_Find_Button_TypeTile_clicked()
{
    ItemSelectDialog selTile(&(mw()->configs), ItemSelectDialog::TAB_TILE, 0, 0, 0, 0, m_curTerrain.data.id, 0, 0, 0, 0, this);
    if(selTile.exec() == QDialog::Accepted)
    {
        int selected = selTile.tileID;
        m_curTerrain.data.id = selected;
        ui->Find_Button_TypeTile->setText(((selected > 0) ? QString("Tile-%1").arg(selected) : tr("[empty]")));
    }
}

void WldSearchBox::on_Find_Button_TypeScenery_clicked()
{
    ItemSelectDialog selScenery(&(mw()->configs), ItemSelectDialog::TAB_SCENERY, 0, 0, 0, 0, 0, m_curScenery.data.id, 0, 0, 0, this);
    if(selScenery.exec() == QDialog::Accepted)
    {
        int selected = selScenery.sceneryID;
        m_curScenery.data.id = selected;
        ui->Find_Button_TypeScenery->setText(((selected > 0) ? QString("Scenery-%1").arg(selected) : tr("[empty]")));
    }
}

void WldSearchBox::on_Find_Button_TypePath_clicked()
{
    ItemSelectDialog selPath(&(mw()->configs), ItemSelectDialog::TAB_PATH, 0, 0, 0, 0, 0, 0, m_curPath.data.id, 0, 0, this);
    if(selPath.exec() == QDialog::Accepted)
    {
        int selected = selPath.pathID;
        m_curPath.data.id = selected;
        ui->Find_Button_TypePath->setText(((selected > 0) ? QString("Path-%1").arg(selected) : tr("[empty]")));
    }
}

void WldSearchBox::on_Find_Button_TypeMusic_clicked()
{
    ItemSelectDialog selMusic(&(mw()->configs), ItemSelectDialog::TAB_MUSIC, 0, 0, 0, 0, 0, 0, 0, 0, m_curMusicBox.data.id, this);
    if(selMusic.exec() == QDialog::Accepted)
    {
        int selected = selMusic.musicID;
        m_curMusicBox.data.id = selected;
        int index = mw()->configs.getMusWldI(selected);
        if(index != -1)
            ui->Find_Button_TypeMusic->setText(mw()->configs.main_music_wld[index].name);
        else
            ui->Find_Button_TypeMusic->setText(((selected > 0) ? QString("Music-%1").arg(selected) : tr("[empty]")));
    }
}


void WldSearchBox::on_FindStartLevel_clicked()
{
    if(!(m_currentSearches & SEARCH_LEVEL))  //start search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            m_currentSearches |= SEARCH_LEVEL;
            ui->FindStartLevel->setText(tr("Next Level"));
            ui->Find_Button_ResetLevel->setText(tr("Stop Search"));

            if(doSearch(m_curLevel, &WldSearchBox::checkLevelCriteria, edit, false, ui->levelSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_LEVEL;
                ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
                ui->FindStartLevel->setText(tr("Search Level"));
                QMessageBox::information(this, tr("Search Complete"), tr("Level search completed!"));
            }
        }
    }
    else   //middle in a search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            if(doSearch(m_curLevel, &WldSearchBox::checkLevelCriteria, edit, false, ui->levelSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_LEVEL;
                ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
                ui->FindStartLevel->setText(tr("Search Level"));
                QMessageBox::information(this, tr("Search Complete"), tr("Level search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartTile_clicked()
{
    if(!(m_currentSearches & SEARCH_TILE))  //start search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            m_currentSearches |= SEARCH_TILE;
            ui->FindStartTile->setText(tr("Next Tile"));
            ui->Find_Button_ResetTile->setText(tr("Stop Search"));

            if(doSearch(m_curTerrain, &WldSearchBox::checkTileCriteria, edit, false, ui->tileSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_TILE;
                ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
                ui->FindStartTile->setText(tr("Search Tile"));
                QMessageBox::information(this, tr("Search Complete"), tr("Tile search completed!"));
            }
        }
    }
    else   //middle in a search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            if(doSearch(m_curTerrain, &WldSearchBox::checkTileCriteria, edit, false, ui->tileSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_TILE;
                ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
                ui->FindStartTile->setText(tr("Search Tile"));
                QMessageBox::information(this, tr("Search Complete"), tr("Tile search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartScenery_clicked()
{
    if(!(m_currentSearches & SEARCH_SCENERY))  //start search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            m_currentSearches |= SEARCH_SCENERY;
            ui->FindStartScenery->setText(tr("Next Scenery"));
            ui->Find_Button_ResetScenery->setText(tr("Stop Search"));

            if(doSearch(m_curScenery, &WldSearchBox::checkSceneryCriteria, edit, false, ui->scenerySelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_SCENERY;
                ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
                ui->FindStartScenery->setText(tr("Search Scenery"));
                QMessageBox::information(this, tr("Search Complete"), tr("Scenery search completed!"));
            }
        }
    }
    else   //middle in a search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            if(doSearch(m_curScenery, &WldSearchBox::checkSceneryCriteria, edit, false, ui->scenerySelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_SCENERY;
                ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
                ui->FindStartScenery->setText(tr("Search Scenery"));
                QMessageBox::information(this, tr("Search Complete"), tr("Scenery search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartPath_clicked()
{
    if(!(m_currentSearches & SEARCH_PATH))  //start search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            m_currentSearches |= SEARCH_PATH;
            ui->FindStartPath->setText(tr("Next Path"));
            ui->Find_Button_ResetPath->setText(tr("Stop Search"));

            if(doSearch(m_curPath, &WldSearchBox::checkPathCriteria, edit, false, ui->pathSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_PATH;
                ui->Find_Button_ResetPath->setText(tr("Reset Search Fields"));
                ui->FindStartPath->setText(tr("Search Path"));
                QMessageBox::information(this, tr("Search Complete"), tr("Path search completed!"));
            }
        }
    }
    else   //middle in a search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            if(doSearch(m_curPath, &WldSearchBox::checkPathCriteria, edit, false, ui->pathSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_PATH;
                ui->Find_Button_ResetPath->setText(tr("Reset Search Fields"));
                ui->FindStartPath->setText(tr("Search Path"));
                QMessageBox::information(this, tr("Search Complete"), tr("Path search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartMusic_clicked()
{
    if(!(m_currentSearches & SEARCH_MUSICBOX))  //start search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            m_currentSearches |= SEARCH_MUSICBOX;
            ui->FindStartMusic->setText(tr("Next Music"));
            ui->Find_Button_ResetMusic->setText(tr("Stop Search"));

            if(doSearch(m_curMusicBox, &WldSearchBox::checkMusicBoxCriteria, edit, false, ui->mboxSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_MUSICBOX;
                ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
                ui->FindStartMusic->setText(tr("Search Music"));
                QMessageBox::information(this, tr("Search Complete"), tr("Music search completed!"));
            }
        }
    }
    else   //middle in a search
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            WorldEdit *edit = mw()->activeWldEditWin();
            if(!edit)
                return;

            if(doSearch(m_curMusicBox, &WldSearchBox::checkMusicBoxCriteria, edit, false, ui->mboxSelectionOnly->isChecked()))
            {
                m_currentSearches ^= SEARCH_MUSICBOX;
                ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
                ui->FindStartMusic->setText(tr("Search Music"));
                QMessageBox::information(this, tr("Search Complete"), tr("Music search completed!"));
            }
        }
    }
}



void WldSearchBox::on_tileSelectAll_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    auto *edit = mw()->activeWldEditWin();
    if(!edit)
        return;

    m_currentSearches |= SEARCH_TILE;
    m_currentSearches ^= SEARCH_TILE;
    ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
    ui->FindStartTile->setText(tr("Search Tile"));
    doSearch(m_curTerrain, &WldSearchBox::checkTileCriteria, edit,
             true, ui->tileSelectionOnly->isChecked());
}

void WldSearchBox::on_scenerySelectAll_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    auto *edit = mw()->activeWldEditWin();
    if(!edit)
        return;

    m_currentSearches |= SEARCH_SCENERY;
    m_currentSearches ^= SEARCH_SCENERY;
    ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
    ui->FindStartScenery->setText(tr("Search Scenery"));
    doSearch(m_curScenery, &WldSearchBox::checkSceneryCriteria, edit,
             true, ui->scenerySelectionOnly->isChecked());
}

void WldSearchBox::on_pathSelectAll_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    auto *edit = mw()->activeWldEditWin();
    if(!edit)
        return;

    m_currentSearches |= SEARCH_PATH;
    m_currentSearches ^= SEARCH_PATH;
    ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
    ui->FindStartScenery->setText(tr("Search Scenery"));
    doSearch(m_curPath, &WldSearchBox::checkPathCriteria, edit,
             true, ui->pathSelectionOnly->isChecked());
}

void WldSearchBox::on_levelSelectAll_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    auto *edit = mw()->activeWldEditWin();
    if(!edit)
        return;

    m_currentSearches |= SEARCH_LEVEL;
    m_currentSearches ^= SEARCH_LEVEL;
    ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
    ui->FindStartLevel->setText(tr("Search Level"));
    doSearch(m_curLevel, &WldSearchBox::checkLevelCriteria, edit,
             true, ui->levelSelectionOnly->isChecked());
}

void WldSearchBox::on_mboxSelectAll_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    auto *edit = mw()->activeWldEditWin();
    if(!edit)
        return;

    m_currentSearches |= SEARCH_MUSICBOX;
    m_currentSearches ^= SEARCH_MUSICBOX;
    ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
    ui->FindStartMusic->setText(tr("Search Music"));
    doSearch(m_curMusicBox, &WldSearchBox::checkMusicBoxCriteria, edit,
             true, ui->mboxSelectionOnly->isChecked());
}


bool WldSearchBox::checkTileCriteria(QGraphicsItem *gri)
{
    if(gri->data(ITEM_TYPE).toString() != "TILE")
        return false;

    auto *it = qgraphicsitem_cast<ItemTile *>(gri);
    Q_ASSERT(it);
    auto &d = it->m_data;

    bool toBeFound = true;

    if(ui->Find_Check_TypeTile->isChecked() && m_curTerrain.data.id != 0 && toBeFound)
        toBeFound = d.id == (unsigned int)m_curTerrain.data.id;

    return toBeFound;
}

bool WldSearchBox::checkSceneryCriteria(QGraphicsItem *gri)
{
    if(gri->data(ITEM_TYPE).toString() != "SCENERY")
        return false;

    auto *it = qgraphicsitem_cast<ItemScene *>(gri);
    Q_ASSERT(it);
    auto &d = it->m_data;

    bool toBeFound = true;

    if(ui->Find_Check_TypeScenery->isChecked() && m_curScenery.data.id != 0 && toBeFound)
        toBeFound = d.id == (unsigned int)m_curScenery.data.id;

    return toBeFound;
}

bool WldSearchBox::checkPathCriteria(QGraphicsItem *gri)
{
    if(gri->data(ITEM_TYPE).toString() != "PATH")
        return false;

    auto *it = qgraphicsitem_cast<ItemPath *>(gri);
    Q_ASSERT(it);
    auto &d = it->m_data;

    bool toBeFound = true;

    if(ui->Find_Check_TypePath->isChecked() && m_curPath.data.id != 0 && toBeFound)
        toBeFound = d.id == (unsigned int)m_curPath.data.id;

    return toBeFound;
}

bool WldSearchBox::checkLevelCriteria(QGraphicsItem *gri)
{
    if(gri->data(ITEM_TYPE).toString() != "LEVEL")
        return false;

    auto *it = qgraphicsitem_cast<ItemLevel *>(gri);
    Q_ASSERT(it);
    auto &d = it->m_data;

    bool toBeFound = true;

    if(ui->Find_Check_TypeLevel->isChecked() && m_curLevel.data.id != 0 && toBeFound)
        toBeFound = d.id == (unsigned int)m_curLevel.data.id;

    if(ui->Find_Check_PathBackground->isChecked() && toBeFound)
        toBeFound = d.pathbg == ui->Find_Check_PathBackgroundActive->isChecked();

    if(ui->Find_Check_BigPathBackground->isChecked() && toBeFound)
        toBeFound = d.bigpathbg == ui->Find_Check_BigPathBackgroundActive->isChecked();

    if(ui->Find_Check_AlwaysVisible->isChecked() && toBeFound)
        toBeFound = d.alwaysVisible == ui->Find_Check_AlwaysVisibleActive->isChecked();

    if(ui->Find_Check_GameStartPoint->isChecked() && toBeFound)
        toBeFound = d.gamestart == ui->Find_Check_GameStartPointActive->isChecked();

    if(ui->Find_Check_LevelFile->isChecked() && toBeFound)
        toBeFound = d.lvlfile.contains(ui->Find_Edit_LevelFile->text(), Qt::CaseInsensitive);

    if(ui->Find_Check_ContainsTitle->isChecked() && toBeFound)
        toBeFound = d.title.contains(ui->Find_Edit_ContainsTitle->text(), Qt::CaseInsensitive);

    return toBeFound;
}

bool WldSearchBox::checkMusicBoxCriteria(QGraphicsItem *gri)
{
    if(gri->data(ITEM_TYPE).toString() != "MUSICBOX")
        return false;

    auto *it = qgraphicsitem_cast<ItemMusic *>(gri);
    Q_ASSERT(it);
    auto &d = it->m_data;

    bool toBeFound = true;

    if(ui->Find_Check_TypeMusic->isChecked() && m_curMusicBox.data.id != 0 && toBeFound)
        toBeFound = d.id == (unsigned int)m_curMusicBox.data.id;

    return toBeFound;
}

bool WldSearchBox::doSearch(WldSearchBox::SearchMeta &meta,
                            WldSearchBox::SearchCriteriaType checkCriteria,
                            WorldEdit *edit,
                            bool all, bool selectionOnly)
{
    auto gr = edit->scene->items();
    auto grSize = static_cast<quint64>(gr.size());

    bool foundAll = false;
    QPoint foundFirst;
    int foundCount = 0;

    //Reset search on changing total number of elements (something added or deleted)
    //... however, will be continued if something replaced
    if(grSize != meta.total || all)
    {
        meta.total = grSize;
        meta.index = 0;
        if(all)
        {
            if(selectionOnly)
            {
                gr = edit->scene->selectedItems();
                grSize = static_cast<quint64>(gr.size());
            }

            for(QGraphicsItem *it : edit->scene->selectedItems())
                it->setSelected(false);
        }
    }

    if(meta.index < grSize)
    {
        for(quint64 i = meta.index; i < grSize; ++i)
        {
            auto *gri = gr[i];
            long x = static_cast<long>(gri->x());
            long y = static_cast<long>(gri->y());

            bool toBeFound = (this->*checkCriteria)(gri);

            if(toBeFound)
            {
                if(!all)
                {
                    for(QGraphicsItem *it : edit->scene->selectedItems())
                        it->setSelected(false);
                }

                gri->setSelected(true);

                if(!all)
                {
                    edit->goTo(x, y, true, QPoint(-300, -300));
                    meta.index = i + 1;//Continue search at next
                    return false;
                }
                else if(!foundAll)
                {
                    foundAll = true;
                    foundFirst.setY(y);
                    foundFirst.setX(x);
                }
                else
                {
                    // Move to left-top of selection group for convenience
                    if(foundFirst.y() > y)
                        foundFirst.setY(y);
                    if(foundFirst.x() > x)
                        foundFirst.setX(x);
                }

                foundCount++;
            }
        }
    }

    if(foundAll)
    {
        edit->goTo(foundFirst.x(), foundFirst.y(), true, QPoint(-300, -300));
        mw()->showStatusMsg(tr("%1 found elements were selected.").arg(foundCount), 3000);
    }

    //end search
    meta.index = 0; //When incrementing then starting with 0
    return true;
}


void WldSearchBox::resetAllSearchFieldsWLD()
{
    resetAllSearchesWLD();

    on_Find_Button_ResetTile_clicked();
    on_Find_Button_ResetScenery_clicked();
    on_Find_Button_ResetPath_clicked();
    on_Find_Button_ResetLevel_clicked();
    on_Find_Button_ResetMusic_clicked();
}

void WldSearchBox::resetAllSearchesWLD()
{
    resetTileSearch();
    resetScenerySearch();
    resetPathSearch();
    resetLevelSearch();
    resetMusicSearch();
}

void WldSearchBox::resetTileSearch()
{
    if(m_lockSettings) return;
    m_lockSettings = true;
    if(m_currentSearches & SEARCH_TILE)
        on_Find_Button_ResetTile_clicked();
    m_lockSettings = false;
}

void WldSearchBox::resetScenerySearch()
{
    if(m_lockSettings) return;
    m_lockSettings = true;
    if(m_currentSearches & SEARCH_SCENERY)
        on_Find_Button_ResetScenery_clicked();
    m_lockSettings = false;
}

void WldSearchBox::resetPathSearch()
{
    if(m_lockSettings) return;
    m_lockSettings = true;
    if(m_currentSearches & SEARCH_PATH)
        on_Find_Button_ResetPath_clicked();
    m_lockSettings = false;
}

void WldSearchBox::resetLevelSearch()
{
    if(m_lockSettings) return;
    m_lockSettings = true;
    if(m_currentSearches & SEARCH_LEVEL)
        on_Find_Button_ResetLevel_clicked();
    m_lockSettings = false;
}

void WldSearchBox::resetMusicSearch()
{
    if(m_lockSettings) return;
    m_lockSettings = true;
    if(m_currentSearches & SEARCH_MUSICBOX)
        on_Find_Button_ResetMusic_clicked();
    m_lockSettings = false;
}

void WldSearchBox::toggleNewWindowWLD(QMdiSubWindow */*window*/)
{
    resetAllSearchesWLD();
}

void WldSearchBox::selectLevelForSearch()
{
    QString dirPath;
    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        dirPath = edit->WldData.meta.path;
    }
    else
        return;

    LevelFileList levelList(dirPath, ui->Find_Edit_LevelFile->text());
    if(levelList.exec() == QDialog::Accepted)
        ui->Find_Edit_LevelFile->setText(levelList.currentFile());
}
