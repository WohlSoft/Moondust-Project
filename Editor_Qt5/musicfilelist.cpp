/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "musicfilelist.h"
#include "ui_musicfilelist.h"
#include <QDir>

MusicFileList::MusicFileList(QString Folder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicFileList)
{
    QStringList filters;
    QDir musicDir(Folder);
    filters << "*.mp3" << "*.ogg" << "*.wav" << "*.mid";
    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(filters);
    ui->setupUi(this);
    ui->FileList->insertItems(musicDir.entryList().size(), musicDir.entryList(filters) );
}

MusicFileList::~MusicFileList()
{
    delete ui;
}

void MusicFileList::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    SelectedFile = item->text();
    accept();
}

void MusicFileList::on_buttonBox_accepted()
{
    foreach (QListWidgetItem * container, ui->FileList->selectedItems()) {
    SelectedFile = container->text();
    }
    if(SelectedFile!="")
        accept();
}

void MusicFileList::on_buttonBox_rejected()
{
    reject();
}
