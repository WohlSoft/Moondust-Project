/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDir>

#include "levelfilelist.h"
#include <ui_levelfilelist.h>

#include <QtConcurrent>

LevelFileList::LevelFileList(QString Folder, QString current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelFileList)
{
    parentFolder = Folder;
    lastCurrentFile = current;
    ui->setupUi(this);
    connect(this, &LevelFileList::itemAdded, this, &LevelFileList::addItem);
    connect(this, &LevelFileList::digFinished, this, &LevelFileList::finalizeDig);
    setCursor(Qt::BusyCursor);
    fileWalker = QtConcurrent::run(this, &LevelFileList::buildLevelList);
}

LevelFileList::~LevelFileList()
{
    if(fileWalker.isRunning())
        fileWalker.cancel();
    fileWalker.waitForFinished();
    delete ui;
}

void LevelFileList::buildLevelList()
{
    QDir musicDir(parentFolder);
    QStringList filters;
    filters << "*.lvl" << "*.lvlx" << "*.lvlb" << "*.lvlz";
    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(filters);
    QDirIterator dirsList(parentFolder, filters,
                          QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);

    while(dirsList.hasNext())
    {
        dirsList.next();
        emit itemAdded(musicDir.relativeFilePath(dirsList.filePath()));
        if(fileWalker.isCanceled())
            break;
    }
    digFinished();
}

void LevelFileList::addItem(QString item)
{
    ui->FileList->addItem(item);
    if(lastCurrentFile == item)
    {
        QList<QListWidgetItem *> list = ui->FileList->findItems(item, Qt::MatchFixedString);
        if(!list.isEmpty())
        {
            list.first()->setSelected(true);
            ui->FileList->scrollToItem(list.first());
        }
    }
}

void LevelFileList::finalizeDig()
{
    ui->FileList->sortItems(Qt::AscendingOrder);
    QList<QListWidgetItem *> list = ui->FileList->findItems(lastCurrentFile, Qt::MatchFixedString);
    if(!list.isEmpty())
    {
        list.first()->setSelected(true);
        ui->FileList->scrollToItem(list.first());
    }
    setCursor(Qt::ArrowCursor);
}

void LevelFileList::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    SelectedFile = item->text();
    if(fileWalker.isRunning())
        fileWalker.cancel();
    accept();
}

void LevelFileList::on_buttonBox_accepted()
{
    foreach(QListWidgetItem *container, ui->FileList->selectedItems())
        SelectedFile = container->text();
    if(SelectedFile != "")
    {
        if(fileWalker.isRunning())
            fileWalker.cancel();
        accept();
    }
}

void LevelFileList::on_buttonBox_rejected()
{
    if(fileWalker.isRunning())
        fileWalker.cancel();
    reject();
}
