/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

LevelFileList::LevelFileList(QString Folder, QString current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelFileList)
{
    QStringList filters;
    QDir levelDir(Folder);
    filters << "*.lvl" << "*.lvlx" << "*.lvlb" << "*.lvlz";
    levelDir.setSorting(QDir::Name);
    levelDir.setNameFilters(filters);
    ui->setupUi(this);
    ui->FileList->insertItems(levelDir.entryList().size(), levelDir.entryList(filters) );

    // Select current item
    for(int i=0; i<ui->FileList->count(); i++)
    {
        if(ui->FileList->item(i)->text()==current)
        {
            ui->FileList->item(i)->setSelected(true);
            ui->FileList->scrollToItem(ui->FileList->item(i));
         break;
        }
    }

}

LevelFileList::~LevelFileList()
{
    delete ui;
}


void LevelFileList::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    SelectedFile = item->text();
    accept();
}

void LevelFileList::on_buttonBox_accepted()
{
    foreach (QListWidgetItem * container, ui->FileList->selectedItems()) {
    SelectedFile = container->text();
    }
    if(SelectedFile!="")
        accept();
}

void LevelFileList::on_buttonBox_rejected()
{
    reject();
}
