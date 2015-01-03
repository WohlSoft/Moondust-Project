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

#include <file_formats/lvl_filedata.h>
#include <file_formats/wld_filedata.h>

#include "../../mainwindow.h"
#include <ui_mainwindow.h>

void MainWindow::on_actionBookmarkBox_triggered(bool checked)
{
    ui->bookmarkBox->setVisible(checked);
    if(checked) ui->bookmarkBox->raise();
}


void MainWindow::on_bookmarkBox_visibilityChanged(bool visible)
{
    ui->actionBookmarkBox->setChecked(visible);
}

void MainWindow::on_bookmarkAdd_clicked()
{
    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText("New Bookmark");
    item->setFlags(item->flags() |
                   Qt::ItemIsEditable |
                   Qt::ItemIsDragEnabled |
                   Qt::ItemIsEnabled |
                   Qt::ItemIsSelectable);
    item->setData(Qt::UserRole+1, true); //x is init
    item->setData(Qt::UserRole+2, true); //y is init
    ui->bookmarkList->addItem(item);
    ui->bookmarkList->setFocus();
    ui->bookmarkList->scrollToItem( item );
    ui->bookmarkList->editItem( item );
}

void MainWindow::on_bookmarkList_itemChanged(QListWidgetItem *item)
{
    if(item->data(Qt::UserRole+1).type() == QVariant::Type::Bool ||
            item->data(Qt::UserRole+2).type() == QVariant::Type::Bool){

        qreal x, y;
        if(getCurrentSceneCoordinates(x,y)){
            item->setData(Qt::UserRole+1, x);
            item->setData(Qt::UserRole+2, y);
        }else{
            delete item;
        }
    }
    updateBookmarkBoxByList();
}

void MainWindow::on_bookmarkRemove_clicked()
{
    if(ui->bookmarkList->selectedItems().isEmpty()) return;

    delete ui->bookmarkList->selectedItems()[0];

    updateBookmarkBoxByList();
}

void MainWindow::on_bookmarkGoto_clicked()
{
    if(ui->bookmarkList->selectedItems().isEmpty()) return;

    QListWidgetItem* item = ui->bookmarkList->selectedItems()[0];
    qreal x = item->data(Qt::UserRole+1).toReal();
    qreal y = item->data(Qt::UserRole+2).toReal();
    long a = qRound(x);
    long b = qRound(y);
    if(activeChildWindow() == 1)
    {
        activeLvlEditWin()->goTo(a, b, true);
    }
    else
    if(activeChildWindow() == 3)
    {
        activeWldEditWin()->goTo(a, b, true);
    }
}

void MainWindow::updateBookmarkBoxByList()
{
    MetaData *mData;
    if(activeChildWindow() == 1){
        mData = &activeLvlEditWin()->LvlData.metaData;
    }
    else if(activeChildWindow() == 3){
        mData = &activeWldEditWin()->WldData.metaData;
    }
    else{
        return;
    }
    mData->bookmarks.clear();
    for(int i = 0; i < ui->bookmarkList->count(); ++i){
        QListWidgetItem *item = ui->bookmarkList->item(i);
        Bookmark bmItem;
        bmItem.bookmarkName = item->text();
        bmItem.x = item->data(Qt::UserRole+1).toReal();
        bmItem.y = item->data(Qt::UserRole+2).toReal();
        mData->bookmarks << bmItem;
    }
}

void MainWindow::updateBookmarkBoxByData()
{
    while(ui->bookmarkList->count())
        delete ui->bookmarkList->item(0);

    MetaData *mData;
    if(activeChildWindow() == 1){
        mData = &activeLvlEditWin()->LvlData.metaData;
    }
    else if(activeChildWindow() == 3){
        mData = &activeWldEditWin()->WldData.metaData;
    }
    else{
        return;
    }

    for(int i = 0; i < mData->bookmarks.size(); ++i){
        Bookmark &bmItem = mData->bookmarks[i];
        QListWidgetItem * item;
        item = new QListWidgetItem;
        item->setText(bmItem.bookmarkName);
        item->setFlags(item->flags() |
                       Qt::ItemIsEditable |
                       Qt::ItemIsDragEnabled |
                       Qt::ItemIsEnabled |
                       Qt::ItemIsSelectable);
        item->setData(Qt::UserRole+1, bmItem.x); //x is init
        item->setData(Qt::UserRole+2, bmItem.y); //y is init
        ui->bookmarkList->addItem(item);
    }
}



void MainWindow::on_bookmarkList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->bookmarkList->selectedItems().isEmpty()) return;

    QPoint globPos = ui->bookmarkList->mapToGlobal(pos);

    WriteToLog(QtDebugMsg, QString("Main Menu's context menu called! %1 %2 -> %3 %4")
               .arg(pos.x()).arg(pos.y())
               .arg(globPos.x()).arg(globPos.y()));

    QMenu *bookmark_menu = new QMenu(this);
    QAction * rename = bookmark_menu->addAction(tr("Rename Bookmark"));

    //bookmark_menu->addSeparator();
    QAction *selected = bookmark_menu->exec( globPos );
    if(selected==rename)
    {
        ui->bookmarkList->editItem(ui->bookmarkList->selectedItems()[0]);
    }


}

void MainWindow::DragAndDroppedBookmark(QModelIndex /*sourceParent*/,int /*sourceStart*/,int /*sourceEnd*/,QModelIndex /*destinationParent*/,int /*destinationRow*/)
{
    updateBookmarkBoxByList();
}

void MainWindow::on_bookmarkList_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_bookmarkGoto_clicked();
}
