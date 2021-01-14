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

#include <common_features/util.h>
#include <common_features/items.h>
#include <common_features/graphics_funcs.h>
#include <editing/_scenes/world/wld_item_placing.h>
#include <data_configs/custom_data.h>
#include <PGE_File_Formats/file_formats.h>
#include <audio/music_player.h>
#include <editing/_dialogs/musicfilelist.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "wld_musicbox_itemset.h"
#include "ui_wld_musicbox_itemset.h"

#include "item_tooltip_make.hpp"
#include "itembox_list_model.h"

WorldMusicBoxItemBox::WorldMusicBoxItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WorldMusicBoxItemBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);

    m_musicBoxModel = new ItemBoxListModel(this);
    ui->WLD_MusicList->setModel(m_musicBoxModel);
    connect(ui->WLD_MusicList, &QListView::clicked, this, &WorldMusicBoxItemBox::MusicList_itemClicked);
    m_musicBoxModel->setSort(ItemBoxListModel::Sort_ById, false);

    mw()->addDockWidget(Qt::LeftDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionMusicBoxes, SLOT(setChecked(bool)));

    m_lastVisibilityState = isVisible();
    mw()->docks_world.addState(this, &m_lastVisibilityState);
}

WorldMusicBoxItemBox::~WorldMusicBoxItemBox()
{
    delete ui;
}

QTabWidget *WorldMusicBoxItemBox::tabWidget()
{
    return ui->WorldToolBoxTabs;
}

void WorldMusicBoxItemBox::re_translate()
{
    ui->retranslateUi(this);
}

void MainWindow::on_actionMusicBoxes_triggered(bool checked)
{
    dock_WldMusicBoxes->setVisible(checked);
    if(checked) dock_WldMusicBoxes->raise();
}



void WorldMusicBoxItemBox::initItemLists()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    WorldEdit *edit = mw()->activeWldEditWin();
    if((edit == nullptr) || (!edit->sceneCreated))
        return;

    WldScene* scene = edit->scene;
    if(!scene)
        return;

    mw()->ui->menuNew->setEnabled(false);

    LogDebugQD("MusicBoxesTools -> Clear current");

    m_musicBoxModel->clear();

    LogDebugQD("MusicBoxesTools -> Declare new");

    LogDebug("MusicBoxesTools -> Fill list of Music Boxes");
    {
        m_musicBoxModel->addElementsBegin();
        for(int i = 1; i < mw()->configs.main_music_wld.size(); i++)
        {
            obj_music &musicItem = mw()->configs.main_music_wld[i];
            ItemBoxListModel::Element e;
            e.pixmap = QPixmap(":/images/playmusic.png");
            e.name = musicItem.name.isEmpty() ? QString("musicbox-%1").arg(musicItem.id) : musicItem.name;
            e.description = QString("ID: %1").arg(musicItem.id);
            e.elementId = musicItem.id;
            e.isCustom = false;
            e.isValid = true;
            m_musicBoxModel->addElement(e);
        }
        m_musicBoxModel->addElementsEnd();
    }

    mw()->ui->menuNew->setEnabled(true);
    LogDebugQD("MusicBoxesTools -> done");
}

void WorldMusicBoxItemBox::MusicList_itemClicked(const QModelIndex &item)
{
    if(!item.isValid())
        return;//Ignore invalid

    //placeMusicBox
    if((mw()->activeChildWindow() == MainWindow::WND_World) && (ui->WLD_MusicList->hasFocus()))
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit)
            return;
        if(!m_musicBoxModel->data(item, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
            return;
        int id = m_musicBoxModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();

        QString customMusicFile;
        if((uint64_t)id == mw()->configs.music_w_custom_id)
        {
            if(edit->isUntitled())
            {
                QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
                return;
            }

            QString dirPath = edit->WldData.meta.path;
            MusicFileList musicList(dirPath, "");
            if(musicList.exec() == QDialog::Accepted)
                customMusicFile = musicList.currentFile();
            else
                return;
        }

        WldPlacingItems::musicSet.music_file = customMusicFile;
        mw()->SwitchPlacingItem(ItemTypes::WLD_MusicBox, id);

        //Play selected music
        edit->currentMusic = id;
        edit->currentCustomMusic = customMusicFile;
        LvlMusPlay::setMusic(mw(), LvlMusPlay::WorldMusic,
                             edit->currentMusic,
                             edit->currentCustomMusic);
        mw()->setMusic();
    }
}
