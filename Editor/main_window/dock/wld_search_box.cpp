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

#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/world/items/item_level.h>
#include <editing/_scenes/world/items/item_music.h>
#include <editing/_scenes/world/items/item_path.h>
#include <editing/_scenes/world/items/item_scene.h>
#include <editing/_scenes/world/items/item_tile.h>
#include <editing/_dialogs/levelfilelist.h>
#include <file_formats/wld_filedata.h>

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
    ui->setupUi(this);
    lockResetWorld=false;

    currentSearches = 0;

    curSearchTile.id = 0;
    curSearchTile.index = 0;

    curSearchScenery.id = 0;
    curSearchScenery.index = 0;

    curSearchPath.id = 0;
    curSearchPath.index = 0;

    curSearchLevel.id = 0;
    curSearchLevel.index = 0;

    curSearchMusic.id = 0;
    curSearchMusic.index = 0;

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    mw()->docks_world.
          addState(this, &GlobalSettings::WorldSearchBoxVis);

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

    connect(mw()->ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(toggleNewWindowWLD(QMdiSubWindow*)));
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
    if(!(currentSearches & SEARCH_LEVEL)){
        ui->Find_Check_TypeLevel->setChecked(true);
        ui->Find_Button_TypeLevel->setText(tr("[empty]"));
        curSearchLevel.id = 0;
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
    }else{
        currentSearches ^= SEARCH_LEVEL;
        ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
        ui->FindStartLevel->setText(tr("Search Level"));
        curSearchLevel.index = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetMusic_clicked()
{
    if(!(currentSearches & SEARCH_MUSICBOX)){
        ui->Find_Check_TypeMusic->setChecked(true);
        ui->Find_Button_TypeMusic->setText(tr("[empty]"));
        curSearchMusic.id = 0;
    }else{
        currentSearches ^= SEARCH_MUSICBOX;
        ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
        ui->FindStartMusic->setText(tr("Search Music"));
        curSearchMusic.index = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetPath_clicked()
{
    if(!(currentSearches & SEARCH_PATH)){
        ui->Find_Check_TypePath->setChecked(true);
        ui->Find_Button_TypePath->setText(tr("[empty]"));
        curSearchPath.id = 0;
    }else{
        currentSearches ^= SEARCH_PATH;
        ui->Find_Button_ResetPath->setText(tr("Reset Search Fields"));
        ui->FindStartPath->setText(tr("Search Path"));
        curSearchPath.index = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetScenery_clicked()
{
    if(!(currentSearches & SEARCH_SCENERY)){
        ui->Find_Check_TypeScenery->setChecked(true);
        ui->Find_Button_TypeScenery->setText(tr("[empty]"));
        curSearchScenery.id = 0;
    }else{
        currentSearches ^= SEARCH_SCENERY;
        ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
        ui->FindStartScenery->setText(tr("Search Scenery"));
        curSearchScenery.index = 0;
    }
}

void WldSearchBox::on_Find_Button_ResetTile_clicked()
{
    if(!(currentSearches & SEARCH_TILE)){
        ui->Find_Check_TypeTile->setChecked(true);
        ui->Find_Button_TypeTile->setText(tr("[empty]"));
        curSearchTile.id = 0;
    }else{
        currentSearches ^= SEARCH_TILE;
        ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
        ui->FindStartTile->setText(tr("Search Tile"));
        curSearchTile.index = 0;
    }
}

void WldSearchBox::on_Find_Button_TypeLevel_clicked()
{
    ItemSelectDialog* selLevel = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_LEVEL,0,0,0,0,0,0,0,curSearchLevel.id);
    if(selLevel->exec()==QDialog::Accepted){
        int selected = selLevel->levelID;
        curSearchLevel.id = selected;
        ui->Find_Button_TypeLevel->setText(((selected>0)?QString("Level-%1").arg(selected):tr("[empty]")));
    }
    delete selLevel;
}

void WldSearchBox::on_Find_Button_TypeTile_clicked()
{
    ItemSelectDialog* selTile = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_TILE,0,0,0,0,curSearchTile.id,0,0,0);
    if(selTile->exec()==QDialog::Accepted){
        int selected = selTile->tileID;
        curSearchTile.id = selected;
        ui->Find_Button_TypeTile->setText(((selected>0)?QString("Tile-%1").arg(selected):tr("[empty]")));
    }
    delete selTile;
}

void WldSearchBox::on_Find_Button_TypeScenery_clicked()
{
    ItemSelectDialog* selScenery = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_SCENERY,0,0,0,0,0,curSearchScenery.id,0,0);
    if(selScenery->exec()==QDialog::Accepted){
        int selected = selScenery->sceneryID;
        curSearchScenery.id = selected;
        ui->Find_Button_TypeScenery->setText(((selected>0)?QString("Scenery-%1").arg(selected):tr("[empty]")));
    }
    delete selScenery;
}

void WldSearchBox::on_Find_Button_TypePath_clicked()
{
    ItemSelectDialog* selPath = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_PATH,0,0,0,0,0,0,curSearchPath.id,0);
    if(selPath->exec()==QDialog::Accepted){
        int selected = selPath->pathID;
        curSearchPath.id = selected;
        ui->Find_Button_TypePath->setText(((selected>0)?QString("Path-%1").arg(selected):tr("[empty]")));
    }
    delete selPath;
}

void WldSearchBox::on_Find_Button_TypeMusic_clicked()
{
    ItemSelectDialog* selMusic = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_MUSIC,0,0,0,0,0,0,0,0,curSearchMusic.id);
    if(selMusic->exec()==QDialog::Accepted){
        int selected = selMusic->musicID;
        curSearchMusic.id = selected;
        int index = mw()->configs.getMusWldI(selected);
        if(index!=-1){
            ui->Find_Button_TypeMusic->setText(mw()->configs.main_music_wld[index].name);
        }else{
            ui->Find_Button_TypeMusic->setText(((selected>0)?QString("Music-%1").arg(selected):tr("[empty]")));
        }
    }
    delete selMusic;
}


void WldSearchBox::on_FindStartLevel_clicked()
{
    if(!(currentSearches & SEARCH_LEVEL)){ //start search
        if(mw()->activeChildWindow()==3){
            currentSearches |= SEARCH_LEVEL;
            ui->FindStartLevel->setText(tr("Next Level"));
            ui->Find_Button_ResetLevel->setText(tr("Stop Search"));
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchLevel(edit)){
                currentSearches ^= SEARCH_LEVEL;
                ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
                ui->FindStartLevel->setText(tr("Search Level"));
                QMessageBox::information(this, tr("Search Complete"), tr("Level search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==3){
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchLevel(edit)){
                currentSearches ^= SEARCH_LEVEL;
                ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
                ui->FindStartLevel->setText(tr("Search Level"));
                QMessageBox::information(this, tr("Search Complete"), tr("Level search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartTile_clicked()
{
    if(!(currentSearches & SEARCH_TILE)){ //start search
        if(mw()->activeChildWindow()==3){
            currentSearches |= SEARCH_TILE;
            ui->FindStartTile->setText(tr("Next Tile"));
            ui->Find_Button_ResetTile->setText(tr("Stop Search"));
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchTile(edit)){
                currentSearches ^= SEARCH_TILE;
                ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
                ui->FindStartTile->setText(tr("Search Tile"));
                QMessageBox::information(this, tr("Search Complete"), tr("Tile search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==3){
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchTile(edit)){
                currentSearches ^= SEARCH_TILE;
                ui->Find_Button_ResetTile->setText(tr("Reset Search Fields"));
                ui->FindStartTile->setText(tr("Search Tile"));
                QMessageBox::information(this, tr("Search Complete"), tr("Tile search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartScenery_clicked()
{
    if(!(currentSearches & SEARCH_SCENERY)){ //start search
        if(mw()->activeChildWindow()==3){
            currentSearches |= SEARCH_SCENERY;
            ui->FindStartScenery->setText(tr("Next Scenery"));
            ui->Find_Button_ResetScenery->setText(tr("Stop Search"));
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchScenery(edit)){
                currentSearches ^= SEARCH_SCENERY;
                ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
                ui->FindStartScenery->setText(tr("Search Scenery"));
                QMessageBox::information(this, tr("Search Complete"), tr("Scenery search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==3)
        {
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchScenery(edit)){
                currentSearches ^= SEARCH_SCENERY;
                ui->Find_Button_ResetScenery->setText(tr("Reset Search Fields"));
                ui->FindStartScenery->setText(tr("Search Scenery"));
                QMessageBox::information(this, tr("Search Complete"), tr("Scenery search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartPath_clicked()
{
    if(!(currentSearches & SEARCH_PATH)){ //start search
        if(mw()->activeChildWindow()==3){
            currentSearches |= SEARCH_PATH;
            ui->FindStartPath->setText(tr("Next Path"));
            ui->Find_Button_ResetPath->setText(tr("Stop Search"));
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchPath(edit)){
                currentSearches ^= SEARCH_PATH;
                ui->Find_Button_ResetPath->setText(tr("Reset Search Fields"));
                ui->FindStartPath->setText(tr("Search Path"));
                QMessageBox::information(this, tr("Search Complete"), tr("Path search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==3){
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchPath(edit)){
                currentSearches ^= SEARCH_PATH;
                ui->Find_Button_ResetPath->setText(tr("Reset Search Fields"));
                ui->FindStartPath->setText(tr("Search Path"));
                QMessageBox::information(this, tr("Search Complete"), tr("Path search completed!"));
            }
        }
    }
}

void WldSearchBox::on_FindStartMusic_clicked()
{
    if(!(currentSearches & SEARCH_MUSICBOX)){ //start search
        if(mw()->activeChildWindow()==3){
            currentSearches |= SEARCH_MUSICBOX;
            ui->FindStartMusic->setText(tr("Next Music"));
            ui->Find_Button_ResetMusic->setText(tr("Stop Search"));
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchMusic(edit)){
                currentSearches ^= SEARCH_MUSICBOX;
                ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
                ui->FindStartMusic->setText(tr("Search Music"));
                QMessageBox::information(this, tr("Search Complete"), tr("Music search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==3){
            WorldEdit* edit = mw()->activeWldEditWin();
            if(!edit) return;
            if(doSearchMusic(edit)){
                currentSearches ^= SEARCH_MUSICBOX;
                ui->Find_Button_ResetMusic->setText(tr("Reset Search Fields"));
                ui->FindStartMusic->setText(tr("Search Music"));
                QMessageBox::information(this, tr("Search Complete"), tr("Music search completed!"));
            }
        }
    }
}

bool WldSearchBox::doSearchTile(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchTile.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchTile.index+1; i < gr.size(); ++i){
            if(gr[i]->data(ITEM_TYPE).toString()=="TILE"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeTile->isChecked()&&curSearchTile.id!=0&&toBeFound){
                    toBeFound = ((ItemTile*)gr[i])->tileData.id == (unsigned int)curSearchTile.id;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemTile*)gr[i])->tileData.x, ((ItemTile*)gr[i])->tileData.y, false, QPoint(-300, -300));
                    curSearchTile.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchTile.index = 0;
    return true;
}

bool WldSearchBox::doSearchScenery(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchScenery.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchScenery.index+1; i < gr.size(); ++i){
            if(gr[i]->data(ITEM_TYPE).toString()=="SCENERY"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeScenery->isChecked()&&curSearchScenery.id!=0&&toBeFound){
                    toBeFound = ((ItemScene*)gr[i])->sceneData.id == (unsigned int)curSearchScenery.id;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemScene*)gr[i])->sceneData.x, ((ItemScene*)gr[i])->sceneData.y, false, QPoint(-300, -300));
                    curSearchScenery.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchScenery.index = 0;
    return true;
}

bool WldSearchBox::doSearchPath(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchPath.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchPath.index+1; i < gr.size(); ++i){
            if(gr[i]->data(ITEM_TYPE).toString()=="PATH"){
                bool toBeFound = true;
                if(ui->Find_Check_TypePath->isChecked()&&curSearchPath.id!=0&&toBeFound){
                    toBeFound = ((ItemPath*)gr[i])->pathData.id == (unsigned int)curSearchPath.id;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemPath*)gr[i])->pathData.x, ((ItemPath*)gr[i])->pathData.y, false, QPoint(-300, -300));
                    curSearchPath.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchPath.index = 0;
    return true;
}

bool WldSearchBox::doSearchLevel(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchLevel.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchLevel.index+1; i < gr.size(); ++i){
            if(gr[i]->data(ITEM_TYPE).toString()=="LEVEL"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeLevel->isChecked()&&curSearchLevel.id!=0&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.id == (unsigned int)curSearchLevel.id;
                }
                if(ui->Find_Check_PathBackground->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.pathbg == ui->Find_Check_PathBackgroundActive->isChecked();
                }
                if(ui->Find_Check_BigPathBackground->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.bigpathbg == ui->Find_Check_BigPathBackgroundActive->isChecked();
                }
                if(ui->Find_Check_AlwaysVisible->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.alwaysVisible == ui->Find_Check_AlwaysVisibleActive->isChecked();
                }
                if(ui->Find_Check_GameStartPoint->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.gamestart == ui->Find_Check_GameStartPointActive->isChecked();
                }
                if(ui->Find_Check_LevelFile->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.lvlfile.contains(ui->Find_Edit_LevelFile->text(), Qt::CaseInsensitive);
                }
                if(ui->Find_Check_ContainsTitle->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.title.contains(ui->Find_Edit_ContainsTitle->text(), Qt::CaseInsensitive);
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemLevel*)gr[i])->levelData.x, ((ItemLevel*)gr[i])->levelData.y, false, QPoint(-300, -300));
                    curSearchLevel.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchLevel.index = 0;
    return true;
}

bool WldSearchBox::doSearchMusic(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchMusic.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchMusic.index+1; i < gr.size(); ++i){
            if(gr[i]->data(ITEM_TYPE).toString()=="MUSICBOX")
            {
                bool toBeFound = true;
                if(ui->Find_Check_TypeMusic->isChecked()&&curSearchMusic.id!=0&&toBeFound){
                    toBeFound = ((ItemMusic*)gr[i])->musicData.id == (unsigned int)curSearchMusic.id;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemMusic*)gr[i])->musicData.x, ((ItemMusic*)gr[i])->musicData.y, false, QPoint(-300, -300));
                    curSearchMusic.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchMusic.index = 0;
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
    if(lockResetWorld) return;
    lockResetWorld = true;
    if(currentSearches & SEARCH_TILE) on_Find_Button_ResetTile_clicked();
    lockResetWorld = false;
}

void WldSearchBox::resetScenerySearch()
{
    if(lockResetWorld) return;
    lockResetWorld = true;
    if(currentSearches & SEARCH_SCENERY) on_Find_Button_ResetScenery_clicked();
    lockResetWorld = false;
}

void WldSearchBox::resetPathSearch()
{
    if(lockResetWorld) return;
    lockResetWorld = true;
    if(currentSearches & SEARCH_PATH) on_Find_Button_ResetPath_clicked();
    lockResetWorld = false;
}

void WldSearchBox::resetLevelSearch()
{
    if(lockResetWorld) return;
    lockResetWorld = true;
    if(currentSearches & SEARCH_LEVEL) on_Find_Button_ResetLevel_clicked();
    lockResetWorld = false;
}

void WldSearchBox::resetMusicSearch()
{
    if(lockResetWorld) return;
    lockResetWorld = true;
    if(currentSearches & SEARCH_MUSICBOX) on_Find_Button_ResetMusic_clicked();
    lockResetWorld = false;
}

void WldSearchBox::toggleNewWindowWLD(QMdiSubWindow */*window*/)
{
    resetAllSearchesWLD();
}

void WldSearchBox::selectLevelForSearch()
{
    QString dirPath;
    if(mw()->activeChildWindow()==3)
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit) return;
        dirPath = edit->WldData.path;
    }
    else
        return;

    LevelFileList levelList(dirPath, ui->Find_Edit_LevelFile->text());
    if( levelList.exec() == QDialog::Accepted )
        ui->Find_Edit_LevelFile->setText(levelList.SelectedFile);
}
